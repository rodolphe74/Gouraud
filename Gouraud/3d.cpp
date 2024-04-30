#include "3d.h"
#include "ScanPolygon.h"
#include "Chronometer.h"
#include <float.h>
#include <fstream>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>


#define MILLEVINGTQUATRE 1024
#define VEC4MULMAT4 vec4MulMat4Mmx

static std::chrono::steady_clock::time_point beginTime;
static std::chrono::steady_clock::time_point endTime;
static double countLap = 0;
static double sumLap = 0;

// default base material for WF objects
float ambient[] = { 0.2f, 0.2f, 0.2f };				// default ambient for exported WF objects
float specularStrength = 1.0f;


void lookAt(RVector &position, RVector &target, RVector &up, RMatrix &mat)
{
	// https://github.com/felselva/mathc
	RVector forward({ target.v[0], target.v[1], target.v[2] }, VTYPE::VEC3);
	forward.vecSubVec(position);
	forward.vecNormalize();

	RVector side(VEC3);
	side.v[0] = forward.v[0];
	side.v[1] = forward.v[1];
	side.v[2] = forward.v[2];

	side.vec3CrossVec3(up);
	side.vecNormalize();

	mat.matSetAt(0, 0, side.v[0]);
	mat.matSetAt(0, 1, side.v[1]);
	mat.matSetAt(0, 2, side.v[2]);
	mat.matSetAt(0, 3, -side.vec3DotReal(position));

	mat.matSetAt(1, 0, up.v[0]);
	mat.matSetAt(1, 1, up.v[1]);
	mat.matSetAt(1, 2, up.v[2]);
	mat.matSetAt(1, 3, -up.vec3DotReal(position));

	mat.matSetAt(2, 0, -forward.v[0]);
	mat.matSetAt(2, 1, -forward.v[1]);
	mat.matSetAt(2, 2, -forward.v[2]);
	mat.matSetAt(2, 3, forward.vec3DotReal(position));

	mat.matSetAt(3, 0, 0.0f);
	mat.matSetAt(3, 1, 0.0f);
	mat.matSetAt(3, 2, 0.0f);
	mat.matSetAt(3, 3, 1.0f);
}

void perspective(float fov_y, float aspect, float n, float f, RMatrix &mat)
{
	// https://github.com/felselva/mathc
	float tan_half_fov_y = (float)(1.0f / std::tan(fov_y * 0.5f));

	mat.matSetAt(0, 0, 1.0f / aspect * tan_half_fov_y);
	mat.matSetAt(0, 1, 0.0f);
	mat.matSetAt(0, 2, 0.0f);
	mat.matSetAt(0, 3, 0.0f);

	mat.matSetAt(1, 0, 0.0f);
	mat.matSetAt(1, 1, 1.0f / tan_half_fov_y);
	mat.matSetAt(1, 2, 0.0f);
	mat.matSetAt(1, 3, 0.0f);

	mat.matSetAt(2, 0, 0.0f);
	mat.matSetAt(2, 1, 0.0f);
	mat.matSetAt(2, 2, f / (n - f));
	mat.matSetAt(2, 3, -(f * n) / (f - n));

	mat.matSetAt(3, 0, 0.0f);
	mat.matSetAt(3, 1, 0.0f);
	mat.matSetAt(3, 2, -1.0f);
	mat.matSetAt(3, 3, 0.0f);
}


Light *createLight(float x, float y, float z, Color c, float i)
{
	Light *l = (Light *)malloc(sizeof(Light));
	if (l) {
		l->pos[0] = (float)x;
		l->pos[1] = (float)y;
		l->pos[2] = (float)z;
		l->intensity = i;
		l->c = c;
	}
	return l;

}


void reflect(RVector &out, RVector &incident, RVector &normal)
{
	float dot = incident.vec3DotReal(normal);
	normal.vecMulScalar(2 * dot);
	out.v[0] = incident.v[0];
	out.v[1] = incident.v[1];
	out.v[2] = incident.v[2];
	out.vecSubVec(normal);
	out.vecNormalize();
}

