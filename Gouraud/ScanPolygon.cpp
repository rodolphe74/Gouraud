#define NOMINMAX

#include "ScanPolygon.h"
#include "Polypartition.h"
#include "Chronometer.h"
#include <iostream>
#include <d3d9.h>
#include <algorithm>

int W = 640;
int H = 480;

bool ScanPolygon::pnpoly(GVertex p[], size_t pLength, GVertex& t)
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

Edge* ScanPolygon::returnEdges(GVertex vertices[], size_t verticesLength, Edge edges[])
{
	int count = 0;
	for (size_t i = 0; i < verticesLength; i++) {
		Edge e = {
			vertices[i].x, vertices[i].y,
			vertices[(i + 1) % verticesLength].x, vertices[(i + 1) % verticesLength].y,
			vertices[i].z, vertices[(i + 1) % verticesLength].z,
			vertices[i].c, vertices[(i + 1) % verticesLength].c
		};

		edges[count] = e;
		count++;
	}
	return edges;
}

Edge ScanPolygon::findEdge(int x, int y, Edge edges[], size_t edgesLength)
{
	float dist1, dist2, dist3, diff, diffMin = std::numeric_limits<float>::max();
	int idx = -1;
	for (int i = 0; i < edgesLength; i++) {

		if (edges[i].x2 - edges[i].x1 != 0) {
			// remove horizontal lines
			float slope = ((float)edges[i].y2 - edges[i].y1) / ((float)edges[i].x2 - edges[i].x1);
			if (slope == 0.0f) {
				continue;
			}
		}

		//dist1 = (float)std::sqrt(std::pow(x - edges[i].x1, 2) + std::pow(y - edges[i].y1, 2));
		//dist2 = (float)std::sqrt(std::pow(x - edges[i].x2, 2) + std::pow(y - edges[i].y2, 2));
		//dist3 = (float)std::sqrt(std::pow(edges[i].x2 - edges[i].x1, 2) + std::pow(edges[i].y2 - edges[i].y1, 2));

		dist1 = (float)std::sqrt((x - edges[i].x1) * (x - edges[i].x1) + (y - edges[i].y1) * (y - edges[i].y1));
		dist2 = (float)std::sqrt((x - edges[i].x2) * (x - edges[i].x2) + (y - edges[i].y2) * (y - edges[i].y2));
		dist3 = (float)std::sqrt((edges[i].x2 - edges[i].x1) * (edges[i].x2 - edges[i].x1) + (edges[i].y2 - edges[i].y1) * (edges[i].y2 - edges[i].y1));


		diff = abs((dist1 + dist2) - dist3);
		if (diff < diffMin) {
			diffMin = diff;
			idx = i;
		}
	}
	return edges[idx == -1 ? 0 : idx];
}


GVertex* ScanPolygon::getFrame(GVertex polygon[], size_t polygonLength, GVertex* frame)
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

void ScanPolygon::gouraudShading(GVertex p[3], char* pixels, int pLength)
{
	DWORD* row = (DWORD*)pixels;
	GVertex* frame = new GVertex[4];
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
			if (pnpoly(p, pLength, gv) && y >= 0 && y < H && x >= 0 && x < W) {

				// TODO triangles decompositions

				// Gouraud on each triangle
				// https://codeplea.com/triangular-interpolation
				float wv0 = (p[1].y - p[2].y) * (x - p[2].x) + (p[2].x - p[1].x) * (y - p[2].y);
				wv0 /= (p[1].y - p[2].y) * (p[0].x - p[2].x) + (p[2].x - p[1].x) * (p[0].y - p[2].y);

				float wv1 = (p[2].y - p[0].y) * (x - p[2].x) + (p[0].x - p[2].x) * (y - p[2].y);
				wv1 /= (p[1].y - p[2].y) * (p[0].x - p[2].x) + (p[2].x - p[1].x) * (p[0].y - p[2].y);

				float wv2 = 1 - wv0 - wv1;

				float r = p[0].c.r * wv0 + p[1].c.r * wv1 + p[2].c.r * wv2;
				float g = p[0].c.g * wv0 + p[1].c.g * wv1 + p[2].c.b * wv2;
				float b = p[0].c.b * wv0 + p[1].c.b * wv1 + p[2].c.b * wv2;

				unsigned char ucr = (unsigned char)r;
				unsigned char ucg = (unsigned char)g;
				unsigned char ucb = (unsigned char)b;

				*(row + (W * y + x)) = D3DCOLOR_XRGB(ucr, ucg, ucb);
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

int ScanPolygon::verticesSorter(void* ctxvar, const void* _a, const void* _b)
{
	GVertex a = *(GVertex*)_a;
	GVertex b = *(GVertex*)_b;
	GVertex c = *(GVertex*)ctxvar;
	double a1 = (TO_DEGREE_INT(std::atan2(a.x - c.x, a.y - c.y)) + 360) % 360;
	double a2 = (TO_DEGREE_INT(std::atan2(b.x - c.x, b.y - c.y)) + 360) % 360;
	return (int)(a1 - a2);

}

void ScanPolygon::sortVertices(GVertex p[], size_t pLength)
{
	GVertex center = findCentroid(p, pLength);
	qsort_s(p, pLength, sizeof(GVertex), verticesSorter, &center);

	int a = 0;

}


void ScanPolygon::trace(char* pixels, GVertex p[], size_t pLength, Color c)
{
	DWORD* row = (DWORD*)pixels;
	GVertex* frame = new GVertex[4];
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
			if (pnpoly(p, pLength, gv) && y >= 0 && y < H && x >= 0 && x < W) {
				*(row + (W * y + x)) = D3DCOLOR_XRGB(c.r, c.g, c.b);
			}
		}
	}


	delete[] frame;
}

void ScanPolygon::traceGouraud(char* pixels, GVertex p[], size_t pLength)
{
	Color c = {255, 255, 0};
	trace(pixels, p, pLength, c);

	// triangles decomposition
	GVertex q[3];
	if (pLength > 3) {

		sortVertices(p, pLength);


		//TPPLPartition pp;
		//std::list<TPPLPoly> onePoly;
		//std::list<TPPLPoly> triangles;
		//onePoly.clear();

		//TPPLPoly poly;
		//poly.Init((long)pLength);
		//poly.SetHole(false);
		//for (int i = 0; i < pLength; i++) {
		//	poly[i].x = p[i].x;
		//	poly[i].y = p[i].y;
		//}
		//onePoly.push_back(poly);
		//// pp.Triangulate_EC(&onePoly, &triangles);
		//pp.Triangulate_OPT(&poly, &triangles);
		//for (auto iter = triangles.begin(); iter != triangles.end(); iter++) {
		//	std::string gnp =  "gnp:" + std::to_string(iter->GetNumPoints());
		//	Chronometer::write(gnp);
		//	for (int i = 0; i < /*iter->GetNumPoints()*/3; i++) {
		//		std::string pt = "pt:" + std::to_string(iter->GetPoint(i).x) + "," + std::to_string(iter->GetPoint(i).y);
		//		Chronometer::write(pt);
		//		q[i].x = (float)iter->GetPoint(i).x;
		//		q[i].y = (float)iter->GetPoint(i).y;
		//	}
		//	gouraudShading(q, pixels, 3);
		//}
	}
	else {
		// and gouraud on each
		gouraudShading(p, pixels, 3);
	}
}
