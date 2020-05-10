#pragma once
#include <ostream>
#include <intrin.h>

namespace FastMath
{
	class Quaternion
	{
	public:
		Quaternion();
		Quaternion(float x, float y, float z, float w);
		~Quaternion() = default;

		Quaternion(const Quaternion& v) = default;
		Quaternion(Quaternion&& v) noexcept = default;
		Quaternion& operator=(const Quaternion& v) = default;
		Quaternion& operator=(Quaternion&& v) noexcept = default;

		void* operator new(size_t size);
		void operator delete(void* p);
		void* operator new[](size_t size);
		void operator delete[](void* p);

		Quaternion& operator+=(const Quaternion& v);
		Quaternion& operator-=(const Quaternion& v);
		Quaternion& operator*=(float v);
		Quaternion& operator/=(float v);

		friend Quaternion operator+(Quaternion lhs, const Quaternion& rhs);
		friend Quaternion operator-(Quaternion lhs, const Quaternion& rhs);
		friend Quaternion operator*(Quaternion lhs, float rhs);
		friend Quaternion operator/(Quaternion lhs, float rhs);
		friend Quaternion operator*(float lhs, Quaternion rhs);
		friend Quaternion operator/(float lhs, Quaternion rhs);

		friend Quaternion operator-(Quaternion v);

		friend std::ostream& operator<<(std::ostream& os, const Quaternion& obj);

		float x() const;
		float y() const;
		float z() const;
		float w() const;

		float magnitude() const;
		void normalize();

		static float Dot(const Quaternion& v1, const Quaternion& v2);
		static Quaternion Slerp(Quaternion v1, Quaternion v2, float t);
		static Quaternion FastSlerp(Quaternion v1, Quaternion v2, float t);
	private:
		__m128 v_{};
	};
}

