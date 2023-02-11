#include "vector2d.hpp"

Vector2D::Vector2D( float x, float y ) {
  this->Set( x, y );
}

Vector2D::Vector2D( const Vector2D& v ) {
  this->Set( v.x, v.y );
}

Vector2D::Vector2D( const float* v ) {
  this->Set( v[0], v[1] );
}

void Vector2D::Set( float x /*= 0.0f*/, float y /*= 0.0f */ ) {
  this->x = x;
  this->y = y;
}

float Vector2D::Dot( const Vector2D& v ) const {
  return ( this->x * v.x +
	 this->y * v.y );
}

float Vector2D::LengthSquared( ) const {
  return ( this->Dot( *this ) );
}

float Vector2D::Length( ) const {
  return ( std::sqrt( this->LengthSquared( ) ) );
}

float Vector2D::operator [] ( const std::uint32_t index ) {
  return ( ( ( float* )this )[index] );
}

const float Vector2D::operator [] ( const std::uint32_t index ) const {
  return ( ( ( const float* )this )[index] );
}

Vector2D& Vector2D::operator = ( const Vector2D& v ) {
  this->Set( v.x, v.y );

  return ( *this );
}

Vector2D& Vector2D::operator = ( const float* v ) {
  this->Set( v[0], v[1] );

  return ( *this );
}

Vector2D& Vector2D::operator += ( const Vector2D& v ) {
  this->x += v.x;
  this->y += v.y;

  return ( *this );
}

Vector2D& Vector2D::operator -= ( const Vector2D& v ) {
  this->x -= v.x;
  this->y -= v.y;

  return ( *this );
}

Vector2D& Vector2D::operator *= ( const Vector2D& v ) {
  this->x *= v.x;
  this->y *= v.y;

  return ( *this );
}

Vector2D& Vector2D::operator /= ( const Vector2D& v ) {
  this->x /= v.x;
  this->y /= v.y;

  return ( *this );
}

Vector2D& Vector2D::operator += ( float fl ) {
  this->x += fl;
  this->y += fl;

  return ( *this );
}

Vector2D& Vector2D::operator -= ( float fl ) {
  this->x -= fl;
  this->y -= fl;

  return ( *this );
}

Vector2D& Vector2D::operator *= ( float fl ) {
  this->x *= fl;
  this->y *= fl;

  return ( *this );
}

Vector2D& Vector2D::operator /= ( float fl ) {
  this->x /= fl;
  this->y /= fl;

  return ( *this );
}

bool Vector2D::operator<( const Vector2D& in ) const{
    return ( x < in.x&& y < in.y );
}

bool Vector2D::operator>( const Vector2D& in ) const{
    return ( x > in.x && y > in.y );
}

Vector2D Vector2D::operator + ( const Vector2D& v ) const {
  return { this->x + v.x,
			this->y + v.y };
}

Vector2D Vector2D::operator - ( const Vector2D& v ) const {
  return { this->x - v.x,
			this->y - v.y };
}

Vector2D Vector2D::operator * ( const Vector2D& v ) const {
  return { this->x * v.x,
			this->y * v.y };
}

Vector2D Vector2D::operator / ( const Vector2D& v ) const {
  return { this->x / v.x,
			this->y / v.y };
}

Vector2D Vector2D::operator + ( float fl ) const {
  return { this->x + fl,
			this->y + fl };
}

Vector2D Vector2D::operator - ( float fl ) const {
  return { this->x - fl,
			this->y - fl };
}

Vector2D Vector2D::operator * ( float fl ) const {
  return { this->x * fl,
			this->y * fl };
}

Vector2D Vector2D::operator / ( float fl ) const {
  return { this->x / fl,
			this->y / fl };
}

bool Vector2D::operator==( const Vector2D & v ) const {
  return v.x == x && v.y == y;
}

bool Vector2D::operator==( const float v ) const {
  return x == v && v == y;
}

bool Vector2D::operator!=( const Vector2D & v ) const {
  return v.x != x || v.y != y;
}

bool Vector2D::operator!=( const float v ) const {
  return x != v || v != y;
}

bool Vector2D::operator<( const Vector2D& v ) {
	return { this->x < v.x&&
		this->y < v.y };
}

bool Vector2D::operator>( const Vector2D& v ) {
	return { this->x > v.x &&
		this->y > v.y};
}

bool Vector2D::operator<=( const Vector2D& v ) {
	return { this->x <= v.x &&
		this->y <= v.y };
}

bool Vector2D::operator>=( const Vector2D& v ) {
	return { this->x >= v.x &&
		this->y >= v.y };
}
