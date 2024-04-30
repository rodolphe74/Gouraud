#define NOMINMAX

#include "ScanPolygon.h"
#include "Chronometer.h"
#include <iostream>
#include <d3d9.h>
#include <algorithm>
#include <string>

int W = 640;
int H = 640;

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


void ScanPolygon::drawPolyBilinear(char *pixels, int x1, int y1, int x2, int y2, int x3, int y3, int c1, int c2, int c3)
{
	DWORD *row = (DWORD *)pixels;

	if (y1 > y2) {
		int xt = x1; x1 = x2; x2 = xt;
		int yt = y1; y1 = y2; y2 = yt;
		int ct = c1; c1 = c2; c2 = ct;
	}
	if (y1 > y3) {
		int xt = x1; x1 = x3; x3 = xt;
		int yt = y1; y1 = y3; y3 = yt;
		int ct = c1; c1 = c3; c3 = ct;
	}
	if (y2 > y3) {
		int xt = x2; x2 = x3; x3 = xt;
		int yt = y2; y2 = y3; y3 = yt;
		int ct = c2; c2 = c3; c3 = ct;
	}

	int r1 = c1 >> 16, g1 = (c1 >> 8) & 0xFF, b1 = c1 & 0xFF;
	int r2 = c2 >> 16, g2 = (c2 >> 8) & 0xFF, b2 = c2 & 0xFF;
	int r3 = c3 >> 16, g3 = (c3 >> 8) & 0xFF, b3 = c3 & 0xFF;

	double ax12 = (double)(x2 - x1) / (y2 - y1);
	double ax13 = (double)(x3 - x1) / (y3 - y1);
	double ax23 = (double)(x3 - x2) / (y3 - y2);

	double x12 = x1;
	double x13 = x1;
	double x23 = x2;

	double ar12 = (double)(r2 - r1) / (y2 - y1);
	double ag12 = (double)(g2 - g1) / (y2 - y1);
	double ab12 = (double)(b2 - b1) / (y2 - y1);
	double ar13 = (double)(r3 - r1) / (y3 - y1);
	double ag13 = (double)(g3 - g1) / (y3 - y1);
	double ab13 = (double)(b3 - b1) / (y3 - y1);
	double ar23 = (double)(r3 - r2) / (y3 - y2);
	double ag23 = (double)(g3 - g2) / (y3 - y2);
	double ab23 = (double)(b3 - b2) / (y3 - y2);

	double r12 = r1;
	double g12 = g1;
	double b12 = b1;
	double r13 = r1;
	double g13 = g1;
	double b13 = b1;
	double r23 = r2;
	double g23 = g2;
	double b23 = b2;

	int sy = y1, ey = y3;

	int eval = (y3 - y1) * (x2 - x1) - (x3 - x1) * (y2 - y1);

	for (int y = sy; y <= ey; y++) {

		double sx, ex;
		double sr, sg, sb;
		double er, eg, eb;
		if (eval < 0) {
			if (y <= y2 && y1 != y2) {
				sx = x12; sr = r12; sg = g12; sb = b12;
			}
			else {
				sx = x23; sr = r23; sg = g23; sb = b23;
			}
			ex = x13; er = r13; eg = g13; eb = b13;
		}
		else {
			sx = x13; sr = r13; sg = g13; sb = b13;
			if (y <= y2 && y1 != y2) {
				ex = x12; er = r12; eg = g12; eb = b12;
			}
			else {
				ex = x23; er = r23; eg = g23; eb = b23;
			}
		}

		if (y <= y2 && y1 != y2) {
			x12 += ax12;
			r12 += ar12;
			g12 += ag12;
			b12 += ab12;
		}
		else {
			x23 += ax23;
			r23 += ar23;
			g23 += ag23;
			b23 += ab23;
		}
		x13 += ax13;
		r13 += ar13;
		g13 += ag13;
		b13 += ab13;

		if (sx > ex) {
			ex = sx = 640;
		}

		double ar = (er - sr) / (ex - sx);
		double ag = (eg - sg) / (ex - sx);
		double ab = (eb - sb) / (ex - sx);

		double pr = sr, pg = sg, pb = sb;

		for (int x = (int)sx; x < (int)ex; x++) {



			//uint8_t *p = img + ((640 * y) + x) * 3;
			//p[0] = (uint8_t)pb;
			//p[1] = (uint8_t)pg;
			//p[2] = (uint8_t)pr;

			unsigned char ucr = (unsigned char)pr;
			unsigned char ucg = (unsigned char)pg;
			unsigned char ucb = (unsigned char)pb;
			int offset = (W * y + x);
			*(row + (offset)) = D3DCOLOR_XRGB(ucr, ucg, ucb);


			pr += ar;
			pg += ag;
			pb += ab;
		}
	}
}

