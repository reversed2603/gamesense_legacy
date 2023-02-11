#include "matrix.hpp"
#include "vector.hpp"
#include "qangle.hpp"
#include "../Classes/CStudioRender.hpp"

float* matrix3x4_t::operator [] ( const std::uint32_t index ) {
   return m[ index ];
}
const float* matrix3x4_t::operator [] ( const std::uint32_t index ) const {
   return m[ index ];
}

Vector matrix3x4_t::at( const std::uint32_t i ) const {
   return Vector{ m[ 0 ][ i ], m[ 1 ][ i ], m[ 2 ][ i ] };
}

void matrix3x4_t::TransformAABB( const Vector& vecMinsIn, const Vector& vecMaxsIn, Vector& vecMinsOut, Vector& vecMaxsOut ) const {
   Vector localCenter = ( vecMinsIn + vecMaxsIn ) * 0.5f;
   Vector localExtents = ( vecMaxsIn - localCenter );

   Vector worldCenter = localCenter.Transform( *this );

   Vector worldExtents = Vector(
	  std::fabsf( localExtents.Dot( m[ 0 ] ) ),
	  std::fabsf( localExtents.Dot( m[ 1 ] ) ),
	  std::fabsf( localExtents.Dot( m[ 2 ] ) ) );

   vecMinsOut = ( worldCenter - worldExtents );
   vecMaxsOut = ( worldCenter + worldExtents );
}

void matrix3x4_t::AngleMatrix( const QAngle& angles ) {
   float sr, sp, sy, cr, cp, cy;
   DirectX::XMScalarSinCos( &sy, &cy, ToRadians( angles[ 1 ] ) );
   DirectX::XMScalarSinCos( &sp, &cp, ToRadians( angles[ 0 ] ) );
   DirectX::XMScalarSinCos( &sr, &cr, ToRadians( angles[ 2 ] ) );

   // matrix = (YAW * PITCH) * ROLL
   m[ 0 ][ 0 ] = cp * cy;
   m[ 1 ][ 0 ] = cp * sy;
   m[ 2 ][ 0 ] = -sp;

   float crcy = cr * cy;
   float crsy = cr * sy;
   float srcy = sr * cy;
   float srsy = sr * sy;
   m[ 0 ][ 1 ] = sp * srcy - crsy;
   m[ 1 ][ 1 ] = sp * srsy + crcy;
   m[ 2 ][ 1 ] = sr * cp;

   m[ 0 ][ 2 ] = ( sp * crcy + srsy );
   m[ 1 ][ 2 ] = ( sp * crsy - srcy );
   m[ 2 ][ 2 ] = cr * cp;

   m[ 0 ][ 3 ] = 0.0f;
   m[ 1 ][ 3 ] = 0.0f;
   m[ 2 ][ 3 ] = 0.0f;
}

void matrix3x4_t::AngleMatrix( const QAngle& angles, const Vector& position ) {
   AngleMatrix( angles );
   MatrixSetColumn( position, 3 );
}

void matrix3x4_t::MatrixAngles( QAngle& angles ) {
   float forward[ 3 ];
   float left[ 3 ];
   float up[ 3 ];

   //
   // Extract the basis vectors from the matrix. Since we only need the Z
   // component of the up vector, we don't get X and Y.
   //
   forward[ 0 ] = m[ 0 ][ 0 ];
   forward[ 1 ] = m[ 1 ][ 0 ];
   forward[ 2 ] = m[ 2 ][ 0 ];
   left[ 0 ] = m[ 0 ][ 1 ];
   left[ 1 ] = m[ 1 ][ 1 ];
   left[ 2 ] = m[ 2 ][ 1 ];
   up[ 2 ] = m[ 2 ][ 2 ];

   float xyDist = sqrtf( forward[ 0 ] * forward[ 0 ] + forward[ 1 ] * forward[ 1 ] );

   // enough here to get angles?
   if ( xyDist > 0.001f ) {
	  // (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
	  angles.yaw = ToDegrees( atan2f( forward[ 1 ], forward[ 0 ] ) );

	  // (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
	  angles.pitch = ToDegrees( atan2f( -forward[ 2 ], xyDist ) );

	  // (roll)	z = ATAN( left.z, up.z );
	  angles.roll = ToDegrees( atan2f( left[ 2 ], up[ 2 ] ) );
   } else // forward is mostly Z, gimbal lock-
   {
	  // (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
	  angles.yaw = ToDegrees( atan2f( -left[ 0 ], left[ 1 ] ) );

	  // (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
	  angles.pitch = ToDegrees( atan2f( -forward[ 2 ], xyDist ) );

	  // Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
	  angles.roll = 0.f;
   }
}

