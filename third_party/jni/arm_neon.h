#ifndef GOF2_THIRD_PARTY_ARM_NEON_H
#define GOF2_THIRD_PARTY_ARM_NEON_H

#if (defined(__ARM_NEON) || defined(__ARM_NEON__)) && defined(__GNUC__)
#include_next <arm_neon.h>
#else

#include <cstdint>
#include <cstring>

typedef struct { float v[4]; } float32x4_t;
typedef struct { int32_t v[4]; } int32x4_t;
typedef struct { uint32_t v[4]; } uint32x4_t;
typedef struct { uint64_t v[2]; } uint64x2_t;

static inline float32x4_t vld1q_f32(const float *p) {
    float32x4_t r;
    std::memcpy(r.v, p, sizeof(r.v));
    return r;
}

static inline void vst1q_f32(float *p, float32x4_t a) {
    std::memcpy(p, a.v, sizeof(a.v));
}

static inline void vst1q_u32(uint32_t *p, uint32x4_t a) {
    std::memcpy(p, a.v, sizeof(a.v));
}

static inline void vst1q_u64(uint64_t *p, uint64x2_t a) {
    std::memcpy(p, a.v, sizeof(a.v));
}

static inline float32x4_t vdupq_n_f32(float x) {
    return {{x, x, x, x}};
}

static inline uint32x4_t vdupq_n_u32(uint32_t x) {
    return {{x, x, x, x}};
}

static inline int32x4_t vdupq_n_s32(int32_t x) {
    return {{x, x, x, x}};
}

static inline float32x4_t vsubq_f32(float32x4_t a, float32x4_t b) {
    return {{a.v[0] - b.v[0], a.v[1] - b.v[1], a.v[2] - b.v[2], a.v[3] - b.v[3]}};
}

static inline float32x4_t vmlaq_f32(float32x4_t a, float32x4_t b, float32x4_t c) {
    return {{a.v[0] + b.v[0] * c.v[0],
             a.v[1] + b.v[1] * c.v[1],
             a.v[2] + b.v[2] * c.v[2],
             a.v[3] + b.v[3] * c.v[3]}};
}

static inline float32x4_t vnegq_f32(float32x4_t a) {
    return {{-a.v[0], -a.v[1], -a.v[2], -a.v[3]}};
}

static inline float vgetq_lane_f32(float32x4_t a, int lane) {
    return a.v[lane];
}

static inline uint64x2_t vreinterpretq_u64_u32(uint32x4_t a) {
    uint64x2_t r;
    std::memcpy(r.v, a.v, sizeof(r.v));
    return r;
}

#endif

#endif
