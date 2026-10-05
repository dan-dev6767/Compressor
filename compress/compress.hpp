#pragma once

#ifdef COMPRESS_EXPORTS
#define COMPRESS_API extern "C" __declspec(dllexport)
#else
#define COMPRESS_API extern "C" __declspec(dllimport)
#endif

COMPRESS_API void LazyCompress(const char* filePath, int level, int threads);
COMPRESS_API void LazyDeCompress(const char* filePath);