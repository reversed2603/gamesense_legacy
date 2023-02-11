#pragma once
#include "../SDK/sdk.hpp"
#include <DirectXMath.h>

#define RAD2DEG(x) DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x) DirectX::XMConvertToRadians(x)

struct Ray_t;
struct mstudiobbox_t;
class CGameTrace;

namespace Math
{
	int ClipRayToHitbox(const Ray_t& ray, mstudiobbox_t* pbox, matrix3x4_t& matrix, CGameTrace& tr);
	bool IntersectSegmentToSegment(Vector s1, Vector s2, Vector k1, Vector k2, float radius);
	bool IntersectSegmentSphere(const Vector& vecRayOrigin, const Vector& vecRayDelta, const Vector& vecSphereCenter, float radius);
	bool IntersectSegmentCapsule(const Vector& start, const Vector& end, const Vector& min, const Vector& max, float radius);
	void VectorRotate(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	bool IntersectionBoundingBox(Vector ray_start, mstudiobbox_t* hitbox, Vector direction, matrix3x4_t& matrix);

	bool IntersectRayWithAABB(Vector& origin, Vector& dir, Vector& min, Vector& max);

	void Rotate(std::array<Vector2D, 3>& points, float rotation);

	void AngleVectors(const QAngle& angles, Vector& forward, Vector& right, Vector& up);

	void VectorAngles(const Vector& forward, QAngle& angles);

	void SinCos(float a, float* s, float* c);

	void AngleVectors(const QAngle& angles, Vector& forward);

	float GetFov( const QAngle& viewAngle, const Vector& start, const Vector& end );

	float AngleNormalize(float angle);

	float ApproachAngle(float target, float value, float speed);

	void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);

	void SmoothAngle(QAngle src, QAngle& dst, float factor);

	QAngle CalcAngle(Vector src, Vector dst, bool bruh = false);

	Vector GetSmoothedVelocity(float min_delta, Vector a, Vector b);

	float AngleDiff(float src, float dst);

	float SmoothStepBounds( float edge0, float edge1, float x );

	float ClampCycle( float flCycleIn );

	float Approach( float target, float value, float speed );

	__forceinline static float Interpolate(const float from, const float to, const float percent) {
		return to * percent + from * (1.f - percent);
	}

	// Returns A + (B-A)*flPercent.
	// float Lerp( float flPercent, float A, float B );
	template <class T>
	__forceinline T Lerp( float flPercent, T const& A, T const& B )	{
		return A + ( B - A ) * flPercent;
	}


	__forceinline static Vector Interpolate(const Vector from, const Vector to, const float percent) {
		return to * percent + from * (1.f - percent);
	}

	__forceinline static void MatrixSetOrigin(Vector pos, matrix3x4_t& matrix) {
		matrix[0][3] = pos.x;
		matrix[1][3] = pos.y;
		matrix[2][3] = pos.z;
	}

	__forceinline static Vector MatrixGetOrigin(const matrix3x4_t& src) {
		return { src[0][3], src[1][3], src[2][3] };
	}

	__forceinline void VectorScale(const float* in, float scale, float* out) {
		out[0] = in[0] * scale;
		out[1] = in[1] * scale;
		out[2] = in[2] * scale;
	}

	struct CapsuleCollider {
		Vector min;
		Vector max;
		float radius;

		bool Intersect(const Vector& a, const Vector& b) const;
	};

	// mixed types involved.
	template < typename T >
	T Clamp(const T& val, const T& minVal, const T& maxVal) {
		if ((T)val < minVal)
			return minVal;
		else if ((T)val > maxVal)
			return maxVal;
		else
			return val;
	}

	template < typename T >
	T Hermite_Spline(
		T p1,
		T p2,
		T d1,
		T d2,
		float t) {
		float tSqr = t * t;
		float tCube = t * tSqr;

		float b1 = 2.0f * tCube - 3.0f * tSqr + 1.0f;
		float b2 = 1.0f - b1; // -2*tCube+3*tSqr;
		float b3 = tCube - 2 * tSqr + t;
		float b4 = tCube - tSqr;

		T output;
		output = p1 * b1;
		output += p2 * b2;
		output += d1 * b3;
		output += d2 * b4;

		return output;
	}

	template < typename T >
	T Hermite_Spline(T p0, T p1, T p2, float t) {
		return Hermite_Spline(p1, p2, p1 - p0, p2 - p1, t);
	}

	// wide -> multi-byte
	__forceinline std::string WideToMultiByte(const std::wstring& str) {
		std::string ret;
		int         str_len;

		// check if not empty str
		if (str.empty())
			return { };

		// count size
		str_len = WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.size(), 0, 0, 0, 0);

		// setup return value
		ret = std::string(str_len, 0);

		// final conversion
		WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.size(), &ret[0], str_len, 0, 0);

		return ret;
	}

	// multi-byte -> wide
	__forceinline std::wstring MultiByteToWide(const std::string& str) {
		std::wstring    ret;
		int		        str_len;

		// check if not empty str
		if (str.empty())
			return { };

		// count size
		str_len = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);

		// setup return value
		ret = std::wstring(str_len, 0);

		// final conversion
		MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &ret[0], str_len);

		return ret;
	}
}
