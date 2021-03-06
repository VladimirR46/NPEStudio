/* Copyright 2018-2019 The MathWorks, Inc. */

/**
 * @file
 * External C-API interfaces for Dubins motion primitive calculations (TBB-accelerated).
 * To fully support code generation, note that this file needs to be fully
 * compliant with the C89/C90 (ANSI) standard.
 */

#ifndef AUTONOMOUSCODEGEN_DUBINS_TBB_API_H_
#define AUTONOMOUSCODEGEN_DUBINS_TBB_API_H_

#ifdef BUILDING_LIBMWAUTONOMOUSCODEGEN
#include "autonomouscodegen/autonomouscodegen_util.hpp"
#else
/* To deal with the fact that PackNGo has no include file hierarchy during test */
#include "autonomouscodegen_util.hpp"
#endif


/**
 * @copydoc autonomousDubinsSegmentsCodegen_real64
 *
 * This function uses TBB to parallelize the computation.
 */
EXTERN_C AUTONOMOUS_CODEGEN_API void autonomousDubinsSegmentsCodegen_tbb_real64(
    const real64_T* startPose,
    const uint32_T numStartPoses,
    const real64_T* goalPose,
    const uint32_T numGoalPoses,
    const real64_T turningRadius,
    const boolean_T* allPathTypes,
    const boolean_T isOptimal,
    const uint32_T nlhs,
    real64_T* distance,
    real64_T* segmentLen,
    real64_T* segmentType);


/**
 * @copydoc autonomousDubinsDistanceCodegen_real64
 *
 * This function uses TBB to parallelize the computation.
 */
EXTERN_C AUTONOMOUS_CODEGEN_API void autonomousDubinsDistanceCodegen_tbb_real64(
    const real64_T* startPose,
    const uint32_T numStartPoses,
    const real64_T* goalPose,
    const uint32_T numGoalPoses,
    const real64_T turningRadius,
    real64_T* distance);

#endif /* AUTONOMOUSCODEGEN_DUBINS_TBB_API_H_ */
