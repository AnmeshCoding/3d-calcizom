#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <windows.h>
#include <GL/gl.h>
#include <math.h>
#include <stdlib.h>
#include "math3d.h"
#include "audio.h"

#define MAX_ZOMBIES 24
#define ZOMBIE_SPEED 1.6f
#define ZOMBIE_HEIGHT 1.8f
#define ZOMBIE_RADIUS 0.42f

typedef struct {
    Vec3 pos;
    float yaw;
    int hp;
    int max_hp;
    int active;
    float walk_timer;
    float hit_flash;
    float death_timer;
    float attack_cooldown;
} Zombie;

typedef struct {
    Zombie list[MAX_ZOMBIES];
    int wave;
    float spawn_timer;
    int zombies_killed;
    int total_spawned_this_wave;
} ZombieHorde;

static void zombie_horde_init(ZombieHorde* horde) {
    memset(horde, 0, sizeof(ZombieHorde));
    horde->wave = 1;
    horde->spawn_timer = 1.0f;
}

// Spawn a single zombie along the room perimeter
static void zombie_spawn(ZombieHorde* horde) {
    int idx = -1;
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        if (!horde->list[i].active && horde->list[i].death_timer <= 0.0f) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return;

    Zombie* z = &horde->list[idx];
    memset(z, 0, sizeof(Zombie));

    // Spawn at random perimeter wall
    float room_bound = 12.5f;
    int side = rand() % 4;
    float r = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 11.0f;

    switch (side) {
        case 0: z->pos = vec3_create(r, 0.0f, -room_bound); break;
        case 1: z->pos = vec3_create(r, 0.0f,  room_bound); break;
        case 2: z->pos = vec3_create(-room_bound, 0.0f, r); break;
        case 3: z->pos = vec3_create( room_bound, 0.0f, r); break;
    }

    z->hp = 3;
    z->max_hp = 3;
    z->active = 1;
    z->walk_timer = (float)(rand() % 100) * 0.1f;
    z->hit_flash = 0.0f;
    z->death_timer = 0.0f;
    z->attack_cooldown = 0.5f;

    audio_play_zombie_groan();
}

// Update zombie AI, walking, and attack against player
static void zombie_horde_update(ZombieHorde* horde, Vec3 player_pos, float dt, int* player_hp) {
    // Spawner
    horde->spawn_timer -= dt;
    if (horde->spawn_timer <= 0.0f) {
        zombie_spawn(horde);
        float interval = 4.2f - (float)horde->wave * 0.35f;
        if (interval < 1.4f) interval = 1.4f;
        horde->spawn_timer = interval;
    }

    for (int i = 0; i < MAX_ZOMBIES; i++) {
        Zombie* z = &horde->list[i];
        if (!z->active) {
            if (z->death_timer > 0.0f) {
                z->death_timer -= dt;
            }
            continue;
        }

        if (z->hit_flash > 0.0f) z->hit_flash -= dt;

        // Pathfind towards player
        Vec3 diff = vec3_sub(player_pos, z->pos);
        diff.y = 0.0f; // purely horizontal
        float dist = vec3_len(diff);

        if (dist > 1e-3f) {
            z->yaw = atan2f(diff.x, -diff.z) * RAD2DEG;
            Vec3 move_dir = vec3_scale(diff, 1.0f / dist);

            // Shambling speed
            float cur_speed = ZOMBIE_SPEED + (float)(horde->wave - 1) * 0.12f;
            if (cur_speed > 2.8f) cur_speed = 2.8f;

            // Stop when close enough to attack
            if (dist > 1.15f) {
                z->pos.x += move_dir.x * cur_speed * dt;
                z->pos.z += move_dir.z * cur_speed * dt;
                z->walk_timer += dt * 7.5f;
            } else {
                // In melee range! Attack player!
                z->attack_cooldown -= dt;
                if (z->attack_cooldown <= 0.0f) {
                    *player_hp -= 15;
                    if (*player_hp < 0) *player_hp = 0;
                    audio_play_player_hurt();
                    z->attack_cooldown = 1.0f;
                }
            }
        }
    }
}

// Raycast against all zombies. Returns 1 if hit, damages zombie
static int zombie_horde_raycast_shoot(ZombieHorde* horde, Ray3 ray, int damage, Vec3* out_hit_pos) {
    int hit_idx = -1;
    float best_t = 100.0f;

    for (int i = 0; i < MAX_ZOMBIES; i++) {
        Zombie* z = &horde->list[i];
        if (!z->active) continue;

        BoundingBox3 box;
        box.min = vec3_create(z->pos.x - ZOMBIE_RADIUS, z->pos.y, z->pos.z - ZOMBIE_RADIUS);
        box.max = vec3_create(z->pos.x + ZOMBIE_RADIUS, z->pos.y + ZOMBIE_HEIGHT, z->pos.z + ZOMBIE_RADIUS);

        float hit_t = 0.0f;
        if (ray_intersect_aabb(ray, box, &hit_t)) {
            if (hit_t > 0.0f && hit_t < best_t) {
                best_t = hit_t;
                hit_idx = i;
            }
        }
    }

    if (hit_idx != -1) {
        Zombie* hit_z = &horde->list[hit_idx];
        hit_z->hp -= damage;
        hit_z->hit_flash = 0.18f;

        if (out_hit_pos) {
            *out_hit_pos = vec3_add(ray.orig, vec3_scale(ray.dir, best_t));
        }

        if (hit_z->hp <= 0) {
            hit_z->active = 0;
            hit_z->death_timer = 0.55f;
            horde->zombies_killed++;
            audio_play_zombie_hit();
        } else {
            audio_play_zombie_hit();
        }
        return 1;
    }
    return 0;
}