void ScanPolygon::gouraudShading(GVertex p[3], char *pixels, int pLength, float *zBuffer)
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
			if (pnpoly(p, pLength, gv) && y >= 0 && y < H && x >= 0 && x < W) {

				


				float dv1 = std::sqrt((p[0].x - x) * (p[0].x - x) + (p[0].y - y) * (p[0].y - y));
				float dv2 = std::sqrt((p[1].x - x) * (p[1].x - x) + (p[1].y - y) * (p[1].y - y));
				float dv3 = std::sqrt((p[2].x - x) * (p[2].x - x) + (p[2].y - y) * (p[2].y - y));
				float w1 = 1 / dv1;
				float w2 = 1 / dv2;	// + > W
				float w3 = 1 / dv3;
				float r = (p[0].c.r * w1 + p[1].c.r * w2 + p[2].c.r * w3) / (w1 + w2 + w3);
				float g = (p[0].c.g * w1 + p[1].c.g * w2 + p[2].c.g * w3) / (w1 + w2 + w3);
				float b = (p[0].c.b * w1 + p[1].c.b * w2 + p[2].c.b * w3) / (w1 + w2 + w3);
				float z = (p[0].z * w1 + p[1].z * w2 + p[2].z * w3) / (w1 + w2 + w3);

				//std::string s = std::to_string(x) + ", " + std::to_string(y) + "  =  " + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b);
				//Chronometer::write(s);


				// Gouraud on each triangle
				// https://codeplea.com/triangular-interpolation
				//float wv0 = (p[1].y - p[2].y) * (x - p[2].x) + (p[2].x - p[1].x) * (y - p[2].y);
				//wv0 /= (p[1].y - p[2].y) * (p[0].x - p[2].x) + (p[2].x - p[1].x) * (p[0].y - p[2].y);

				//float wv1 = (p[2].y - p[0].y) * (x - p[2].x) + (p[0].x - p[2].x) * (y - p[2].y);
				//wv1 /= (p[1].y - p[2].y) * (p[0].x - p[2].x) + (p[2].x - p[1].x) * (p[0].y - p[2].y);

				//float wv2 = 1 - wv0 - wv1;

				//float r = p[0].c.r * wv0 + p[1].c.r * wv1 + p[2].c.r * wv2;
				//float g = p[0].c.g * wv0 + p[1].c.g * wv1 + p[2].c.b * wv2;
				//float b = p[0].c.b * wv0 + p[1].c.b * wv1 + p[2].c.b * wv2;

				//float z = p[0].z * wv0 + p[1].z * wv1 + p[2].z * wv2;

				unsigned char ucr = (unsigned char)r;
				unsigned char ucg = (unsigned char)g;
				unsigned char ucb = (unsigned char)b;

				int offset = (W * y + x);
				if (zBuffer && z > zBuffer[x + y * W]) {
					*(row + (offset)) = D3DCOLOR_XRGB(ucr, ucg, ucb);
					zBuffer[x + W * y] = z;
				}
				else if (!zBuffer) {
					*(row + (offset)) = D3DCOLOR_XRGB(ucr, ucg, ucb);
				}
			}
		}
	}
	delete[] frame;
}



