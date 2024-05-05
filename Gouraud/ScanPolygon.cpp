#define NOMINMAX

#include "Chronometer.h"
#include "ScanPolygon.h"
#include "Shading.h"
#include <algorithm>
#include <d3d9.h>
#include <iostream>
#include <string>


bool ScanPolygon::pnpoly(GVertex p[], size_t pLength, GVertex &t)
{
	// https://wrf.ecse.rpi.edu/Research/Short_Notes/pnpoly.html
	size_t i, j = 0;
	bool c = false;
	for (i = 0, j = pLength - 1; i < pLength; j = i++) {
		if (((p[i].y > t.y) != (p[j].y > t.y))
			&& (t.x < (p[j].x - p[i].x) * (t.y - p[i].y) / (p[j].y - p[i].y) + p[i].x))
			c = !c;
	}
	return c;
}

GVertex *ScanPolygon::getFrame(GVertex polygon[], size_t polygonLength, GVertex *frame)
{
	float minx = std::numeric_limits<float>::max();
	float miny = std::numeric_limits<float>::max();
	float maxx = std::numeric_limits<float>::min();
	float maxy = std::numeric_limits<float>::min();

	for (unsigned int i = 0; i < polygonLength; i++) {
		if (polygon[i].x < minx)
			minx = polygon[i].x;
		if (polygon[i].x > maxx)
			maxx = polygon[i].x;
		if (polygon[i].y < miny)
			miny = polygon[i].y;
		if (polygon[i].y > maxy)
			maxy = polygon[i].y;
	}
	frame[0] = { minx, miny, 0, 0,  {0, 0, 0}, {0, 0, 0, 0} };
	frame[1] = { maxx, miny, 0, 0,  {0, 0, 0}, {0, 0, 0, 0} };;
	frame[2] = { maxx, maxy, 0, 0,  {0, 0, 0}, {0, 0, 0, 0} };
	frame[3] = { minx, maxy, 0, 0,  {0, 0, 0}, {0, 0, 0, 0} };;

	return frame;
}

