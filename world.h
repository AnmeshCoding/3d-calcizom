#ifndef WORLD_H
#define WORLD_H

#include <windows.h>
#include <GL/gl.h>
#include <math.h>

#define ROOM_HALF_SIZE 14.0f
#define ROOM_HEIGHT 5.2f

static void world_setup_lighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Light 0: Spot/Point light right above the central desk
    GLfloat light0_pos[] = {0.0f, 4.6f, 0.6f, 1.0f};
    GLfloat light0_ambient[] = {0.24f, 0.25f, 0.28f, 1.0f};
    GLfloat light0_diffuse[] = {0.98f, 0.95f, 0.90f, 1.0f};
    GLfloat light0_specular[] = {0.7f, 0.7f, 0.7f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    // Light 1: Eerie emergency sci-fi red/blue ambient fill light
    glEnable(GL_LIGHT1);
    GLfloat light1_pos[] = {0.0f, 3.2f, -8.0f, 1.0f};
    GLfloat light1_diffuse[] = {0.25f, 0.15f, 0.35f, 1.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);

    GLfloat global_ambient[] = {0.22f, 0.22f, 0.26f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
}

// Render the 3D room, floor tiles, neon strips, walls, and desk pedestal
static void world_render() {
    // 1. Tiled Floor
    int tiles = 28;
    float tile_size = (ROOM_HALF_SIZE * 2.0f) / (float)tiles;
    float start_coord = -ROOM_HALF_SIZE;

    glNormal3f(0.0f, 1.0f, 0.0f);
    for (int ix = 0; ix < tiles; ix++) {
        for (int iz = 0; iz < tiles; iz++) {
            float x0 = start_coord + (float)ix * tile_size;
            float x1 = x0 + tile_size;
            float z0 = start_coord + (float)iz * tile_size;
            float z1 = z0 + tile_size;

            if ((ix + iz) % 2 == 0) {
                glColor3f(0.12f, 0.13f, 0.16f);
            } else {
                glColor3f(0.16f, 0.17f, 0.21f);
            }

            glBegin(GL_QUADS);
            glVertex3f(x0 + 0.02f, 0.0f, z0 + 0.02f);
            glVertex3f(x0 + 0.02f, 0.0f, z1 - 0.02f);
            glVertex3f(x1 - 0.02f, 0.0f, z1 - 0.02f);
            glVertex3f(x1 - 0.02f, 0.0f, z0 + 0.02f);
            glEnd();

            // Glowing grid grout lines
            glDisable(GL_LIGHTING);
            glColor3f(0.05f, 0.25f, 0.32f);
            glBegin(GL_LINE_LOOP);
            glVertex3f(x0, 0.001f, z0);
            glVertex3f(x0, 0.001f, z1);
            glVertex3f(x1, 0.001f, z1);
            glVertex3f(x1, 0.001f, z0);
            glEnd();
            glEnable(GL_LIGHTING);
        }
    }

    // 2. Ceiling
    glNormal3f(0.0f, -1.0f, 0.0f);
    glColor3f(0.09f, 0.10f, 0.12f);
    glBegin(GL_QUADS);
    glVertex3f(-ROOM_HALF_SIZE, ROOM_HEIGHT, -ROOM_HALF_SIZE);
    glVertex3f( ROOM_HALF_SIZE, ROOM_HEIGHT, -ROOM_HALF_SIZE);
    glVertex3f( ROOM_HALF_SIZE, ROOM_HEIGHT,  ROOM_HALF_SIZE);
    glVertex3f(-ROOM_HALF_SIZE, ROOM_HEIGHT,  ROOM_HALF_SIZE);
    glEnd();

    // Ceiling Light Panels
    glDisable(GL_LIGHTING);
    glColor3f(0.92f, 0.95f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-2.0f, ROOM_HEIGHT - 0.01f, -2.0f);
    glVertex3f( 2.0f, ROOM_HEIGHT - 0.01f, -2.0f);
    glVertex3f( 2.0f, ROOM_HEIGHT - 0.01f,  2.0f);
    glVertex3f(-2.0f, ROOM_HEIGHT - 0.01f,  2.0f);
    glEnd();
    glEnable(GL_LIGHTING);

    // 3. Walls
    // North Wall (-Z)
    glColor3f(0.14f, 0.15f, 0.18f);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-ROOM_HALF_SIZE, 0.0f, -ROOM_HALF_SIZE);
    glVertex3f( ROOM_HALF_SIZE, 0.0f, -ROOM_HALF_SIZE);
    glVertex3f( ROOM_HALF_SIZE, ROOM_HEIGHT, -ROOM_HALF_SIZE);
    glVertex3f(-ROOM_HALF_SIZE, ROOM_HEIGHT, -ROOM_HALF_SIZE);
    glEnd();

    // South Wall (+Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glBegin(GL_QUADS);
    glVertex3f( ROOM_HALF_SIZE, 0.0f, ROOM_HALF_SIZE);
    glVertex3f(-ROOM_HALF_SIZE, 0.0f, ROOM_HALF_SIZE);
    glVertex3f(-ROOM_HALF_SIZE, ROOM_HEIGHT, ROOM_HALF_SIZE);
    glVertex3f( ROOM_HALF_SIZE, ROOM_HEIGHT, ROOM_HALF_SIZE);
    glEnd();

    // West Wall (-X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(-ROOM_HALF_SIZE, 0.0f,  ROOM_HALF_SIZE);
    glVertex3f(-ROOM_HALF_SIZE, 0.0f, -ROOM_HALF_SIZE);
    glVertex3f(-ROOM_HALF_SIZE, ROOM_HEIGHT, -ROOM_HALF_SIZE);
    glVertex3f(-ROOM_HALF_SIZE, ROOM_HEIGHT,  ROOM_HALF_SIZE);
    glEnd();

    // East Wall (+X)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(ROOM_HALF_SIZE, 0.0f, -ROOM_HALF_SIZE);
    glVertex3f(ROOM_HALF_SIZE, 0.0f,  ROOM_HALF_SIZE);
    glVertex3f(ROOM_HALF_SIZE, ROOM_HEIGHT,  ROOM_HALF_SIZE);
    glVertex3f(ROOM_HALF_SIZE, ROOM_HEIGHT, -ROOM_HALF_SIZE);
    glEnd();

    // 4. Glowing Neon Accent Stripes along walls
    glDisable(GL_LIGHTING);
    // Cyan baseboard neon trim
    glColor3f(0.0f, 0.85f, 1.0f);
    glLineWidth(3.0f);
    float y_neon = 0.15f;
    glBegin(GL_LINE_LOOP);
    glVertex3f(-ROOM_HALF_SIZE + 0.01f, y_neon, -ROOM_HALF_SIZE + 0.01f);
    glVertex3f( ROOM_HALF_SIZE - 0.01f, y_neon, -ROOM_HALF_SIZE + 0.01f);
    glVertex3f( ROOM_HALF_SIZE - 0.01f, y_neon,  ROOM_HALF_SIZE - 0.01f);
    glVertex3f(-ROOM_HALF_SIZE + 0.01f, y_neon,  ROOM_HALF_SIZE - 0.01f);
    glEnd();

    // Crimson warning neon trim
    glColor3f(1.0f, 0.2f, 0.2f);
    float y_neon2 = 2.6f;
    glBegin(GL_LINE_LOOP);
    glVertex3f(-ROOM_HALF_SIZE + 0.01f, y_neon2, -ROOM_HALF_SIZE + 0.01f);
    glVertex3f( ROOM_HALF_SIZE - 0.01f, y_neon2, -ROOM_HALF_SIZE + 0.01f);
    glVertex3f( ROOM_HALF_SIZE - 0.01f, y_neon2,  ROOM_HALF_SIZE - 0.01f);
    glVertex3f(-ROOM_HALF_SIZE + 0.01f, y_neon2,  ROOM_HALF_SIZE - 0.01f);
    glEnd();
    glEnable(GL_LIGHTING);

    // 5. Pillars in the room corners
    float pillar_w = 1.0f;
    float pillar_dist = 11.0f;
    // Helper to draw pillar
    #define DRAW_PILLAR(px, pz) \
        glColor3f(0.22f, 0.23f, 0.28f); \
        glBegin(GL_QUADS); \
        glVertex3f(px - pillar_w, 0.0f, pz - pillar_w); \
        glVertex3f(px + pillar_w, 0.0f, pz - pillar_w); \
        glVertex3f(px + pillar_w, ROOM_HEIGHT, pz - pillar_w); \
        glVertex3f(px - pillar_w, ROOM_HEIGHT, pz - pillar_w); \
        glVertex3f(px - pillar_w, 0.0f, pz + pillar_w); \
        glVertex3f(px + pillar_w, 0.0f, pz + pillar_w); \
        glVertex3f(px + pillar_w, ROOM_HEIGHT, pz + pillar_w); \
        glVertex3f(px - pillar_w, ROOM_HEIGHT, pz + pillar_w); \
        glVertex3f(px - pillar_w, 0.0f, pz - pillar_w); \
        glVertex3f(px - pillar_w, 0.0f, pz + pillar_w); \
        glVertex3f(px - pillar_w, ROOM_HEIGHT, pz + pillar_w); \
        glVertex3f(px - pillar_w, ROOM_HEIGHT, pz - pillar_w); \
        glVertex3f(px + pillar_w, 0.0f, pz - pillar_w); \
        glVertex3f(px + pillar_w, 0.0f, pz + pillar_w); \
        glVertex3f(px + pillar_w, ROOM_HEIGHT, pz + pillar_w); \
        glVertex3f(px + pillar_w, ROOM_HEIGHT, pz - pillar_w); \
        glEnd();

    DRAW_PILLAR(-pillar_dist, -pillar_dist);
    DRAW_PILLAR( pillar_dist, -pillar_dist);
    DRAW_PILLAR(-pillar_dist,  pillar_dist);
    DRAW_PILLAR( pillar_dist,  pillar_dist);
    #undef DRAW_PILLAR

    // 6. Central Pedestal / Sci-Fi Command Desk (Enlarged to fit the big calculator)
    // Desk Base
    glColor3f(0.10f, 0.11f, 0.14f);
    glBegin(GL_QUADS);
    // Base quad
    glVertex3f(-0.9f, 0.0f, -0.7f);
    glVertex3f( 0.9f, 0.0f, -0.7f);
    glVertex3f( 0.9f, 0.15f, -0.7f);
    glVertex3f(-0.9f, 0.15f, -0.7f);
    glEnd();

    // Central Pillar / Pedestal support
    glColor3f(0.18f, 0.19f, 0.23f);
    glBegin(GL_QUADS);
    glVertex3f(-0.6f, 0.0f, -0.5f);
    glVertex3f( 0.6f, 0.0f, -0.5f);
    glVertex3f( 0.6f, 0.80f, -0.5f);
    glVertex3f(-0.6f, 0.80f, -0.5f);
    glEnd();

    // Heavy Tabletop (top surface at y = 0.88m)
    glColor3f(0.13f, 0.14f, 0.17f);
    float tw = 1.15f; // half-width (total 2.3m wide)
    float td = 0.85f; // half-depth (total 1.7m deep)
    float ty0 = 0.80f;
    float ty1 = 0.88f;

    // Draw tabletop box
    glBegin(GL_QUADS);
    // Top
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-tw, ty1, -td);
    glVertex3f(-tw, ty1,  td);
    glVertex3f( tw, ty1,  td);
    glVertex3f( tw, ty1, -td);
    // Front
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-tw, ty0, td);
    glVertex3f( tw, ty0, td);
    glVertex3f( tw, ty1, td);
    glVertex3f(-tw, ty1, td);
    // Back
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-tw, ty0, -td);
    glVertex3f(-tw, ty1, -td);
    glVertex3f( tw, ty1, -td);
    glVertex3f( tw, ty0, -td);
    // Left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-tw, ty0, -td);
    glVertex3f(-tw, ty0,  td);
    glVertex3f(-tw, ty1,  td);
    glVertex3f(-tw, ty1, -td);
    // Right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(tw, ty0, -td);
    glVertex3f(tw, ty1, -td);
    glVertex3f(tw, ty1,  td);
    glVertex3f(tw, ty0,  td);
    glEnd();

    // Tabletop Glass Inset Plate
    glColor3f(0.06f, 0.10f, 0.13f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-tw + 0.1f, ty1 + 0.002f, -td + 0.1f);
    glVertex3f(-tw + 0.1f, ty1 + 0.002f,  td - 0.1f);
    glVertex3f( tw - 0.1f, ty1 + 0.002f,  td - 0.1f);
    glVertex3f( tw - 0.1f, ty1 + 0.002f, -td + 0.1f);
    glEnd();

    // Table Edge Neon Trim (Cyan/Emerald)
    glDisable(GL_LIGHTING);
    glColor3f(0.1f, 0.95f, 0.8f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-tw - 0.002f, ty1 + 0.001f, -td - 0.002f);
    glVertex3f( tw + 0.002f, ty1 + 0.001f, -td - 0.002f);
    glVertex3f( tw + 0.002f, ty1 + 0.001f,  td + 0.002f);
    glVertex3f(-tw - 0.002f, ty1 + 0.001f,  td + 0.002f);
    glEnd();
    glEnable(GL_LIGHTING);
}

#endif // WORLD_H
