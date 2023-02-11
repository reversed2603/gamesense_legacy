#include "vector2d.hpp"
#include "vector4d.hpp"

Vector4D::Vector4D( ) {
  Set( );
}

Vector4D::Vector4D( float x, float y, float z, float w ) {
  Set( x, y, z, w );
}

Vector4D::Vector4D( const Vector4D& v ) {
  Set( v.x, v.y, v.z, v.w );
}

Vector4D::Vector4D( const float* v ) {
  Set( v[0], v[1], v[2], v[3] );
}

Vector4D::Vector4D( Vector2D a, Vector2D b ) {
  min = a;
  max = b;
}

void Vector4D::Set( float x /*= 0.0f*/, float y /*= 0.0f */, float z /*= 0.0f*/, float w /*= 0.0f*/ ) {
  this->x = x;
  this->y = y;
  this->z = z;
  this->w = w;
}

float Vector4D::Dot( const Vector4D& v ) const {
  return ( this->x * v.x +
	 this->y * v.y +
	 this->z * v.z +
	 this->w * v.w );
}

float Vector4D::LengthSquared( ) const {
  return ( this->Dot( *this ) );
}

float Vector4D::Length( ) const {
  return ( std::sqrt( this->LengthSquared( ) ) );
}

float Vector4D::operator[]( int i ) const {
  return ( ( float* )this )[i];
}

float & Vector4D::operator[]( int i ) {
  return ( ( float* )this )[i];
}

Vector4D& Vector4D::operator += ( const Vector4D& v ) {
  this->x += v.x;
  this->y += v.y;
  this->z += v.z;
  this->w += v.w;

  return ( *this );
}

Vector4D& Vector4D::operator -= ( const Vector4D& v ) {
  this->x -= v.x;
  this->y -= v.y;
  this->z -= v.z;
  this->w -= v.w;

  return ( *this );
}

Vector4D& Vector4D::operator *= ( const Vector4D& v ) {
  this->x *= v.x;
  this->y *= v.y;
  this->z *= v.z;
  this->w *= v.w;

  return ( *this );
}

Vector4D& Vector4D::operator /= ( const Vector4D& v ) {
  this->x /= v.x;
  this->y /= v.y;
  this->z /= v.z;
  this->w /= v.w;

  return ( *this );
}

Vector4D& Vector4D::operator += ( float fl ) {
  this->x += fl;
  this->y += fl;
  this->z += fl;
  this->w += fl;

  return ( *this );
}

Vector4D& Vector4D::operator -= ( float fl ) {
  this->x -= fl;
  this->y -= fl;
  this->z -= fl;
  this->w -= fl;

  return ( *this );
}

Vector4D& Vector4D::operator *= ( float fl ) {
  this->x *= fl;
  this->y *= fl;
  this->z *= fl;
  this->w *= fl;

  return ( *this );
}

Vector4D& Vector4D::operator /= ( float fl ) {
  this->x /= fl;
  this->y /= fl;
  this->z /= fl;
  this->w /= fl;

  return ( *this );
}

Vector4D Vector4D::operator + ( const Vector4D& v ) const {
  return { this->x + v.x,
			this->y + v.y,
			this->z + v.z,
			this->w + v.w };
}

Vector4D Vector4D::operator - ( const Vector4D& v ) const {
  return { this->x - v.x,
			this->y - v.y,
			this->z - v.z,
			this->w - v.w };
}

Vector4D Vector4D::operator * ( const Vector4D& v ) const {
  return { this->x * v.x,
			this->y * v.y,
			this->z * v.z,
			this->w * v.w };
}

Vector4D Vector4D::operator / ( const Vector4D& v ) const {
  return { this->x / v.x,
			this->y / v.y,
			this->z / v.z,
			this->w / v.w };
}

Vector4D Vector4D::operator + ( float fl ) const {
  return { this->x + fl,
			this->y + fl,
			this->z + fl,
			this->w + fl };
}

Vector4D Vector4D::operator - ( float fl ) const {
  return { this->x - fl,
			this->y - fl,
			this->z - fl,
			this->w - fl };
}

Vector4D Vector4D::operator * ( float fl ) const {
  return { this->x * fl,
			this->y * fl,
			this->z * fl,
			this->w * fl };
}

Vector4D Vector4D::operator / ( float fl ) const {
  return { this->x / fl,
			this->y / fl,
			this->z / fl,
			this->w / fl };
}

Vector4D & Vector4D::operator=( const Vector4D & vOther ) {
  x = vOther.x;
  y = vOther.y;
  z = vOther.z;
  w = vOther.w;
  return *this;
}