void ScanPolygon::gouraudShading(GVertex p[3], char *pixels, int pLength, int w, int h, float *zBuffer)
{
	DWORD *row = (DWORD *)pixels;
	GVertex *frame = new GVertex[4];
	frame = getFrame(p, pLength, frame);

	float y1 = frame[0].y;
	float y2 = y1;
	float x1 = frame[0].x;
	float x2 = x1;
	for (int i = 1; i < 4; i++) {
		if (frame[i].y != y1) {
			y2 = frame[i].y;
			break;
		}
	}
	for (int i = 1; i < 4; i++) {
		if (frame[i].x != x1) {
			x2 = frame[i].x;
			break;
		}
	}
	float t = y1;
	y1 = std::min(y1, y2);
	y2 = std::max(t, y2);
	t = x1;
	x1 = std::min(x1, x2);
	x2 = std::max(t, x2);


	int iy1 = (int)std::round(y1);
	int iy2 = (int)std::round(y2);
	int ix1 = (int)std::round(x1);
	int ix2 = (int)std::round(x2);
	for (int y = iy1; y <= iy2; y++) {
		for (int x = ix1; x <= ix2; x++) {
			GVertex gv = { (float)x, (float)y };
			if (pnpoly(p, pLength, gv) && y >= 0 && y < h && x >= 0 && x < w) {

				// Gouraud on each triangle


				// Distance
				// https://codeplea.com/triangular-interpolation
				//float dv1 = std::sqrt((p[0].x - x) * (p[0].x - x) + (p[0].y - y) * (p[0].y - y));
				//float dv2 = std::sqrt((p[1].x - x) * (p[1].x - x) + (p[1].y - y) * (p[1].y - y));
				//float dv3 = std::sqrt((p[2].x - x) * (p[2].x - x) + (p[2].y - y) * (p[2].y - y));
				//float w1 = 1 / dv1;
				//float w2 = 1 / dv2;
				//float w3 = 1 / dv3;
				//float r = (p[0].c.r * w1 + p[1].c.r * w2 + p[2].c.r * w3) / (w1 + w2 + w3);
				//float g = (p[0].c.g * w1 + p[1].c.g * w2 + p[2].c.g * w3) / (w1 + w2 + w3);
				//float b = (p[0].c.b * w1 + p[1].c.b * w2 + p[2].c.b * w3) / (w1 + w2 + w3);
				//float z = (p[0].z * w1 + p[1].z * w2 + p[2].z * w3) / (w1 + w2 + w3);


				// Barycentre
				// https://codeplea.com/triangular-interpolation
				float wv0 = (p[1].y - p[2].y) * (x - p[2].x) + (p[2].x - p[1].x) * (y - p[2].y);
				wv0 /= (p[1].y - p[2].y) * (p[0].x - p[2].x) + (p[2].x - p[1].x) * (p[0].y - p[2].y);
				float wv1 = (p[2].y - p[0].y) * (x - p[2].x) + (p[0].x - p[2].x) * (y - p[2].y);
				wv1 /= (p[1].y - p[2].y) * (p[0].x - p[2].x) + (p[2].x - p[1].x) * (p[0].y - p[2].y);
				float wv2 = 1 - wv0 - wv1;
				float r = p[0].c.r * wv0 + p[1].c.r * wv1 + p[2].c.r * wv2;
				float g = p[0].c.g * wv0 + p[1].c.g * wv1 + p[2].c.g * wv2;
				float b = p[0].c.b * wv0 + p[1].c.b * wv1 + p[2].c.b * wv2;
				float z = p[0].z * wv0 + p[1].z * wv1 + p[2].z * wv2;

				RVector v0(VEC3);
				RVector v1(VEC3);
				RVector v2(VEC3);

				unsigned char ucr = (unsigned char)r;
				unsigned char ucg = (unsigned char)g;
				unsigned char ucb = (unsigned char)b;

				int offset = (w * y + x);
				if (zBuffer && z > zBuffer[x + y * w]) {
					*(row + (offset)) = D3DCOLOR_XRGB(ucr, ucg, ucb);
					zBuffer[offset] = z;
				}
				else if (!zBuffer) {
					*(row + (offset)) = D3DCOLOR_XRGB(ucr, ucg, ucb);
				}
			}
		}
	}
	delete[] frame;
}

