#pragma once
#include "Globals.h"
#include "Obj.h"
#include "RVector.h"
#include <cmath>

#define M_PI 3.14159265358979323846
#define TO_DEGREE(x)((x * (180.0/3.141592653589793238463)))
#define TO_DEGREE_INT(x)((int) std::round(x * (180.0/3.141592653589793238463)))


//typedef struct _gnormal {
//	float x, y, z, w;
//} GNormal;
//
//typedef struct _gvertex {
//	float x, y, z, w;
//	Color c;
//	GNormal n;
//} GVertex;
//
//typedef struct _edge {
//	float x1, y1;
//	float x2, y2;
//	float z1, z2;
//	Color c1, c2;
//	GNormal n1, n2;
//} Edge;

typedef GVertex Triangle[3];
typedef GVertex *TrianglePtr[3];

//int verticesSorter(void* ctxvar, const void* _a, const void* _b) {
//	GVertex a = *(GVertex*)_a;
//	GVertex b = *(GVertex*)_b;
//	GVertex c = *(GVertex*)ctxvar;
//	double a1 = (TO_DEGREE_INT(std::atan2(a.x - c.x, a.y - c.y)) + 360) % 360;
//	double a2 = (TO_DEGREE_INT(std::atan2(b.x - c.x, b.y - c.y)) + 360) % 360;
//	return (int)(a1 - a2);
//}

class ScanPolygon
{
private:
	static bool pnpoly(GVertex p[], size_t pLength, GVertex &t);
	static GVertex *getFrame(GVertex polygon[], size_t polygonLength, GVertex *square);
	static void gouraudShading(GVertex p[3], char *pixels, int pLength, int w, int h, float *zBuffer = nullptr);
	static void phongShading(GVertex p[3], char *pixels, int pLength, int w, int h,
		RVector &worldNorm, RVector &lightDir, Light *lg, RVector &worldPos,
		RVector &diffuseLightColorV, Material *currentMaterial, RVector &ambientDiffuseSpecular, RVector &viewDir, RVector &from, RVector &negLightDir,
		RVector &reflectDir, RVector &specular, RVector &lightColor, RVector &objectColor, RVector &c, RVector &cameraPos, RMatrix &view,
		RVector &projectionPos, RMatrix &perspective, float *zBuffer = nullptr);
	static int verticesSorter(void *ctxvar, const void *_a, const void *_b);
	static int verticesBUSorter(void *ctxvar, const void *_a, const void *_b);
	static void getTriangle(Triangle t, GVertex &p1, GVertex &p2, GVertex &p3);


public:
	static GVertex findCentroid(GVertex p[], size_t pLength);
	static void sortVertices(GVertex p[], size_t pLength);
	static void sortVerticesUpBottom(GVertex p[], size_t pLength);
	static void trace(char *pixels, GVertex p[], size_t pLength, Color c, int w, int h);
	static void traceGouraud(char *pixels, GVertex p[], size_t pLength, int w, int h, float *zBuffer = nullptr);
	static void debugPolygon(GVertex p[], size_t pLength);
	static void getVector3FromGVertexNormal(const GVertex &v, RVector &r);

};

