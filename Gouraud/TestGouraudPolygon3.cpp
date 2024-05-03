#include "TestGouraudPolygon3.h"
#include "TestRoot.h"

unsigned int TestGouraudPolygon3::counter;
GVertex TestGouraudPolygon3::poly3[3];
GVertex TestGouraudPolygon3::vCenter;

void TestGouraudPolygon3::initPolygons()
{
	poly3[0].x = 175; poly3[0].y = 50; poly3[0].z = 6; poly3[0].c = { BLUE };
	poly3[0].n.x = .1f; poly3[0].n.y = 0.0f; poly3[0].n.z = 1.0f;

	poly3[1].x = 50; poly3[1].y = 300; poly3[1].z = 6; poly3[1].c = { WHITE };
	poly3[1].n.x = -.1f; poly3[1].n.y = 0.0f; poly3[1].n.z = 1.0f;

	poly3[2].x = 300; poly3[2].y = 300; poly3[2].z = 6; poly3[2].c = { RED };
	poly3[2].n.x = .2f; poly3[2].n.y = 0.0f; poly3[2].n.z = 1.0f;



	//poly3[0].x = 50; poly3[0].y = 300; poly3[0].z = 6; poly3[0].c = { BLUE };
	//poly3[0].n.x = -.1f; poly3[0].n.y = 0.0f; poly3[0].n.z = 1.0f;

	//poly3[1].x = 300; poly3[1].y = 300; poly3[1].z = 6; poly3[1].c = { WHITE };
	//poly3[1].n.x = .2f; poly3[1].n.y = 0.0f; poly3[1].n.z = 1.0f;

	//poly3[2].x = 175; poly3[2].y = 50; poly3[2].z = 6; poly3[2].c = { RED };
	//poly3[2].n.x = .1f; poly3[2].n.y = 0.0f; poly3[2].n.z = 1.0f;



	// ScanPolygon::sortVertices(poly3, 3);
	// ScanPolygon::sortVerticesUpBottom(poly3, 3);
	vCenter = ScanPolygon::findCentroid(poly3, 6);

}

void TestGouraudPolygon3::rotateVertex(GVertex &v)
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

void TestGouraudPolygon3::drawPolygons(char *pixels, int w, int h, int pitch, bool tictac)
{
	DWORD *row = (DWORD *)pixels;
	for (int i = 0; i < w * h; i++) {
		*(row + i) = 0x00323232;
	}

	// draw the polygon
	ScanPolygon::traceGouraud(pixels, poly3, 3, w, h);

	// next frame transformation
	translationToOrigin.translation2(-vCenter.x, -vCenter.y);
	translationBack.translation2(vCenter.x, vCenter.y);
	rotation.rotation2((REAL)TO_RADIAN(1));
	//rotateVertex(poly3[0]);
	//rotateVertex(poly3[1]);
	//rotateVertex(poly3[2]);

}

void TestGouraudPolygon3::close()
{
}
