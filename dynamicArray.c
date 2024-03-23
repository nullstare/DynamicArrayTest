#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long s64;
typedef unsigned long u64;
typedef float f32;
typedef double f64;

typedef union {
	s8 s8;
	u8 u8;
	s16 s16;
	u16 u16;
	s32 s32;
	u32 u32;
	s64 s64;
	u64 u64;
	f32 f32;
	f64 f64;
} Var;

typedef struct DynArrStruct {
	size_t elementSize;
	size_t capacity;
	size_t size;
	void* elements;
} DynArray;

DynArray dynArrayInit( size_t elementSize, size_t capacity ) {
	DynArray arr = {
		.elementSize = elementSize,
		.capacity = capacity,
		.size = 0
	};
	arr.elements = malloc( elementSize * capacity );
	memset( arr.elements, 0, elementSize * capacity );
	return arr;
}

bool dynArrayInBounds( DynArray* arr, size_t n ) {
	if ( 0 <= n && n < arr->size ) {
		return true;
	}
	else {
		printf( "Error! DynamicArray access out of bounds: %u\n", n );
		return false;
	}
}

void dynArrayAdd( DynArray* arr, void* value ) {
	void* p = arr->elements;

	if ( arr->size == arr->capacity ) {
		arr->capacity *= 2; /* Doubles the capacity when full. */
		arr->elements = realloc( arr->elements, arr->capacity * arr->elementSize );
		p += arr->size * arr->elementSize;
		memset( p, 0, ( arr->capacity - arr->size ) * arr->elementSize );
	}
	p = arr->elements;
	p += arr->elementSize * arr->size;
	arr->size++;
	memcpy( p, value, arr->elementSize );
}

void dynArraySet( DynArray* arr, size_t n, void* value ) {
	if ( !dynArrayInBounds( arr, n ) ) {
		return;
	}
	void* p = arr->elements;
	p += arr->elementSize * n;
	memcpy( p, value, arr->elementSize );
}

/* Return NULL if out of bounds. */
void* dynArrayGet( DynArray* arr, size_t n ) {
	if ( !dynArrayInBounds( arr, n ) ) {
		return NULL;
	}
	void* p = arr->elements;
	p += arr->elementSize * n;
	return p;
}

void dynArrayDelRange( DynArray* arr, size_t n, size_t len ) {
	if ( !dynArrayInBounds( arr, n ) || !dynArrayInBounds( arr, n + len ) ) {
		return;
	}
	/* Shift block size of len from n forward and set values in end to 0. 1xx456 -> 145600. Also shrink size. */
	void* dst = arr->elements;
	void* src = arr->elements;
	dst += arr->elementSize * n;
	src += arr->elementSize * ( n + len );
	memcpy( dst, src, ( arr->size - n ) * arr->elementSize );
	dst = arr->elements;
	dst += arr->elementSize * ( arr->size - 1 );
	memset( dst, 0, arr->elementSize );
	arr->size -= len;
}

void dynArrayDel( DynArray* arr, size_t n ) {
	dynArrayDelRange( arr, n, 1 );
}

void dynArrayFree( DynArray* arr ) {
	free( arr->elements );
}

/* Functions for types. */

/* Var */

void dynArrayAddVar( DynArray* arr, Var value ) {
	dynArrayAdd( arr, &value );
}

void dynArraySetVar( DynArray* arr, size_t n, Var value ) {
	dynArraySet( arr, n, &value );
}

Var dynArrayGetVar( DynArray* arr, size_t n ) {
	Var* p = dynArrayGet( arr, n );
	return *p;
}

/* u8 unsigned char */

void dynArrayAddU8( DynArray* arr, u8 value ) {
	dynArrayAdd( arr, &value );
}

void dynArraySetU8( DynArray* arr, size_t n, u8 value ) {
	dynArraySet( arr, n, &value );
}

u8 dynArrayGetU8( DynArray* arr, size_t n ) {
	u8* p = dynArrayGet( arr, n );
	return *p;
}

/* s32 signed int */

void dynArrayAddS32( DynArray* arr, s32 value ) {
	dynArrayAdd( arr, &value );
}

void dynArraySetS32( DynArray* arr, size_t n, s32 value ) {
	dynArraySet( arr, n, &value );
}

