#pragma once

#include "3d.h"
#include "Obj.h"
#include "RMatrix.h"
#include "RVector.h"
#include <string>


class TestGouraudRendering
{
public:
	static Obj *o;
	static Light *lg;
	static RVector *_fromPosition_;
	static RVector *_toTarget_;
	static RVector *_up_;
	static RMatrix *_view_;
	static RMatrix *_perspective_;
	static RMatrix *_rotationY_;
	static RMatrix *_rotationZ_;
	static RVector *_translationY_;
	static RMatrix *_demi_tour_;
	static float *zBuffer;


	TestGouraudRendering()
	{
	}

	~TestGouraudRendering()
	{
	}

	static void initObject();
	static void renderObject(char *pixels, int w, int h, int pitch, bool tictac);
	static void close();
};

