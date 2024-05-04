#pragma once
#include "RVector.h"
#include "Globals.h"
#include "Obj.h"

class Shading
{
public:
	static void findGouraudColorAtVertex(
		RVector &worldNorm, RVector &lightDir, Light *lg, RVector &worldPos, RVector &diffuseLightColorV,
		Material *currentMaterial, RVector &ambientDiffuseSpecular, RVector &viewDir, RVector &from,
		RVector &negLightDir, RVector &reflectDir, RVector &specular, RVector &lightColor, RVector &objectColor,
		RVector &c, RVector &cameraPos, RMatrix &view, RVector &projectionPos,
		RMatrix &perspective, GVertex *vertices, int j, int w, int h);

	static void findPhongColorAtPixel(RVector &worldNorm, RVector &lightDir, Light *lg, RVector &worldPos, RVector &diffuseLightColorV,
		Material *currentMaterial, RVector &ambientDiffuseSpecular, RVector &viewDir, RVector &from,
		RVector &negLightDir, RVector &reflectDir, RVector &specular, RVector &lightColor, RVector &objectColor,
		RVector &c, RVector &cameraPos, RMatrix &view, RVector &projectionPos,
		RMatrix &perspective);
};

