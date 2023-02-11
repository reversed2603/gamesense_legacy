#include "vector.hpp"
#include "vector2d.hpp"
#include "vector4d.hpp"
#include "qangle.hpp"
#include "matrix.hpp"

Vector::Vector( ) {
	this->Set( );
}

Vector::Vector( float x, float y, float z ) {
	this->Set( x, y, z );
}

Vector::Vector( const Vector& v ) {
	this->Set( v.x, v.y, v.z );
}

Vector::Vector( const float* v ) {
	this->Set( v[ 0 ], v[ 1 ], v[ 2 ] );
}

void Vector::Set( float x /*= 0.0f*/, float y /*= 0.0f */, float z /*= 0.0f*/ ) {
	this->x = x;
	this->y = y;
	this->z = z;
}

bool Vector::IsZero( float tolerance /*= 0.01f */ ) const {
	return ( this->x > -tolerance && this->x < tolerance&&
		this->y > -tolerance && this->y < tolerance&&
		this->z > -tolerance && this->z < tolerance );
}

bool Vector::VectorsAreEqual( const Vector& other, float tolerance ) const {
	if( std::fabsf( this->x - other.x ) > tolerance )
		return false;
	if( std::fabsf( this->y - other.y ) > tolerance )
		return false;
	return ( std::fabsf( this->z - other.z ) <= tolerance );
}

float Vector::Normalize( ) {
	auto length = this->Length( );
	if( length > 0.f ) {
		( *this ) /= length;
	}
	return length;
}

float Vector::Dot( const Vector& v ) const {
	return ( this->x * v.x +
		this->y * v.y +
		this->z * v.z );
}

float Vector::LengthSquared( ) const {
	return ( this->Dot( *this ) );
}

float Vector::Length( ) const {
	return ( std::sqrt( this->LengthSquared( ) ) );
}

float Vector::Length2DSquared( ) const {
	return ( x * x + y * y );
}

float Vector::Length2D( ) const {
	return std::sqrt( x * x + y * y );
}

float Vector::DistanceSquared( const Vector& v ) const {
	return ( ( *this - v ).LengthSquared( ) );
}

float Vector::Distance( const Vector& v ) const {
	return ( ( *this - v ).Length( ) );
}

Vector Vector::Transform( const matrix3x4_t& in2) const {
	//return { Dot( transform[ 0 ] ) + transform[ 0 ][ 3 ], Dot( transform[ 1 ] ) + transform[ 1 ][ 3 ], Dot( transform[ 2 ] ) + transform[ 2 ][ 3 ] };
	return Vector(Dot(Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3],
		Dot(Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3],
		Dot(Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3]);
}

Vector Vector::Normalized( ) const {
	Vector copy( *this );
	copy.Normalize( );
	return copy;
}

Vector Vector::Cross( const Vector& v ) const {
	return { this->y * v.z - this->z * v.y,
		this->z * v.x - this->x * v.z,
		this->x * v.y - this->y * v.x };
}

Vector2D Vector::ToVector2D( ) {
	return { this->x, this->y };
}

Vector4D Vector::ToVector4D( float w /*= 0.0f */ ) {
	return { this->x, this->y, this->z, w };
}

QAngle Vector::ToEulerAngles( Vector* pseudoup /*= nullptr*/ ) {
	auto pitch = 0.0f;
	auto yaw = 0.0f;
	auto roll = 0.0f;

	auto length = this->ToVector2D( ).Length( );

	if( pseudoup ) {
		auto left = pseudoup->Cross( *this );

		left.Normalize( );

		pitch = ToDegrees( std::atan2( -this->z, length ) );

		if( pitch < 0.0f )
			pitch += 360.0f;

		if( length > 0.001f ) {
			yaw = ToDegrees( std::atan2( this->y, this->x ) );

			if( yaw < 0.0f )
				yaw += 360.0f;

			auto up_z = ( this->x * left.y ) - ( this->y * left.x );

			roll = ToDegrees( std::atan2( left.z, up_z ) );

			if( roll < 0.0f )
				roll += 360.0f;
		}
		else {
			yaw = ToDegrees( std::atan2( -left.x, left.y ) );

			if( yaw < 0.0f )
				yaw += 360.0f;
		}
	}
	else {
		if( this->x == 0.0f && this->y == 0.0f ) {
			if( this->z > 0.0f )
				pitch = 270.0f;
			else
				pitch = 90.0f;
		}
		else {
			pitch = ToDegrees( std::atan2( -this->z, length ) );

			if( pitch < 0.0f )
				pitch += 360.0f;

			yaw = ToDegrees( std::atan2( this->y, this->x ) );

			if( yaw < 0.0f )
				yaw += 360.0f;
		}
	}

	return { pitch, yaw, roll };
}

