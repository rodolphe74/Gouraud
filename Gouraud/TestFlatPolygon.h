#pragma once
#include "Tests.h"
#include "Globals.h"
#include "Application.h"
#include "Chronometer.h"
#include "ScanPolygon.h"
#include "RMatrix.h"
#include "RVector.h"
#include <windows.h>
#include <d3d9.h>


static RMatrix rotation(MAT33);
static RMatrix translationToOrigin(MAT33);
static RMatrix translationBack(MAT33);
static RVector coords(VEC3);


class TestFlatPolygon
{
public:
	static unsigned int counter;
	static GVertex poly3[3], poly4[4], poly6[6];
	static GVertex vCenter;



	//INIT_FUNC_PTR initFunction;
	// DRAW_CALL_BACK_PTR drawFunction;

	static void rotateVertex(GVertex &v);
	static void initPolygons();
	static void drawPolygons(char *pixels, int w, int h, int pitch, bool tictac);
};
