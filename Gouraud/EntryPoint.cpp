#include "Application.h"
#include "Chronometer.h"
#include "ScanPolygon.h"
#include "RMatrix.h"
#include "RVector.h"
#include <windows.h>
#include <d3d9.h>

static unsigned int counter;
static GVertex v[3], z[4];
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


	vCenter.x = (v[0].x + v[1].x + v[2].x) / 3;
	vCenter.y = (v[0].y + v[1].y + v[2].y) / 3;
}

void drawTriangle(char* pixels, int w, int h, int pitch, bool tictac)
{
	// clear the frame
	DWORD* row = (DWORD*)pixels;
	for (int i = 0; i < w * h; i++) {
		*(row + i) = 0x00323232;
	}

	// draw the polygon
	ScanPolygon::traceGouraud(pixels, v, 3);

	// next frame transformation
	translationToOrigin.translation2(-vCenter.x, -vCenter.y);
	translationBack.translation2(vCenter.x, vCenter.y);

	rotation.rotation2((REAL)TO_RADIAN(1));
	coords.v[0] = v[0].x;
	coords.v[1] = v[0].y;
	coords.v[2] = 1;
	coords.vecMulMat(translationToOrigin);
	coords.vecMulMat(rotation);
	coords.vecMulMat(translationBack);
	v[0].x = coords.v[0];
	v[0].y = coords.v[1];

	coords.v[0] = v[1].x;
	coords.v[1] = v[1].y;
	coords.v[2] = 1;
	coords.vecMulMat(translationToOrigin);
	coords.vecMulMat(rotation);
	coords.vecMulMat(translationBack);
	v[1].x = coords.v[0];
	v[1].y = coords.v[1];

	coords.v[0] = v[2].x;
	coords.v[1] = v[2].y;
	coords.v[2] = 1;
	coords.vecMulMat(translationToOrigin);
	coords.vecMulMat(rotation);
	coords.vecMulMat(translationBack);
	v[2].x = coords.v[0];
	v[2].y = coords.v[1];


	counter++;
	if (counter == INT_MAX) {
		counter = 0;
	}
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
	ScanPolygon::traceGouraud(pixels, z, 4);

	// next frame transformation
	//translationToOrigin.translation2(-vCenter.x, -vCenter.y);
	//translationBack.translation2(vCenter.x, vCenter.y);

	//rotation.rotation2((REAL)TO_RADIAN(1));
	//coords.v[0] = v[0].x;
	//coords.v[1] = v[0].y;
	//coords.v[2] = 1;
	//coords.vecMulMat(translationToOrigin);
	//coords.vecMulMat(rotation);
	//coords.vecMulMat(translationBack);
	//v[0].x = coords.v[0];
	//v[0].y = coords.v[1];

	//coords.v[0] = v[1].x;
	//coords.v[1] = v[1].y;
	//coords.v[2] = 1;
	//coords.vecMulMat(translationToOrigin);
	//coords.vecMulMat(rotation);
	//coords.vecMulMat(translationBack);
	//v[1].x = coords.v[0];
	//v[1].y = coords.v[1];

	//coords.v[0] = v[2].x;
	//coords.v[1] = v[2].y;
	//coords.v[2] = 1;
	//coords.vecMulMat(translationToOrigin);
	//coords.vecMulMat(rotation);
	//coords.vecMulMat(translationBack);
	//v[2].x = coords.v[0];
	//v[2].y = coords.v[1];

	//coords.v[0] = v[3].x;
	//coords.v[1] = v[3].y;
	//coords.v[2] = 1;
	//coords.vecMulMat(translationToOrigin);
	//coords.vecMulMat(rotation);
	//coords.vecMulMat(translationBack);
	//v[3].x = coords.v[0];
	//v[3].y = coords.v[1];



	counter++;
	if (counter == INT_MAX) {
		counter = 0;
	}
}


int WINAPI WinMain(HINSTANCE _In_ hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	init();
	Chronometer::toFile = 2;
	Application a;
	a.waitRR(false);
	a.createWindow(hInstance, "Minimal DX window", 640, 480);
	a.messagesLoop(drawQuad);
	Chronometer::meanLap("Mean");
	return 0;
}