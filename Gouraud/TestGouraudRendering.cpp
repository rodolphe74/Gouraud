#include "TestGouraudRendering.h"
#include "TestRoot.h"

Obj *TestGouraudRendering::o;
Light *TestGouraudRendering::lg;
RVector *TestGouraudRendering::_fromPosition_;
RVector *TestGouraudRendering::_toTarget_;
RVector *TestGouraudRendering::_up_;
RMatrix *TestGouraudRendering::_view_;
RMatrix *TestGouraudRendering::_perspective_;
RMatrix *TestGouraudRendering::_rotationY_;
RMatrix *TestGouraudRendering::_rotationZ_;
RMatrix *TestGouraudRendering::_demi_tour_;
RVector *TestGouraudRendering::_translationY_;
float *TestGouraudRendering::zBuffer;


void TestGouraudRendering::initObject()
{
	o = new Obj();
	//o->loadObjects("./obj/loupiotte.obj");
	//o->applyMaterial(o->objects["Lamp_Mesh"], &BRONZE);
	//o->applyMaterial(o->objects["Lamp_chain_Mesh.001"], &TURQUOISE);

	o->loadObjects("./obj/cubetris.obj");
	o->applyMaterial(o->objects["Cube"], &TURQUOISE);

	//o->loadObjects("./obj/lpbull.obj");
	// o->loadMaterials("./obj/lpbull.mtl");	// TODO:check pointers
	// o->applyMaterials();
	//o->applyMaterial(o->objects["polySurface11"], &YELLOWG);

	Color c = { 255, 255, 255 };
	lg = createLight(0.0f, 0.0f, 8.0f, c, 255.0f);

	_fromPosition_ = new RVector({ 0.0f, 0.0f, 5.0f }, VTYPE::VEC3);
	//_fromPosition_ = new RVector({ 0.0f, -1.3f, 3.0f }, VTYPE::VEC3);
	_toTarget_ = new RVector({ 0.0f, 0.0f, 0.0f }, VTYPE::VEC3);
	_up_ = new RVector({ 0.0f, 1.0f, 0.0f }, VTYPE::VEC3);
	_view_ = new RMatrix(MTYPE::MAT44);
	_perspective_ = new RMatrix(MAT44);
	_rotationY_ = new RMatrix(MAT44);
	_rotationZ_ = new RMatrix(MAT44);
	_demi_tour_ = new RMatrix(MAT44);
	_translationY_ = new RVector(VEC4);

	lookAt(*_fromPosition_, *_toTarget_, *_up_, *_view_);

	perspective((float)TO_RADIAN(90.0f), 1.0f, 0.1f, 100.0f, *_perspective_);
	rotationY((float)TO_RADIAN(1.0f/10), *_rotationY_);
	rotationZ((float)TO_RADIAN(0.5f/10), *_rotationZ_);

	//rotationX((float)TO_RADIAN(180.0f), *_demi_tour_);
	//transformObject(*o, *_demi_tour_);
	zBuffer = new float[640 * 640];
}



void TestGouraudRendering::renderObject(char *pixels, int w, int h, int pitch, bool tictac)
{
	int32_t *row = (int32_t *)pixels;
	for (int i = 0; i < w * h; i++) {
		*(row + i) = 0x00000000;
	}
	// z-buffer
	for (int i = 0; i < 640 * 640; i++)
		zBuffer[i] = -FLT_MAX;

	// Transformations
	transformObject(*o, *_rotationY_);
	transformObject(*o, *_rotationZ_);

	render(GOURAUD, pixels, lg, *o, *_view_, *_perspective_, *_fromPosition_, w, h, zBuffer);
}

void TestGouraudRendering::close()
{
	delete o;
	delete _fromPosition_;
	delete _toTarget_;
	delete _up_;
	delete _view_;
	delete _perspective_;
	delete _rotationY_;
	delete _rotationZ_;
	delete _translationY_;
	delete[] zBuffer;
}
