#ifndef QCORE_QMATH_H
#define QCORE_QMATH_H

#include "he/core/types.h"

struct FRect2D {

};


struct Qf32x4_s {
	union {
		struct {
			float x, y, z, w;
		};
		struct {
			float r, g, b, a;
		};
		float v[4];
	};
};
HE_COMPILE_ASSERT( sizeof( struct Qf32x4_s ) == 4 * 4 );

struct Recti16 {
	union {
		struct {
			int16_t x, y, w, h;
		};
		int16_t v[4];
	};
};
HE_COMPILE_ASSERT( sizeof( struct Recti16  ) == 4 * 2 );

struct QRectf32_s {
	union {
		struct {
			float x, y, w, h;
		};
		float v[4];
	};
};
HE_COMPILE_ASSERT( sizeof( struct QRectf32_s ) == 4 * 4 );

struct Qf32x3_s {
	union {
		struct {
			float x, y, z;
		};
		struct {
			float r, g, b;
		};
		float v[3];
	};
};
HE_COMPILE_ASSERT( sizeof( struct Qf32x3_s ) == 3 * 4 );

struct Qf64x4_s {
	union {
		struct {
			double x, y, z, w;
		};
		double v[4];
	};
};
HE_COMPILE_ASSERT( sizeof( struct Qf64x4_s ) == 8 * 4 );

struct Qu8x3_s {
	union {
		struct {
			uint8_t x, y, z;
		};
		uint8_t v[3];
	};
};
HE_COMPILE_ASSERT( sizeof( struct Qu8x3_s ) == 3 );

struct Qf64x3_s {
	union {
		struct {
			double x, y, z;
		};
		double v[3];
	};
};
HE_COMPILE_ASSERT( sizeof( struct Qf64x3_s ) == 8 * 3 );
#endif
