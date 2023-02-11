#pragma once

#include "../core.hpp"

class Vector
{
public:
  Vector( );
  Vector( float x, float y, float z );
  Vector( const Vector& v );
  Vector( const float* v );

  void Init( float ix = 0.0f, float iy = 0.0f, float iz = 0.0f ) {
	 x = ix; y = iy; z = iz;
  }

  FORCEINLINE void ValidateVector()
  {
	  if (std::isnan(this->x)
		  || std::isnan(this->y)
		  || std::isnan(this->z))
		  this->Set();

	  if (std::isinf(this->x)
		  || std::isinf(this->y)
		  || std::isinf(this->z))
		  this->Set();
  }

public:
  void Set( float x = 0.0f, float y = 0.0f, float z = 0.0f );

  bool IsZero( float tolerance = 0.01f ) const;
  bool VectorsAreEqual( const Vector& a, float tolerance = 0.01f ) const;

  float Normalize( );

  float Dot( const Vector& v ) const;

  float LengthSquared( ) const;
  float Length( ) const;

  float Length2DSquared( ) const;
  float Length2D( ) const;

  float DistanceSquared( const Vector& vector ) const;
  float Distance( const Vector& vector ) const;

  Vector Transform( const matrix3x4_t& transform ) const;

  Vector Normalized( ) const;
  Vector Cross( const Vector& v ) const;

  Vector2D ToVector2D( );
  Vector4D ToVector4D( float w = 0.0f );

  QAngle ToEulerAngles( Vector* pseudoup = nullptr );

  Vector Min( const Vector& other );
  Vector Max( const Vector& other );

  __inline void MulAdd( const Vector& a, const Vector& b, float scalar ) {
	 x = a.x + b.x * scalar;
  }

  __inline void Mul( float scalar ) {
	 x *= scalar;
	 y *= scalar;
	 z *= scalar;
  }

  void GetVectors( Vector& right, Vector& up );

public:
  float &operator[]( int i ) {
	 return ( ( float* )this )[i];
  }

  float operator[]( int i ) const {
	 return ( ( float* )this )[i];
  }

  bool operator==( const Vector& src ) const {
	 return ( src.x == x ) && ( src.y == y ) && ( src.z == z );
  }
  bool operator!=( const Vector& src ) const {
	 return ( src.x != x ) || ( src.y != y ) || ( src.z != z );
  }

  Vector& operator = ( const Vector& v );
  Vector& operator = ( const float* v );

  Vector& operator += ( const Vector& v );
  Vector& operator -= ( const Vector& v );
  Vector& operator *= ( const Vector& v );
  Vector& operator /= ( const Vector& v );

  Vector& operator += ( float fl );
  Vector& operator -= ( float fl );
  Vector& operator *= ( float fl );
  Vector& operator /= ( float fl );

  Vector operator + ( const Vector& v ) const;
  Vector operator - ( const Vector& v ) const;
  Vector operator * ( const Vector& v ) const;
  Vector operator / ( const Vector& v ) const;

  Vector operator + ( float fl ) const;
  Vector operator - ( float fl ) const;
  Vector operator * ( float fl ) const;
  Vector operator / ( float fl ) const;

  bool operator < ( const Vector& v );
  bool operator > ( const Vector& v );
  bool operator <= ( const Vector& v );
  bool operator >= ( const Vector& v );

public:
  static Vector Zero;

public:
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
};

class __declspec( align( 16 ) ) VectorAligned : public Vector
{

public:

  inline VectorAligned( void ) { };
  inline VectorAligned( float X, float Y, float Z ) {
	 Init( X, Y, Z );
  }

public:

  explicit VectorAligned( const Vector &vOther ) {
	 Init( vOther.x, vOther.y, vOther.z );
  }

  VectorAligned &operator=( const Vector &vOther ) {
	 Init( vOther.x, vOther.y, vOther.z );
	 return *this;
  }

  VectorAligned &operator=( const VectorAligned &vOther ) {
	 Init( vOther.x, vOther.y, vOther.z );
	 return *this;
  }

  float w;
};
