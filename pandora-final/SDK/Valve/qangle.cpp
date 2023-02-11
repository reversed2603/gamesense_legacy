#include "qangle.hpp"
#include "vector.hpp"

QAngle::QAngle( ) {
   this->Set( );
}

QAngle::QAngle( float x, float y, float z ) {
   this->Set( x, y, z );
}

QAngle::QAngle( const QAngle& v ) {
   this->Set( v.x, v.y, v.z );
}

QAngle::QAngle( const float* v ) {
   this->Set( v[ 0 ], v[ 1 ], v[ 2 ] );
}

void QAngle::Set( float x /*= 0.0f*/, float y /*= 0.0f */, float z /*= 0.0f*/ ) {
   this->x = x;
   this->y = y;
   this->z = z;
}

void QAngle::Normalize( ) {
   AngleNormalize( this->x );
   AngleNormalize( this->y );
   AngleNormalize( this->z );
}

void QAngle::Clamp( ) {
   Normalize( );

   LimitValue( this->x, -89.0f, 89.0f );
   LimitValue( this->y, -180.0f, 180.0f );
   LimitValue( this->z, -50.0f, 50.0f );
}

bool QAngle::IsZero( float tolerance /*= 0.01f */ ) {
   return ( this->x > -tolerance && this->x < tolerance &&
	  this->y > -tolerance && this->y < tolerance &&
	  this->z > -tolerance && this->z < tolerance );
}

bool QAngle::AngleAreEqual( QAngle angle, float tolerance ) {
   if ( std::fabsf( this->x - angle.x ) > tolerance )
	  return false;
   if ( std::fabsf( this->y - angle.y ) > tolerance )
	  return false;
   return ( std::fabsf( this->z - angle.z ) <= tolerance );
}

QAngle QAngle::Normalized( ) {
   QAngle copy( *this );
   copy.Normalize( );
   return copy;
}

QAngle QAngle::Clamped( ) {
   QAngle copy( *this );
   copy.Clamp( );
   return copy;
}

Vector QAngle::ToVectors( Vector* side /*= nullptr*/, Vector* up /*= nullptr*/ ) {
   float  rad_pitch = ToRadians( this->x );
   float  rad_yaw = ToRadians( this->y );
   float sin_pitch;
   float sin_yaw;
   float sin_roll;
   float cos_pitch;
   float cos_yaw;
   float cos_roll;

   DirectX::XMScalarSinCos( &sin_pitch, &cos_pitch, rad_pitch );
   DirectX::XMScalarSinCos( &sin_yaw, &cos_yaw, rad_yaw );

   if ( side || up )
	  DirectX::XMScalarSinCos( &sin_roll, &cos_roll, ToRadians( this->z ) );

   if ( side ) {
	  side->x = -1.0f * sin_roll * sin_pitch * cos_yaw + -1.0f * cos_roll * -sin_yaw;
	  side->y = -1.0f * sin_roll * sin_pitch * sin_yaw + -1.0f * cos_roll * cos_yaw;
	  side->z = -1.0f * sin_roll * cos_pitch;
   }

   if ( up ) {
	  up->x = cos_roll * sin_pitch * cos_yaw + -sin_roll * -sin_yaw;
	  up->y = cos_roll * sin_pitch * sin_yaw + -sin_roll * cos_yaw;
	  up->z = cos_roll * cos_pitch;
   }

   return { cos_pitch * cos_yaw, cos_pitch * sin_yaw, -sin_pitch };
}

