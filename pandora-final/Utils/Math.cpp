#include "Math.h"
#include "../source.hpp"
#include "../SDK/Classes/Player.hpp"
#include "../SDK/variables.hpp"

#include <algorithm>
#include <numeric>
#include <xmmintrin.h>

static Vector DirBetweenLines( const Vector& a, const Vector& b, const Vector& c, const Vector& d ) {
	const Vector d1 = ( b - a );
	const Vector d2 = ( d - c );

	const Vector cross = d1.Cross( d2 );

	const Vector cross1 = d1.Cross( cross );
	const Vector cross2 = d2.Cross( cross );

	const Vector sp = c + d2 * Math::Clamp( ( a - c ).Dot( cross1 ) / ( d2.Dot( cross1 ) ), 0.f, 1.f );
	const Vector ep = a + d1 * Math::Clamp( ( c - a ).Dot( cross2 ) / ( d1.Dot( cross2 ) ), 0.f, 1.f );

	return ep - sp;
}

typedef __declspec( align( 16 ) ) union {
	float f[ 4 ];
	__m128 v;
} m128;

__forceinline __m128 sqrt_ps( const __m128 squared ) {
	return _mm_sqrt_ps( squared );
}

#pragma optimize("", off)
int Math::ClipRayToHitbox( const Ray_t& ray, mstudiobbox_t* pbox, matrix3x4_t& matrix, CGameTrace& tr )
{
	//crash crash crash crash
	static auto clp_fn = Memory::Scan( XorStr( "client.dll" ), XorStr( "55 8B EC 83 E4 F8 F3 0F 10 42" ) );
	return reinterpret_cast< int( __fastcall* )( const Ray_t&, mstudiobbox_t*, matrix3x4_t&, CGameTrace& ) >( clp_fn )( ray, pbox, matrix, tr );
}
#pragma optimize("", on)

#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)
#define norm(v)    sqrt(dot(v,v))  // norm = length of  vector