void ScanPolygon::phongShading(
	GVertex p[3], char *pixels, int pLength, int w, int h, RVector &worldNorm, RVector &lightDir, Light *lg, RVector &worldPos,
	RVector &diffuseLightColorV, Material *currentMaterial, RVector &ambientDiffuseSpecular, RVector &viewDir, RVector &from, RVector &negLightDir,
	RVector &reflectDir, RVector &specular, RVector &lightColor, RVector &objectColor, RVector &c, RVector &cameraPos, RMatrix &view, RVector &projectionPos,
	RMatrix &perspective, float *zBuffer)
{

	DWORD *row = (DWORD *)pixels;
	GVertex *frame = new GVertex[4];
	frame = getFrame(p, pLength, frame);

	float y1 = frame[0].y;
	float y2 = y1;
	float x1 = frame[0].x;
	float x2 = x1;
	for (int i = 1; i < 4; i++) {
		if (frame[i].y != y1) {
			y2 = frame[i].y;
			break;
		}
	}
	for (int i = 1; i < 4; i++) {
		if (frame[i].x != x1) {
			x2 = frame[i].x;
			break;
		}
	}
	float t = y1;
	y1 = std::min(y1, y2);
	y2 = std::max(t, y2);
	t = x1;
	x1 = std::min(x1, x2);
	x2 = std::max(t, x2);


	int iy1 = (int)std::round(y1);
	int iy2 = (int)std::round(y2);
	int ix1 = (int)std::round(x1);
	int ix2 = (int)std::round(x2);
	for (int y = iy1; y <= iy2; y++) {
		for (int x = ix1; x <= ix2; x++) {
			GVertex gv = { (float)x, (float)y };
			if (pnpoly(p, pLength, gv) && y >= 0 && y < h && x >= 0 && x < w) {

				// Phong on each triangle


				// Barycentre
				// https://codeplea.com/triangular-interpolation
				float wv0 = (p[1].y - p[2].y) * (x - p[2].x) + (p[2].x - p[1].x) * (y - p[2].y);
				wv0 /= (p[1].y - p[2].y) * (p[0].x - p[2].x) + (p[2].x - p[1].x) * (p[0].y - p[2].y);
				float wv1 = (p[2].y - p[0].y) * (x - p[2].x) + (p[0].x - p[2].x) * (y - p[2].y);
				wv1 /= (p[1].y - p[2].y) * (p[0].x - p[2].x) + (p[2].x - p[1].x) * (p[0].y - p[2].y);
				float wv2 = 1 - wv0 - wv1;
				float z = p[0].z * wv0 + p[1].z * wv1 + p[2].z * wv2;


				RVector v0(VEC3);
				RVector v1(VEC3);
				RVector v2(VEC3);
				RVector normInterp(VEC3);
				getVector3FromGVertexNormal(p[0], v0);
				getVector3FromGVertexNormal(p[1], v1);
				getVector3FromGVertexNormal(p[2], v2);
				v0.vecNormalize();
				v1.vecNormalize();
				v2.vecNormalize();
				v0.vecMulScalar(wv0);
				v1.vecMulScalar(wv1);
				v2.vecMulScalar(wv2);
				normInterp.vecAddVec(v0);
				normInterp.vecAddVec(v1);
				normInterp.vecAddVec(v2);


				// find pixel color regarding normal interpolation
				// x & y & n (= sum)
				// pixel projection & lightning here

				// color
				Shading::findPhongColorAtPixel(normInterp, lightDir, lg, worldPos, diffuseLightColorV, currentMaterial, ambientDiffuseSpecular, viewDir,
					from, negLightDir, reflectDir, specular, lightColor, objectColor, c, cameraPos, view, projectionPos, perspective);

				unsigned char ucr = (unsigned char)MIN(255, MAX(0, c.v[0]));
				unsigned char ucg = (unsigned char)MIN(255, MAX(0, c.v[1]));
				unsigned char ucb = (unsigned char)MIN(255, MAX(0, c.v[1]));

				// draw pixel
				int offset = (w * (int)y + (int)x);
				if (zBuffer && z > zBuffer[x + y * w]) {
					*(row + (offset)) = D3DCOLOR_XRGB(ucr, ucg, ucb);
					zBuffer[offset] = z;
				}
				else if (!zBuffer) {
					*(row + (offset)) = D3DCOLOR_XRGB(ucr, ucg, ucb);
				}
			}
		}
	}
	delete[] frame;
}

GVertex ScanPolygon::findCentroid(GVertex p[], size_t pLength)
{
	float x = 0;
	float y = 0;
	for (int i = 0; i < pLength; i++) {
		x += p[i].x;
		y += p[i].y;
	}
	GVertex center = { 0, 0 };
	center.x = x / pLength;
	center.y = y / pLength;
	return center;
}

int ScanPolygon::verticesSorter(void *ctxvar, const void *_a, const void *_b)
{
	GVertex a = *(GVertex *)_a;
	GVertex b = *(GVertex *)_b;
	GVertex c = *(GVertex *)ctxvar;
	double a1 = (TO_DEGREE_INT(std::atan2(a.x - c.x, a.y - c.y)) + 360) % 360;
	double a2 = (TO_DEGREE_INT(std::atan2(b.x - c.x, b.y - c.y)) + 360) % 360;
	return (int)(a1 - a2);	// (counter clockwise  : a2 - a1)  (for clockwise : a1 - a2)

}

int ScanPolygon::verticesBUSorter(void *ctxvar, const void *_a, const void *_b)
{
	GVertex a = *(GVertex *)_a;
	GVertex b = *(GVertex *)_b;
	return (int)(a.y - b.y);

}

void ScanPolygon::getTriangle(Triangle t, GVertex &p1, GVertex &p2, GVertex &p3)
{
	t[0] = p1;
	t[1] = p2;
	t[2] = p3;
}

