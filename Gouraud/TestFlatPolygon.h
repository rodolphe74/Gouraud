#pragma once
#include "Application.h"
#include "Chronometer.h"
#include "Globals.h"
#include "RMatrix.h"
#include "RVector.h"
#include "ScanPolygon.h"
#include "Tests.h"
#include <d3d9.h>
#include <windows.h>

static RMatrix rotation(MAT33);
static RMatrix translationToOrigin(MAT33);
static RMatrix translationBack(MAT33);
static RVector coords(VEC3);

class TestFlatPolygon
{
public:
	static unsigned int counter;
	static GVertex poly3[3], poly6[6];
	static GVertex vCenter;

	//INIT_FUNC_PTR initFunction;
	// DRAW_CALL_BACK_PTR drawFunction;

	static void rotateVertex(GVertex &v);
	static void initPolygons();
	static void drawPolygons(char *pixels, int w, int h, int pitch, bool tictac);
	static void close();
};
