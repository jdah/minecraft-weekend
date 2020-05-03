/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*
 Macros:
   GLMS_IVEC2_ONE_INIT
   GLMS_IVEC2_ZERO_INIT
   GLMS_IVEC2_ONE
   GLMS_IVEC2_ZERO

 Functions:
   CGLM_INLINE ivec2s glms_ivec2(vec3s v3)
   CGLM_INLINE void  glms_ivec2_pack(ivec2s dst[], ivec2 src[], size_t len)
   CGLM_INLINE void  glms_ivec2_unpack(ivec2 dst[], ivec2s src[], size_t len)
   CGLM_INLINE ivec2s glms_ivec2_zero(void)
   CGLM_INLINE ivec2s glms_ivec2_one(void)
   CGLM_INLINE int glms_ivec2_dot(ivec2s a, ivec2s b)
   CGLM_INLINE int glms_ivec2_cross(ivec2s a, ivec2s b)
   CGLM_INLINE int glms_ivec2_norm2(ivec2s v)
   CGLM_INLINE int glms_ivec2_norm(ivec2s v)
   CGLM_INLINE ivec2s glms_ivec2_add(ivec2s a, ivec2s b)
   CGLM_INLINE ivec2s glms_ivec2_adds(ivec2s a, int s)
   CGLM_INLINE ivec2s glms_ivec2_sub(ivec2s a, ivec2s b)
   CGLM_INLINE ivec2s glms_ivec2_subs(ivec2s a, int s)
   CGLM_INLINE ivec2s glms_ivec2_mul(ivec2s a, ivec2s b)
   CGLM_INLINE ivec2s glms_ivec2_scale(ivec2s v, int s)
   CGLM_INLINE ivec2s glms_ivec2_scale_as(ivec2s v, int s)
   CGLM_INLINE ivec2s glms_ivec2_div(ivec2s a, ivec2s b)
   CGLM_INLINE ivec2s glms_ivec2_divs(ivec2s a, int s)
   CGLM_INLINE ivec2s glms_ivec2_addadd(ivec2s a, ivec2s b, ivec2s dest)
   CGLM_INLINE ivec2s glms_ivec2_subadd(ivec2s a, ivec2s b, ivec2s dest)
   CGLM_INLINE ivec2s glms_ivec2_muladd(ivec2s a, ivec2s b, ivec2s dest)
   CGLM_INLINE ivec2s glms_ivec2_muladds(ivec2s a, int s, ivec2s dest)
   CGLM_INLINE ivec2s glms_ivec2_maxadd(ivec2s a, ivec2s b, ivec2s dest)
   CGLM_INLINE ivec2s glms_ivec2_minadd(ivec2s a, ivec2s b, ivec2s dest)
   CGLM_INLINE ivec2s glms_ivec2_negate(ivec2s v)
   CGLM_INLINE ivec2s glms_ivec2_normalize(ivec2s v)
   CGLM_INLINE ivec2s glms_ivec2_rotate(ivec2s v, int angle, ivec2s axis)
   CGLM_INLINE int glms_ivec2_distance(ivec2s a, ivec2s b)
   CGLM_INLINE int glms_ivec2_distance2(ivec2s a, ivec2s b)
   CGLM_INLINE ivec2s glms_ivec2_maxv(ivec2s a, ivec2s b)
   CGLM_INLINE ivec2s glms_ivec2_minv(ivec2s a, ivec2s b)
   CGLM_INLINE ivec2s glms_ivec2_clamp(ivec2s v, int minVal, int maxVal)
   CGLM_INLINE ivec2s glms_ivec2_lerp(ivec2s from, ivec2s to, int t)
 */

#ifndef cglms_ivec2s_h
#define cglms_ivec2s_h


#include <cglm/common.h>
#include <cglm/types-struct.h>
#include <cglm/util.h>

#include "ivec2.h"

typedef union ivec2s {
  ivec2 raw;
#if CGLM_USE_ANONYMOUS_STRUCT
  struct {
    int x;
    int y;
  };
#endif
} ivec2s;

#define GLMS_IVEC2_ONE_INIT   {GLM_IVEC2_ONE_INIT}
#define GLMS_IVEC2_ZERO_INIT  {GLM_IVEC2_ZERO_INIT}

#define GLMS_IVEC2_ONE  ((ivec2s)GLMS_IVEC2_ONE_INIT)
#define GLMS_IVEC2_ZERO ((ivec2s)GLMS_IVEC2_ZERO_INIT)

// /*!
//  * @brief init ivec2 using ivec2
//  *
//  * @param[in]  v3   vector3
//  * @returns         destination
//  */
// CGLM_INLINE
// ivec2s
// glms_ivec2(vec3s v3) {
//   ivec2s r;
//   glm_ivec2(v3.raw, r.raw);
//   return r;
// }

