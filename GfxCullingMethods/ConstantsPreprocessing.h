#ifndef CONSTANTS_PREPROCESSING
#define CONSTANTS_PREPROCESSING

#ifdef CPU_SIDE_CODE
#define DECLARE_CONSTANT_STRUCTURE struct
#define DECLARE_MATERIAL_CONSTANT_STRUCTURE struct
#define DECLARE_FLOAT float
#define DECLARE_FLOAT3 glm::vec3
#define DECLARE_FLOAT4 glm::vec4
#define DECLARE_FLOAT3X3 glm::mat3x3
#define DECLARE_FLOAT4X4 glm::mat4x4
#define DECLARE_FLOAT4X3 glm::mat4x3
#define DECLARE_FLOAT4X2 glm::mat4x2
#define DECLARE_UINT unsigned int
#define DEFINE_MATERIAL_CONSTANT_STRUCTURE(name, type)
#else
#define DECLARE_CONSTANT_STRUCTURE cbuffer
#ifdef BATCH
#define DEFINE_MATERIAL_CONSTANT_STRUCTURE(name, type) StructuredBuffer< type > name ;
#define DECLARE_MATERIAL_CONSTANT_STRUCTURE struct
#else
#define DEFINE_MATERIAL_CONSTANT_STRUCTURE(name, type)
#define DECLARE_MATERIAL_CONSTANT_STRUCTURE cbuffer
#endif
#define DECLARE_FLOAT float
#define DECLARE_FLOAT3 float3
#define DECLARE_FLOAT4 float4
#define DECLARE_FLOAT3X3 float3x3
#define DECLARE_FLOAT4X4 float4x4
#define DECLARE_FLOAT4X3 float4x3
#define DECLARE_FLOAT4X2 float4x2
#define DECLARE_UINT uint
#endif

#ifdef CPU_SIDE_CODE
#include "glm/glm.hpp"
#endif

#endif
