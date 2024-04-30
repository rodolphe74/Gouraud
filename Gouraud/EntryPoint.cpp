#include "TestFlatPolygon.h"
#include "TestGouraudPolygon.h"
#include "TestGouraudRendering.h"
#include "Tests.h"
#include <chrono>
#include <d3d9.h>
#include <thread>
#include <windows.h>


int WINAPI WinMain(HINSTANCE _In_ hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	Chronometer::toFile = 2;

	TestFunction t1 = { TestFlatPolygon::initPolygons, TestFlatPolygon::drawPolygons, TestFlatPolygon::close };
	TestFunction t2 = { TestGouraudPolygon::initPolygons, TestGouraudPolygon::drawPolygons, TestGouraudPolygon::close };
	TestFunction t3 = { TestGouraudRendering::initObject, TestGouraudRendering::renderObject, TestGouraudRendering::close };

	Tests t;
	
	t.addTests(t1);
	t.addTests(t2);
	t.addTests(t3);

	t.runTests(hInstance);

	return 0;
}