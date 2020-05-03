/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*
 Macros:
   GLM_IVEC2_ONE_INIT
   GLM_IVEC2_ZERO_INIT
   GLM_IVEC2_ONE
   GLM_IVEC2_ZERO

 Functions:
   CGLM_INLINE void  glm_ivec2(int * __restrict v, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_copy(ivec2 a, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_zero(ivec2 v)
   CGLM_INLINE void  glm_ivec2_one(ivec2 v)
   CGLM_INLINE int glm_ivec2_dot(ivec2 a, ivec2 b)
   CGLM_INLINE int glm_ivec2_cross(ivec2 a, ivec2 b)
   CGLM_INLINE int glm_ivec2_norm2(ivec2 v)
   CGLM_INLINE int glm_ivec2_norm(ivec2 vec)
   CGLM_INLINE void  glm_ivec2_add(ivec2 a, ivec2 b, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_adds(ivec2 v, int s, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_sub(ivec2 a, ivec2 b, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_subs(ivec2 v, int s, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_mul(ivec2 a, ivec2 b, ivec2 d)
   CGLM_INLINE void  glm_ivec2_scale(ivec2 v, int s, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_scale_as(ivec2 v, int s, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_div(ivec2 a, ivec2 b, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_divs(ivec2 v, int s, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_addadd(ivec2 a, ivec2 b, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_subadd(ivec2 a, ivec2 b, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_muladd(ivec2 a, ivec2 b, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_muladds(ivec2 a, int s, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_maxadd(ivec2 a, ivec2 b, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_minadd(ivec2 a, ivec2 b, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_negate_to(ivec2 v, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_negate(ivec2 v)
   CGLM_INLINE void  glm_ivec2_normalize(ivec2 v)
   CGLM_INLINE void  glm_ivec2_normalize_to(ivec2 vec, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_rotate(ivec2 v, int angle, ivec2 dest)
   CGLM_INLINE int glm_ivec2_distance2(ivec2 a, ivec2 b)
   CGLM_INLINE int glm_ivec2_distance(ivec2 a, ivec2 b)
   CGLM_INLINE void  glm_ivec2_maxv(ivec2 v1, ivec2 v2, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_minv(ivec2 v1, ivec2 v2, ivec2 dest)
   CGLM_INLINE void  glm_ivec2_clamp(ivec2 v, int minVal, int maxVal)
   CGLM_INLINE void  glm_ivec2_lerp(ivec2 from, ivec2 to, int t, ivec2 dest)

 */

#ifndef cglm_ivec2_h
#define cglm_ivec2_h

#include <cglm/common.h>
#include <cglm/util.h>

typedef int ivec2[2];

#define GLM_IVEC2_ONE_INIT   {1.0f, 1.0f}
#define GLM_IVEC2_ZERO_INIT  {0.0f, 0.0f}

#define GLM_IVEC2_ONE  ((ivec2)GLM_IVEC2_ONE_INIT)
#define GLM_IVEC2_ZERO ((ivec2)GLM_IVEC2_ZERO_INIT)

/*!
 * @brief init ivec2 using another vector
 *
 * @param[in]  v    a vector
 * @param[out] dest destination
 */
CGLM_INLINE
void
glm_ivec2(int * __restrict v, ivec2 dest) {
  dest[0] = v[0];
  dest[1] = v[1];
}

/*!
 * @brief copy all members of [a] to [dest]
 *
 * @param[in]  a    source
 * @param[out] dest destination
 */
CGLM_INLINE
void
glm_ivec2_copy(ivec2 a, ivec2 dest) {
  dest[0] = a[0];
  dest[1] = a[1];
}

/*!
 * @brief make vector zero
 *
 * @param[in, out]  v vector
 */
CGLM_INLINE
void
glm_ivec2_zero(ivec2 v) {
  v[0] = v[1] = 0.0f;
}

/*!
 * @brief make vector one
 *
 * @param[in, out]  v vector
 */
CGLM_INLINE
void
glm_ivec2_one(ivec2 v) {
  v[0] = v[1] = 1.0f;
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
glm_ivec2_dot(ivec2 a, ivec2 b) {
  return a[0] * b[0] + a[1] * b[1];
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
glm_ivec2_cross(ivec2 a, ivec2 b) {
  /* just calculate the z-component */
  return a[0] * b[1] - a[1] * b[0];
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
glm_ivec2_add(ivec2 a, ivec2 b, ivec2 dest) {
  dest[0] = a[0] + b[0];
  dest[1] = a[1] + b[1];
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
glm_ivec2_adds(ivec2 v, int s, ivec2 dest) {
  dest[0] = v[0] + s;
  dest[1] = v[1] + s;
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
glm_ivec2_sub(ivec2 a, ivec2 b, ivec2 dest) {
  dest[0] = a[0] - b[0];
  dest[1] = a[1] - b[1];
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
glm_ivec2_subs(ivec2 v, int s, ivec2 dest) {
  dest[0] = v[0] - s;
  dest[1] = v[1] - s;
}

/*!
 * @brief multiply two vector (component-wise multiplication)
 *
 * @param a    v1
 * @param b    v2
 * @param dest v3 = (a[0] * b[0], a[1] * b[1])
 */
CGLM_INLINE
void
glm_ivec2_mul(ivec2 a, ivec2 b, ivec2 dest) {
  dest[0] = a[0] * b[0];
  dest[1] = a[1] * b[1];
}

/*!
 * @brief multiply/scale vector with scalar: result = v * s
 *
 * @param[in]  v    vector
 * @param[in]  s    scalar
 * @param[out] dest destination vector
 */
CGLM_INLINE
void
glm_ivec2_scale(ivec2 v, int s, ivec2 dest) {
  dest[0] = v[0] * s;
  dest[1] = v[1] * s;
}

/*!
 * @brief div vector with another component-wise division: d = a / b
 *
 * @param[in]  a    vector 1
 * @param[in]  b    vector 2
 * @param[out] dest result = (a[0]/b[0], a[1]/b[1])
 */
CGLM_INLINE
void
glm_ivec2_div(ivec2 a, ivec2 b, ivec2 dest) {
  dest[0] = a[0] / b[0];
  dest[1] = a[1] / b[1];
}


/*!
 * @brief mod vector with another component-wise modulo: d = a % b
 *
 * @param[in]  a    vector 1
 * @param[in]  b    vector 2
 * @returns         result = (a[0]%b[0], a[1]%b[1])
 */
CGLM_INLINE
void
glm_ivec2_mod(ivec2 a, ivec2 b, ivec2 dest) {
  dest[0] = a[0] % b[0];
  dest[1] = a[1] % b[1];
}


/*!
 * @brief div vector with scalar: d = v / s
 *
 * @param[in]  v    vector
 * @param[in]  s    scalar
 * @param[out] dest result = (a[0]/s, a[1]/s)
 */
CGLM_INLINE
void
glm_ivec2_divs(ivec2 v, int s, ivec2 dest) {
  dest[0] = v[0] / s;
  dest[1] = v[1] / s;
}

CGLM_INLINE
void
glm_ivec2_print(ivec2            vec,
                FILE * __restrict ostream) {
  int i;

#define m 2

  fprintf(ostream, "Vector (int%d):\n\t|", m);

  for (i = 0; i < m; i++) {
    fprintf(ostream, "%d", vec[i]);

    if (i != m - 1)
      fprintf(ostream, "\t");
  }

  fprintf(ostream, "|\n\n");
  
#undef m
}


#endif /* cglm_ivec2_h */