void ScanPolygon::sortVertices(GVertex p[], size_t pLength)
{
	GVertex center = findCentroid(p, pLength);
	qsort_s(p, pLength, sizeof(GVertex), verticesSorter, &center);
}

void ScanPolygon::sortVerticesUpBottom(GVertex p[], size_t pLength)
{
	qsort_s(p, pLength, sizeof(GVertex), verticesBUSorter, nullptr);
	int a = 0;
}

void ScanPolygon::trace(char *pixels, GVertex p[], size_t pLength, Color c, int w, int h)
{
	DWORD *row = (DWORD *)pixels;
	GVertex *frame = new GVertex[4];
	frame = getFrame(p, pLength, frame);

	float y1 = frame[0].y;
	float y2 = y1;
	float x1 = frame[0].x;
	float x2 = x1;
	for (int i = 1; i < 4; i++) {
		if (frame[i].y != y1) {
			y2 = frame[i].y;
			break;
		}
	}
	for (int i = 1; i < 4; i++) {
		if (frame[i].x != x1) {
			x2 = frame[i].x;
			break;
		}
	}
	float t = y1;
	y1 = std::min(y1, y2);
	y2 = std::max(t, y2);
	t = x1;
	x1 = std::min(x1, x2);
	x2 = std::max(t, x2);


	int iy1 = (int)std::round(y1);
	int iy2 = (int)std::round(y2);
	int ix1 = (int)std::round(x1);
	int ix2 = (int)std::round(x2);
	for (int y = iy1; y <= iy2; y++) {
		for (int x = ix1; x <= ix2; x++) {
			GVertex gv = { (float)x, (float)y };
			if (pnpoly(p, pLength, gv) && y >= 0 && y < h && x >= 0 && x < w) {
				*(row + (w * y + x)) = D3DCOLOR_XRGB(c.r, c.g, c.b);
			}
		}
	}

	delete[] frame;
}

void ScanPolygon::traceGouraud(char *pixels, GVertex p[], size_t pLength, int w, int h, float *zBuffer)
{
	if (pLength > 3) {

		//sortVertices(p, pLength);
		Triangle *triangles = new Triangle[pLength - 1];

		for (int i = 0; i < pLength - 2; i++) {
			getTriangle(triangles[i], p[0], p[i + 1], p[i + 2]);
			gouraudShading(triangles[i], pixels, 3, w, h, zBuffer);
		}

		delete[] triangles;

	}
	else {
		// and gouraud on each
		gouraudShading(p, pixels, 3, w, h, zBuffer);
	}
}

void ScanPolygon::tracePhong(
	char *pixels, GVertex p[], size_t pLength, int w, int h, RVector &worldNorm, RVector &lightDir, Light *lg, RVector &worldPos,
	RVector &diffuseLightColorV, Material *currentMaterial, RVector &ambientDiffuseSpecular, RVector &viewDir, RVector &from, RVector &negLightDir,
	RVector &reflectDir, RVector &specular, RVector &lightColor, RVector &objectColor, RVector &c, RVector &cameraPos, RMatrix &view,
	RVector &projectionPos, RMatrix &perspective, float *zBuffer)
{
	if (pLength > 3) {
		// TODO
	}
	else {
		phongShading(p, pixels, pLength, w, h, worldNorm, lightDir, lg, worldPos, diffuseLightColorV, currentMaterial, ambientDiffuseSpecular,
			viewDir, from, negLightDir, reflectDir, specular, lightColor, objectColor, c, cameraPos, view, projectionPos, perspective, zBuffer);
	}
}


void ScanPolygon::debugPolygon(GVertex p[], size_t pLength)
{
	Chronometer::write("Polygon:");
	for (int i = 0; i < pLength; i++) {
		std::string str = "  " + std::to_string(p[i].x) + "," + std::to_string(p[i].y);
		Chronometer::write(str);
	}
	Chronometer::write("\n");

}

void ScanPolygon::getVector3FromGVertexNormal(const GVertex &v, RVector &r)
{
	r.v[0] = v.n.x;
	r.v[1] = v.n.y;
	r.v[2] = v.n.z;
}
