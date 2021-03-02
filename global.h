#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <string>
#include <vector>

#define DEBUG 0

#define DEBUG_CPP (DEBUG && __cplusplus)

#if !DEBUG
#define NDEBUG
#endif

#if DEBUG
#define DEBUG_M(x) x
#define NDEBUG_M(x)

#else
#define DEBUG_M(x)
#define NDEBUG_M(x) x
#endif

//if C
#ifndef __cplusplus
#define constexpr
#endif

#define lengthof(arr) (sizeof(arr)/sizeof(arr[0]))

#endif // GLOBAL_H
