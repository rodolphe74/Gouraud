#include "TestFlatPolygon.h"

unsigned int TestFlatPolygon::counter;
GVertex TestFlatPolygon::poly3[3], TestFlatPolygon::poly6[6];
GVertex TestFlatPolygon::vCenter;


void TestFlatPolygon::initPolygons()
{
	poly3[0].x = 50; poly3[0].y = 250; poly3[0].z = 6; poly3[0].c = { WHITE };
	poly3[0].n.x = .1f; poly3[0].n.y = 0.0f; poly3[0].n.z = 1.0f;

	poly3[1].x = 400; poly3[1].y = 600; poly3[1].z = 4; poly3[1].c = { BLACK };
	poly3[1].n.x = -.1f; poly3[1].n.y = 0.0f; poly3[1].n.z = 1.0f;

	poly3[2].x = 250; poly3[2].y = 300; poly3[2].z = 8; poly3[2].c = { GRAY };
	poly3[2].n.x = .2f; poly3[2].n.y = 0.0f; poly3[2].n.z = 1.0f;



	// each vertice must be followed by the real follower vertex.

	poly6[5].x = 400; poly6[5].y = 000; poly6[5].z = 4; poly6[5].c = CYAN;
	poly6[5].n.x = -.1f; poly6[5].n.y = 0.0f; poly6[5].n.z = 1.0f;

	poly6[4].x = 200; poly6[4].y = 300; poly6[4].z = 6; poly6[4].c = RED;
	poly6[4].n.x = .1f; poly6[4].n.y = 0.0f; poly6[4].n.z = 1.0f;

	poly6[3].x = 300; poly6[3].y = 500; poly6[3].z = 8; poly6[3].c = BLUE;
	poly6[3].n.x = .2f; poly6[3].n.y = 0.0f; poly6[3].n.z = 1.0f;

	poly6[2].x = 600; poly6[2].y = 200; poly6[2].z = 8; poly6[2].c = YELLOW;
	poly6[2].n.x = .2f; poly6[2].n.y = 0.0f; poly6[2].n.z = 1.0f;

	poly6[1].x = 500; poly6[1].y = 400; poly6[1].z = 8; poly6[1].c = MAGENTA;
	poly6[1].n.x = .2f; poly6[1].n.y = 0.0f; poly6[1].n.z = 1.0f;

	poly6[0].x = 500; poly6[0].y = 100; poly6[0].z = 8; poly6[0].c = CYAN;
	poly6[0].n.x = .2f; poly6[0].n.y = 0.0f; poly6[0].n.z = 1.0f;



	ScanPolygon::sortVertices(poly6, 6);
	vCenter = ScanPolygon::findCentroid(poly6, 6);

	ScanPolygon::debugPolygon(poly6, 6);
}

void TestFlatPolygon::rotateVertex(GVertex &v)
{
	coords.v[0] = v.x;
	coords.v[1] = v.y;
	coords.v[2] = 1;
	coords.vecMulMat(translationToOrigin);
	coords.vecMulMat(rotation);
	coords.vecMulMat(translationBack);
	v.x = coords.v[0];
	v.y = coords.v[1];
}

void TestFlatPolygon::drawPolygons(char *pixels, int w, int h, int pitch, bool tictac)
{
	DWORD *row = (DWORD *)pixels;
	for (int i = 0; i < w * h; i++) {
		*(row + i) = 0x00323232;
	}

	// draw the polygon
	ScanPolygon::trace(pixels, poly3, 3, BLUE);
	ScanPolygon::trace(pixels, poly6, 6, GREEN);

	// next frame transformation
	translationToOrigin.translation2(-vCenter.x, -vCenter.y);
	translationBack.translation2(vCenter.x, vCenter.y);
	rotation.rotation2((REAL)TO_RADIAN(1));
	rotateVertex(poly6[0]);
	rotateVertex(poly6[1]);
	rotateVertex(poly6[2]);
	rotateVertex(poly6[3]);
	rotateVertex(poly6[4]);
	rotateVertex(poly6[5]);

}

void TestFlatPolygon::close()
{
}
