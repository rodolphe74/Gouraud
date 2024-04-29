#pragma once

#include "Obj.h"
#include "RMatrix.h"
#include "RVector.h"
#include <string>


class TestGouraudRendering
{
public:
	static Obj *o;
	static Light *lg;
	static RMatrix *_fromPosition_;
	static RVector *_toTarget_;
	static RVector *_up_;
	static RMatrix *_view_;
	static RMatrix *_perspective_;
	static RMatrix *_rotationY_;
	static RMatrix *_rotationZ_;
	static RVector *_translationY_;


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

