#pragma once

inline float alpha_mod = -1.f;
class Color {
public:
	unsigned char RGBA[ 4 ];

	Color( ) {
		RGBA[ 0 ] = 255;
		RGBA[ 1 ] = 255;
		RGBA[ 2 ] = 255;
		RGBA[ 3 ] = 255;
	}
	Color( int r, int g, int b, int a = 255 ) {
		RGBA[ 0 ] = r;
		RGBA[ 1 ] = g;
		RGBA[ 2 ] = b;
		RGBA[ 3 ] = a;
	}

	bool operator!=( Color color ) { return RGBA[ 0 ] != color.RGBA[ 0 ] || RGBA[ 1 ] != color.RGBA[ 1 ] || RGBA[ 2 ] != color.RGBA[ 2 ] || RGBA[ 3 ] != color.RGBA[ 3 ]; }
	bool operator==( Color color ) { return RGBA[ 0 ] == color.RGBA[ 0 ] && RGBA[ 1 ] == color.RGBA[ 1 ] && RGBA[ 2 ] == color.RGBA[ 2 ] && RGBA[ 3 ] == color.RGBA[ 3 ]; }
	Color operator*( float fl ) { return Color( RGBA[ 0 ] * fl, RGBA[ 1 ] * fl, RGBA[ 2 ] * fl, RGBA[ 3 ] * fl ); }

	// member accessors.
	__forceinline uint8_t& r( ) { return RGBA[ 0 ]; }
	__forceinline uint8_t& g( ) { return RGBA[ 1 ]; }
	__forceinline uint8_t& b( ) { return RGBA[ 2 ]; }
	__forceinline uint8_t a( ) {
		return RGBA[ 3 ];
	}

	// returns the color from 0.f - 1.f
	static float Base( const unsigned char col ) {
		return col / 255.f;
	}

	float* Base( ) {
		float clr[ 3 ];

		clr[ 0 ] = RGBA[ 0 ] / 255.0f;
		clr[ 1 ] = RGBA[ 1 ] / 255.0f;
		clr[ 2 ] = RGBA[ 2 ] / 255.0f;

		return &clr[ 0 ];
	}

	static Color Inverse( const Color color ) {
		return Color( 255 - color.RGBA[ 0 ], 255 - color.RGBA[ 1 ], 255 - color.RGBA[ 2 ] );
	}

	int GetD3DColor( ) const {
		return ( ( int )( ( ( ( RGBA[ 3 ] ) & 0xff ) << 24 ) | ( ( ( RGBA[ 0 ] ) & 0xff ) << 16 ) | ( ( ( RGBA[ 1 ] ) & 0xff ) << 8 ) | ( ( RGBA[ 2 ] ) & 0xff ) ) );
	}

	D3DCOLOR ToARGB( ) {
		return D3DCOLOR_ARGB( a( ), r( ), g( ), b( ) );
	}

	struct MenuColors {
		// RGB 11, 11, 11
		static Color Form( ) {
			return Color( 19, 19, 19 );
		}

		// RGB 32, 32, 32
		static Color Outline( ) {
			return Color( 37, 37, 37 );
		}

		// RGB 8, 8, 8
		static Color Titlebar( ) {
			return Color( 16, 16, 16 );
		}
	};

	static Color Red( ) {
		return Color( 255, 0, 0 );
	}

	static Color Green( ) {
		return Color( 0, 255, 0 );
	}

	static Color Blue( ) {
		return Color( 0, 0, 255 );
	}

	static Color LightBlue( ) {
		return Color( 100, 100, 255 );
	}

	static Color Grey( ) {
		return Color( 128, 128, 128 );
	}

	static Color DarkGrey( ) {
		return Color( 45, 45, 45 );
	}

	static Color Black( ) {
		return Color( 0, 0, 0 );
	}

	static Color White( ) {
		return Color( 255, 255, 255 );
	}

	static Color Purple( ) {
		return Color( 220, 0, 220 );
	}

