#include "RMatrix.h"
#include <xmmintrin.h>
#include <intrin.h>

std::ostream &operator<<(std::ostream &os, const RMatrix &m)
{
	for (int y = 0; y < m.r; y++) {
		for (int x = 0; x < m.c; x++) {
			os << *(m.v + y * 4 + x) << " ";
		}
		os << std::endl;
	}
	return os;
}


void RMatrix::matAddMat(RMatrix &w)
{
	// same dimensions
	for (int y = 0; y < r; y++) {
		for (int x = 0; x < c; x++) {
			*(v + y * 4 + x) += *(w.v + y * 4 + x);
		}
	}
}

void RMatrix::matSubMat(RMatrix &w)
{
	// same dimensions
	for (int y = 0; y < r; y++) {
		for (int x = 0; x < c; x++) {
			*(v + y * 4 + x) += *(w.v + y * 4 + x);
		}
	}
}



void RMatrix::matMulMat(RMatrix &w)
{
	// !! MANDATORY !! c == w.r
	retain();

	for (int i = 0; i < r; i++) {
		for (int j = 0; j < w.c; j++) {
			*(v + i * 4 + j) = 0;
			for (int k = 0; k < w.r; k++) {
				*(v + i * 4 + j) += *(x + i * 4 + k) * *(w.v + k * 4 + j);
			}
		}
	}
}

void RMatrix::matMulMatMmx(RMatrix &w)
{
	__m128 a, b, s;
	retain();
	// w.storeTransposed(); // must be called before matMulMatMmx

	for (int i = 0; i < r; i++) {
		memcpy(mmxA, x + i * 4, 16);
		a = _mm_load_ps(mmxA);
		for (int j = 0; j < w.c; j++) {
			memcpy(mmxB, w.y + j * 4, 16);
			b = _mm_load_ps(mmxB);
			s = _mm_mul_ps(a, b);

			// s = { s3, s2, s1, s0 }
			s = _mm_hadd_ps(s, s);
			// s = {s3+s2, s1+s0, s3+s2, s1+s0}
			s = _mm_hadd_ps(s, s);
			// s = {s2+s3+s1+s0, s3+s2+s1+s0, s3+s2+s1+s0, s3+s2+s1+s0}

			_mm_store_ps(mmxR, s);
			*(v + i * 4 + j) = mmxR[0];
		}
	}
}


void RMatrix::rotation2(REAL angle)
{
	float cs = std::cos(angle);
	float sn = std::sin(angle);
	clear();
	v[0] = cs;
	v[1] = -sn;
	v[4] = sn;
	v[5] = cs;
	v[10] = 1;
}

void RMatrix::translation2(REAL x, REAL y)
{
	clear();
	v[0] = 1;
	v[1] = 0;
	v[2] = x;
	v[4] = 0;
	v[5] = 1;
	v[6] = y;
	v[8] = 0;
	v[9] = 0;
	v[10] = 1;
}

void RMatrix::rotation3X(REAL angle)
{
	float cs = std::cos(angle);
	float sn = std::sin(angle);
	clear();
	v[0] = 1;
	v[5] = cs;
	v[6] = -sn;
	v[9] = sn;
	v[10] = cs;
	v[15] = 1;
}

void RMatrix::rotation3Y(REAL angle)
{
	float cs = std::cos(angle);
	float sn = std::sin(angle);
	clear();
	v[0] = cs;
	v[2] = sn;
	v[5] = 1;
	v[8] = -sn;
	v[10] = cs;
	v[15] = 1;
}

void RMatrix::rotation3Z(REAL angle)
{
	float cs = std::cos(angle);
	float sn = std::sin(angle);
	clear();
	v[0] = cs;
	v[1] = -sn;
	v[4] = sn;
	v[5] = cs;
	v[10] = 1;
	v[15] = 1;
}