void render(char *pixels, Light *lg, Obj &o, RMatrix &view, RMatrix &perspective, RVector &from, int w, int h, float *zBuffer)
{
	RVector worldPos(VEC3);
	RVector worldNorm(VEC3);
	RVector lightDir(VEC3);
	RVector viewDir(VEC3);
	RVector negLightDir(VEC3);
	RVector reflectDir(VEC3);
	RVector specular(VEC3);
	RVector lightColor(VEC3);
	RVector objectColor(VEC3);
	RVector c(VEC3);
	RVector diffuseLightColorV(VEC3);
	RVector ambientDiffuseSpecular(VEC3);
	RVector cameraPos(VEC4);
	Material *currentMaterial;

	lightColor.v[0] = lg->c.r;
	lightColor.v[1] = lg->c.g;
	lightColor.v[2] = lg->c.b;

	objectColor.v[0] = o.color.r;
	objectColor.v[1] = o.color.g;
	objectColor.v[2] = o.color.b;

	std::map<std::string, Object *>::iterator it;
	for (it = o.objects.begin(); it != o.objects.end(); it++) {

		currentMaterial = &it->second->material;

		for (size_t i = 0; i < it->second->faces.size(); i++) {
			Face *f = it->second->faces[i];
			int sz = f->vertices.size();
			GVertex *vertices = new GVertex[sz];

			for (int j = 0; j < sz; j++) {
				Vertex *v = f->vertices[j];

				worldPos.v[0] = v->pos.x;
				worldPos.v[1] = v->pos.y;
				worldPos.v[2] = v->pos.z;
				worldNorm.v[0] = f->normals[j].x;
				worldNorm.v[1] = f->normals[j].y;
				worldNorm.v[2] = f->normals[j].z;

				// Gouraud ////////////
				worldNorm.vecNormalize();
				lightDir.v[0] = lg->pos[0];
				lightDir.v[1] = lg->pos[1];
				lightDir.v[2] = lg->pos[2];
				lightDir.vecSubVec(worldPos);
				lightDir.vecNormalize();

				float diff = MAX(worldNorm.vec3DotReal(lightDir), 0);
				diffuseLightColorV.v[0] = currentMaterial->diffuseLightColor[0];
				diffuseLightColorV.v[1] = currentMaterial->diffuseLightColor[1];
				diffuseLightColorV.v[2] = currentMaterial->diffuseLightColor[2];
				diffuseLightColorV.vecMulScalar(diff);
				ambientDiffuseSpecular.v[0] = currentMaterial->ambient[0];
				ambientDiffuseSpecular.v[1] = currentMaterial->ambient[1];
				ambientDiffuseSpecular.v[2] = currentMaterial->ambient[2];
				ambientDiffuseSpecular.vecAddVec(diffuseLightColorV);

				//			// Specular ///////////
				viewDir.v[0] = from.v[0];
				viewDir.v[1] = from.v[1];
				viewDir.v[2] = from.v[2];
				viewDir.vecSubVec(worldPos);
				viewDir.vecNormalize();
				negLightDir.v[0] = -lightDir.v[0];
				negLightDir.v[1] = -lightDir.v[1];
				negLightDir.v[2] = -lightDir.v[2];
				reflectDir.clear();
				reflect(reflectDir, negLightDir, worldNorm);
				float spec = (float)std::pow((float)MAX(viewDir.vec3DotReal(reflectDir), 0.0f), currentMaterial->shininess /*(float)shininess*/);
				specular.v[0] = currentMaterial->specularLightColor[0];
				specular.v[1] = currentMaterial->specularLightColor[1];
				specular.v[2] = currentMaterial->specularLightColor[2];
				specular.vecMulScalar(spec * specularStrength);
				specular.vecMulVec(lightColor);

				// Melt lights with object color ///////////
				ambientDiffuseSpecular.vecAddVec(specular);
				ambientDiffuseSpecular.vecMulVec(objectColor);
				c.v[0] = ambientDiffuseSpecular.v[0];
				c.v[1] = ambientDiffuseSpecular.v[1];
				c.v[2] = ambientDiffuseSpecular.v[2];

				// Projection /////////
				cameraPos.v[0] = worldPos.v[0];
				cameraPos.v[1] = worldPos.v[1];
				cameraPos.v[2] = worldPos.v[2];
				cameraPos.v[3] = 1.0f;

				cameraPos.vec4MulMat4(view);
				cameraPos.vec4MulMat4(perspective);
				cameraPos.vecMulScalar(1 / cameraPos.v[3]);

				// Feed polygons /////////
				// 1.333 to compensate resolution ratio
				vertices[j].x = (float)MIN(w - 1, (cameraPos.v[0] + 1) * 0.5 * w);
				vertices[j].y = (float)MIN(h - 1, (cameraPos.v[1] * 1/*.333*/ + 1) * 0.5 * h);
				vertices[j].z = (float)MIN(65535, (cameraPos.v[2] + 1) * 0.5 * 65536);

				vertices[j].c.r = (int)MIN(255, MAX(0, c.v[0]));
				vertices[j].c.g = (int)MIN(255, MAX(0, c.v[1]));
				vertices[j].c.b = (int)MIN(255, MAX(0, c.v[2]));

				std::string log = "[" + std::to_string(i) + "," + std::to_string(j) + "]    " + std::to_string(vertices[j].c.r) + "," + std::to_string(vertices[j].c.g) + "," + std::to_string(vertices[j].c.b);
				Chronometer::write(log);
				
			}

			//		//  drawing here //
			for (int i = 0; i < sz; i++) {
				ScanPolygon::traceGouraud(pixels, vertices, 3, zBuffer);
			}

			delete[] vertices;
		}
	}
	Chronometer::write("----------");
}
