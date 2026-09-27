#ifndef WEAPON_H
#define WEAPON_H

#include <windows.h>
#include <GL/gl.h>
#include <math.h>
#include "math3d.h"
#include "audio.h"

typedef struct {
    float recoil_t;         // 0.0 = rest, >0.0 = kicking back
    float muzzle_flash_t;   // >0.0 = glowing flash visible
    float fire_cooldown;    // delay between shots
    int ammo;
    int max_ammo;
} Weapon;

static void weapon_init(Weapon* w) {
    w->recoil_t = 0.0f;
    w->muzzle_flash_t = 0.0f;
    w->fire_cooldown = 0.0f;
    w->ammo = 12;
    w->max_ammo = 12;
}

static void weapon_update(Weapon* w, float dt) {
    if (w->recoil_t > 0.0f) {
        w->recoil_t -= dt * 6.5f;
        if (w->recoil_t < 0.0f) w->recoil_t = 0.0f;
    }
    if (w->muzzle_flash_t > 0.0f) {
        w->muzzle_flash_t -= dt;
        if (w->muzzle_flash_t < 0.0f) w->muzzle_flash_t = 0.0f;
    }
    if (w->fire_cooldown > 0.0f) {
        w->fire_cooldown -= dt;
        if (w->fire_cooldown < 0.0f) w->fire_cooldown = 0.0f;
    }
}

// Fire weapon
static int weapon_fire(Weapon* w) {
    if (w->fire_cooldown > 0.0f) return 0;
    w->recoil_t = 1.0f;
    w->muzzle_flash_t = 0.06f;
    w->fire_cooldown = 0.16f; // Rapid, punchy semi-auto
    audio_play_gunshot();
    return 1;
}

// Render first-person 3D Gun in Camera Space
static void weapon_render(Weapon* w, int is_focus_mode) {
    // Holster gun when zoomed into the desk calculator
    if (is_focus_mode) return;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Position gun at bottom-right of camera view
    float recoil_z = w->recoil_t * 0.055f;
    float recoil_pitch = w->recoil_t * 12.0f;

    glTranslatef(0.24f, -0.22f, -0.48f + recoil_z);
    glRotatef(recoil_pitch, -1.0f, 0.0f, 0.0f);
    glRotatef(3.0f, 0.0f, 1.0f, 0.0f); // slight inward cant

    // Helper macro to draw gun box
    #define G_BOX(x, y, z, gw, gh, gd) \
        { \
            float hw = (gw)*0.5f, hh = (gh)*0.5f, hd = (gd)*0.5f; \
            glBegin(GL_QUADS); \
            glNormal3f(0.0f, 1.0f, 0.0f); \
            glVertex3f(x-hw, y+hh, z-hd); glVertex3f(x-hw, y+hh, z+hd); \
            glVertex3f(x+hw, y+hh, z+hd); glVertex3f(x+hw, y+hh, z-hd); \
            glNormal3f(0.0f, -1.0f, 0.0f); \
            glVertex3f(x-hw, y-hh, z-hd); glVertex3f(x+hw, y-hh, z-hd); \
            glVertex3f(x+hw, y-hh, z+hd); glVertex3f(x-hw, y-hh, z+hd); \
            glNormal3f(0.0f, 0.0f, 1.0f); \
            glVertex3f(x-hw, y-hh, z+hd); glVertex3f(x+hw, y-hh, z+hd); \
            glVertex3f(x+hw, y+hh, z+hd); glVertex3f(x-hw, y+hh, z+hd); \
            glNormal3f(0.0f, 0.0f, -1.0f); \
            glVertex3f(x-hw, y-hh, z-hd); glVertex3f(x-hw, y+hh, z-hd); \
            glVertex3f(x+hw, y+hh, z-hd); glVertex3f(x+hw, y-hh, z-hd); \
            glNormal3f(-1.0f, 0.0f, 0.0f); \
            glVertex3f(x-hw, y-hh, z-hd); glVertex3f(x-hw, y-hh, z+hd); \
            glVertex3f(x-hw, y+hh, z+hd); glVertex3f(x-hw, y+hh, z-hd); \
            glNormal3f(1.0f, 0.0f, 0.0f); \
            glVertex3f(x+hw, y-hh, z-hd); glVertex3f(x+hw, y+hh, z-hd); \
            glVertex3f(x+hw, y+hh, z+hd); glVertex3f(x+hw, y-hh, z+hd); \
            glEnd(); \
        }

    // 1. Lower Receiver & Grip
    glColor3f(0.12f, 0.13f, 0.15f);
    G_BOX(0.0f, -0.06f, 0.05f, 0.038f, 0.12f, 0.065f); // Grip

    // 2. Slide / Upper Receiver (Gunmetal Titanium)
    glColor3f(0.24f, 0.25f, 0.28f);
    G_BOX(0.0f, 0.015f, -0.06f, 0.042f, 0.048f, 0.24f);

    // 3. Barrel & Compensator
    glColor3f(0.15f, 0.16f, 0.18f);
    G_BOX(0.0f, 0.018f, -0.19f, 0.032f, 0.032f, 0.06f);

    // 4. Glowing Plasma Energy Conduit (Cyan Neon)
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.95f, 1.0f);
    G_BOX(0.0f, 0.042f, -0.07f, 0.012f, 0.012f, 0.16f);
    glEnable(GL_LIGHTING);

    // 5. Muzzle Flash
    if (w->muzzle_flash_t > 0.0f) {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 0.95f, 0.3f);
        // Starburst flash at barrel tip
        float fs = 0.09f;
        glBegin(GL_QUADS);
        glVertex3f(-fs, 0.018f - fs, -0.23f);
        glVertex3f( fs, 0.018f - fs, -0.23f);
        glVertex3f( fs, 0.018f + fs, -0.23f);
        glVertex3f(-fs, 0.018f + fs, -0.23f);
        glEnd();

        glColor3f(1.0f, 0.5f, 0.1f);
        glBegin(GL_QUADS);
        glVertex3f(0.0f, 0.018f - fs*1.4f, -0.23f);
        glVertex3f(0.0f, 0.018f + fs*1.4f, -0.23f);
        glVertex3f(-fs*1.4f, 0.018f, -0.23f);
        glVertex3f( fs*1.4f, 0.018f, -0.23f);
        glEnd();
        glEnable(GL_LIGHTING);
    }

    #undef G_BOX
    glPopMatrix();
}

#endif // WEAPON_H
