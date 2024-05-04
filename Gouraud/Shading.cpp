#include "Shading.h"


// default base material for WF objects
float ambient[] = { 0.2f, 0.2f, 0.2f };				// default ambient for exported WF objects
float specularStrength = 1.0f;

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

void Shading::findGouraudColorAtVertex(
	RVector &worldNorm, RVector &lightDir, Light *lg, RVector &worldPos, RVector &diffuseLightColorV, 
	Material *currentMaterial, RVector &ambientDiffuseSpecular, RVector &viewDir, RVector &from, 
	RVector &negLightDir, RVector &reflectDir, RVector &specular, RVector &lightColor, RVector &objectColor, 
	RVector &c, RVector &cameraPos, RMatrix &view, RVector &projectionPos, RMatrix &perspective, GVertex *vertices, 
	int j, int w, int h)
{
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

	vertices[j].c.r = (int)MIN(255, MAX(0, c.v[0]));
	vertices[j].c.g = (int)MIN(255, MAX(0, c.v[1]));
	vertices[j].c.b = (int)MIN(255, MAX(0, c.v[2]));
}

void Shading::findPhongColorAtPixel(RVector &worldNorm, RVector &lightDir, Light *lg, RVector &worldPos, RVector &diffuseLightColorV, Material *currentMaterial, RVector &ambientDiffuseSpecular, RVector &viewDir, RVector &from, RVector &negLightDir, RVector &reflectDir, RVector &specular, RVector &lightColor, RVector &objectColor, RVector &c, RVector &cameraPos, RMatrix &view, RVector &projectionPos, RMatrix &perspective, int x, int y, int w, int h)
{
}


void rotationX(REAL angle, RMatrix &mat)
{
	float cs = std::cos(angle);
	float sn = std::sin(angle);
	mat.clear();
	mat.v[0] = 1;
	mat.v[5] = cs;
	mat.v[6] = -sn;
	mat.v[9] = sn;
	mat.v[10] = cs;
	mat.v[15] = 1;

}
