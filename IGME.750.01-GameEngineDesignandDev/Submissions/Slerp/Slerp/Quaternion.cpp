#include <cmath>
#include <iostream>
#include "Quaternion.h"

SlowMath::Quaternion::Quaternion()
	:x_(0.0f), y_(0.0f), z_(0.0f), w_(0.0f)
{
}

SlowMath::Quaternion::Quaternion(const float x, const float y, const float z, const float w)
	: x_(x), y_(y), z_(z), w_(w)
{ }

SlowMath::Quaternion& SlowMath::Quaternion::operator+=(const Quaternion& v)
{
	x_ += v.x_;
	y_ += v.y_;
	z_ += v.z_;
	w_ += v.w_;
	return *this;
}

SlowMath::Quaternion& SlowMath::Quaternion::operator-=(const Quaternion& v)
{
	x_ -= v.x_;
	y_ -= v.y_;
	z_ -= v.z_;
	w_ -= v.w_;
	return *this;
}

SlowMath::Quaternion& SlowMath::Quaternion::operator*=(const float v)
{
	x_ *= v;
	y_ *= v;
	z_ *= v;
	w_ *= v;
	return *this;
}

SlowMath::Quaternion& SlowMath::Quaternion::operator/=(const float v)
{
	x_ /= v;
	y_ /= v;
	z_ /= v;
	w_ /= v;
	return *this;
}

SlowMath::Quaternion SlowMath::operator+(Quaternion lhs, const Quaternion& rhs)
{
	lhs += rhs;
	return lhs;
}

SlowMath::Quaternion SlowMath::operator-(Quaternion lhs, const Quaternion& rhs)
{
	lhs -= rhs;
	return lhs;
}

SlowMath::Quaternion SlowMath::operator*(Quaternion lhs, const float rhs)
{
	lhs *= rhs;
	return  lhs;
}

SlowMath::Quaternion SlowMath::operator/(Quaternion lhs, const float rhs)
{
	lhs /= rhs;
	return lhs;
}

SlowMath::Quaternion SlowMath::operator*(const float lhs, Quaternion rhs)
{
	rhs *= lhs;
	return rhs;
}

SlowMath::Quaternion SlowMath::operator/(const float lhs, Quaternion rhs)
{
	rhs /= lhs;
	return rhs;
}

SlowMath::Quaternion SlowMath::operator-(Quaternion v)
{
	v *= -1.0f;
	return v;
}

std::ostream& SlowMath::operator<<(std::ostream& os, const Quaternion& obj)
{
	os << "x = " << obj.x_
		<< ",\ty = " << obj.y_
		<< ",\tz = " << obj.z_
		<< ",\tw = " << obj.w_;
	return os;
}

float SlowMath::Quaternion::x() const
{
	return x_;
}

float SlowMath::Quaternion::y() const
{
	return y_;
}

float SlowMath::Quaternion::z() const
{
	return z_;
}

float SlowMath::Quaternion::w() const
{
	return w_;
}

float SlowMath::Quaternion::magnitude() const
{
	return sqrtf(Dot(*this, *this));
}

void SlowMath::Quaternion::normalize()
{
	const float d = magnitude();
	*this /= d;
}

float SlowMath::Quaternion::Dot(const Quaternion& v1, const Quaternion& v2)
{
	return v1.x_* v2.x_
		+ v1.y_ * v2.y_
		+ v1.z_ * v2.z_
		+ v1.w_ * v2.w_;
}

SlowMath::Quaternion SlowMath::Quaternion::Slerp(Quaternion v1, Quaternion v2, const float t)
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

SlowMath::Quaternion SlowMath::Quaternion::FastSlerp(Quaternion v1, Quaternion v2, float t)
{
	float dot = v1.x_ * v2.x_
		+ v1.y_ * v2.y_
		+ v1.z_ * v2.z_
		+ v1.w_ * v2.w_;

	if (dot < 0.0f)
	{
		v2.x_ *= -1;
		v2.y_ *= -1;
		v2.z_ *= -1;
		v2.w_ *= -1;
		dot = -dot;
	}

	const float omega = acosf(dot);
	const float theta = omega * t;
	const float sinTheta = sinf(theta);
	const float sinOmega = sinf(omega);

	const float s0 = cosf(theta) - dot * sinTheta / sinOmega;
	const float s1 = sinTheta / sinOmega;

	const Quaternion result(
		s0 * v1.x_ + s1 * v2.x_,
		s0 * v1.y_ + s1 * v2.y_,
		s0 * v1.z_ + s1 * v2.z_,
		s0 * v1.w_ + s1 * v2.w_
	);
	return result;
}
