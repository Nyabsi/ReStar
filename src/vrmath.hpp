#pragma once

#include <cmath>

#ifndef M_PI
#define M_PI 3.1415926535
#endif // M_PI

// from vrmath.h
template < class T >
vr::HmdQuaternion_t HmdQuaternion_FromMatrix(const T& matrix)
{
	vr::HmdQuaternion_t q{};

	q.w = sqrt(fmax(0, 1 + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2;
	q.x = sqrt(fmax(0, 1 + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.y = sqrt(fmax(0, 1 - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2;
	q.z = sqrt(fmax(0, 1 - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2;

	q.x = copysign(q.x, matrix.m[2][1] - matrix.m[1][2]);
	q.y = copysign(q.y, matrix.m[0][2] - matrix.m[2][0]);
	q.z = copysign(q.z, matrix.m[1][0] - matrix.m[0][1]);

	return q;
}


vr::HmdQuaternion_t Multiply(
	const vr::HmdQuaternion_t& a,
	const vr::HmdQuaternion_t& b)
{
	return {
		a.w* b.w - a.x * b.x - a.y * b.y - a.z * b.z,	// w - dot
		// notice, x is first here in the cross (x -> y -> z)
		// normal xzy cross product of the x being first
		a.w* b.x + a.x * b.w + a.y * b.z - a.z * b.y, // cross xzy
		// notice, y is first here in the cross (y -> z -> x)
		// x and y swappes because z is calculated first for the j component
		a.w* b.y - a.x * b.z + a.y * b.w + a.z * b.x, // cross yzx
		// notice, z is first here in the cross (z -> y -> x)
		// z and y swappes because z is calculated first for the k component
		a.w* b.z + a.x * b.y - a.y * b.x + a.z * b.w  // cross zyx
	};
}

vr::HmdVector3_t RotateVector(
	const vr::HmdQuaternion_t& q,
	const vr::HmdVector3_t& v)
{
	vr::HmdQuaternion_t vq{ 0, v.v[0], v.v[1], v.v[2] };

	vr::HmdQuaternion_t qInv{
		q.w, -q.x, -q.y, -q.z
	};

	vr::HmdQuaternion_t r =
		Multiply(Multiply(q, vq), qInv);

	return { static_cast<float>(r.x), static_cast<float>(r.y), static_cast<float>(r.z) };
}

vr::HmdQuaternion_t Normalize(const vr::HmdQuaternion_t& q)
{
	const double lenSq =
		q.w * q.w +
		q.x * q.x +
		q.y * q.y +
		q.z * q.z;

	// Avoid division by zero
	if (lenSq <= 0.0f)
		return { 1.f, 0.f, 0.f, 0.f };

	const double invLen = 1.0f / std::sqrt(lenSq);

	return {
		q.w * invLen,
		q.x * invLen,
		q.y * invLen,
		q.z * invLen
	};
}

// Basic Spherical Linear Interpolation for Quaternions
vr::HmdQuaternion_t Slerp(const vr::HmdQuaternion_t& q1, const vr::HmdQuaternion_t& q2, double t)
{
	vr::HmdQuaternion_t q;
	// Calculate dot product (cosine of angle)
	double dot = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;

	// Adjust signs if necessary (take shortest path)
	double sign = 1.0f;
	if (dot < 0.0f) {
		dot = -dot;
		sign = -1.0f;
	}

	// If inputs are too close, just linearly interpolate to avoid division by zero
	if (dot > 0.9995f) {
		q.w = q1.w + t * (sign * q2.w - q1.w);
		q.x = q1.x + t * (sign * q2.x - q1.x);
		q.y = q1.y + t * (sign * q2.y - q1.y);
		q.z = q1.z + t * (sign * q2.z - q1.z);
		return Normalize(q); // Assume you have a Normalize function
	}

	// Standard Slerp
	double theta_0 = acos(dot);        // theta_0 = angle between input vectors
	double theta = theta_0 * t;         // theta = angle between v0 and result
	double sin_theta = sin(theta);     // compute this value only once
	double sin_theta_0 = sin(theta_0); // compute this value only once

	double s0 = cos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
	double s1 = sin_theta / sin_theta_0;

	q.w = (s0 * q1.w) + (s1 * (sign * q2.w));
	q.x = (s0 * q1.x) + (s1 * (sign * q2.x));
	q.y = (s0 * q1.y) + (s1 * (sign * q2.y));
	q.z = (s0 * q1.z) + (s1 * (sign * q2.z));

	return q;
}