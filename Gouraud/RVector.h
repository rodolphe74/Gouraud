#pragma once

#define VEC4SZ 4
#define VEC3SZ 3
#define VEC2SZ 2

#include "RMatrix.h"
#include <iostream>

enum VTYPE { VEC2, VEC3, VEC4 };
typedef float REAL;
typedef REAL MEMORY4[4];

class RVector
{
private:
	MEMORY4 x = { 0 };	// used to cache
	int c = VEC4SZ;
	
public:
	MEMORY4 v = { 0 };
	VTYPE t = VTYPE::VEC4;

	void dimIt(VTYPE t)
	{
		switch (t)
		{
		case VEC2:
			c = 2;
			break;
		case VEC3:
			c = 3;
			break;
		case VEC4:
			c = 4;
			break;
		default:
			break;
		}
	}

	RVector(VTYPE t)
	{
		this->t = t;
		dimIt(t);
	}
	
	
	void retain() {
		memcpy(x, v, sizeof(REAL) * 4);
	}

	void leave() {
		memcpy(v, x, sizeof(REAL) * 4);
	}

	void clear() {
		memset(v, 0, sizeof(REAL) * 4);
	}


	RVector(std::initializer_list<REAL> lst, VTYPE t)
	{
		this->t = t;
		dimIt(t);
		int x = 0;
		for (REAL l : lst) {
			*(v + x) = l;
			x++;
		}
	}

	void vecAddVec(RVector &w);
	void vecSubVec(RVector &w);
	void vecMulVec(RVector &w);
	void vecMulMat(RMatrix &w);
	void vec4MulMat4(RMatrix &w);
	void vecMulScalar(REAL s);
	void vec3CrossVec3(RVector &w);
	void vecNormalize();
	REAL vec3DotReal(RVector &w);

	friend std::ostream &operator<<(std::ostream &os, const RVector &m);
};

