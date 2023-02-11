#pragma once

#include "../core.hpp"

class QAngle
{
public:
	QAngle( );
	QAngle( float x, float y, float z );
	QAngle( const QAngle& v );
	QAngle( const float* v );

public:
	void Set( float x = 0.0f, float y = 0.0f, float z = 0.0f );

	void Normalize( );
	void Clamp( );

	bool IsZero( float tolerance = 0.01f );
	bool AngleAreEqual( QAngle angle, float tolerance = 0.01f );

	QAngle Normalized( );
	QAngle Clamped( );

	QAngle Direction( ) {
		return QAngle( cos( y * 3.14159265358979323846 / 180.0f ) * cos( x * 3.14159265358979323846 / 180.0f ), sin( y * 3.14159265358979323846 / 180.0f ) * cos( x * 3.14159265358979323846 / 180.0f ), sin( -x * 3.14159265358979323846 / 180.0f ) ).Normalized( );
	}

	QAngle Forward( ) {
		return Direction( );
	}

	QAngle Up( ) {
		return QAngle( cos( y * 3.14159265358979323846 / 180.0f ) * cos( x * 3.14159265358979323846 / 180.0f ), sin( y * 3.14159265358979323846 / 180.0f ) * cos( x * 3.14159265358979323846 / 180.0f ), sin( -( x - 90.0f ) * 3.14159265358979323846 / 180.0f ) ).Normalized( );
	}

	QAngle Right( ) {
		return QAngle( cos( ( y + 90.f ) * 3.14159265358979323846 / 180.0f ) * cos( x * 3.14159265358979323846 / 180.0f ), sin( ( y + 90.f ) * 3.14159265358979323846 / 180.0f ) * cos( x * 3.14159265358979323846 / 180.0f ), sin( -x * 3.14159265358979323846 / 180.0f ) ).Normalized( );
	}

	Vector ToVectors( Vector* side = nullptr, Vector* up = nullptr );
	Vector ToVectorsTranspose( Vector* side = nullptr, Vector* up = nullptr );

public:
	float operator [] ( const std::uint32_t index );
	const float operator [] ( const std::uint32_t index ) const;

	QAngle& operator = ( const QAngle& v );
	QAngle& operator = ( const float* v );

	QAngle& operator += ( const QAngle& v );
	QAngle& operator -= ( const QAngle& v );
	QAngle& operator *= ( const QAngle& v );
	QAngle& operator /= ( const QAngle& v );

	QAngle& operator += ( float fl );
	QAngle& operator -= ( float fl );
	QAngle& operator *= ( float fl );
	QAngle& operator /= ( float fl );

	QAngle operator + ( const QAngle& v ) const;
	QAngle operator - ( const QAngle& v ) const;
	QAngle operator * ( const QAngle& v ) const;
	QAngle operator / ( const QAngle& v ) const;

	QAngle operator + ( float fl ) const;
	QAngle operator - ( float fl ) const;
	QAngle operator * ( float fl ) const;
	QAngle operator / ( float fl ) const;

public:
	static QAngle Zero;

public:
	union {
		struct {
			float pitch;
			float yaw;
			float roll;
		};

		struct {
			float x;
			float y;
			float z;
		};
	};
};
