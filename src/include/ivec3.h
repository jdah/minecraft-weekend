// Defines missing iivec3, ivec2 types for cglms
/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*
 Macros:
   GLM_IVEC3_ONE_INIT
   GLM_IVEC3_ZERO_INIT
   GLM_IVEC3_ONE
   GLM_IVEC3_ZERO
   GLM_YUP
   GLM_ZUP
   GLM_XUP

 Functions:
   CGLM_INLINE void  glm_ivec3(vec4 v4, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_copy(ivec3 a, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_zero(ivec3 v);
   CGLM_INLINE void  glm_ivec3_one(ivec3 v);
   CGLM_INLINE int glm_ivec3_dot(ivec3 a, ivec3 b);
   CGLM_INLINE int glm_ivec3_norm2(ivec3 v);
   CGLM_INLINE int glm_ivec3_norm(ivec3 v);
   CGLM_INLINE int glm_ivec3_norm_one(ivec3 v);
   CGLM_INLINE int glm_ivec3_norm_inf(ivec3 v);
   CGLM_INLINE void  glm_ivec3_add(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_adds(ivec3 a, int s, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_sub(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_subs(ivec3 a, int s, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_mul(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_scale(ivec3 v, int s, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_scale_as(ivec3 v, int s, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_div(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_divs(ivec3 a, int s, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_addadd(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_subadd(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_muladd(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_muladds(ivec3 a, int s, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_maxadd(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_minadd(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_flipsign(ivec3 v);
   CGLM_INLINE void  glm_ivec3_flipsign_to(ivec3 v, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_negate_to(ivec3 v, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_negate(ivec3 v);
   CGLM_INLINE void  glm_ivec3_inv(ivec3 v);
   CGLM_INLINE void  glm_ivec3_inv_to(ivec3 v, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_normalize(ivec3 v);
   CGLM_INLINE void  glm_ivec3_normalize_to(ivec3 v, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_cross(ivec3 a, ivec3 b, ivec3 d);
   CGLM_INLINE void  glm_ivec3_crossn(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE int glm_ivec3_angle(ivec3 a, ivec3 b);
   CGLM_INLINE void  glm_ivec3_rotate(ivec3 v, int angle, ivec3 axis);
   CGLM_INLINE void  glm_ivec3_rotate_m4(mat4 m, ivec3 v, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_rotate_m3(mat3 m, ivec3 v, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_proj(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_center(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE int glm_ivec3_distance(ivec3 a, ivec3 b);
   CGLM_INLINE int glm_ivec3_distance2(ivec3 a, ivec3 b);
   CGLM_INLINE void  glm_ivec3_maxv(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_minv(ivec3 a, ivec3 b, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_ortho(ivec3 v, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_clamp(ivec3 v, int minVal, int maxVal);
   CGLM_INLINE void  glm_ivec3_lerp(ivec3 from, ivec3 to, int t, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_lerpc(ivec3 from, ivec3 to, int t, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_mix(ivec3 from, ivec3 to, int t, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_mixc(ivec3 from, ivec3 to, int t, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_step_uni(int edge, ivec3 x, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_step(ivec3 edge, ivec3 x, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_smoothstep_uni(int edge0, int edge1, ivec3 x, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_smoothstep(ivec3 edge0, ivec3 edge1, ivec3 x, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_smoothinterp(ivec3 from, ivec3 to, int t, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_smoothinterpc(ivec3 from, ivec3 to, int t, ivec3 dest);
   CGLM_INLINE void  glm_ivec3_swizzle(ivec3 v, int mask, ivec3 dest);

 Convenient:
   CGLM_INLINE void  glm_cross(ivec3 a, ivec3 b, ivec3 d);
   CGLM_INLINE int glm_dot(ivec3 a, ivec3 b);
   CGLM_INLINE void  glm_normalize(ivec3 v);
   CGLM_INLINE void  glm_normalize_to(ivec3 v, ivec3 dest);

 DEPRECATED:
   glm_ivec3_dup
   glm_ivec3_flipsign
   glm_ivec3_flipsign_to
   glm_ivec3_inv
   glm_ivec3_inv_to
   glm_ivec3_mulv
 */

#ifndef cglm_ivec3_h
#define cglm_ivec3_h

#include "cglm/common.h"
#include "cglm/util.h"

/* DEPRECATED! use _copy, _ucopy versions */
#define glm_ivec3_dup(v, dest)         glm_ivec3_copy(v, dest)
#define glm_ivec3_flipsign(v)          glm_ivec3_negate(v)
#define glm_ivec3_flipsign_to(v, dest) glm_ivec3_negate_to(v, dest)
#define glm_ivec3_inv(v)               glm_ivec3_negate(v)
#define glm_ivec3_inv_to(v, dest)      glm_ivec3_negate_to(v, dest)
#define glm_ivec3_mulv(a, b, d)        glm_ivec3_mul(a, b, d)

#define GLM_IVEC3_ONE_INIT   {1, 1, 1}
#define GLM_IVEC3_ZERO_INIT  {0, 0, 0}

#define GLM_IVEC3_ONE  ((ivec3)GLM_IVEC3_ONE_INIT)
#define GLM_IVEC3_ZERO ((ivec3)GLM_IVEC3_ZERO_INIT)

#define GLM_IVEC3_YUP       ((ivec3){0,  1,  0})
#define GLM_IVEC3_ZUP       ((ivec3){0,  0,  1})
#define GLM_IVEC3_XUP       ((ivec3){1,  0,  0})
#define GLM_IVEC3_FORWARD   ((ivec3){0,  0, -1})

#define GLM_IVEC3_XXX GLM_SHUFFLE3(0, 0, 0)
#define GLM_IVEC3_YYY GLM_SHUFFLE3(1, 1, 1)
#define GLM_IVEC3_ZZZ GLM_SHUFFLE3(2, 2, 2)
#define GLM_IVEC3_ZYX GLM_SHUFFLE3(0, 1, 2)

/*!
 * @brief init ivec3 using vec4
 *
 * @param[in]  v4   vector4
 * @param[out] dest destination
 */
CGLM_INLINE
void
glm_ivec3(vec4 v4, ivec3 dest) {
  dest[0] = v4[0];
  dest[1] = v4[1];
  dest[2] = v4[2];
}

/*!
 * @brief copy all members of [a] to [dest]
 *
 * @param[in]  a    source
 * @param[out] dest destination
 */
CGLM_INLINE
void
glm_ivec3_copy(ivec3 a, ivec3 dest) {
  dest[0] = a[0];
  dest[1] = a[1];
  dest[2] = a[2];
}

/*!
 * @brief make vector zero
 *
 * @param[in, out]  v vector
 */
CGLM_INLINE
void
glm_ivec3_zero(ivec3 v) {
  v[0] = v[1] = v[2] = 0;
}

/*!
 * @brief make vector one
 *
 * @param[in, out]  v vector
 */
CGLM_INLINE
void
glm_ivec3_one(ivec3 v) {
  v[0] = v[1] = v[2] = 1;
}

/*!
 * @brief ivec3 dot product
 *
 * @param[in] a vector1
 * @param[in] b vector2
 *
 * @return dot product
 */
CGLM_INLINE
int
glm_ivec3_dot(ivec3 a, ivec3 b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

/*!
 * @brief norm * norm (magnitude) of vec
 *
 * we can use this func instead of calling norm * norm, because it would call
 * sqrtf fuction twice but with this func we can avoid func call, maybe this is
 * not good name for this func
 *
 * @param[in] v vector
 *
 * @return norm * norm
 */
CGLM_INLINE
int
glm_ivec3_norm2(ivec3 v) {
  return glm_ivec3_dot(v, v);
}

/*!
 * @brief euclidean norm (magnitude), also called L2 norm
 *        this will give magnitude of vector in euclidean space
 *
 * @param[in] v vector
 *
 * @return norm
 */
CGLM_INLINE
int
glm_ivec3_norm(ivec3 v) {
  return (int) sqrtf(glm_ivec3_norm2(v));
}

/*!
 * @brief add a vector to b vector store result in dest
 *
 * @param[in]  a    vector1
 * @param[in]  b    vector2
 * @param[out] dest destination vector
 */
CGLM_INLINE
void
glm_ivec3_add(ivec3 a, ivec3 b, ivec3 dest) {
  dest[0] = a[0] + b[0];
  dest[1] = a[1] + b[1];
  dest[2] = a[2] + b[2];
}

/*!
 * @brief add scalar to v vector store result in dest (d = v + s)
 *
 * @param[in]  v    vector
 * @param[in]  s    scalar
 * @param[out] dest destination vector
 */
CGLM_INLINE
void
glm_ivec3_adds(ivec3 v, int s, ivec3 dest) {
  dest[0] = v[0] + s;
  dest[1] = v[1] + s;
  dest[2] = v[2] + s;
}

/*!
 * @brief subtract b vector from a vector store result in dest
 *
 * @param[in]  a    vector1
 * @param[in]  b    vector2
 * @param[out] dest destination vector
 */
CGLM_INLINE
void
glm_ivec3_sub(ivec3 a, ivec3 b, ivec3 dest) {
  dest[0] = a[0] - b[0];
  dest[1] = a[1] - b[1];
  dest[2] = a[2] - b[2];
}

/*!
 * @brief subtract scalar from v vector store result in dest (d = v - s)
 *
 * @param[in]  v    vector
 * @param[in]  s    scalar
 * @param[out] dest destination vector
 */
CGLM_INLINE
void
glm_ivec3_subs(ivec3 v, int s, ivec3 dest) {
  dest[0] = v[0] - s;
  dest[1] = v[1] - s;
  dest[2] = v[2] - s;
}

/*!
 * @brief multiply two vector (component-wise multiplication)
 *
 * @param a    vector1
 * @param b    vector2
 * @param dest v3 = (a[0] * b[0], a[1] * b[1], a[2] * b[2])
 */
CGLM_INLINE
void
glm_ivec3_mul(ivec3 a, ivec3 b, ivec3 dest) {
  dest[0] = a[0] * b[0];
  dest[1] = a[1] * b[1];
  dest[2] = a[2] * b[2];
}

/*!
 * @brief multiply/scale ivec3 vector with scalar: result = v * s
 *
 * @param[in]  v    vector
 * @param[in]  s    scalar
 * @param[out] dest destination vector
 */
CGLM_INLINE
void
glm_ivec3_scale(ivec3 v, int s, ivec3 dest) {
  dest[0] = v[0] * s;
  dest[1] = v[1] * s;
  dest[2] = v[2] * s;
}

/*!
 * @brief div vector with another component-wise division: d = a / b
 *
 * @param[in]  a    vector 1
 * @param[in]  b    vector 2
 * @param[out] dest result = (a[0]/b[0], a[1]/b[1], a[2]/b[2])
 */
CGLM_INLINE
void
glm_ivec3_div(ivec3 a, ivec3 b, ivec3 dest) {
  dest[0] = a[0] / b[0];
  dest[1] = a[1] / b[1];
  dest[2] = a[2] / b[2];
}

CGLM_INLINE
void
glm_ivec3_mod(ivec3 a, ivec3 b, ivec3 dest) {
  dest[0] = a[0] % b[0];
  dest[1] = a[1] % b[1];
  dest[2] = a[2] % b[2];
}


/*!
 * @brief div vector with scalar: d = v / s
 *
 * @param[in]  v    vector
 * @param[in]  s    scalar
 * @param[out] dest result = (a[0]/s, a[1]/s, a[2]/s)
 */
CGLM_INLINE
void
glm_ivec3_divs(ivec3 v, int s, ivec3 dest) {
  dest[0] = v[0] / s;
  dest[1] = v[1] / s;
  dest[2] = v[2] / s;
}

#endif