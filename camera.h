#ifndef CAMERA_H
#define CAMERA_H

#include "math3d.h"
#include "world.h"

#define PLAYER_EYE_HEIGHT 1.68f
#define WALK_SPEED 4.8f
#define JUMP_VELOCITY 5.2f
#define GRAVITY 14.0f

typedef struct {
    Vec3 pos;
    float yaw;
    float pitch;
    float vel_y;
    int on_ground;

    float bob_timer;
    float bob_y;

    int focus_mode;
    float focus_t;

    Vec3 forward;
    Vec3 right;
    Vec3 up;
} Camera;

static void camera_init(Camera* cam) {
    cam->pos = vec3_create(0.0f, 0.0f, 3.2f);
    cam->yaw = 0.0f;
    cam->pitch = -10.0f * DEG2RAD;
    cam->vel_y = 0.0f;
    cam->on_ground = 1;
    cam->bob_timer = 0.0f;
    cam->bob_y = 0.0f;
    cam->focus_mode = 0;
    cam->focus_t = 0.0f;
}

static void camera_update_vectors(Camera* cam) {
    float cp = cosf(cam->pitch);
    float sp = sinf(cam->pitch);
    float cy = cosf(cam->yaw);
    float sy = sinf(cam->yaw);

    cam->forward = vec3_normalize(vec3_create(sy * cp, sp, -cy * cp));
    cam->right   = vec3_normalize(vec3_create(cy, 0.0f, sy));
    cam->up      = vec3_normalize(vec3_cross(cam->right, cam->forward));
}

static void camera_update(Camera* cam, float dt, int move_fwd, int move_back, int move_left, int move_right, int do_jump) {
    float target_focus = cam->focus_mode ? 1.0f : 0.0f;
    if (cam->focus_t < target_focus) {
        cam->focus_t += dt * 4.0f;
        if (cam->focus_t > target_focus) cam->focus_t = target_focus;
    } else if (cam->focus_t > target_focus) {
        cam->focus_t -= dt * 4.0f;
        if (cam->focus_t < target_focus) cam->focus_t = target_focus;
    }

    if (cam->focus_t > 0.95f) return;

    if (do_jump && cam->on_ground) {
        cam->vel_y = JUMP_VELOCITY;
        cam->on_ground = 0;
    }

    cam->vel_y -= GRAVITY * dt;
    cam->pos.y += cam->vel_y * dt;

    if (cam->pos.y <= 0.0f) {
        cam->pos.y = 0.0f;
        cam->vel_y = 0.0f;
        cam->on_ground = 1;
    }

    float move_x = 0.0f;
    float move_z = 0.0f;

    float walk_fwd_x = sinf(cam->yaw);
    float walk_fwd_z = -cosf(cam->yaw);
    float walk_right_x = cosf(cam->yaw);
    float walk_right_z = sinf(cam->yaw);

    if (move_fwd)   { move_x += walk_fwd_x; move_z += walk_fwd_z; }
    if (move_back)  { move_x -= walk_fwd_x; move_z -= walk_fwd_z; }
    if (move_right) { move_x += walk_right_x; move_z += walk_right_z; }
    if (move_left)  { move_x -= walk_right_x; move_z -= walk_right_z; }

    float move_len = sqrtf(move_x * move_x + move_z * move_z);
    if (move_len > 1e-4f) {
        float inv = (1.0f / move_len) * WALK_SPEED * dt;
        cam->pos.x += move_x * inv;
        cam->pos.z += move_z * inv;

        if (cam->on_ground) {
            cam->bob_timer += dt * 11.0f;
            cam->bob_y = sinf(cam->bob_timer) * 0.035f;
        }
    } else {
        cam->bob_y *= 0.9f;
    }

    // Room boundaries
    float bound = ROOM_HALF_SIZE - 0.7f;
    if (cam->pos.x < -bound) cam->pos.x = -bound;
    if (cam->pos.x >  bound) cam->pos.x =  bound;
    if (cam->pos.z < -bound) cam->pos.z = -bound;
    if (cam->pos.z >  bound) cam->pos.z =  bound;

    // Pedestal collision
    float ped_x = 1.25f;
    float ped_z = 0.95f;
    if (fabsf(cam->pos.x) < ped_x && fabsf(cam->pos.z) < ped_z) {
        float dx = ped_x - fabsf(cam->pos.x);
        float dz = ped_z - fabsf(cam->pos.z);
        if (dx < dz) cam->pos.x = (cam->pos.x > 0.0f) ? ped_x : -ped_x;
        else cam->pos.z = (cam->pos.z > 0.0f) ? ped_z : -ped_z;
    }

    camera_update_vectors(cam);
}

static void camera_get_view(Camera* cam, Vec3* out_eye, Vec3* out_target, Vec3* out_up) {
    Vec3 fp_eye = vec3_create(cam->pos.x, cam->pos.y + PLAYER_EYE_HEIGHT + cam->bob_y, cam->pos.z);
    Vec3 fp_target = vec3_add(fp_eye, cam->forward);

    // Focus View: Perfect close-up of scientific calculator desk
    Vec3 focus_eye = vec3_create(0.0f, 1.72f, 0.88f);
    Vec3 focus_target = vec3_create(0.0f, 1.15f, 0.0f);

    float s = cam->focus_t;
    float t = s * s * (3.0f - 2.0f * s);

    out_eye->x = fp_eye.x * (1.0f - t) + focus_eye.x * t;
    out_eye->y = fp_eye.y * (1.0f - t) + focus_eye.y * t;
    out_eye->z = fp_eye.z * (1.0f - t) + focus_eye.z * t;

    out_target->x = fp_target.x * (1.0f - t) + focus_target.x * t;
    out_target->y = fp_target.y * (1.0f - t) + focus_target.y * t;
    out_target->z = fp_target.z * (1.0f - t) + focus_target.z * t;

    *out_up = vec3_create(0.0f, 1.0f, 0.0f);
}

static Ray3 camera_generate_ray(Camera* cam, int screen_w, int screen_h, int mouse_x, int mouse_y, int use_mouse) {
    Vec3 eye, target, up_ref;
    camera_get_view(cam, &eye, &target, &up_ref);

    Vec3 forward = vec3_normalize(vec3_sub(target, eye));
    Vec3 right   = vec3_normalize(vec3_cross(forward, up_ref));
    Vec3 up      = vec3_normalize(vec3_cross(right, forward));

    float fov = 60.0f * DEG2RAD;
    float aspect = (float)screen_w / (float)screen_h;
    float tanHalf = tanf(fov * 0.5f);

    float ndc_x = 0.0f;
    float ndc_y = 0.0f;

    if (use_mouse) {
        ndc_x = (2.0f * (float)mouse_x / (float)screen_w) - 1.0f;
        ndc_y = 1.0f - (2.0f * (float)mouse_y / (float)screen_h);
    }

    Vec3 ray_dir = vec3_normalize(
        vec3_add(
            forward,
            vec3_add(
                vec3_scale(right, ndc_x * aspect * tanHalf),
                vec3_scale(up, ndc_y * tanHalf)
            )
        )
    );

    Ray3 ray = {eye, ray_dir};
    return ray;
}

#endif // CAMERA_H
