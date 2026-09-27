#ifndef MATH3D_H
#define MATH3D_H

#include <math.h>

#define PI 3.14159265358979323846f
#define DEG2RAD (PI / 180.0f)
#define RAD2DEG (180.0f / PI)

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    Vec3 orig;
    Vec3 dir; // assumed normalized
} Ray3;

typedef struct {
    Vec3 min;
    Vec3 max;
} BoundingBox3;

static inline Vec3 vec3_create(float x, float y, float z) {
    Vec3 v = {x, y, z};
    return v;
}

static inline Vec3 vec3_add(Vec3 a, Vec3 b) {
    return vec3_create(a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline Vec3 vec3_sub(Vec3 a, Vec3 b) {
    return vec3_create(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline Vec3 vec3_scale(Vec3 v, float s) {
    return vec3_create(v.x * s, v.y * s, v.z * s);
}

static inline float vec3_dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline Vec3 vec3_cross(Vec3 a, Vec3 b) {
    return vec3_create(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

static inline float vec3_len(Vec3 v) {
    return sqrtf(vec3_dot(v, v));
}

static inline Vec3 vec3_normalize(Vec3 v) {
    float l = vec3_len(v);
    if (l > 1e-6f) {
        float inv = 1.0f / l;
        return vec3_scale(v, inv);
    }
    return vec3_create(0, 0, 0);
}

// Ray vs Axis-Aligned Bounding Box (Slab test)
static inline int ray_intersect_aabb(Ray3 ray, BoundingBox3 box, float* t_out) {
    float tmin = -1e20f;
    float tmax = 1e20f;

    // X axis
    if (fabsf(ray.dir.x) < 1e-7f) {
        if (ray.orig.x < box.min.x || ray.orig.x > box.max.x) return 0;
    } else {
        float invD = 1.0f / ray.dir.x;
        float t1 = (box.min.x - ray.orig.x) * invD;
        float t2 = (box.max.x - ray.orig.x) * invD;
        if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return 0;
    }

    // Y axis
    if (fabsf(ray.dir.y) < 1e-7f) {
        if (ray.orig.y < box.min.y || ray.orig.y > box.max.y) return 0;
    } else {
        float invD = 1.0f / ray.dir.y;
        float t1 = (box.min.y - ray.orig.y) * invD;
        float t2 = (box.max.y - ray.orig.y) * invD;
        if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return 0;
    }

    // Z axis
    if (fabsf(ray.dir.z) < 1e-7f) {
        if (ray.orig.z < box.min.z || ray.orig.z > box.max.z) return 0;
    } else {
        float invD = 1.0f / ray.dir.z;
        float t1 = (box.min.z - ray.orig.z) * invD;
        float t2 = (box.max.z - ray.orig.z) * invD;
        if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return 0;
    }

    if (tmax < 0.0f) return 0; // Box is behind ray
    float t_hit = (tmin > 0.0f) ? tmin : tmax;
    if (t_out) *t_out = t_hit;
    return 1;
}

#endif // MATH3D_H
