#ifndef DIRECTIONAL_LIGHT_COUNT
#define DIRECTIONAL_LIGHT_COUNT 0
#endif
#ifndef SPOT_LIGHT_COUNT
#define SPOT_LIGHT_COUNT 0
#endif
#ifndef POINT_LIGHT_COUNT
#define POINT_LIGHT_COUNT 0
#endif
#if (DIRECTIONAL_LIGHT_COUNT > 0) || (POINT_LIGHT_COUNT > 0) || (SPOT_LIGHT_COUNT > 0)
#define LIGHTING
#endif
#ifdef BUMPED
#undef BUMPED // Bump mapping isn't supported right now
#endif
#ifndef SPECULAR
#define SPECULAR
#endif

///////////////////////////////////////////////////////////
// Atributes
attribute vec4 a_position;

#if defined(SKINNING)
attribute vec4 a_blendWeights;
attribute vec4 a_blendIndices;
#endif

attribute vec3 a_normal;

///////////////////////////////////////////////////////////
// Uniforms
uniform mat4 u_worldViewProjectionMatrix;

uniform mat4 u_inverseTransposeWorldViewMatrix;
uniform mat4 u_worldViewMatrix;
uniform vec3 u_cameraPosition;

uniform mat4 u_inverseTransposeWorldMatrix;
uniform mat4 u_worldMatrix;
uniform vec3 u_cameraWorldPosition;

#if defined(SKINNING)
uniform vec4 u_matrixPalette[SKINNING_JOINT_COUNT * 3];
#endif

#if defined(LIGHTING)
#if (POINT_LIGHT_COUNT > 0)
uniform vec3 u_pointLightPosition[POINT_LIGHT_COUNT];
#endif

#if (SPOT_LIGHT_COUNT > 0) 
uniform vec3 u_spotLightPosition[SPOT_LIGHT_COUNT];
#endif

#endif

///////////////////////////////////////////////////////////
// Varyings
varying vec3 v_normalVector;
varying vec3 v_cameraDirection;

varying vec3 v_normalReflectionVector;
varying vec3 v_cameraReflectionDirection;

#if defined(LIGHTING)

#if (POINT_LIGHT_COUNT > 0)
varying vec3 v_vertexToPointLightDirection[POINT_LIGHT_COUNT];
#endif

#if (SPOT_LIGHT_COUNT > 0)
varying vec3 v_vertexToSpotLightDirection[SPOT_LIGHT_COUNT];
#endif

#endif

#include "lighting.vert"

#if !defined(LIGHTING)
#define LIGHTING
#endif

#if defined(SKINNING)
#include "skinning.vert"
#else
#include "skinning-none.vert" 
#endif

void main()
{
    vec4 position = getPosition();
    gl_Position = u_worldViewProjectionMatrix * position;

    v_normalReflectionVector = normalize((u_inverseTransposeWorldMatrix * vec4(a_normal.xyz, 0)).xyz);
    v_cameraReflectionDirection = u_cameraWorldPosition - (u_worldMatrix * position).xyz;

    vec3 normal = getNormal();
    // Transform the normal to view space.
    v_normalVector = normalize((u_inverseTransposeWorldViewMatrix * vec4(a_normal.xyz, 0)).xyz);

    applyLight(position);
}