// Wipe all active zombies (EMP Shockwave!)
static int zombie_horde_emp_blast(ZombieHorde* horde) {
    int count = 0;
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        if (horde->list[i].active) {
            horde->list[i].active = 0;
            horde->list[i].death_timer = 0.6f;
            count++;
            horde->zombies_killed++;
        }
    }
    return count;
}

// Helper to draw a box relative to zombie
static void z_box(float x, float y, float z, float w, float h, float d) {
    float hw = w * 0.5f, hh = h * 0.5f, hd = d * 0.5f;
    glBegin(GL_QUADS);
    // Top
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(x - hw, y + hh, z - hd); glVertex3f(x - hw, y + hh, z + hd);
    glVertex3f(x + hw, y + hh, z + hd); glVertex3f(x + hw, y + hh, z - hd);
    // Bottom
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(x - hw, y - hh, z - hd); glVertex3f(x + hw, y - hh, z - hd);
    glVertex3f(x + hw, y - hh, z + hd); glVertex3f(x - hw, y - hh, z + hd);
    // Front (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(x - hw, y - hh, z + hd); glVertex3f(x + hw, y - hh, z + hd);
    glVertex3f(x + hw, y + hh, z + hd); glVertex3f(x - hw, y + hh, z + hd);
    // Back (-Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(x - hw, y - hh, z - hd); glVertex3f(x - hw, y + hh, z - hd);
    glVertex3f(x + hw, y + hh, z - hd); glVertex3f(x + hw, y - hh, z - hd);
    // Left (-X)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(x - hw, y - hh, z - hd); glVertex3f(x - hw, y - hh, z + hd);
    glVertex3f(x - hw, y + hh, z + hd); glVertex3f(x - hw, y + hh, z - hd);
    // Right (+X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(x + hw, y - hh, z - hd); glVertex3f(x + hw, y + hh, z - hd);
    glVertex3f(x + hw, y + hh, z + hd); glVertex3f(x + hw, y - hh, z + hd);
    glEnd();
}

// Render all zombies
static void zombie_horde_render(ZombieHorde* horde) {
    for (int i = 0; i < MAX_ZOMBIES; i++) {
        Zombie* z = &horde->list[i];
        if (!z->active && z->death_timer <= 0.0f) continue;

        glPushMatrix();
        glTranslatef(z->pos.x, z->pos.y, z->pos.z);
        glRotatef(z->yaw, 0.0f, 1.0f, 0.0f);

        // Death falling animation
        if (!z->active && z->death_timer > 0.0f) {
            float death_progress = 1.0f - (z->death_timer / 0.55f);
            glRotatef(death_progress * -85.0f, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, -death_progress * 0.4f, 0.0f);
        }

        // Color palettes (flashes red/white when shot!)
        float skin_r = 0.28f, skin_g = 0.58f, skin_b = 0.30f; // decayed zombie green
        float cloth_r = 0.16f, cloth_g = 0.22f, cloth_b = 0.28f; // ripped dark navy suit
        float pants_r = 0.12f, pants_g = 0.13f, pants_b = 0.16f;

        if (z->hit_flash > 0.0f) {
            skin_r = 1.0f; skin_g = 0.3f; skin_b = 0.3f;
            cloth_r = 1.0f; cloth_g = 0.6f; cloth_b = 0.6f;
        }

        float leg_swing = sinf(z->walk_timer) * 0.18f;
        float arm_bob = sinf(z->walk_timer * 1.2f) * 0.06f;

        // 1. Legs
        glColor3f(pants_r, pants_g, pants_b);
        z_box(-0.16f, 0.35f,  leg_swing, 0.16f, 0.70f, 0.20f);
        z_box( 0.16f, 0.35f, -leg_swing, 0.16f, 0.70f, 0.20f);

        // 2. Torso
        glColor3f(cloth_r, cloth_g, cloth_b);
        z_box(0.0f, 1.05f, 0.0f, 0.52f, 0.70f, 0.30f);

        // 3. Shambling Outstretched Arms (facing -Z towards player)
        glColor3f(skin_r, skin_g, skin_b);
        // Arms are held straight out in classic zombie pose
        z_box(-0.35f, 1.15f + arm_bob, -0.32f, 0.15f, 0.15f, 0.65f);
        z_box( 0.35f, 1.15f - arm_bob, -0.32f, 0.15f, 0.15f, 0.65f);

        // 4. Decayed Head
        glColor3f(skin_r, skin_g, skin_b);
        z_box(0.0f, 1.58f, 0.0f, 0.36f, 0.36f, 0.36f);

        // 5. Glowing Red Eyes (Emissive, facing -Z)
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 0.05f, 0.05f);
        z_box(-0.09f, 1.62f, -0.185f, 0.07f, 0.05f, 0.02f);
        z_box( 0.09f, 1.62f, -0.185f, 0.07f, 0.05f, 0.02f);
        glEnable(GL_LIGHTING);

        // 6. Health Bar Floating above Zombie
        if (z->active && z->hp < z->max_hp) {
            glDisable(GL_LIGHTING);
            float hp_ratio = (float)z->hp / (float)z->max_hp;
            glColor3f(0.8f, 0.1f, 0.1f);
            z_box(0.0f, 1.95f, 0.0f, 0.50f, 0.04f, 0.02f);
            glColor3f(0.1f, 0.9f, 0.2f);
            z_box(-0.25f + hp_ratio * 0.25f, 1.95f, 0.002f, 0.50f * hp_ratio, 0.045f, 0.025f);
            glEnable(GL_LIGHTING);
        }

        glPopMatrix();
    }
}

#endif // ZOMBIE_H
