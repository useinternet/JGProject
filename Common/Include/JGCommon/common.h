#pragma once
#include<iostream>
#include<assert.h>


#ifdef WIN32
	typedef unsigned int uint;
#else
	typedef unsigned long long uint;
#endif

#ifdef USEDOUBLE
	typedef double real;
#define SQRT(x) sqrt(x)
#define ABS(x)  fabs(x)
#define COS(x)  cos(x)
#define SIN(x)  sin(x)
#define CSC(x)  1 / sin(x)
#define SEC(x)  1 / cos(x)
#define COT(x)  1 / tan(x)
#define JGPI 3.14159265359
#define ERRORRANGE 0.00000000001
#else
	typedef float real;
#define SQRT(x) sqrtf(x)
#define ABS(x)  fabsf(x)
#define COS(x)  cosf(x)
#define SIN(x)  sinf(x)
#define CSC(x)  1 / sinf(x)
#define SEC(x)  1 / cosf(x)
#define COT(x)  1 / tanf(x)
#define JGPI 3.1415926f
#define ERRORRANGE 0.000001
#endif

#define JGToRadian(x)  x / (180 / JGPI)
#define JGToDegree(x) (x * 180) / JGPI


#define TT(x) L##x







