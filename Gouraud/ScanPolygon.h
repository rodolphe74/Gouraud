#pragma once

typedef struct _color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} Color;

typedef struct _gnormal {
	float x, y, z, w;
} GNormal;

typedef struct _gvertex {
	float x, y, z, w;
	Color c;
	GNormal n;
} GVertex;

typedef struct _edge {
	float x1, y1;
	float x2, y2;
	float z1, z2;
	Color c1, c2;
	GNormal n1, n2;
} Edge;

class ScanPolygon
{
private:
	static bool pnpoly(GVertex p[], size_t pLength, GVertex& t);
    static Edge* returnEdges(GVertex vertices[], size_t verticesLength, Edge edges[]);
	static Edge findEdge(int x, int y, Edge edges[], size_t edgesLength);

	static GVertex* getFrame(GVertex polygon[], size_t polygonLength, GVertex* square);
public:
	static void trace(char *pixels, GVertex p[], size_t pLength, Color c);
	static void traceGouraud(char* pixels, GVertex p[], size_t pLength);
};