void matrix3x4_t::MatrixAngles( QAngle& angles, Vector& position ) {
   MatrixAngles( angles );
   MatrixSetColumn( position, 3 );
}

void matrix3x4_t::MatrixSetColumn( const Vector& in, int column ) {
   m[ 0 ][ column ] = in.x;
   m[ 1 ][ column ] = in.y;
   m[ 2 ][ column ] = in.z;
}

void matrix3x4_t::QuaternionMatrix( const Quaternion& q ) {
   // Original code
   // This should produce the same code as below with optimization, but looking at the assmebly,
   // it doesn't.  There are 7 extra multiplies in the release build of this, go figure.
#if 1
   m[ 0 ][ 0 ] = 1.0 - 2.0 * q.y * q.y - 2.0 * q.z * q.z;
   m[ 1 ][ 0 ] = 2.0 * q.x * q.y + 2.0 * q.w * q.z;
   m[ 2 ][ 0 ] = 2.0 * q.x * q.z - 2.0 * q.w * q.y;

   m[ 0 ][ 1 ] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
   m[ 1 ][ 1 ] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
   m[ 2 ][ 1 ] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;

   m[ 0 ][ 2 ] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
   m[ 1 ][ 2 ] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
   m[ 2 ][ 2 ] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;

   m[ 0 ][ 3 ] = 0.0f;
   m[ 1 ][ 3 ] = 0.0f;
   m[ 2 ][ 3 ] = 0.0f;
#else
   float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

   // precalculate common multiplitcations
   x2 = q.x + q.x;
   y2 = q.y + q.y;
   z2 = q.z + q.z;
   xx = q.x * x2;
   xy = q.x * y2;
   xz = q.x * z2;
   yy = q.y * y2;
   yz = q.y * z2;
   zz = q.z * z2;
   wx = q.w * x2;
   wy = q.w * y2;
   wz = q.w * z2;

   matrix[ 0 ][ 0 ] = 1.0 - ( yy + zz );
   matrix[ 0 ][ 1 ] = xy - wz;
   matrix[ 0 ][ 2 ] = xz + wy;
   matrix[ 0 ][ 3 ] = 0.0f;

   matrix[ 1 ][ 0 ] = xy + wz;
   matrix[ 1 ][ 1 ] = 1.0 - ( xx + zz );
   matrix[ 1 ][ 2 ] = yz - wx;
   matrix[ 1 ][ 3 ] = 0.0f;

   matrix[ 2 ][ 0 ] = xz - wy;
   matrix[ 2 ][ 1 ] = yz + wx;
   matrix[ 2 ][ 2 ] = 1.0 - ( xx + yy );
   matrix[ 2 ][ 3 ] = 0.0f;
#endif
}

void matrix3x4_t::QuaternionMatrix( const Quaternion& q, const Vector& pos ) {
   QuaternionMatrix( q );

   this->m[ 0 ][ 3 ] = pos.x;
   this->m[ 1 ][ 3 ] = pos.y;
   this->m[ 2 ][ 3 ] = pos.z;
}