Vector Vector::Min( const Vector& other ) {
	return Vector( x < other.x ? x : other.x, y < other.y ? y : other.y, z < other.z ? z : other.z );
}

Vector Vector::Max( const Vector& other ) {
	return Vector( x > other.x ? x : other.x, y > other.y ? y : other.y, z > other.z ? z : other.z );
}

void Vector::GetVectors( Vector& right, Vector& up ) { // VectorVectors
	Vector tmp;
	if( x == 0.f && y == 0.f ) {
		// pitch 90 degrees up/down from identity
		right[ 0 ] = 0.f;
		right[ 1 ] = -1.f;
		right[ 2 ] = 0.f;
		up[ 0 ] = -z;
		up[ 1 ] = 0.f;
		up[ 2 ] = 0.f;
	}
	else {
		tmp[ 0 ] = 0.f;
		tmp[ 1 ] = 0.f;
		tmp[ 2 ] = 1.0f;
		right = Cross( tmp );
		up = right.Cross( *this );

		right.Normalize( );
		up.Normalize( );
	}
}

Vector& Vector::operator = ( const Vector& v ) {
	this->Set( v.x, v.y, v.z );

	return ( *this );
}

Vector& Vector::operator = ( const float* v ) {
	this->Set( v[ 0 ], v[ 1 ], v[ 2 ] );

	return ( *this );
}

Vector& Vector::operator += ( const Vector& v ) {
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;

	return ( *this );
}

Vector& Vector::operator -= ( const Vector& v ) {
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;

	return ( *this );
}

Vector& Vector::operator *= ( const Vector& v ) {
	this->x *= v.x;
	this->y *= v.y;
	this->z *= v.z;

	return ( *this );
}

Vector& Vector::operator /= ( const Vector& v ) {
	this->x /= v.x;
	this->y /= v.y;
	this->z /= v.z;

	return ( *this );
}

Vector& Vector::operator += ( float fl ) {
	this->x += fl;
	this->y += fl;
	this->z += fl;

	return ( *this );
}

Vector& Vector::operator -= ( float fl ) {
	this->x -= fl;
	this->y -= fl;
	this->z -= fl;

	return ( *this );
}

Vector& Vector::operator *= ( float fl ) {
	this->x *= fl;
	this->y *= fl;
	this->z *= fl;

	return ( *this );
}

Vector& Vector::operator /= ( float fl ) {
	this->x /= fl;
	this->y /= fl;
	this->z /= fl;

	return ( *this );
}

Vector Vector::operator + ( const Vector& v ) const {
	return { this->x + v.x,
		this->y + v.y,
		this->z + v.z };
}

Vector Vector::operator - ( const Vector& v ) const {
	return { this->x - v.x,
		this->y - v.y,
		this->z - v.z };
}

Vector Vector::operator * ( const Vector& v ) const {
	return { this->x * v.x,
		this->y * v.y,
		this->z * v.z };
}

Vector Vector::operator / ( const Vector& v ) const {
	return { this->x / v.x,
		this->y / v.y,
		this->z / v.z };
}

Vector Vector::operator + ( float fl ) const {
	return { this->x + fl,
		this->y + fl,
		this->z + fl };
}

Vector Vector::operator - ( float fl ) const {
	return { this->x - fl,
		this->y - fl,
		this->z - fl };
}

Vector Vector::operator * ( float fl ) const {
	return { this->x * fl,
		this->y * fl,
		this->z * fl };
}

Vector Vector::operator / ( float fl ) const {
	return { this->x / fl,
		this->y / fl,
		this->z / fl };
}

bool Vector::operator < ( const Vector& v ) {
	return { this->x < v.x&&
		this->y < v.y&&
		this->z < v.z };
}

bool Vector::operator > ( const Vector& v ) {
	return { this->x > v.x &&
		this->y > v.y &&
		this->z > v.z };
}

bool Vector::operator<=( const Vector& v ) {
	return { this->x <= v.x &&
		this->y <= v.y &&
		this->z <= v.z };
}

bool Vector::operator>=( const Vector& v ) {
	return { this->x >= v.x &&
		this->y >= v.y &&
		this->z >= v.z };
}

Vector Vector::Zero( 0.0f, 0.0f, 0.0f );
