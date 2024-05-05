#include "RVector.h"

std::ostream &operator<<(std::ostream &os, const RVector &m)
{
	for (int x = 0; x < m.c; x++) {
		os << *(m.v + x);
		os << std::endl;
	}
	return os;
}

void RVector::vecAddVec(RVector &w)
{
	for (int x = 0; x < c; x++) {
		*(v + x) += *(w.v + x);
	}
}

void RVector::vecSubVec(RVector &w)
{
	for (int x = 0; x < c; x++) {
		*(v + x) -= *(w.v + x);
	}
}

void RVector::vecMulVec(RVector &w)
{
	for (int x = 0; x < c; x++) {
		*(v + x) *= *(w.v + x);
	}
}

void RVector::vecMulMat(RMatrix &w)
{
	retain();
	clear();
	for (int _y = 0; _y < w.r; _y++) {
		for (int _x = 0; _x < w.c; _x++) {
			v[_y] += (x[_x] * w.v[_y * 4 + _x]);
		}
	}
}


void RVector::vec4MulMat4(RMatrix &w)
{
	retain();
	*(v) = *(w.v) * *(x)+*(w.v + 4) * *(x + 1) + *(w.v + 8) * *(x + 2) + *(w.v + 12) * *(x + 3);
	*(v + 1) = *(w.v + 1) * *(x)+*(w.v + 5) * *(x + 1) + *(w.v + 9) * *(x + 2) + *(w.v + 13) * *(x + 3);
	*(v + 2) = *(w.v + 2) * *(x)+*(w.v + 6) * *(x + 1) + *(w.v + 10) * *(x + 2) + *(w.v + 14) * *(x + 3);
	*(v + 3) = *(w.v + 3) * *(x)+*(w.v + 7) * *(x + 1) + *(w.v + 11) * *(x + 2) + *(w.v + 15) * *(x + 3);
}

void RVector::vecMulScalar(REAL s)
{
	*(v) *= s;
	*(v + 1) *= s;
	if (c >= 3)
		*(v + 2) *= s;
	if (c >= 4)
		*(v + 3) *= s;
}

void RVector::vec3CrossVec3(RVector &w)
{
	retain();
	*(v) = *(x + 1) * *(w.v + 2) - *(w.v + 1) * *(x + 2);
	*(v + 1) = -(*(x) * *(w.v + 2) - *(w.v) * *(x + 2));
	*(v + 2) = *(x) * *(w.v + 1) - *(w.v) * *(x + 1);
}

void RVector::vecNormalize()
{
	float m = std::sqrt(*(v) * *(v)+*(v + 1) * *(v + 1) + *(v + 2) * *(v + 2));
	*(v) = *(v) / m;
	*(v + 1) = *(v + 1) / m;
	*(v + 2) = *(v + 2) / m;
	if (c >= 4)
		*(v + 3) = *(v + 3) / m;
}

REAL RVector::vec3DotReal(RVector &w)
{
	return *(v) * *(w.v) + *(v + 1) * *(w.v + 1) + *(v + 2) * *(w.v + 2);
}