// —œ¿—»¡Œ œ¿œ¿ÿ¿( ƒ∆ŒÕ  ¿–Ã¿  ) «¿ ›“”  ¬¿ Œ¬— ”ﬁ Ã¿“≈Ã¿“» ” 20 ¬≈ ¿ ( Õ” “”“ Õ≈ —œ»«ƒ»À –≈¿À‹ÕŒ 20 ¬≈  )
matrix3x4_t matrix3x4_t::ConcatTransforms( matrix3x4_t in ) const {
   matrix3x4_t out;
   out[ 0 ][ 0 ] = m[ 0 ][ 0 ] * in[ 0 ][ 0 ] + m[ 0 ][ 1 ] * in[ 1 ][ 0 ] + m[ 0 ][ 2 ] * in[ 2 ][ 0 ];
   out[ 0 ][ 1 ] = m[ 0 ][ 0 ] * in[ 0 ][ 1 ] + m[ 0 ][ 1 ] * in[ 1 ][ 1 ] + m[ 0 ][ 2 ] * in[ 2 ][ 1 ];
   out[ 0 ][ 2 ] = m[ 0 ][ 0 ] * in[ 0 ][ 2 ] + m[ 0 ][ 1 ] * in[ 1 ][ 2 ] + m[ 0 ][ 2 ] * in[ 2 ][ 2 ];
   out[ 0 ][ 3 ] = m[ 0 ][ 0 ] * in[ 0 ][ 3 ] + m[ 0 ][ 1 ] * in[ 1 ][ 3 ] + m[ 0 ][ 2 ] * in[ 2 ][ 3 ] + m[ 0 ][ 3 ];
   out[ 1 ][ 0 ] = m[ 1 ][ 0 ] * in[ 0 ][ 0 ] + m[ 1 ][ 1 ] * in[ 1 ][ 0 ] + m[ 1 ][ 2 ] * in[ 2 ][ 0 ];
   out[ 1 ][ 1 ] = m[ 1 ][ 0 ] * in[ 0 ][ 1 ] + m[ 1 ][ 1 ] * in[ 1 ][ 1 ] + m[ 1 ][ 2 ] * in[ 2 ][ 1 ];
   out[ 1 ][ 2 ] = m[ 1 ][ 0 ] * in[ 0 ][ 2 ] + m[ 1 ][ 1 ] * in[ 1 ][ 2 ] + m[ 1 ][ 2 ] * in[ 2 ][ 2 ];
   out[ 1 ][ 3 ] = m[ 1 ][ 0 ] * in[ 0 ][ 3 ] + m[ 1 ][ 1 ] * in[ 1 ][ 3 ] + m[ 1 ][ 2 ] * in[ 2 ][ 3 ] + m[ 1 ][ 3 ];
   out[ 2 ][ 0 ] = m[ 2 ][ 0 ] * in[ 0 ][ 0 ] + m[ 2 ][ 1 ] * in[ 1 ][ 0 ] + m[ 2 ][ 2 ] * in[ 2 ][ 0 ];
   out[ 2 ][ 1 ] = m[ 2 ][ 0 ] * in[ 0 ][ 1 ] + m[ 2 ][ 1 ] * in[ 1 ][ 1 ] + m[ 2 ][ 2 ] * in[ 2 ][ 1 ];
   out[ 2 ][ 2 ] = m[ 2 ][ 0 ] * in[ 0 ][ 2 ] + m[ 2 ][ 1 ] * in[ 1 ][ 2 ] + m[ 2 ][ 2 ] * in[ 2 ][ 2 ];
   out[ 2 ][ 3 ] = m[ 2 ][ 0 ] * in[ 0 ][ 3 ] + m[ 2 ][ 1 ] * in[ 1 ][ 3 ] + m[ 2 ][ 2 ] * in[ 2 ][ 3 ] + m[ 2 ][ 3 ];
   return out;
}

Vector matrix3x4_t::operator*( const Vector& vVec ) const {
   Vector vRet;
   vRet.x = m[ 0 ][ 0 ] * vVec.x + m[ 0 ][ 1 ] * vVec.y + m[ 0 ][ 2 ] * vVec.z + m[ 0 ][ 3 ];
   vRet.y = m[ 1 ][ 0 ] * vVec.x + m[ 1 ][ 1 ] * vVec.y + m[ 1 ][ 2 ] * vVec.z + m[ 1 ][ 3 ];
   vRet.z = m[ 2 ][ 0 ] * vVec.x + m[ 2 ][ 1 ] * vVec.y + m[ 2 ][ 2 ] * vVec.z + m[ 2 ][ 3 ];

   return vRet;
}

matrix3x4_t matrix3x4_t::operator+( const matrix3x4_t& other ) const {
   matrix3x4_t ret;
   for ( int i = 0; i < 12; i++ ) {
	  ( ( float* ) ret.m )[ i ] = ( ( float* ) m )[ i ] + ( ( float* ) other.m )[ i ];
   }
   return ret;
}

matrix3x4_t matrix3x4_t::operator-( const matrix3x4_t& other ) const {
   matrix3x4_t ret;
   for ( int i = 0; i < 12; i++ ) {
	  ( ( float* ) ret.m )[ i ] = ( ( float* ) m )[ i ] - ( ( float* ) other.m )[ i ];
   }
   return ret;
}

matrix3x4_t matrix3x4_t::operator*( const float& other ) const {
   matrix3x4_t ret;
   for ( int i = 0; i < 12; i++ ) {
	  ( ( float* ) ret.m )[ i ] = ( ( float* ) m )[ i ] * other;
   }
   return ret;
}

inline void MatrixGetColumn( const matrix3x4_t& src, int nCol, Vector& pColumn ) {
   pColumn.x = src[ 0 ][ nCol ];
   pColumn.y = src[ 1 ][ nCol ];
   pColumn.z = src[ 2 ][ nCol ];
}

inline void MatrixPosition( const matrix3x4_t& matrix, Vector& position ) {
   MatrixGetColumn( matrix, 3, position );
}

float* VMatrix::operator [] ( const std::uint32_t index ) {
   return m[ index ];
}

const float* VMatrix::operator [] ( const std::uint32_t index ) const {
   return m[ index ];
}
