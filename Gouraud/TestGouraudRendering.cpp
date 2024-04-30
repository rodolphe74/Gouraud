#include "TestGouraudRendering.h"
#include "TestRoot.h"

Obj *TestGouraudRendering::o;
Light *TestGouraudRendering::lg;
RMatrix *TestGouraudRendering::_fromPosition_;
RVector *TestGouraudRendering::_toTarget_;
RVector *TestGouraudRendering::_up_;
RMatrix *TestGouraudRendering::_view_;
RMatrix *TestGouraudRendering::_perspective_;
RMatrix *TestGouraudRendering::_rotationY_;
RMatrix *TestGouraudRendering::_rotationZ_;
RVector *TestGouraudRendering::_translationY_;


namespace Fx {
	typedef struct {
		float x, y;
		float z;
		float argb;
	} Vertex;
}

void TestGouraudRendering::initObject()
{
	o = new Obj();
	o->loadObjects("./obj/icosphere.obj");
	lg = nullptr;

	_fromPosition_ = new RMatrix({ 0.0f, 0.0f, 3.0f }, MTYPE::MAT44);
	_toTarget_ = new RVector({ 0.0f, 0.0f, 0.0f }, VTYPE::VEC3);
	_up_ = new RVector({ 0.0f, 1.0f, 0.0f }, VTYPE::VEC3);
	_view_ = new RMatrix(MTYPE::MAT44);
	_perspective_ = new RMatrix(MAT44);
	_rotationY_ = new RMatrix(MAT44);
	_rotationZ_ = new RMatrix(MAT44);
	_translationY_ = new RVector(VEC4);
}



void TestGouraudRendering::renderObject(char *pixels, int w, int h, int pitch, bool tictac)
{
	int32_t *row = (int32_t *)pixels;
	for (int i = 0; i < w * h; i++) {
		*(row + i) = 0x00326432;
	}

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
}
