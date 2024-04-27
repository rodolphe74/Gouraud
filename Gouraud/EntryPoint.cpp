#include "Application.h"
#include "Chronometer.h"
#include "ScanPolygon.h"
#include "RMatrix.h"
#include "RVector.h"
#include <windows.h>
#include <d3d9.h>

static unsigned int counter;
static GVertex v[3], z[4], r[6];
static GVertex vCenter;

RMatrix rotation(MAT33);
RMatrix translationToOrigin(MAT33);
RMatrix translationBack(MAT33);
RVector coords(VEC3);

void init() {
	v[0].x = 100; v[0].y = 100; v[0].z = 6; v[0].c = { 255, 255, 255 };
	v[0].n.x = .1f; v[0].n.y = 0.0f; v[0].n.z = 1.0f;

	v[1].x = 450; v[1].y = 450; v[1].z = 4; v[1].c = { 255, 255, 0 };
	v[1].n.x = -.1f; v[1].n.y = 0.0f; v[1].n.z = 1.0f;

	v[2].x = 300; v[2].y = 150; v[2].z = 8; v[2].c = { 0, 255, 255 };
	v[2].n.x = .2f; v[2].n.y = 0.0f; v[2].n.z = 1.0f;



	z[0].x = 100; z[0].y = 100; z[0].z = 6; z[0].c = { 255, 255, 255 };
	z[0].n.x = .1f; z[0].n.y = 0.0f; z[0].n.z = 1.0f;

	z[1].x = 300; z[1].y = 300; z[1].z = 4; z[1].c = { 255, 255, 0 };
	z[1].n.x = -.1f; z[1].n.y = 0.0f; z[1].n.z = 1.0f;

	z[2].x = 500; z[2].y = 400; z[2].z = 8; z[2].c = { 0, 255, 255 };
	z[2].n.x = .2f; z[2].n.y = 0.0f; z[2].n.z = 1.0f;

	z[3].x = 400; z[3].y = 100; z[3].z = 8; z[3].c = { 0, 0, 255 };
	z[3].n.x = .2f; z[3].n.y = 0.0f; z[3].n.z = 1.0f;



	Color YELLOW = { 255, 255, 255 };
	Color RED = { 255, 0, 0 };
	Color BLUE = { 255, 0, 0 };
	Color GREEN = { 0, 0 , 255 };
	Color MAGENTA = { 255, 0, 255 };
	Color CYAN = { 0, 255, 255 };

	// each vertice must be followed by the real follower vertex.

	r[5].x = 400; r[5].y = 000; r[5].z = 4; r[5].c = CYAN;
	r[5].n.x = -.1f; r[5].n.y = 0.0f; r[5].n.z = 1.0f;

	r[4].x = 200; r[4].y = 300; r[4].z = 6; r[4].c = RED;
	r[4].n.x = .1f; r[4].n.y = 0.0f; r[4].n.z = 1.0f;

	r[3].x = 300; r[3].y = 500; r[3].z = 8; r[3].c = BLUE;
	r[3].n.x = .2f; r[3].n.y = 0.0f; r[3].n.z = 1.0f;

	r[2].x = 600; r[2].y = 200; r[2].z = 8; r[2].c = YELLOW;
	r[2].n.x = .2f; r[2].n.y = 0.0f; r[2].n.z = 1.0f;

	r[1].x = 500; r[1].y = 400; r[1].z = 8; r[1].c = MAGENTA;
	r[1].n.x = .2f; r[1].n.y = 0.0f; r[1].n.z = 1.0f;

	r[0].x = 500; r[0].y = 100; r[0].z = 8; r[0].c = CYAN;
	r[0].n.x = .2f; r[0].n.y = 0.0f; r[0].n.z = 1.0f;

	

	ScanPolygon::sortVertices(r, 6);
	vCenter = ScanPolygon::findCentroid(r, 6);

	ScanPolygon::debugPolygon(r, 6);

}



void rotateVertex(GVertex &v) {
	coords.v[0] = v.x;
	coords.v[1] = v.y;
	coords.v[2] = 1;
	coords.vecMulMat(translationToOrigin);
	coords.vecMulMat(rotation);
	coords.vecMulMat(translationBack);
	v.x = coords.v[0];
	v.y = coords.v[1];
}

void drawQuad(char* pixels, int w, int h, int pitch, bool tictac)
{
	// clear the frame
	DWORD* row = (DWORD*)pixels;
	for (int i = 0; i < w * h; i++) {
		*(row + i) = 0x00323232;
	}

	// draw the polygon
	//ScanPolygon::traceGouraud(pixels, v, 3);
	//ScanPolygon::traceGouraud(pixels, z, 4);
	ScanPolygon::traceGouraud(pixels, r, 6);

	// next frame transformation
	translationToOrigin.translation2(-vCenter.x, -vCenter.y);
	translationBack.translation2(vCenter.x, vCenter.y);

	rotation.rotation2((REAL)TO_RADIAN(1));

	rotateVertex(r[0]);
	rotateVertex(r[1]);
	rotateVertex(r[2]);
	rotateVertex(r[3]);
	rotateVertex(r[4]);
	rotateVertex(r[5]);

	counter++;
	if (counter == INT_MAX) {
		counter = 0;
	}
}


int WINAPI WinMain(HINSTANCE _In_ hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	Chronometer::toFile = 2;
	init();
	Application a;
	a.waitRR(false);
	a.createWindow(hInstance, "Minimal DX window", 640, 640);
	a.messagesLoop(drawQuad);
	Chronometer::meanLap("Mean");
	return 0;
}