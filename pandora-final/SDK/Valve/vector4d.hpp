#pragma once

#include "../core.hpp"

class Vector4D
{
public:
  Vector4D( );
  Vector4D( float x, float y, float z, float w );
  Vector4D( const Vector4D& v );
  Vector4D( const float* v );
  Vector4D( Vector2D a, Vector2D b );
  Vector4D( Vector4D&& a );

public:
  void Set( float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f );

  float Dot( const Vector4D& v ) const;

  float LengthSquared( ) const;
  float Length( ) const;

  float operator[]( int i ) const;
  float& operator[]( int i );

  float    Height() const { return this->bottom - this->top; }
  float    Width()  const { return this->right - this->left; }
  Vector2D Pos()    const { return Vector2D(left, top);        }   
  Vector2D Mid()    const { return Vector2D((left + right) / 2, (top + bottom) / 2); }
  Vector2D Section()    const { return Vector2D((left + 14), (top)); }
  Vector2D Button()    const { return Vector2D((left + 10), (top + bottom) / 2); }

  const bool ContainsPoint(const Vector2D& pt) const
  {
	 const auto tmp = *this; /* Fixes some weird bux I had */
	 if (tmp.top    > pt.y) return false;
	 if (tmp.bottom < pt.y) return false;
	 if (tmp.left   > pt.x) return false;
	 if (tmp.right  < pt.x) return false;
	 return true;
  }

public:
  Vector4D& operator += ( const Vector4D& v );
  Vector4D& operator -= ( const Vector4D& v );
  Vector4D& operator *= ( const Vector4D& v );
  Vector4D& operator /= ( const Vector4D& v );

  Vector4D& operator += ( float fl );
  Vector4D& operator -= ( float fl );
  Vector4D& operator *= ( float fl );
  Vector4D& operator /= ( float fl );

  Vector4D operator + ( const Vector4D& v ) const;
  Vector4D operator - ( const Vector4D& v ) const;
  Vector4D operator * ( const Vector4D& v ) const;
  Vector4D operator / ( const Vector4D& v ) const;

  Vector4D operator + ( float fl ) const;
  Vector4D operator - ( float fl ) const;
  Vector4D operator * ( float fl ) const;
  Vector4D operator / ( float fl ) const;

  Vector4D& operator=( const Vector4D& vOther );

public:
  // Members
  union {
	 // if vector is rectangle
	 struct {
		float left;
		float top;

		float right;
		float bottom;
	 };

	 struct {
		Vector2D min;
		Vector2D max;
	 };

	 struct {
		float x;
		float y;
		float z;
		float w;
	 };
  };
};
