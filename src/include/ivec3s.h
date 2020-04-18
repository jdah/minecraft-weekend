/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*
 Macros:
   GLMS_IVEC3_ONE_INIT
   GLMS_IVEC3_ZERO_INIT
   GLMS_IVEC3_ONE
   GLMS_IVEC3_ZERO
   GLMS_YUP
   GLMS_ZUP
   GLMS_XUP

 Functions:
   CGLM_INLINE ivec3s glms_ivec3(vec4s v4);
   CGLM_INLINE void  glms_ivec3_pack(ivec3s dst[], ivec3 src[], size_t len);
   CGLM_INLINE void  glms_ivec3_unpack(ivec3 dst[], ivec3s src[], size_t len);
   CGLM_INLINE ivec3s glms_ivec3_zero(void);
   CGLM_INLINE ivec3s glms_ivec3_one(void);
   CGLM_INLINE int glms_ivec3_dot(ivec3s a, ivec3s b);
   CGLM_INLINE int glms_ivec3_norm2(ivec3s v);
   CGLM_INLINE int glms_ivec3_norm(ivec3s v);
   CGLM_INLINE int glms_ivec3_norm_one(ivec3s v);
   CGLM_INLINE int glms_ivec3_norm_inf(ivec3s v);
   CGLM_INLINE ivec3s glms_ivec3_add(ivec3s a, ivec3s b);
   CGLM_INLINE ivec3s glms_ivec3_adds(ivec3s a, int s);
   CGLM_INLINE ivec3s glms_ivec3_sub(ivec3s a, ivec3s b);
   CGLM_INLINE ivec3s glms_ivec3_subs(ivec3s a, int s);
   CGLM_INLINE ivec3s glms_ivec3_mul(ivec3s a, ivec3s b);
   CGLM_INLINE ivec3s glms_ivec3_scale(ivec3s v, int s);
   CGLM_INLINE ivec3s glms_ivec3_scale_as(ivec3s v, int s);
   CGLM_INLINE ivec3s glms_ivec3_div(ivec3s a, ivec3s b);
   CGLM_INLINE ivec3s glms_ivec3_divs(ivec3s a, int s);
   CGLM_INLINE ivec3s glms_ivec3_addadd(ivec3s a, ivec3s b, ivec3s dest);
   CGLM_INLINE ivec3s glms_ivec3_subadd(ivec3s a, ivec3s b, ivec3s dest);
   CGLM_INLINE ivec3s glms_ivec3_muladd(ivec3s a, ivec3s b, ivec3s dest);
   CGLM_INLINE ivec3s glms_ivec3_muladds(ivec3s a, int s, ivec3s dest);
   CGLM_INLINE ivec3s glms_ivec3_maxadd(ivec3s a, ivec3s b, ivec3s dest);
   CGLM_INLINE ivec3s glms_ivec3_minadd(ivec3s a, ivec3s b, ivec3s dest);
   CGLM_INLINE ivec3s glms_ivec3_flipsign(ivec3s v);
   CGLM_INLINE ivec3s glms_ivec3_negate(ivec3s v);
   CGLM_INLINE ivec3s glms_ivec3_inv(ivec3s v);
   CGLM_INLINE ivec3s glms_ivec3_normalize(ivec3s v);
   CGLM_INLINE ivec3s glms_ivec3_cross(ivec3s a, ivec3s b);
   CGLM_INLINE ivec3s glms_ivec3_crossn(ivec3s a, ivec3s b);
   CGLM_INLINE int glms_ivec3_angle(ivec3s a, ivec3s b);
   CGLM_INLINE ivec3s glms_ivec3_rotate(ivec3s v, int angle, ivec3s axis);
   CGLM_INLINE ivec3s glms_ivec3_rotate_m4(mat4s m, ivec3s v);
   CGLM_INLINE ivec3s glms_ivec3_rotate_m3(mat3s m, ivec3s v);
   CGLM_INLINE ivec3s glms_ivec3_proj(ivec3s a, ivec3s b);
   CGLM_INLINE ivec3s glms_ivec3_center(ivec3s a, ivec3s b);
   CGLM_INLINE int glms_ivec3_distance(ivec3s a, ivec3s b);
   CGLM_INLINE int glms_ivec3_distance2(ivec3s a, ivec3s b);
   CGLM_INLINE ivec3s glms_ivec3_maxv(ivec3s a, ivec3s b);
   CGLM_INLINE ivec3s glms_ivec3_minv(ivec3s a, ivec3s b);
   CGLM_INLINE ivec3s glms_ivec3_ortho(ivec3s v);
   CGLM_INLINE ivec3s glms_ivec3_clamp(ivec3s v, int minVal, int maxVal);
   CGLM_INLINE ivec3s glms_ivec3_lerp(ivec3s from, ivec3s to, int t);
   CGLM_INLINE ivec3s glms_ivec3_lerpc(ivec3s from, ivec3s to, int t);
   CGLM_INLINE ivec3s glms_ivec3_mix(ivec3s from, ivec3s to, int t);
   CGLM_INLINE ivec3s glms_ivec3_mixc(ivec3s from, ivec3s to, int t);
   CGLM_INLINE ivec3s glms_ivec3_step_uni(int edge, ivec3s x);
   CGLM_INLINE ivec3s glms_ivec3_step(ivec3s edge, ivec3s x);
   CGLM_INLINE ivec3s glms_ivec3_smoothstep_uni(int edge0, int edge1, ivec3s x);
   CGLM_INLINE ivec3s glms_ivec3_smoothstep(ivec3s edge0, ivec3s edge1, ivec3s x);
   CGLM_INLINE ivec3s glms_ivec3_smoothinterp(ivec3s from, ivec3s to, int t);
   CGLM_INLINE ivec3s glms_ivec3_smoothinterpc(ivec3s from, ivec3s to, int t);
   CGLM_INLINE ivec3s glms_ivec3_swizzle(ivec3s v, int mask);

 Convenient:
   CGLM_INLINE ivec3s glms_cross(ivec3s a, ivec3s b);
   CGLM_INLINE int glms_dot(ivec3s a, ivec3s b);
   CGLM_INLINE ivec3s glms_normalize(ivec3s v);
 */

