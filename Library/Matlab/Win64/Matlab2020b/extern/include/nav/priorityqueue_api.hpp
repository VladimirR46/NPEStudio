/* Copyright 2019 The MathWorks, Inc. */
#ifndef PRIORITYQUEUE_CODEGEN_API_HPP
#define PRIORITYQUEUE_CODEGEN_API_HPP

#ifdef BUILDING_LIBMWPRIORITYQUEUECODEGEN
#include "priorityqueuecodegen/priorityqueue_codegen_util.hpp"
#else
/* To deal with the fact that PackNGo has no include file hierarchy during test */
#include "priorityqueue_codegen_util.hpp"
#endif

EXTERN_C PRIORITYQUEUE_CODEGEN_API void priorityqueuecodegen_constructPriorityQueueCodegen(const real64_T dim,
                                                                      void** ptr2pqObj);

EXTERN_C PRIORITYQUEUE_CODEGEN_API void priorityqueuecodegen_push(void* ptrObj, const real64_T* newNode, int numRows);

EXTERN_C PRIORITYQUEUE_CODEGEN_API void priorityqueuecodegen_top(void* ptrObj, real64_T* topNode);

EXTERN_C PRIORITYQUEUE_CODEGEN_API void priorityqueuecodegen_pop(void* ptrObj);

EXTERN_C PRIORITYQUEUE_CODEGEN_API real64_T priorityqueuecodegen_size(void* ptrObj);

EXTERN_C PRIORITYQUEUE_CODEGEN_API boolean_T priorityqueuecodegen_doesNodeExist(void* ptrObj, const real64_T id);

EXTERN_C PRIORITYQUEUE_CODEGEN_API boolean_T priorityqueuecodegen_isEmpty(void* ptrObj);

EXTERN_C PRIORITYQUEUE_CODEGEN_API void priorityqueuecodegen_getNodeData(void* ptrObj,
                                                    const int* idList,
                                                    real64_T numRows,
                                                    real64_T* nodeData);

EXTERN_C PRIORITYQUEUE_CODEGEN_API void priorityqueuecodegen_replaceNodeData(void* ptrObj, const real64_T* node, int numRows);

EXTERN_C PRIORITYQUEUE_CODEGEN_API void priorityqueuecodegen_getAllData(void* ptrObj, real64_T* nodeData);

EXTERN_C PRIORITYQUEUE_CODEGEN_API void priorityqueuecodegen_getfScorePriorityQueue(void* ptrObj, real64_T* nodeData);

EXTERN_C PRIORITYQUEUE_CODEGEN_API void priorityqueuecodegen_traceBack(void* ptrObj,
                                                  const real64_T lastId,
                                                  real64_T* path,
                                                  real64_T* pathLen);
EXTERN_C PRIORITYQUEUE_CODEGEN_API real64_T priorityqueuecodegen_getNodeSize(void* ptrObj);
#endif
