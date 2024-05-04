#include "3d.h"
#include "Chronometer.h"
#include "ScanPolygon.h"
#include "Shading.h"
#include <float.h>
#include <fstream>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>


#define MILLEVINGTQUATRE 1024
#define NORMALS_OPPOSITE 0
#define VEC4MULMAT4 vec4MulMat4Mmx

static std::chrono::steady_clock::time_point beginTime;
static std::chrono::steady_clock::time_point endTime;
static double countLap = 0;
static double sumLap = 0;


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


void render(RENDER_TYPE rt, char *pixels, Light *lg, Obj &o, RMatrix &view, RMatrix &perspective, RVector &from, int w, int h, float *zBuffer)
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
	RVector projectionPos(VEC4);
	Material *currentMaterial;

	lightColor.v[0] = lg->c.r;
	lightColor.v[1] = lg->c.g;
	lightColor.v[2] = lg->c.b;

	objectColor.v[0] = o.color.r;
	objectColor.v[1] = o.color.g;
	objectColor.v[2] = o.color.b;

	std::map<std::string, Object *>::iterator it;
	static int countFace = 0;
	for (it = o.objects.begin(); it != o.objects.end(); it++) {

		currentMaterial = &it->second->material;

		for (size_t i = 0; i < it->second->faces.size(); i++) {
			Face *f = it->second->faces[i];
			int sz = (int)f->vertices.size();
			GVertex *vertices = new GVertex[sz];

			countFace++;

			for (int j = 0; j < sz; j++) {
				Vertex *v = f->vertices[j];

				worldPos.v[0] = v->pos.x;
				worldPos.v[1] = v->pos.y;
				worldPos.v[2] = v->pos.z;
				worldNorm.v[0] = f->normals[j].x;
				worldNorm.v[1] = f->normals[j].y;
				worldNorm.v[2] = f->normals[j].z;

				if (rt == GOURAUD) {
					Shading::findGouraudColorAtVertex(
						worldNorm, lightDir, lg, worldPos, diffuseLightColorV, currentMaterial,
						ambientDiffuseSpecular, viewDir, from, negLightDir, reflectDir, specular,
						lightColor, objectColor, c, cameraPos, view, projectionPos, perspective, vertices, j, w, h);
				}
				else if (rt == PHONG) {
					// TODO only projection to get projected vertices
					// Projection /////////
					cameraPos.v[0] = worldPos.v[0];
					cameraPos.v[1] = worldPos.v[1];
					cameraPos.v[2] = worldPos.v[2];
					cameraPos.v[3] = 1.0f;
					cameraPos.vec4MulMat4(view);
					projectionPos.v[0] = cameraPos.v[0];
					projectionPos.v[1] = cameraPos.v[1];
					projectionPos.v[2] = cameraPos.v[2];
					projectionPos.v[3] = 1.0f;
					projectionPos.vec4MulMat4(perspective);
					projectionPos.vecMulScalar(1 / projectionPos.v[3]);

					// Feed polygons /////////
					// 1.333 to compensate resolution ratio
					vertices[j].x = (float)MIN(w - 1, (projectionPos.v[0] + 1) * 0.5 * w);
					vertices[j].y = (float)MIN(h - 1, (projectionPos.v[1] /** 1.333*/ + 1) * 0.5 * h);
					vertices[j].z = (float)cameraPos.v[2];

				}
				else {

				}

				std::string log = "[" +
					std::to_string(i) + "," +
					std::to_string(j) + "]    " +
					std::to_string(vertices[j].c.r) + "," +
					std::to_string(vertices[j].c.g) + "," +
					std::to_string(vertices[j].c.b);
				Chronometer::write(log);
			}

			// drawing polygon here //
			if (rt == GOURAUD) {
				ScanPolygon::traceGouraud(pixels, vertices, sz, w, h, zBuffer);
			}
			else if (rt == PHONG) {
				ScanPolygon::tracePhong(pixels, vertices, sz, w, h, worldNorm, lightDir, lg, worldPos, diffuseLightColorV, currentMaterial,
					ambientDiffuseSpecular, viewDir, from, negLightDir, reflectDir, specular, lightColor, objectColor, c, cameraPos, view,
					projectionPos, perspective, zBuffer);
			}
			else {
			}

			delete[] vertices;
		}
	}
	Chronometer::write("----------" + std::to_string(countFace));
}

void rotationY(REAL angle, RMatrix &mat)
{
	float cs = std::cos(angle);
	float sn = std::sin(angle);
	mat.clear();
	mat.v[0] = cs;
	mat.v[2] = sn;
	mat.v[5] = 1;
	mat.v[8] = -sn;
	mat.v[10] = cs;
	mat.v[15] = 1;
}

void rotationZ(REAL angle, RMatrix &mat)
{
	float cs = std::cos(angle);
	float sn = std::sin(angle);
	mat.clear();
	mat.v[0] = cs;
	mat.v[1] = -sn;
	mat.v[4] = sn;
	mat.v[5] = cs;
	mat.v[10] = 1;
	mat.v[15] = 1;
}

void translateObject(Obj &o, RMatrix &m)
{
	RVector vx(VTYPE::VEC3);
	for (size_t i = 0; i < o.vertices.size(); i++) {
		Vertex *vertex = o.vertices[i];
		vx.v[0] = vertex->pos.x;
		vx.v[1] = vertex->pos.y;
		vx.v[2] = vertex->pos.z;
		vx.vec4MulMat4(m);
		vertex->pos.x = vx.v[0];
		vertex->pos.y = vx.v[1];
		vertex->pos.z = vx.v[2];
	}

	std::map<std::string, Object *>::iterator it;
	for (it = o.objects.begin(); it != o.objects.end(); it++) {
		for (size_t i = 0; i < it->second->faces.size(); i++) {
			Face *face = it->second->faces[i];
			for (size_t j = 0; j < face->normals.size(); j++) {
				vx.v[0] = face->normals[j].x;
				vx.v[1] = face->normals[j].y;
				vx.v[2] = face->normals[j].z;
				vx.vec4MulMat4(m);
				face->normals[j].x = vx.v[0];
				face->normals[j].y = vx.v[1];
				face->normals[j].z = vx.v[2];
			}
		}
	}
}

void transformObject(Obj &o, RMatrix &m)
{
	RVector vx(VEC4);
	for (size_t i = 0; i < o.vertices.size(); i++) {
		Vertex *vertex = o.vertices[i];
		vx.v[0] = vertex->pos.x;
		vx.v[1] = vertex->pos.y;
		vx.v[2] = vertex->pos.z;
		vx.vec4MulMat4(m);
		vertex->pos.x = vx.v[0];
		vertex->pos.y = vx.v[1];
		vertex->pos.z = vx.v[2];
	}

	std::map<std::string, Object *>::iterator it;
	for (it = o.objects.begin(); it != o.objects.end(); it++) {
		for (size_t i = 0; i < it->second->faces.size(); i++) {
			Face *face = it->second->faces[i];
			for (size_t j = 0; j < face->normals.size(); j++) {
				vx.v[0] = face->normals[j].x;
				vx.v[1] = face->normals[j].y;
				vx.v[2] = face->normals[j].z;
				vx.vec4MulMat4(m);
				face->normals[j].x = vx.v[0];
				face->normals[j].y = vx.v[1];
				face->normals[j].z = vx.v[2];
			}
		}
	}
}