#ifndef cglms_ivec3s_h
#define cglms_ivec3s_h

#include <cglm/common.h>
#include <cglm/types-struct.h>
#include <cglm/util.h>

#include "ivec3.h"

#define GLMS_IVEC3_ONE_INIT   {GLM_IVEC3_ONE_INIT}
#define GLMS_IVEC3_ZERO_INIT  {GLM_IVEC3_ZERO_INIT}

#define GLMS_IVEC3_ONE  ((ivec3s)GLMS_IVEC3_ONE_INIT)
#define GLMS_IVEC3_ZERO ((ivec3s)GLMS_IVEC3_ZERO_INIT)

#define GLMS_IVEC3S_YUP  ((ivec3s){{0.0f, 1.0f, 0.0f}})
#define GLMS_IVEC3S_ZUP  ((ivec3s){{0.0f, 0.0f, 1.0f}})
#define GLMS_IVEC3S_XUP  ((ivec3s){{1.0f, 0.0f, 0.0f}})

/*!
 * @brief init ivec3 using vec4
 *
 * @param[in]  v4   vector4
 * @returns         destination
 */
CGLM_INLINE
ivec3s
glms_ivec3(vec4s v4) {
  ivec3s r;
  glm_ivec3(v4.raw, r.raw);
  return r;
}

/*!
 * @brief pack an array of ivec3 into an array of ivec3s
 *
 * @param[out] dst array of ivec3
 * @param[in]  src array of ivec3s
 * @param[in]  len number of elements
 */
CGLM_INLINE
void
glms_ivec3_pack(ivec3s dst[], ivec3 src[], size_t len) {
  size_t i;

  for (i = 0; i < len; i++) {
    glm_ivec3_copy(src[i], dst[i].raw);
  }
}

/*!
 * @brief unpack an array of ivec3s into an array of ivec3
 *
 * @param[out] dst array of ivec3s
 * @param[in]  src array of ivec3
 * @param[in]  len number of elements
 */
CGLM_INLINE
void
glms_ivec3_unpack(ivec3 dst[], ivec3s src[], size_t len) {
  size_t i;

  for (i = 0; i < len; i++) {
    glm_ivec3_copy(src[i].raw, dst[i]);
  }
}

/*!
 * @brief make vector zero
 *
 * @returns       zero vector
 */
CGLM_INLINE
ivec3s
glms_ivec3_zero(void) {
  ivec3s r;
  glm_ivec3_zero(r.raw);
  return r;
}

