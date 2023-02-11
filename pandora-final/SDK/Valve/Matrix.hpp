#pragma once

#include "../core.hpp"
class Quaternion;
class Vector;
class matrix3x4_t {
public:
   float m[ 3 ][ 4 ] = { };

   float* operator [] ( const std::uint32_t index );
   const float* operator [] ( const std::uint32_t index ) const;

   Vector at( const std::uint32_t i ) const;

   void TransformAABB( const Vector& vecMinsIn, const Vector& vecMaxsIn, Vector& vecMinsOut, Vector& vecMaxsOut ) const;
   void AngleMatrix( const QAngle& angles );
   void AngleMatrix( const QAngle& angles, const Vector& position );
   void MatrixAngles( QAngle& angles );
   void MatrixAngles( QAngle& angles, Vector& position );
   void MatrixSetColumn( const Vector& in, int column );
   void QuaternionMatrix( const Quaternion& q );
   void QuaternionMatrix( const Quaternion& q, const Vector& pos );
   matrix3x4_t ConcatTransforms( matrix3x4_t in ) const;

   matrix3x4_t( ) { }
   matrix3x4_t(
	  float m00, float m01, float m02, float m03,
	  float m10, float m11, float m12, float m13,
	  float m20, float m21, float m22, float m23 ) {
	  m[ 0 ][ 0 ] = m00; m[ 0 ][ 1 ] = m01; m[ 0 ][ 2 ] = m02; m[ 0 ][ 3 ] = m03;
	  m[ 1 ][ 0 ] = m10; m[ 1 ][ 1 ] = m11; m[ 1 ][ 2 ] = m12; m[ 1 ][ 3 ] = m13;
	  m[ 2 ][ 0 ] = m20; m[ 2 ][ 1 ] = m21; m[ 2 ][ 2 ] = m22; m[ 2 ][ 3 ] = m23;
   }

   Vector operator*( const Vector& vVec ) const;
   matrix3x4_t matrix3x4_t::operator+( const matrix3x4_t& other ) const;
   matrix3x4_t matrix3x4_t::operator-( const matrix3x4_t& other ) const;
   matrix3x4_t matrix3x4_t::operator*( const float& other ) const;
   matrix3x4_t matrix3x4_t::operator*( const matrix3x4_t& vm ) const {
	  return ConcatTransforms( vm );
   }
};

class VMatrix {
public:
   float* operator [] ( const std::uint32_t index );
   const float* operator [] ( const std::uint32_t index ) const;

public:
   float m[ 4 ][ 4 ] = { };
};
