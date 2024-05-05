#pragma once

#include <iostream>
// #include "RVector.h"

#define MAT4SZ 4
#define MAT3SZ 3
#define MAT2SZ 2

#define M_PI 3.14159265358979323846
#define TO_RADIAN(x) (x * (M_PI / 180))

typedef float REAL;
typedef REAL MEMORY16[16];
enum MTYPE { MAT12, MAT13, MAT14, MAT22, MAT33, MAT44 };

class RMatrix
{

private:

	// msvc ensure array is aligned
	float __declspec(align(16)) mmxA[4];
	float __declspec(align(16)) mmxB[4];
	float __declspec(align(16)) mmxR[4];

	MTYPE t = MTYPE::MAT44;
	int r = MAT4SZ;
	int c = MAT4SZ;

	RMatrix();




public:
	MEMORY16 x = { 0 };	// used to cache
	MEMORY16 y = { 0 };	// used to hold transposed
	MEMORY16 v = { 0 };

	void dimIt(MTYPE t)
	{
		switch (t)
		{
		case MAT12:
			r = 1;
			c = 2;
			break;
		case MAT13:
			r = 1;
			c = 3;
			break;
		case MAT14:
			r = 1;
			c = 4;
			break;
		case MAT22:
			r = 2;
			c = 2;
			break;
		case MAT33:
			r = 3;
			c = 3;
			break;
		case MAT44:
			r = 4;
			c = 4;
			break;
		default:
			break;
		}
	}

	RMatrix(MTYPE t)
	{
		this->t = t;
		dimIt(t);
	}

	RMatrix(std::initializer_list<REAL> lst, MTYPE t)
	{
		this->t = t;
		dimIt(t);
		int x = 0, y = 0;
		for (REAL l : lst) {
			*(v + y * 4 + x) = l;
			x++;
			if (x == c) {
				x = 0;
				y++;
			}
		}
	}

	void setType(MTYPE t) {
		this->t = t;
		dimIt(t);
	}

	void retain() {
		memcpy(x, v, sizeof(REAL) * 16);
	}

	void leave() {
		memcpy(v, x, sizeof(REAL) * 16);
	}


	inline void copy(RMatrix &w) {
		memcpy(v, w.v, sizeof(float) * 16);
	}

	void clear() {
		memset(v, 0, sizeof(REAL) * 16);
	}

	void vecSetAt(int x, REAL value)
	{
		*(v + x) = value;
	}

	void matSetAt(int x, int y, REAL value)
	{
		*(v + y * 4 + x) = value;
	}

	REAL vecGetAt(int x)
	{
		return  *(v + x);
	}

	REAL matGetAt(int x, int y) {
		return *(v + y * 4 + x);
	}

	void getColumn(int index, float *result)
	{
		for (int y = 0; y < 4; y++) {
			result[y] = *(v + y * 4 + index);
		}
	}

	void storeTransposed() {
		int i, j;
		for (i = 0; i < r; i++)
			for (j = 0; j < c; j++)
				*(y + i * 4 + j) = *(v + j * 4 + i);
	}

	void transpose()
	{
		storeTransposed();
		memcpy(v, y, sizeof(REAL) * 16);
	}

	void matAddMat(RMatrix &w);
	void matSubMat(RMatrix &w);
	void matMulMat(RMatrix &w);
	void matMulMatMmx(RMatrix &w);
	void matInvert();

	void rotation2(REAL angle);
	void translation2(REAL x, REAL y);
	void rotation3X(REAL angle);
	void rotation3Y(REAL angle);
	void rotation3Z(REAL angle);

	friend std::ostream &operator<<(std::ostream &os, const RMatrix &m);
	friend class RVector;
};