/*!
 * @brief make vector one
 *
 * @returns       one vector
 */
CGLM_INLINE
ivec3s
glms_ivec3_one(void) {
  ivec3s r;
  glm_ivec3_one(r.raw);
  return r;
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
glms_ivec3_dot(ivec3s a, ivec3s b) {
  return glm_ivec3_dot(a.raw, b.raw);
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
glms_ivec3_norm2(ivec3s v) {
  return glm_ivec3_norm2(v.raw);
}

/*!
 * @brief norm (magnitude) of vec3
 *
 * @param[in] v vector
 *
 * @return norm
 */
CGLM_INLINE
int
glms_ivec3_norm(ivec3s v) {
  return glm_ivec3_norm(v.raw);
}

/*!
 * @brief add a vector to b vector store result in dest
 *
 * @param[in]  a    vector1
 * @param[in]  b    vector2
 * @returns         destination vector
 */
CGLM_INLINE
ivec3s
glms_ivec3_add(ivec3s a, ivec3s b) {
  ivec3s r;
  glm_ivec3_add(a.raw, b.raw, r.raw);
  return r;
}

/*!
 * @brief add scalar to v vector store result in dest (d = v + s)
 *
 * @param[in]  a    vector
 * @param[in]  s    scalar
 * @returns         destination vector
 */
CGLM_INLINE
ivec3s
glms_ivec3_adds(ivec3s a, int s) {
  ivec3s r;
  glm_ivec3_adds(a.raw, s, r.raw);
  return r;
}

/*!
 * @brief subtract b vector from a vector store result in dest
 *
 * @param[in]  a    vector1
 * @param[in]  b    vector2
 * @returns         destination vector
 */
CGLM_INLINE
ivec3s
glms_ivec3_sub(ivec3s a, ivec3s b) {
  ivec3s r;
  glm_ivec3_sub(a.raw, b.raw, r.raw);
  return r;
}

/*!
 * @brief subtract scalar from v vector store result in dest (d = v - s)
 *
 * @param[in]  a    vector
 * @param[in]  s    scalar
 * @returns         destination vector
 */
CGLM_INLINE
ivec3s
glms_ivec3_subs(ivec3s a, int s) {
  ivec3s r;
  glm_ivec3_subs(a.raw, s, r.raw);
  return r;
}

/*!
 * @brief multiply two vector (component-wise multiplication)
 *
 * @param     a     vector1
 * @param     b     vector2
 * @returns         v3 = (a[0] * b[0], a[1] * b[1], a[2] * b[2])
 */
CGLM_INLINE
ivec3s
glms_ivec3_mul(ivec3s a, ivec3s b) {
  ivec3s r;
  glm_ivec3_mul(a.raw, b.raw, r.raw);
  return r;
}

/*!
 * @brief multiply/scale ivec3 vector with scalar: result = v * s
 *
 * @param[in]  v    vector
 * @param[in]  s    scalar
 * @returns         destination vector
 */
CGLM_INLINE
ivec3s
glms_ivec3_scale(ivec3s v, int s) {
  ivec3s r;
  glm_ivec3_scale(v.raw, s, r.raw);
  return r;
}

/*!
 * @brief div vector with another component-wise division: d = a / b
 *
 * @param[in]  a    vector 1
 * @param[in]  b    vector 2
 * @returns         result = (a[0]/b[0], a[1]/b[1], a[2]/b[2])
 */
CGLM_INLINE
ivec3s
glms_ivec3_div(ivec3s a, ivec3s b) {
  ivec3s r;
  glm_ivec3_div(a.raw, b.raw, r.raw);
  return r;
}

CGLM_INLINE
ivec3s
glms_ivec3_mod(ivec3s a, ivec3s b) {
  ivec3s r;
  glm_ivec3_mod(a.raw, b.raw, r.raw);
  return r;
}

/*!
 * @brief div vector with scalar: d = v / s
 *
 * @param[in]  a    vector
 * @param[in]  s    scalar
 * @returns         result = (a[0]/s, a[1]/s, a[2]/s)
 */
CGLM_INLINE
ivec3s
glms_ivec3_divs(ivec3s a, int s) {
  ivec3s r;
  glm_ivec3_divs(a.raw, s, r.raw);
  return r;
}

#endif