s32 dynArrayGetS32( DynArray* arr, size_t n ) {
	s32* p = dynArrayGet( arr, n );
	return *p;
}

/* f32 float */

void dynArrayAddF32( DynArray* arr, f32 value ) {
	dynArrayAdd( arr, &value );
}

void dynArraySetF32( DynArray* arr, size_t n, f32 value ) {
	dynArraySet( arr, n, &value );
}

f32 dynArrayGetF32( DynArray* arr, size_t n ) {
	f32* p = dynArrayGet( arr, n );
	return *p;
}

/* f64 double */

void dynArrayAddF64( DynArray* arr, f64 value ) {
	dynArrayAdd( arr, &value );
}

void dynArraySetF64( DynArray* arr, size_t n, f64 value ) {
	dynArraySet( arr, n, &value );
}

f64 dynArrayGetF64( DynArray* arr, size_t n ) {
	f64* p = dynArrayGet( arr, n );
	return *p;
}

/* Test. */

typedef struct {
	f32 x;
	f32 y;
} Vec2;

/* Temp value to 'cast' type Vec2. Test. Could be static in library. */
Vec2 _addVec2;

Vec2* addVec2( f32 x, f32 y ) {
	_addVec2 = (Vec2){ x, y };
	return &_addVec2;
}

int main() {
	/* Int. */
	DynArray arrS32 = dynArrayInit( sizeof( s32 ), 4 );
	for ( u32 i = 0; i < 14; i++ ) {
		dynArrayAddS32( &arrS32, i+1 );
	}
	dynArrayDelRange( &arrS32, 3, 5 );
	dynArraySetS32( &arrS32, 1, 42 );
	dynArrayAddS32( &arrS32, 100 );
	dynArrayAddS32( &arrS32, 200 );
	/* Float. */
	DynArray arrF = dynArrayInit( sizeof( f32 ), 8 );
	dynArrayAddF32( &arrF, 42.84 );
	dynArrayAddF32( &arrF, 42354.42545824 );
	// dynArraySetF32( &arrF, 1, 83.457842478 );
	/* Double. */
	DynArray arrD = dynArrayInit( sizeof( f64 ), 8 );
	dynArrayAddF64( &arrD, 1458222474.48424564813548 );
	dynArrayAddF64( &arrD, 42.4254853248872211 );
	/* Arbitrary struct array. */
	DynArray arrVec = dynArrayInit( sizeof( Vec2 ), 8 );
	Vec2 vec2 = { 8.42, 12.484 };
	dynArrayAdd( &arrVec, &vec2 );
	dynArrayAdd( &arrVec, addVec2( 458.42, 1442.484 ) );
	/* String. */
	DynArray string = dynArrayInit( sizeof( u8 ), 1 );
	const char* str = "Hello World!";
	for ( u32 i = 0; i < strlen( str ); i++ ) {
		dynArrayAddU8( &string, str[i] );
	}
	/* 2D array. */
	DynArray array2D = dynArrayInit( sizeof( DynArray ), 5 );
	for ( u32 i = 0; i < 5; i++ ) {
		DynArray rows = dynArrayInit( sizeof( u8 ), 5 );
		dynArrayAdd( &array2D, (DynArray*)&rows );
		DynArray* row = (DynArray*)dynArrayGet( &array2D, i );
		for ( u32 j = 0; j < 5; j++ ) {
			dynArrayAddU8( row, str[j+i] );
		}
	}
	/* Var. */
	DynArray arrS8 = dynArrayInit( sizeof( s8 ), 2 );
	dynArrayAddVar( &arrS8, (Var)100 );
	dynArrayAddVar( &arrS8, (Var)50 );
	dynArraySetVar( &arrS8, 0, (Var)64000 ); // No overflow.
	DynArray arrU8 = dynArrayInit( sizeof( u8 ), 2 );
	dynArrayAddVar( &arrU8, (Var)100 );
	dynArrayAddVar( &arrU8, (Var)200 );
	DynArray arrU64 = dynArrayInit( sizeof( u64 ), 2 );
	dynArrayAddVar( &arrU64, (Var)456875322348934 );
	dynArrayAddVar( &arrU64, (Var)1248889778344665 );
	DynArray arrF32 = dynArrayInit( sizeof( f32 ), 2 );
	// dynArrayAddVar( &arrF32, (Var)124.48 ); // Reguires f.
	// dynArrayAddVar( &arrF32, (Var)42.42 );
	// dynArrayAddVar( &arrF32, (Var)124.48f ); // Reguires f.
	// dynArrayAddVar( &arrF32, (Var)42.42f );
	dynArrayAddVar( &arrF32, (Var)(f32)124.48 );
	dynArrayAddVar( &arrF32, (Var)(f32)42.42 );
	DynArray arrF64 = dynArrayInit( sizeof( f64 ), 2 );
	// dynArrayAddVar( &arrF64, (Var)1458222474.48424564813548 );
	// dynArrayAddVar( &arrF64, (Var)42.4254853248872211 );
	dynArrayAddVar( &arrF64, (Var)(f64)1458222474.48424564813548 );
	dynArrayAddVar( &arrF64, (Var)(f64)42.4254853248872211 );

	printf( "sizeof( Var ) = %u\n", sizeof( Var ) );

	/* Printing. */

	printf( "\n" );
	for ( u32 i = 0; i < arrS32.size; i++ ) {
		printf( "arrS32[%d] = %d\n", i, dynArrayGetS32( &arrS32, i ) );
	}
	printf( "\n" );
	for ( u32 i = 0; i < arrF.size; i++ ) {
		printf( "arrF[%d] = %f\n", i, dynArrayGetF32( &arrF, i ) );
	}
	printf( "\n" );
	for ( u32 i = 0; i < arrD.size; i++ ) {
		printf( "arrD[%d] = %lf\n", i, dynArrayGetF64( &arrD, i ) );
	}
	printf( "\n" );
	for ( u32 i = 0; i < arrVec.size; i++ ) {
		Vec2* vecp = (Vec2*)dynArrayGet( &arrVec, i );
		printf( "arrVec[%d] = %.2f, %.2f\n", vecp->x, vecp->y );
	}
	printf( "\n" );
	for ( u32 i = 0; i < string.size; i++ ) {
		printf( "%c", dynArrayGetU8( &string, i ) );
	}
	printf( "\n" );
	for ( u32 y = 0; y < array2D.size; y++ ) {
		DynArray *row = (DynArray*)dynArrayGet( &array2D, y );
		printf( "array2D.row[%d] = ", y );

		for ( u32 x = 0; x < row->size; x++ ) {
			printf( "%c", dynArrayGetU8( row, x ) );
		}
		printf( "\n" );
	}
	printf( "\n" );
	for ( u32 i = 0; i < arrS8.size; i++ ) {
		printf( "arrS8[%d] = %d\n", i, dynArrayGetVar( &arrS8, i ).s8 );
	}
	for ( u32 i = 0; i < arrU8.size; i++ ) {
		printf( "arrU8[%d] = %d\n", i, dynArrayGetVar( &arrU8, i ).u8 );
	}
	for ( u32 i = 0; i < arrU64.size; i++ ) {
		printf( "arrU64[%d] = %lu\n", i, dynArrayGetVar( &arrU64, i ).u64 );
	}
	for ( u32 i = 0; i < arrF32.size; i++ ) {
		printf( "arrF32[%d] = %f\n", i, dynArrayGetVar( &arrF32, i ).f32 );
	}
	for ( u32 i = 0; i < arrF64.size; i++ ) {
		printf( "arrF64[%d] = %lf\n", i, dynArrayGetVar( &arrF64, i ).f64 );
	}

	/* Free. */

	dynArrayFree( &arrS32 );
	dynArrayFree( &arrF );
	dynArrayFree( &arrVec );
	dynArrayFree( &string );
	dynArrayFree( &arrS8 );
	dynArrayFree( &arrU8 );
	dynArrayFree( &arrU64 );
	dynArrayFree( &arrF32 );
	dynArrayFree( &arrF64 );

	Var a = { .u8 = 12 };
	Var b = { .f32 = 12.21 };

	printf( "%d + %.2f = %.2f\n", a.u8, b.f32, a.u8 + b.f32 );

	return 0;
}
