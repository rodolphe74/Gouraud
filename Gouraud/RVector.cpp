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
	if (c >= 3)
		*(v + 1) = *(v + 1) / m;
	if (c >= 4)
		*(v + 2) = *(v + 2) / m;
}

REAL RVector::vec3DotReal(RVector &w)
{
	return *(v) * *(w.v) + *(v + 1) * *(w.v + 1) + *(v + 2) * *(w.v + 2);
}