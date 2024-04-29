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


class TestGouraudPolygon
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
