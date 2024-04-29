#pragma once

#include "RMatrix.h"
#include "RVector.h"
#include "ScanPolygon.h"

static RMatrix rotation(MAT33);
static RMatrix translationToOrigin(MAT33);
static RMatrix translationBack(MAT33);
static RVector coords(VEC3);

class TestRoot
{
public:
	static unsigned int counter;
	static GVertex poly3[3], poly4[4], poly6[6];
	static GVertex vCenter;

	static void rotateVertex(GVertex &v);
	static void initPolygons();
};

