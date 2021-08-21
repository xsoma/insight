#pragma once

#include "../valve_sdk/sdk.hpp"

#include <DirectXMath.h>

#define RAD2DEG(x) DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x) DirectX::XMConvertToRadians(x)
#define M_PI 3.14159265358979323846

#define PI_F	((float)(M_PI)) 
#define TIME_TO_TICKS(dt) ((int)( 0.5f + (float)(dt) / g_GlobalVars->interval_per_tick))
#define TICKS_TO_TIME(t) (g_GlobalVars->interval_per_tick * (t) )

enum
{
	PITCH = 0,	// up / down
	YAW,		// left / right
	ROLL		// fall over
};
namespace Math
{
	void inline SinCos(float radians, float* sine, float* cosine);
	inline float FASTSQRT(float x)
	{
		unsigned int i = *(unsigned int*)&x;

		i += 127 << 23;
		// approximation of square root
		i >>= 1;
		return *(float*)&i;
	}
	double Deg2Rad(double degrees);
	float Rad2Deg(float val);
	float NormalizePitch(float pitch);
	Vector VectorRotate(Vector& in1, Vector& in2);
	void AngleMatrix(const QAngle &angles, matrix3x4_t& matrix);
	void ConcatTransforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out);
	Vector ClampMagnitude(Vector vector, float maxLength);
	float Segment2Segment(const Vector s1, const Vector s2, const Vector k1, const Vector k2);
	void vector_i_transform(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	void vector_i_rotate(Vector in1, matrix3x4_t in2, Vector& out);
	bool intersect_line_with_bb(Vector& start, Vector& end, Vector& min, Vector& max);
	float RandomFloat(float min, float max);
	int RandomInt(int min, int max);
	void FixAngles(QAngle &angle);
	QAngle NormalizeAng(QAngle angs);
	void AngleVectors2(const Vector& angles, Vector& forward);
	void AngleQ(const QAngle& angles, Vector* forward, Vector* right, Vector* up);
	void AngleVectorS (const Vector & angles, Vector * forward, Vector * right = nullptr, Vector * up = nullptr);
	float VectorDistance(const Vector& v1, const Vector& v2);
	float DistanceToRay(const Vector &pos, const Vector &rayStart, const Vector &rayEnd, float *along = nullptr, Vector *pointOnRay = nullptr);
	float DotProduct(const Vector & a, const Vector & b);
	QAngle CalcAngle(const Vector& src, const Vector& dst);
	Vector CalcAngle2(const Vector& src, const Vector& dst);
	Vector CalculateAngle(const Vector& src, const Vector& dst);
	void NormalizeAnglesV(Vector& angles);
	float GetFOV(const QAngle& viewAngle, const QAngle& aimAngle);
	void AngleVectors_2(const Vector& angles, Vector* forward);
	template<class T>
	void Normalize3(T& vec)
	{
		for (auto i = 0; i < 2; i++) {
			while (vec[i] < -180.0f) vec[i] += 360.0f;
			while (vec[i] >  180.0f) vec[i] -= 360.0f;
		}
		vec[2] = 0.f;
	}
	inline float ClampYaw(float yaw)
	{
		while (yaw > 180.f)
			yaw -= 360.f;
		while (yaw < -180.f)
			yaw += 360.f;
		return yaw;
	}
	template<class T, class U>
	static T Clamp(T in, U low, U high) {
		if (in <= low)
			return low;

		if (in >= high)
			return high;

		return in;
	}
	__forceinline static float Interpolate(const float from, const float to, const float percent)
	{
		return to * percent + from * (1.f - percent);
	}

	__forceinline static QAngle Interpolate(const QAngle from, const QAngle to, const float percent)
	{
		return to * percent + from * (1.f - percent);
	}

	__forceinline static Vector Interpolate(const Vector from, const Vector to, const float percent)
	{
		return to * percent + from * (1.f - percent);
	}

	float NormalizeYaw(float yaw);
	void NormalizeAngles(QAngle& angles);
    void ClampAngles(QAngle& angles);
	void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	void VectorTransformWrapper(const Vector& in1, const matrix3x4_t &in2, Vector &out);
	Vector Vector_Transform(Vector vector, matrix3x4_t matrix);
    void AngleVectors(const QAngle &angles, Vector& forward);

	void VectorAngles2(const Vector& forward, Vector& up, QAngle& angles);
	float VectorNormalize(Vector& v);
	float FastVecNormalize(Vector& vec);
	bool IntersectionBoundingBox(const Vector& src, const Vector& dir, const Vector& min, const Vector& max, Vector* hit_point);
    void AngleVectorsQangle(const QAngle& angles, QAngle& forward, Vector& right, Vector& up);
	void FixVectors(const QAngle & angles, Vector * forward, Vector *right = nullptr, Vector *up = nullptr) ;
    void VectorAngles (const Vector& forward, QAngle& angles);
	void VectorAngles2(const Vector& forward, Vector& angles);
	void AngleVectors(const QAngle& angles, Vector& forward, Vector& right, Vector& up);
	void Vector_Angles(const Vector& forward, Vector& up, Vector& angles);
    bool WorldToScreen(const Vector& in, Vector& out);
}