bool Math::IntersectSegmentToSegment( Vector s1, Vector s2, Vector k1, Vector k2, float radius ) {
	Vector   u = s2 - s1;
	Vector   v = k2 - k1;
	Vector   w = s1 - k1;
	float    a = dot( u, u );
	float    b = dot( u, v );
	float    c = dot( v, v );
	float    d = dot( u, w );
	float    e = dot( v, w );
	float    D = a * c - b * b;
	float    sc, sN, sD = D;
	float    tc, tN, tD = D;

	if( D < 0.00000001f ) {
		sN = 0.0;
		sD = 1.0;
		tN = e;
		tD = c;
	}
	else {
		sN = ( b * e - c * d );
		tN = ( a * e - b * d );
		if( sN < 0.0 ) {
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if( sN > sD ) {
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if( tN < 0.0 ) {
		tN = 0.0;

		if( -d < 0.0 )
			sN = 0.0;
		else if( -d > a )
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if( tN > tD ) {
		tN = tD;

		if( ( -d + b ) < 0.0 )
			sN = 0;
		else if( ( -d + b ) > a )
			sN = sD;
		else {
			sN = ( -d + b );
			sD = a;
		}
	}

	sc = ( abs( sN ) < 0.00000001f ? 0.0 : sN / sD );
	tc = ( abs( tN ) < 0.00000001f ? 0.0 : tN / tD );

	Vector  dP = w + ( u * sc ) - ( v * tc );

	auto shit = norm( dP );
	//printf( "shit %f | rad %f\n", shit, radius );
	return shit < radius;
}

bool Math::CapsuleCollider::Intersect( const Vector& start, const Vector& end ) const {
#if 1
	static auto constexpr epsilon = 0.00000001f;

	const auto s1 = min;
	const auto s2 = max;
	const auto k1 = start;
	const auto k2 = end;

	auto u = s2 - s1;
	auto v = k2 - k1;
	const auto w = s1 - k1;

	const auto a = u.Dot( u );
	const auto b = u.Dot( v );
	const auto c = v.Dot( v );
	const auto d = u.Dot( w );
	const auto e = v.Dot( w );
	const auto D = a * c - b * b;
	float sn, sd = D;
	float tn, td = D;

	if( D < epsilon ) {
		sn = 0.0f;
		sd = 1.0f;
		tn = e;
		td = c;
	}
	else {
		sn = b * e - c * d;
		tn = a * e - b * d;

		if( sn < 0.0f ) {
			sn = 0.0f;
			tn = e;
			td = c;
		}
		else if( sn > sd ) {
			sn = sd;
			tn = e + b;
			td = c;
		}
	}

	if( tn < 0.0f ) {
		tn = 0.0f;

		if( -d < 0.0f )
			sn = 0.0f;
		else if( -d > a )
			sn = sd;
		else {
			sn = -d;
			sd = a;
		}
	}
	else if( tn > td ) {
		tn = td;

		if( -d + b < 0.0f )
			sn = 0.f;
		else if( -d + b > a )
			sn = sd;
		else {
			sn = -d + b;
			sd = a;
		}
	}

	const float sc = abs( sn ) < epsilon ? 0.0f : sn / sd;
	const float tc = abs( tn ) < epsilon ? 0.0f : tn / td;

	m128 n;
	auto dp = w + u * sc - v * tc;
	n.f[ 0 ] = dp.Dot( dp );
	const auto calc = sqrt_ps( n.v );
	return radius < reinterpret_cast< const m128* >( &calc )->f[ 0 ];

	//auto dp = w + u * sc - v * tc;
	//auto scale = dp.Dot( dp );
	//return scale < radius * radius;
#endif

#if 0
	const Vector dir = DirBetweenLines( min, max, start, end );
	return dir.LengthSquared( ) <= radius * radius;


	return IntersectSegmentCapsule( start, end, min, max, radius );
#endif
}

bool Math::IntersectSegmentSphere( const Vector& vecRayOrigin, const Vector& vecRayDelta, const Vector& vecSphereCenter, float flRadius ) {
	// Solve using the ray equation + the sphere equation
	// P = o + dt
	// (x - xc)^2 + (y - yc)^2 + (z - zc)^2 = r^2
	// (ox + dx * t - xc)^2 + (oy + dy * t - yc)^2 + (oz + dz * t - zc)^2 = r^2
	// (ox - xc)^2 + 2 * (ox-xc) * dx * t + dx^2 * t^2 +
	//		(oy - yc)^2 + 2 * (oy-yc) * dy * t + dy^2 * t^2 +
	//		(oz - zc)^2 + 2 * (oz-zc) * dz * t + dz^2 * t^2 = r^2
	// (dx^2 + dy^2 + dz^2) * t^2 + 2 * ((ox-xc)dx + (oy-yc)dy + (oz-zc)dz) t +
	//		(ox-xc)^2 + (oy-yc)^2 + (oz-zc)^2 - r^2 = 0
	// or, t = (-b +/- sqrt( b^2 - 4ac)) / 2a
	// a = DotProduct( vecRayDelta, vecRayDelta );
	// b = 2 * DotProduct( vecRayOrigin - vecCenter, vecRayDelta )
	// c = DotProduct(vecRayOrigin - vecCenter, vecRayOrigin - vecCenter) - flRadius * flRadius;

	Vector vecSphereToRay = vecRayOrigin - vecSphereCenter;

	float a = vecRayDelta.Dot( vecRayDelta );

	// This would occur in the case of a zero-length ray
	if( a == 0.0f )
		return vecSphereToRay.LengthSquared( ) <= flRadius * flRadius;

	float b = 2.f * vecSphereToRay.Dot( vecRayDelta );
	float c = vecSphereToRay.Dot( vecSphereToRay ) - flRadius * flRadius;
	float flDiscrim = b * b - 4.f * a * c;
	return flDiscrim >= 0.0f;
}


bool Math::IntersectSegmentCapsule( const Vector& start, const Vector& end, const Vector& min, const Vector& max, float radius ) {
	Vector d = max - min, m = start - min, n = end - start;
	float md = m.Dot( d );
	float nd = n.Dot( d );
	float dd = d.Dot( d );

	if( md < 0.0f && md + nd < 0.0f ) {
		return IntersectSegmentSphere( start, n, min, radius );
	}
	if( md > dd && md + nd > dd ) {
		return IntersectSegmentSphere( start, n, max, radius );
	}

	float t = 0.0f;
	float nn = n.Dot( n );
	float mn = m.Dot( n );
	float a = dd * nn - nd * nd;
	float k = m.Dot( m ) - radius * radius;
	float c = dd * k - md * md;
	if( std::fabsf( a ) < FLT_EPSILON ) {
		if( c > 0.0f )
			return 0;
		if( md < 0.0f )
			IntersectSegmentSphere( start, n, min, radius );
		else if( md > dd )
			IntersectSegmentSphere( start, n, max, radius );
		else
			t = 0.0f;
		return true;
	}
	float b = dd * mn - nd * md;
	float discr = b * b - a * c;
	if( discr < 0.0f )
		return false;

	t = ( -b - sqrt( discr ) ) / a;
	float t0 = t;
	if( md + t * nd < 0.0f ) {
		return IntersectSegmentSphere( start, n, min, radius );
	}
	else if( md + t * nd > dd ) {

		return IntersectSegmentSphere( start, n, max, radius );
	}
	t = t0;
	return t > 0.0f && t < 1.0f;
}

void Math::VectorRotate( const Vector& in1, const matrix3x4_t& in2, Vector& out )
{
	out.x = in1.x * in2[ 0 ][ 0 ] + in1.y * in2[ 1 ][ 0 ] + in1.z * in2[ 2 ][ 0 ];
	out.y = in1.x * in2[ 0 ][ 1 ] + in1.y * in2[ 1 ][ 1 ] + in1.z * in2[ 2 ][ 1 ];
	out.z = in1.x * in2[ 0 ][ 2 ] + in1.y * in2[ 1 ][ 2 ] + in1.z * in2[ 2 ][ 2 ];
}

bool Math::IntersectionBoundingBox( Vector ray_start, mstudiobbox_t* hitbox, Vector direction, matrix3x4_t& matrix )
{
	//Transform ray into model space of hitbox so we only have to deal with an AABB instead of OBB
	Vector ray_trans, dir_trans;
	Math::VectorTransform( ray_start, matrix, ray_trans );
	Math::VectorRotate( direction, matrix, dir_trans ); //only rotate direction vector! no translation!

	return IntersectRayWithAABB( ray_trans, dir_trans, hitbox->bbmin, hitbox->bbmax );
}

bool Math::IntersectRayWithAABB( Vector& origin, Vector& dir, Vector& min, Vector& max )
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	if( dir.x >= 0 )
	{
		tmin = ( min.x - origin.x ) / dir.x;
		tmax = ( max.x - origin.x ) / dir.x;
	}
	else
	{
		tmin = ( max.x - origin.x ) / dir.x;
		tmax = ( min.x - origin.x ) / dir.x;
	}

	if( dir.y >= 0 )
	{
		tymin = ( min.y - origin.y ) / dir.y;
		tymax = ( max.y - origin.y ) / dir.y;
	}
	else
	{
		tymin = ( max.y - origin.y ) / dir.y;
		tymax = ( min.y - origin.y ) / dir.y;
	}

	if( tmin > tymax || tymin > tmax )
		return false;

	if( tymin > tmin )
		tmin = tymin;

	if( tymax < tmax )
		tmax = tymax;

	if( dir.z >= 0 )
	{
		tzmin = ( min.z - origin.z ) / dir.z;
		tzmax = ( max.z - origin.z ) / dir.z;
	}
	else
	{
		tzmin = ( max.z - origin.z ) / dir.z;
		tzmax = ( min.z - origin.z ) / dir.z;
	}

	if( tmin > tzmax || tzmin > tmax )
		return false;

	//behind us
	if( tmin < 0 || tmax < 0 )
		return false;

	return true;
}

void Math::Rotate( std::array< Vector2D, 3 >& points, float rotation ) {
	const auto points_center = ( points.at( 0 ) + points.at( 1 ) + points.at( 2 ) ) / 3;
	for( auto& point : points ) {
		point -= points_center;

		const auto temp_x = point.x;
		const auto temp_y = point.y;

		const auto theta = ToRadians( rotation );
		float c, s;
		DirectX::XMScalarSinCos( &s, &c, theta );

		point.x = temp_x * c - temp_y * s;
		point.y = temp_x * s + temp_y * c;

		point += points_center;
	}
}
void Math::AngleVectors( const QAngle& angles, Vector& forward, Vector& right, Vector& up ) {
	float sr, sp, sy, cr, cp, cy;

	SinCos( DEG2RAD( angles[ 1 ] ), &sy, &cy );
	SinCos( DEG2RAD( angles[ 0 ] ), &sp, &cp );
	SinCos( DEG2RAD( angles[ 2 ] ), &sr, &cr );

	forward.x = ( cp * cy );
	forward.y = ( cp * sy );
	forward.z = ( -sp );
	right.x = ( -1 * sr * sp * cy + -1 * cr * -sy );
	right.y = ( -1 * sr * sp * sy + -1 * cr * cy );
	right.z = ( -1 * sr * cp );
	up.x = ( cr * sp * cy + -sr * -sy );
	up.y = ( cr * sp * sy + -sr * cy );
	up.z = ( cr * cp );
}
void Math::VectorAngles( const Vector& forward, QAngle& angles ) {
	float tmp, yaw, pitch;

	if( forward[ 1 ] == 0 && forward[ 0 ] == 0 ) {
		yaw = 0;
		if( forward[ 2 ] > 0 )
			pitch = 270;
		else
			pitch = 90;
	}
	else {
		yaw = ( atan2( forward[ 1 ], forward[ 0 ] ) * 180 / M_PI );
		if( yaw < 0 )
			yaw += 360;

		tmp = sqrt( forward[ 0 ] * forward[ 0 ] + forward[ 1 ] * forward[ 1 ] );
		pitch = ( atan2( -forward[ 2 ], tmp ) * 180 / M_PI );
		if( pitch < 0 )
			pitch += 360;
	}

	angles.x = pitch;
	angles.y = yaw;
	angles.z = 0;
}

void Math::SinCos( float a, float* s, float* c ) {
	*s = sin( a );
	*c = cos( a );
}

void Math::AngleVectors( const QAngle& angles, Vector& forward ) {
	float	sp, sy, cp, cy;

	SinCos( DEG2RAD( angles[ 1 ] ), &sy, &cy );
	SinCos( DEG2RAD( angles[ 0 ] ), &sp, &cp );

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}

float Math::GetFov( const QAngle& viewAngle, const Vector& start, const Vector& end ) {

	Vector dir, fw;

	// get direction and normalize.
	dir = ( end - start ).Normalized( );

	// get the forward direction vector of the view angles.
	AngleVectors( viewAngle, fw );

	// get the angle between the view angles forward directional vector and the target location.
	return std::max( RAD2DEG( std::acos( fw.Dot( dir ) ) ), 0.f );
}

float Math::AngleNormalize( float angle ) {
	if( angle > 180.f || angle < -180.f ) {
		auto revolutions = round( abs( angle / 360.f ) );

		if( angle < 0.f )
			angle = angle + 360.f * revolutions;
		else
			angle = angle - 360.f * revolutions;
	}

	return angle;
}
float Math::ApproachAngle( float target, float value, float speed ) {
	target = ( target * 182.04445f ) * 0.0054931641f;
	value = ( value * 182.04445f ) * 0.0054931641f;

	// Speed is assumed to be positive
	if( speed < 0 )
		speed = -speed;

	float delta = target - value;
	if( delta < -180.0f )
		delta += 360.0f;
	else if( delta > 180.0f )
		delta -= 360.0f;

	if( delta > speed )
		value += speed;
	else if( delta < -speed )
		value -= speed;
	else
		value = target;

	return value;
}
void Math::VectorTransform( const Vector& in1, const matrix3x4_t& in2, Vector& out ) {
	out[ 0 ] = in1.Dot( in2[ 0 ] ) + in2[ 0 ][ 3 ];
	out[ 1 ] = in1.Dot( in2[ 1 ] ) + in2[ 1 ][ 3 ];
	out[ 2 ] = in1.Dot( in2[ 2 ] ) + in2[ 2 ][ 3 ];
}

void Math::SmoothAngle( QAngle src, QAngle& dst, float factor ) {
	QAngle delta = dst - src;

	delta.Normalize( );

	dst = src + delta / factor;
}

QAngle Math::CalcAngle( Vector src, Vector dst, bool bruh ) {
	//xd
	if( bruh ) {
		Vector qAngles;
		Vector delta = Vector( ( src[ 0 ] - dst[ 0 ] ), ( src[ 1 ] - dst[ 1 ] ), ( src[ 2 ] - dst[ 2 ] ) );
		double hyp = std::sqrtf( delta[ 0 ] * delta[ 0 ] + delta[ 1 ] * delta[ 1 ] );
		qAngles[ 0 ] = ( float )( std::atan( delta[ 2 ] / hyp ) * ( 180.0 / M_PI ) );
		qAngles[ 1 ] = ( float )( std::atan( delta[ 1 ] / delta[ 0 ] ) * ( 180.0 / M_PI ) );
		qAngles[ 2 ] = 0.f;
		if( delta[ 0 ] >= 0.f )
			qAngles[ 1 ] += 180.f;

		return QAngle( qAngles[ 0 ], qAngles[ 1 ], qAngles[ 2 ] );
	}
	else {
		QAngle angles;
		Vector delta = src - dst;

		angles = delta.ToEulerAngles( );

		angles.Normalize( );

		return angles;
	}
}

Vector Math::GetSmoothedVelocity( float min_delta, Vector a, Vector b ) {
	Vector delta = a - b;
	float delta_length = delta.Length( );

	if( delta_length <= min_delta ) {
		Vector result;
		if( -min_delta <= delta_length ) {
			return a;
		}
		else {
			float iradius = 1.0f / ( delta_length + FLT_EPSILON );
			return b - ( ( delta * iradius ) * min_delta );
		}
	}
	else {
		float iradius = 1.0f / ( delta_length + FLT_EPSILON );
		return b + ( ( delta * iradius ) * min_delta );
	}
}

float Math::AngleDiff( float src, float dst ) {
	float delta;

	delta = fmodf( dst - src, 360.0f );
	if( dst > src )
	{
		if( delta >= 180 )
			delta -= 360;
	}
	else
	{
		if( delta <= -180 )
			delta += 360;
	}
	return delta;
}

float Math::SmoothStepBounds( float edge0, float edge1, float x ) {
	float v1 = std::clamp( ( x - edge0 ) / ( edge1 - edge0 ), 0.f, 1.f );
	return v1 * v1 * ( 3 - 2 * v1 );
}

float Math::ClampCycle( float flCycleIn ) {
	flCycleIn -= int( flCycleIn );

	if( flCycleIn < 0 ) {
		flCycleIn += 1;
	}
	else if( flCycleIn > 1 ) {
		flCycleIn -= 1;
	}

	return flCycleIn;
}

float Math::Approach( float target, float value, float speed ) {
	float delta = target - value;

	if( delta > speed )
		value += speed;
	else if( delta < -speed )
		value -= speed;
	else
		value = target;

	return value;
}