Vector QAngle::ToVectorsTranspose( Vector* side /*= nullptr*/, Vector* up /*= nullptr*/ ) {
   auto rad_pitch = ToRadians( this->x );
   auto rad_yaw = ToRadians( this->y );
   auto rad_roll = ToRadians( this->z );

   auto sin_pitch = std::sin( rad_pitch );
   auto sin_yaw = std::sin( rad_yaw );
   auto sin_roll = std::sin( rad_roll );

   auto cos_pitch = std::cos( rad_pitch );
   auto cos_yaw = std::cos( rad_yaw );
   auto cos_roll = std::cos( rad_roll );

   if ( side ) {
	  side->x = cos_pitch * sin_yaw;
	  side->y = sin_roll * sin_pitch * sin_yaw + cos_roll * cos_yaw;
	  side->z = cos_roll * sin_pitch * sin_yaw + -sin_roll * cos_yaw;
   }

   if ( up ) {
	  up->x = -sin_pitch;
	  up->y = sin_roll * cos_pitch;
	  up->z = cos_roll * cos_pitch;
   }

   return { cos_pitch * cos_yaw,
			 sin_roll * sin_pitch * cos_yaw + cos_roll * -sin_yaw,
			 cos_roll * sin_pitch * cos_yaw + -sin_roll * -sin_yaw };
}

float QAngle::operator [] ( const std::uint32_t index ) {
   return ( ( ( float* ) this )[ index ] );
}

const float QAngle::operator [] ( const std::uint32_t index ) const {
   return ( ( ( const float* ) this )[ index ] );
}

QAngle& QAngle::operator = ( const QAngle& v ) {
   this->Set( v.x, v.y, v.z );

   return ( *this );
}

QAngle& QAngle::operator = ( const float* v ) {
   this->Set( v[ 0 ], v[ 1 ], v[ 2 ] );

   return ( *this );
}

QAngle& QAngle::operator += ( const QAngle& v ) {
   this->x += v.x;
   this->y += v.y;
   this->z += v.z;

   return ( *this );
}

QAngle& QAngle::operator -= ( const QAngle& v ) {
   this->x -= v.x;
   this->y -= v.y;
   this->z -= v.z;

   return ( *this );
}

QAngle& QAngle::operator *= ( const QAngle& v ) {
   this->x *= v.x;
   this->y *= v.y;
   this->z *= v.z;

   return ( *this );
}

QAngle& QAngle::operator /= ( const QAngle& v ) {
   this->x /= v.x;
   this->y /= v.y;
   this->z /= v.z;

   return ( *this );
}

QAngle& QAngle::operator += ( float fl ) {
   this->x += fl;
   this->y += fl;
   this->z += fl;

   return ( *this );
}

QAngle& QAngle::operator -= ( float fl ) {
   this->x -= fl;
   this->y -= fl;
   this->z -= fl;

   return ( *this );
}

QAngle& QAngle::operator *= ( float fl ) {
   this->x *= fl;
   this->y *= fl;
   this->z *= fl;

   return ( *this );
}

QAngle& QAngle::operator /= ( float fl ) {
   this->x /= fl;
   this->y /= fl;
   this->z /= fl;

   return ( *this );
}

QAngle QAngle::operator + ( const QAngle& v ) const {
   return { this->x + v.x,
			 this->y + v.y,
			 this->z + v.z };
}

QAngle QAngle::operator - ( const QAngle& v ) const {
   return { this->x - v.x,
			 this->y - v.y,
			 this->z - v.z };
}

QAngle QAngle::operator * ( const QAngle& v ) const {
   return { this->x * v.x,
			 this->y * v.y,
			 this->z * v.z };
}

QAngle QAngle::operator / ( const QAngle& v ) const {
   return { this->x / v.x,
			 this->y / v.y,
			 this->z / v.z };
}

QAngle QAngle::operator + ( float fl ) const {
   return { this->x + fl,
			 this->y + fl,
			 this->z + fl };
}

QAngle QAngle::operator - ( float fl ) const {
   return { this->x - fl,
			 this->y - fl,
			 this->z - fl };
}

QAngle QAngle::operator * ( float fl ) const {
   return { this->x * fl,
			 this->y * fl,
			 this->z * fl };
}

QAngle QAngle::operator / ( float fl ) const {
   return { this->x / fl,
			 this->y / fl,
			 this->z / fl };
}

QAngle QAngle::Zero( 0.0f, 0.0f, 0.0f );