/*!
 * @brief pack an array of ivec2 into an array of ivec2s
 *
 * @param[out] dst array of ivec2
 * @param[in]  src array of ivec2s
 * @param[in]  len number of elements
 */
CGLM_INLINE
void
glms_ivec2_pack(ivec2s dst[], ivec2 src[], size_t len) {
  size_t i;

  for (i = 0; i < len; i++) {
    glm_ivec2_copy(src[i], dst[i].raw);
  }
}

/*!
 * @brief unpack an array of ivec2s into an array of ivec2
 *
 * @param[out] dst array of ivec2s
 * @param[in]  src array of ivec2
 * @param[in]  len number of elements
 */
CGLM_INLINE
void
glms_ivec2_unpack(ivec2 dst[], ivec2s src[], size_t len) {
  size_t i;

  for (i = 0; i < len; i++) {
    glm_ivec2_copy(src[i].raw, dst[i]);
  }
}

/*!
 * @brief make vector zero
 *
 * @returns zero vector
 */
CGLM_INLINE
ivec2s
glms_ivec2_zero(void) {
  ivec2s r;
  glm_ivec2_zero(r.raw);
  return r;
}

/*!
 * @brief make vector one
 *
 * @returns one vector
 */
CGLM_INLINE
ivec2s
glms_ivec2_one(void) {
  ivec2s r;
  glm_ivec2_one(r.raw);
  return r;
}

/*!
 * @brief ivec2 dot product
 *
 * @param[in] a vector1
 * @param[in] b vector2
 *
 * @return dot product
 */
CGLM_INLINE
int
glms_ivec2_dot(ivec2s a, ivec2s b) {
  return glm_ivec2_dot(a.raw, b.raw);
}

/*!
 * @brief ivec2 cross product
 *
 * REF: http://allenchou.net/2013/07/cross-product-of-2d-vectors/
 *
 * @param[in]  a vector1
 * @param[in]  b vector2
 *
 * @return Z component of cross product
 */
CGLM_INLINE
int
glms_ivec2_cross(ivec2s a, ivec2s b) {
  return glm_ivec2_cross(a.raw, b.raw);
}

/*!
 * @brief add a vector to b vector store result in dest
 *
 * @param[in]  a    vector1
 * @param[in]  b    vector2
 * @returns         destination vector
 */
CGLM_INLINE
ivec2s
glms_ivec2_add(ivec2s a, ivec2s b) {
  ivec2s r;
  glm_ivec2_add(a.raw, b.raw, r.raw);
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
ivec2s
glms_ivec2_adds(ivec2s a, int s) {
  ivec2s r;
  glm_ivec2_adds(a.raw, s, r.raw);
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
ivec2s
glms_ivec2_sub(ivec2s a, ivec2s b) {
  ivec2s r;
  glm_ivec2_sub(a.raw, b.raw, r.raw);
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
ivec2s
glms_ivec2_subs(ivec2s a, int s) {
  ivec2s r;
  glm_ivec2_subs(a.raw, s, r.raw);
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
ivec2s
glms_ivec2_mul(ivec2s a, ivec2s b) {
  ivec2s r;
  glm_ivec2_mul(a.raw, b.raw, r.raw);
  return r;
}

/*!
 * @brief multiply/scale ivec2 vector with scalar: result = v * s
 *
 * @param[in]  v    vector
 * @param[in]  s    scalar
 * @returns         destination vector
 */
CGLM_INLINE
ivec2s
glms_ivec2_scale(ivec2s v, int s) {
  ivec2s r;
  glm_ivec2_scale(v.raw, s, r.raw);
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
ivec2s
glms_ivec2_div(ivec2s a, ivec2s b) {
  ivec2s r;
  glm_ivec2_div(a.raw, b.raw, r.raw);
  return r;
}


/*!
 * @brief mod vector with another component-wise modulo: d = a % b
 *
 * @param[in]  a    vector 1
 * @param[in]  b    vector 2
 * @returns         result = (a[0]%b[0], a[1]%b[1])
 */
CGLM_INLINE
ivec2s
glms_ivec2_mod(ivec2s a, ivec2s b) {
  ivec2s r;
  glm_ivec2_mod(a.raw, b.raw, r.raw);
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
ivec2s
glms_ivec2_divs(ivec2s a, int s) {
  ivec2s r;
  glm_ivec2_divs(a.raw, s, r.raw);
  return r;
}

CGLM_INLINE
void
glms_ivec2_print(ivec2s            vec,
                 FILE * __restrict ostream) {
  glm_ivec2_print(vec.raw, ostream);
}

#endif