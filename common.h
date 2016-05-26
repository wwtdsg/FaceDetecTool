/***************************************************************************
 *   huang_zhaolu@foxmail.com   *
 ***************************************************************************/

#ifndef _COMMON_INCLUDE_H_
#define _COMMON_INCLUDE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <malloc.h>


#ifndef MIN
#define MIN(a, b)   (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)   (((a) > (b)) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a)      (((a) > 0) ? (a) : (-(a)))
#endif

#ifndef SWAP
#define SWAP(a,b,t) ((t) = (a), (a) = (b), (b) = (t))
#endif

#ifndef NULL
#define NULL        (0)
#endif

#ifndef FALSE
#define FALSE       (0)
#endif

#ifndef TRUE
#define TRUE        (1)
#endif

#ifndef CLIP
#define CLIP(x, low, high) (((x) < (low)) ? (low) : (((x) > (high)) ? (high) : (x)))
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(x) {if(x) { free(x); x = NULL; }}
#endif


#define CHECK_ERROR(state, code) \
if (state)                       \
{                                \
	return code;                 \
}


#ifndef BOOL
typedef int                 BOOL;
#endif

#ifndef BYTE
typedef unsigned char       BYTE;
#endif

#ifndef BYTE
typedef unsigned char*      PBYTE;
#endif


//点(整型）
typedef struct _iv_point_
{
	int x;
	int y;
}iv_point;

//点(浮点型）
typedef struct _iv_point_32f_
{
	float x;
	float y;
}iv_point_32f;


typedef struct _iv_point_64d_
{
	double x;
	double y;
}iv_point_64d;


//矩形(整型)
typedef struct _iv_rect_
{
	int left; 
	int right; 
	int top;
	int bottom;
}iv_rect;

//矩形(浮点型)
typedef struct _iv_rect_32f_
{
	float left; 
	float right; 
	float top;
	float bottom;
}iv_rect_32f;




#ifdef __cplusplus
}
#endif


#endif