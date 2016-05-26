
#ifndef _LIB_PARSEFACE_INCLUDE_H_
#define _LIB_PARSEFACE_INCLUDE_H_


#define PARSE_FACE_API

#ifdef LIBPARSEFACE_EXPORTS
#define PARSE_FACE_API __declspec(dllexport)
#else
#define PARSE_FACE_API __declspec(dllimport)
#endif


//����������˵����
//��üë��1 - 5
//��üë��6 - 10
//���۾���11 - 16
//���۾���17 - 22
//��  ��: 23 - 27
//��  �ͣ�28 - 45

#ifdef __cplusplus
extern "C"
{
#endif

#include "common.h"


	PARSE_FACE_API void* ParseFace_init();

	PARSE_FACE_API int   ParseFace_face_detecte();

	PARSE_FACE_API int   ParseFace_face_track();

	PARSE_FACE_API int   ParseFace_face_align(void *phandle, unsigned char *pgray, int image_w, int image_h, int width_step, iv_rect face_bbox, iv_point_64d **pp_landmarks);

	PARSE_FACE_API int   ParseFace_face_position();

	PARSE_FACE_API int   ParseFace_uninit();







#ifdef __cplusplus
};
#endif

#endif