#pragma once

#include "../core.hpp"

class Vector2D
{
public:
  Vector2D( float x = 0.0f, float y = 0.0f );
  Vector2D( const Vector2D& v );
  Vector2D( const float* v );

public:
  void Set( float x = 0.0f, float y = 0.0f );

  float Dot( const Vector2D& v ) const;

  float LengthSquared( ) const;
  float Length( ) const;

public:
  float operator [] ( const std::uint32_t index );
  const float operator [] ( const std::uint32_t index ) const;

  Vector2D& operator = ( const Vector2D& v );
  Vector2D& operator = ( const float* v );

  Vector2D& operator += ( const Vector2D& v );
  Vector2D& operator -= ( const Vector2D& v );
  Vector2D& operator *= ( const Vector2D& v );
  Vector2D& operator /= ( const Vector2D& v );

  Vector2D& operator += ( float fl );
  Vector2D& operator -= ( float fl );
  Vector2D& operator *= ( float fl );
  Vector2D& operator /= ( float fl );

  bool operator<( const Vector2D& in ) const;

  bool operator>( const Vector2D& in ) const;

  Vector2D operator + ( const Vector2D& v ) const;
  Vector2D operator - ( const Vector2D& v ) const;
  Vector2D operator * ( const Vector2D& v ) const;
  Vector2D operator / ( const Vector2D& v ) const;

  Vector2D operator + ( float fl ) const;
  Vector2D operator - ( float fl ) const;
  Vector2D operator * ( float fl ) const;
  Vector2D operator / ( float fl ) const;

  bool operator == ( const Vector2D& v ) const;
  bool operator == ( const float v )const;

  bool operator != ( const Vector2D& v ) const;
  bool operator != ( const float v )const;

  bool operator < ( const Vector2D& v );
  bool operator > ( const Vector2D& v );
  bool operator <= ( const Vector2D& v );
  bool operator >= ( const Vector2D& v );

public:
  float x = 0.0f;
  float y = 0.0f;
};
