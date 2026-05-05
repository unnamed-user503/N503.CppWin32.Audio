#pragma once
#include <stdint.h>

#ifdef N503_DLL_EXPORTS
#define N503_API __declspec(dllexport)
#else
#define N503_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct N503Sound_t* N503Sound;

    N503_API N503Sound N503CreateSound(const char* path, uint32_t type);

    N503_API int N503DestroySound(N503Sound sound);

    N503_API int N503PlaySound(N503Sound sound);

    N503_API int N503StopSound(N503Sound sound);

    N503_API int N503ResumeSound(N503Sound sound);

    N503_API int N503PauseSound(N503Sound sound);

#ifdef __cplusplus
}
#endif
