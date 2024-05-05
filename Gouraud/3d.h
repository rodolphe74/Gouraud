#pragma once

#include <string>
#include <fstream>
#include <chrono>
#include "globals.h"
#include "RMatrix.h"
#include "RVector.h"
#include "obj.h"
#include "ScanPolygon.h"


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

enum RENDER_TYPE { GOURAUD = 0, FLAT };

void lookAt(RVector &position, RVector &target, RVector &up, RMatrix &mat);
void perspective(float fov_y, float aspect, float n, float f, RMatrix &mat);
Light *createLight(float x, float y, float z, Color c, float i);
void render(RENDER_TYPE rt, char *pixels, Light *l, Obj &o, RMatrix &view, RMatrix &perspective, RVector &from, int w, int h, float *zBuffer);
void rotationX(REAL angle, RMatrix &mat);
void rotationY(REAL angle, RMatrix &mat);
void rotationZ(REAL angle, RMatrix &mat);
void translateObject(Obj &o, RMatrix &v);
void transformObject(Obj &o, RMatrix &m);