//void ScanPolygon::gouraudShading(GVertex p[3], char* pixels, int pLength, float *zBuffer)
//{
//	DWORD* row = (DWORD*)pixels;
//	GVertex* frame = new GVertex[4];
//	frame = getFrame(p, pLength, frame);
//
//	float y1 = frame[0].y;
//	float y2 = y1;
//	float x1 = frame[0].x;
//	float x2 = x1;
//	for (int i = 1; i < 4; i++) {
//		if (frame[i].y != y1) {
//			y2 = frame[i].y;
//			break;
//		}
//	}
//	for (int i = 1; i < 4; i++) {
//		if (frame[i].x != x1) {
//			x2 = frame[i].x;
//			break;
//		}
//	}
//	float t = y1;
//	y1 = std::min(y1, y2);
//	y2 = std::max(t, y2);
//	t = x1;
//	x1 = std::min(x1, x2);
//	x2 = std::max(t, x2);
//
//
//	int iy1 = (int)std::round(y1);
//	int iy2 = (int)std::round(y2);
//	int ix1 = (int)std::round(x1);
//	int ix2 = (int)std::round(x2);
//	for (int y = iy1; y <= iy2; y++) {
//		for (int x = ix1; x <= ix2; x++) {
//			GVertex gv = { (float)x, (float)y };
//			if (pnpoly(p, pLength, gv) && y >= 0 && y < H && x >= 0 && x < W) {
//
//				// TODO triangles decompositions
//
//				// Gouraud on each triangle
//				// https://codeplea.com/triangular-interpolation
//				float wv0 = (p[1].y - p[2].y) * (x - p[2].x) + (p[2].x - p[1].x) * (y - p[2].y);
//				wv0 /= (p[1].y - p[2].y) * (p[0].x - p[2].x) + (p[2].x - p[1].x) * (p[0].y - p[2].y);
//
//				float wv1 = (p[2].y - p[0].y) * (x - p[2].x) + (p[0].x - p[2].x) * (y - p[2].y);
//				wv1 /= (p[1].y - p[2].y) * (p[0].x - p[2].x) + (p[2].x - p[1].x) * (p[0].y - p[2].y);
//
//				float wv2 = 1 - wv0 - wv1;
//
//				float r = p[0].c.r * wv0 + p[1].c.r * wv1 + p[2].c.r * wv2;
//				float g = p[0].c.g * wv0 + p[1].c.g * wv1 + p[2].c.b * wv2;
//				float b = p[0].c.b * wv0 + p[1].c.b * wv1 + p[2].c.b * wv2;
//
//				float z = p[0].z * wv0 + p[1].z * wv1 + p[2].z * wv2;
//
//				unsigned char ucr = (unsigned char)r;
//				unsigned char ucg = (unsigned char)g;
//				unsigned char ucb = (unsigned char)b;
//
//				int offset = (W * y + x);
//				if (zBuffer && z > zBuffer[x + y * W]) {
//					*(row + (offset)) = D3DCOLOR_XRGB(ucr, ucg, ucb);
//					zBuffer[x + W * y] = z;
//				}
//				else if (!zBuffer) {
//					*(row + (offset)) = D3DCOLOR_XRGB(ucr, ucg, ucb);
//				}
//			}
//		}
//	}
//	delete[] frame;
//}

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

void ScanPolygon::traceGouraud(char* pixels, GVertex p[], size_t pLength, float *zBuffer)
{
	if (pLength > 3) {

		//sortVertices(p, pLength);
		Triangle *triangles = new Triangle[pLength - 1];

		for (int i = 0; i < pLength - 2; i++) {
			getTriangle(triangles[i], p[0], p[i + 1], p[i + 2]);
			gouraudShading(triangles[i], pixels, 3, zBuffer);
		}

		delete[] triangles;
		
	}
	else {
		// and gouraud on each
		gouraudShading(p, pixels, 3, zBuffer);
	}
}

void ScanPolygon::_traceGouraud(char *pixels, GVertex p[], size_t pLength, float *zBuffer)
{
	uint32_t c0, c1, c2;
	c0 = (uint32_t)0 | (uint32_t)p[0].c.r << 16 | (uint32_t)p[0].c.g << 8 | (uint32_t)p[0].c.b;
	c1 = (uint32_t)0 | (uint32_t)p[1].c.r << 16 | (uint32_t)p[1].c.g << 8 | (uint32_t)p[1].c.b;
	c2 = (uint32_t)0 | (uint32_t)p[2].c.r << 16 | (uint32_t)p[2].c.g << 8 | (uint32_t)p[2].c.b;
	ScanPolygon::drawPolyBilinear(pixels, p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y, c0, c1, c2);
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
