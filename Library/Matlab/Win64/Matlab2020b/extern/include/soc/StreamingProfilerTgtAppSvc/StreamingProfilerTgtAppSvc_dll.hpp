/* Copyright 2017 The MathWorks, Inc. */

#ifndef STREAMINGPROFILERTGTAPPSVC_dll_h
#define STREAMINGPROFILERTGTAPPSVC_dll_h

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

#ifdef SL_INTERNAL

#include "version.h"

#if defined(BUILDING_LIBMWSOC_STREAMINGPROFILERTGTAPPSVC)
#define STREAMINGPROFILERTGTAPPSVC_API DLL_EXPORT_SYM
#define STREAMINGPROFILERTGTAPPSVC_API_C EXTERN_C DLL_EXPORT_SYM
#else
#define STREAMINGPROFILERTGTAPPSVC_API DLL_IMPORT_SYM
#define STREAMINGPROFILERTGTAPPSVC_API_C EXTERN_C DLL_IMPORT_SYM
#endif

#else

#define STREAMINGPROFILERTGTAPPSVC_API
#define STREAMINGPROFILERTGTAPPSVC_API_C EXTERN_C

#endif

#endif