	float Difference( const Color color ) const // from 0.f - 1.f with 1.f being the most different
	{
		float red_diff = std::fabs( Base( RGBA[ 0 ] ) - Base( color.RGBA[ 0 ] ) );
		float green_diff = std::fabs( Base( RGBA[ 1 ] ) - Base( color.RGBA[ 1 ] ) );
		float blue_diff = std::fabs( Base( RGBA[ 2 ] ) - Base( color.RGBA[ 2 ] ) );
		float alpha_diff = std::fabs( Base( RGBA[ 3 ] ) - Base( color.RGBA[ 3 ] ) );

		return ( red_diff + green_diff + blue_diff + alpha_diff ) * 0.25f;
	}

	// RGB -> HSB
	static float Hue( const Color color ) {
		float R = Base( color.RGBA[ 0 ] );
		float G = Base( color.RGBA[ 1 ] );
		float B = Base( color.RGBA[ 2 ] );

		float mx = std::max( R, std::max( G, B ) );
		float mn = std::min( R, std::min( G, B ) );
		if( mx == mn )
			return 0.f;

		float delta = mx - mn;

		float hue = 0.f;
		if( mx == R )
			hue = ( G - B ) / delta;
		else if( mx == G )
			hue = 2.f + ( B - R ) / delta;
		else
			hue = 4.f + ( R - G ) / delta;

		hue *= 60.f;
		if( hue < 0.f )
			hue += 360.f;

		return hue / 360.f;
	}

	static float Saturation( const Color color ) {
		float R = Base( color.RGBA[ 0 ] );
		float G = Base( color.RGBA[ 1 ] );
		float B = Base( color.RGBA[ 2 ] );

		float mx = std::max( R, std::max( G, B ) );
		float mn = std::min( R, std::min( G, B ) );

		float delta = mx - mn;

		if( mx == 0.f )
			return delta;

		return delta / mx;
	}

	static float Brightness( const Color color ) {
		float R = Base( color.RGBA[ 0 ] );
		float G = Base( color.RGBA[ 1 ] );
		float B = Base( color.RGBA[ 2 ] );

		return  std::max( R, std::max( G, B ) );
	}

	float Hue( ) const {
		return Hue( *this );
	}

	float Saturation( ) const {
		return Saturation( *this );
	}

	float Brightness( ) const {
		return Brightness( *this );
	}

	// HSB -> RGB
	static Color HSBtoRGB( float hue /* 0.f - 1.f*/,
		float saturation /* 0.f - 1.f */,
		float brightness /* 0.f - 1.f */,
		int alpha = 255 ) {
		hue = std::clamp( hue, 0.f, 1.f );
		saturation = std::clamp( saturation, 0.f, 1.f );
		brightness = std::clamp( brightness, 0.f, 1.f );

		float h = ( hue == 1.f ) ? 0.f : ( hue * 6.f );
		float f = h - static_cast< int >( h );
		float p = brightness * ( 1.f - saturation );
		float q = brightness * ( 1.f - saturation * f );
		float t = brightness * ( 1.f - ( saturation * ( 1.f - f ) ) );

		if( h < 1.f )
			return Color( brightness * 255, t * 255, p * 255, alpha );
		else if( h < 2.f )
			return Color( q * 255, brightness * 255, p * 255, alpha );
		else if( h < 3.f )
			return Color( p * 255, brightness * 255, t * 255, alpha );
		else if( h < 4 )
			return Color( p * 255, q * 255, brightness * 255, alpha );
		else if( h < 5 )
			return Color( t * 255, p * 255, brightness * 255, alpha );
		else
			return Color( brightness * 255, p * 255, q * 255, alpha );
	}
	__inline void SetColor( int _r, int _g, int _b, int _a ) {
		RGBA[ 0 ] = ( unsigned char )_r;
		RGBA[ 1 ] = ( unsigned char )_g;
		RGBA[ 2 ] = ( unsigned char )_b;
		RGBA[ 3 ] = ( unsigned char )_a;
	}
	__inline void SetColor( float _r, float _g, float _b, float _a ) {
		RGBA[ 0 ] = static_cast< unsigned char >( _r * 255.0f );
		RGBA[ 1 ] = static_cast< unsigned char >( _g * 255.0f );
		RGBA[ 2 ] = static_cast< unsigned char >( _b * 255.0f );
		RGBA[ 3 ] = static_cast< unsigned char >( _a * 255.0f );
	}
	void SetColor( float* color ) {
		if( !color )
			return;

		RGBA[ 0 ] = ( unsigned char )( color[ 0 ] * 255.f );
		RGBA[ 1 ] = ( unsigned char )( color[ 1 ] * 255.f );
		RGBA[ 2 ] = ( unsigned char )( color[ 2 ] * 255.f );
		RGBA[ 3 ] = ( unsigned char )( color[ 3 ] * 255.f );
	}

