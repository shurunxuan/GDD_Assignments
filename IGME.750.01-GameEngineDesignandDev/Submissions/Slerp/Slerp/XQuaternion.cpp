#include <iostream>
#include "XQuaternion.h"


FastMath::Quaternion::Quaternion()
{
	v_ = _mm_set_ps1(0.0f);
}

FastMath::Quaternion::Quaternion(float x, float y, float z, float w)
{
	v_ = _mm_set_ps(w, z, y, x);
}

void* FastMath::Quaternion::operator new(const size_t size)
{
	return _aligned_malloc(size, sizeof(__m128));
}

void FastMath::Quaternion::operator delete(void* p)
{
	_aligned_free(p);
}

void* FastMath::Quaternion::operator new[](const size_t size)
{
	return _aligned_malloc(size, sizeof(__m128));
}

void FastMath::Quaternion::operator delete[](void* p)
{
	_aligned_free(p);
}

FastMath::Quaternion& FastMath::Quaternion::operator+=(const Quaternion& v)
{
	v_ = _mm_add_ps(v_, v.v_);
	return *this;
}

FastMath::Quaternion& FastMath::Quaternion::operator-=(const Quaternion& v)
{
	v_ = _mm_sub_ps(v_, v.v_);
	return *this;
}

FastMath::Quaternion& FastMath::Quaternion::operator*=(float v)
{
	const __m128 s = _mm_set_ps1(v);
	v_ = _mm_mul_ps(v_, s);
	return *this;
}

FastMath::Quaternion& FastMath::Quaternion::operator/=(float v)
{
	const __m128 s = _mm_set_ps1(v);
	v_ = _mm_div_ps(v_, s);
	return *this;
}

FastMath::Quaternion FastMath::operator+(Quaternion lhs, const Quaternion& rhs)
{
	lhs += rhs;
	return lhs;
}

FastMath::Quaternion FastMath::operator-(Quaternion lhs, const Quaternion& rhs)
{
	lhs -= rhs;
	return lhs;
}

FastMath::Quaternion FastMath::operator*(Quaternion lhs, const float rhs)
{
	lhs *= rhs;
	return  lhs;
}

FastMath::Quaternion FastMath::operator/(Quaternion lhs, const float rhs)
{
	lhs /= rhs;
	return lhs;
}

FastMath::Quaternion FastMath::operator*(const float lhs, Quaternion rhs)
{
	rhs *= lhs;
	return rhs;
}

FastMath::Quaternion FastMath::operator/(const float lhs, Quaternion rhs)
{
	rhs /= lhs;
	return rhs;
}

FastMath::Quaternion FastMath::operator-(Quaternion v)
{
	v *= -1.0f;
	return v;
}

std::ostream& FastMath::operator<<(std::ostream& os, const Quaternion& obj)
{
	os << "x = " << obj.x()
		<< ",\ty = " << obj.y()
		<< ",\tz = " << obj.z()
		<< ",\tw = " << obj.w();
	return os;
}

float FastMath::Quaternion::x() const
{
	float x;
	_mm_store_ss(&x, v_);
	return x;
}

float FastMath::Quaternion::y() const
{
	float y;
	_mm_store_ss(&y, _mm_shuffle_ps(v_, v_, 1));
	return y;
}

float FastMath::Quaternion::z() const
{
	float z;
	_mm_store_ss(&z, _mm_movehl_ps(v_, v_));
	return z;
}

float FastMath::Quaternion::w() const
{
	float w;
	_mm_store_ss(&w, _mm_shuffle_ps(v_, v_, 3));
	return w;
}

float FastMath::Quaternion::magnitude() const
{
	const __m128 sq = _mm_mul_ps(v_, v_);
	__m128 t = _mm_add_ps(sq, _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(0, 3, 2, 1)));
	t = _mm_add_ps(t, _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(1, 0, 3, 2)));
	t = _mm_add_ps(t, _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(2, 1, 0, 3)));
	t = _mm_sqrt_ps(t);
	float r;
	_mm_store_ss(&r, t);
	return r;
}

void FastMath::Quaternion::normalize()
{
	const float d = magnitude();
	*this /= d;
}

float FastMath::Quaternion::Dot(const Quaternion& v1, const Quaternion& v2)
{
	const __m128 sq = _mm_mul_ps(v1.v_, v2.v_);
	__m128 t = _mm_add_ps(sq, _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(0, 3, 2, 1)));
	t = _mm_add_ps(t, _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(1, 0, 3, 2)));
	t = _mm_add_ps(t, _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(2, 1, 0, 3)));
	float r;
	_mm_store_ss(&r, t);
	return r;
}

FastMath::Quaternion FastMath::Quaternion::Slerp(Quaternion v1, Quaternion v2, float t)
{
	float dot = Dot(v1, v2);

	if (dot < 0.0f)
	{
		v2 = -v2;
		dot = -dot;
	}

	const float omega = acosf(dot);
	const float theta = omega * t;
	const float sinTheta = sinf(theta);
	const float sinOmega = sinf(omega);

	const float s0 = cosf(theta) - dot * sinTheta / sinOmega;
	const float s1 = sinTheta / sinOmega;

	return s0 * v1 + s1 * v2;
}

FastMath::Quaternion FastMath::Quaternion::FastSlerp(Quaternion v1, Quaternion v2, float t)
{
	const __m128 sq = _mm_mul_ps(v1.v_, v2.v_);
	__m128 d = _mm_add_ps(sq, _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(0, 3, 2, 1)));
	d = _mm_add_ps(d, _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(1, 0, 3, 2)));
	d = _mm_add_ps(d, _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(2, 1, 0, 3)));
	float dot;
	_mm_store_ss(&dot, d);

	if (dot < 0.0f)
	{
		const __m128 s = _mm_set_ps1(-1);
		v2.v_ = _mm_mul_ps(v2.v_, s);
		dot = -dot;
	}

	const float omega = acosf(dot);
	const float theta = omega * t;
	const float sinTheta = sinf(theta);
	const float sinOmega = sinf(omega);

	const float s0 = cosf(theta) - dot * sinTheta / sinOmega;
	const float s1 = sinTheta / sinOmega;

	const __m128 m0 = _mm_set_ps1(s0);
	const __m128 m1 = _mm_set_ps1(s1);

	Quaternion result;
	result.v_ = _mm_add_ps(_mm_mul_ps(m0, v1.v_), _mm_mul_ps(m1, v2.v_));
	return result;
}
