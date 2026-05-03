// Include/N503/Core/Details/Api.h
#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct n503_audio_source_t* n503_audio_source_h;

    __declspec(dllexport) n503_audio_source_h n503_audio_source_create(const char* path, uint32_t type);

    __declspec(dllexport) int n503_audio_source_destroy(n503_audio_source_h instance);

    __declspec(dllexport) int n503_audio_source_play(n503_audio_source_h instance);

    __declspec(dllexport) int n503_audio_source_stop(n503_audio_source_h instance);

    __declspec(dllexport) int n503_audio_source_resume(n503_audio_source_h instance);

    __declspec(dllexport) int n503_audio_source_pause(n503_audio_source_h instance);

#ifdef __cplusplus
}
#endif
