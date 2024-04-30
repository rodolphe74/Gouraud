#pragma once

#include <string>
#include <fstream>
#include <chrono>
#include "globals.h"
#include "RMatrix.h"
#include "RVector.h"
#include "obj.h"


#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define M_PI 3.14159265358979323846
#define TO_RADIAN(x) (x * (M_PI / 180))
#define VEC3_SIZE 3
#define VEC4_SIZE 4

namespace Fx {
	typedef struct {
		float x, y;	// (x,y) coordinates
		float z;
		float argb;		// color of each vertex
	} Vertex;
}

void lookAt(RVector &position, RVector &target, RVector &up, RMatrix &mat);
void perspective(float fov_y, float aspect, float n, float f, RMatrix &mat);
Light *createLight(float x, float y, float z, Color c, float i);
void render(char *pixels, Light *l, Obj &o, RMatrix &view, RMatrix &perspective, RVector &from, int w, int h, float *zBuffer);

