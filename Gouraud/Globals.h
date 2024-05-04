#pragma once

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define M_PI 3.14159265358979323846
#define TO_RADIAN(x) (x * (M_PI / 180))
#define VEC3_SIZE 3
#define VEC4_SIZE 4

typedef struct _color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} Color;



// Used in ScanPolygon
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



static Color WHITE = { 255, 255, 255 };
static Color BLACK = { 0, 0, 0};
static Color GRAY = { 128, 128, 128 };
static Color BLUE = { 0, 0, 255 };
static Color GREEN = { 0, 255, 0 };
static Color YELLOW = { 255, 255, 0 };
static Color CYAN = { 0, 255, 255 };
static Color RED = { 255, 0, 0 };
static Color MAGENTA = { 255, 0, 255 };