	void SetAlpha( int alpha ) {
		RGBA[ 3 ] = alpha;
	}


	struct Palette_t {
		// 27, 27, 32
		static Color FormColor( ) {
			return Color( 27, 27, 32 );
		}

		// 27, 27, 32
		static Color FormColorDark( ) {
			return Color( 24, 24, 29 );
		}

		// 32, 32, 37
		static Color FormColorLight( ) {
			return Color( 32, 32, 37 );
		}

		// 35, 35, 41
		static Color GroupColor( ) {
			return Color( 35, 35, 41 );
		}

		// 40, 40, 46
		static Color GroupColorLight( ) {
			return Color( 40, 40, 46 );
		}

		// 42, 42, 48
		static Color ElementOutlines( ) {
			return Color( 42, 42, 48 );
		}

		// 228, 228, 242
		static Color MenuWhite( ) {
			return Color( 228, 228, 242 );
		}

		// 73, 73, 78
		static Color MenuGrey( ) {
			return Color( 73, 73, 78 );
		}
	};

	__forceinline Color OverrideAlpha( int alpha, bool adaptive = false, bool FORCE_FORCE = true ) {
		return Color( r( ), g( ), b( ), ( adaptive ? static_cast< int >( static_cast< float >( alpha ) * ( RGBA[ 3 ] / 255.f ) ) : alpha ) );
	}

	__forceinline static Color Blend( Color a, Color b, float multiplier ) {
		return Color(
			a.r( ) + static_cast< int >( multiplier * ( b.r( ) - a.r( ) ) ),
			a.g( ) + static_cast< int >( multiplier * ( b.g( ) - a.g( ) ) ),
			a.b( ) + static_cast< int >( multiplier * ( b.b( ) - a.b( ) ) ),
			a.a( ) + static_cast< int >( multiplier * ( b.a( ) - a.a( ) ) )
		);
	}

	struct Hsv_t {
		float Hue, Saturation, Value;
	};

	static Color HSVtoRGB( float hue, float sat, float val ) {
		float red, grn, blu;
		float i, f, p, q, t;
		Color result;

		if( val == 0 ) {
			red = 0;
			grn = 0;
			blu = 0;
		}
		else {
			hue /= 60;
			i = floor( hue );
			f = hue - i;
			p = val * ( 1 - sat );
			q = val * ( 1 - ( sat * f ) );
			t = val * ( 1 - ( sat * ( 1 - f ) ) );
			if( i == 0 ) {
				red = val;
				grn = t;
				blu = p;
			}
			else if( i == 1 ) {
				red = q;
				grn = val;
				blu = p;
			}
			else if( i == 2 ) {
				red = p;
				grn = val;
				blu = t;
			}
			else if( i == 3 ) {
				red = p;
				grn = q;
				blu = val;
			}
			else if( i == 4 ) {
				red = t;
				grn = p;
				blu = val;
			}
			else if( i == 5 ) {
				red = val;
				grn = p;
				blu = q;
			}
		}

		result = Color( int( red * 255 ), int( grn * 255 ), int( blu * 255 ) );
		return result;
	}

	static Hsv_t RGBtoHSV( Color a ) {
		float red, grn, blu;
		red = ( float )a.r( ) / 255.f;
		grn = ( float )a.g( ) / 255.f;
		blu = ( float )a.b( ) / 255.f;
		float hue, sat, val;
		float x, f, i;
		Hsv_t result;

		x = std::min( std::min( red, grn ), blu );
		val = std::max( std::max( red, grn ), blu );
		if( x == val ) {
			hue = 0;
			sat = 0;
		}
		else {
			f = ( red == x ) ? grn - blu : ( ( grn == x ) ? blu - red : red - grn );
			i = ( red == x ) ? 3 : ( ( grn == x ) ? 5 : 1 );
			hue = fmod( ( i - f / ( val - x ) ) * 60, 360 );
			sat = ( ( val - x ) / val );
		}
		result.Hue = hue;
		result.Saturation = sat;
		result.Value = val;

		return result;
	}
};
