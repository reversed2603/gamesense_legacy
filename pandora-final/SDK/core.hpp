#pragma once

#include "Utils/auto.hpp"
#include <DirectXMath.h> // beste trigonometry header

constexpr auto RadPi = 3.14159265358979323846;
constexpr auto DegPi = 180.0;

class Vector2D;
class Vector;
class Vector4D;

class QAngle;

class matrix3x4_t;
class VMatrix;

template<typename T>
T Square( T value ) {
  return ( value * value );
}

template<typename T>
void LimitValue( T& value, const T& min, const T& max ) {
  if( value > max )
	 value = max;
  else if( value < min )
	 value = min;
}

template<typename T>
T ToRadians( T degrees ) {
  return ( degrees * ( static_cast< T >( RadPi ) / static_cast< T >( DegPi ) ) );
}

template<typename T>
T ToDegrees( T radians ) {
  return ( radians * ( static_cast< T >( DegPi ) / static_cast< T >( RadPi ) ) );
}

void FORCEINLINE AngleNormalize( float& angle ) {
  angle = std::fmod( angle, 360.0f );

  if( angle > 180.0f )
	 angle -= 360.0f;
  else if( angle < -180.0f )
	 angle += 360.0f;
}

float FORCEINLINE FastRSqrt( float number ) {
  union {
	 float f;
	 uint32_t i;
  } conv;

  float x2;
  const float threehalfs = 1.5f;

  x2 = number * 0.5f;
  conv.f = number;
  conv.i = 0x5F375A86 - ( conv.i >> 1 );
  conv.f = conv.f * ( threehalfs - ( x2 * conv.f * conv.f ) );
  return conv.f;
}

