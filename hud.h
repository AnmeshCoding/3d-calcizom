#ifndef HUD_H
#define HUD_H

#include <windows.h>
#include <GL/gl.h>
#include <stdio.h>
#include <string.h>
#include "calculator.h"
#include "math_game.h"
#include "zombie.h"

// 2D Vector font drawer
static void hud_draw_char(char c, float x, float y, float s) {
    glBegin(GL_LINES);
    switch (c) {
        case 'A': case 'a':
            glVertex2f(x, y + s); glVertex2f(x + s*0.5f, y);
            glVertex2f(x + s*0.5f, y); glVertex2f(x + s, y + s);
            glVertex2f(x + s*0.2f, y + s*0.55f); glVertex2f(x + s*0.8f, y + s*0.55f);
            break;
        case 'B': case 'b':
            glVertex2f(x, y); glVertex2f(x, y + s);
            glVertex2f(x, y); glVertex2f(x + s*0.7f, y);
            glVertex2f(x, y + s*0.5f); glVertex2f(x + s*0.7f, y + s*0.5f);
            glVertex2f(x, y + s); glVertex2f(x + s*0.7f, y + s);
            glVertex2f(x + s*0.7f, y); glVertex2f(x + s*0.7f, y + s*0.5f);
            glVertex2f(x + s*0.7f, y + s*0.5f); glVertex2f(x + s*0.7f, y + s);
            break;
        case 'C': case 'c':
            glVertex2f(x + s, y); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x, y + s);
            glVertex2f(x, y + s); glVertex2f(x + s, y + s);
            break;
        case 'D': case 'd':
            glVertex2f(x, y); glVertex2f(x, y + s);
            glVertex2f(x, y); glVertex2f(x + s*0.6f, y);
            glVertex2f(x, y + s); glVertex2f(x + s*0.6f, y + s);
            glVertex2f(x + s*0.6f, y); glVertex2f(x + s, y + s*0.5f);
            glVertex2f(x + s, y + s*0.5f); glVertex2f(x + s*0.6f, y + s);
            break;
        case 'E': case 'e':
            glVertex2f(x, y); glVertex2f(x, y + s);
            glVertex2f(x, y); glVertex2f(x + s*0.8f, y);
            glVertex2f(x, y + s*0.5f); glVertex2f(x + s*0.6f, y + s*0.5f);
            glVertex2f(x, y + s); glVertex2f(x + s*0.8f, y + s);
            break;
        case 'F': case 'f':
            glVertex2f(x, y); glVertex2f(x, y + s);
            glVertex2f(x, y); glVertex2f(x + s*0.8f, y);
            glVertex2f(x, y + s*0.5f); glVertex2f(x + s*0.6f, y + s*0.5f);
            break;
        case 'G': case 'g':
            glVertex2f(x + s, y); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x, y + s);
            glVertex2f(x, y + s); glVertex2f(x + s, y + s);
            glVertex2f(x + s, y + s); glVertex2f(x + s, y + s*0.5f);
            glVertex2f(x + s, y + s*0.5f); glVertex2f(x + s*0.5f, y + s*0.5f);
            break;
        case 'H': case 'h':
            glVertex2f(x, y); glVertex2f(x, y + s);
            glVertex2f(x + s, y); glVertex2f(x + s, y + s);
            glVertex2f(x, y + s*0.5f); glVertex2f(x + s, y + s*0.5f);
            break;
        case 'I': case 'i':
            glVertex2f(x + s*0.5f, y); glVertex2f(x + s*0.5f, y + s);
            glVertex2f(x + s*0.2f, y); glVertex2f(x + s*0.8f, y);
            glVertex2f(x + s*0.2f, y + s); glVertex2f(x + s*0.8f, y + s);
            break;
        case 'J': case 'j':
            glVertex2f(x + s*0.8f, y); glVertex2f(x + s*0.8f, y + s);
            glVertex2f(x + s*0.8f, y + s); glVertex2f(x, y + s);
            glVertex2f(x, y + s); glVertex2f(x, y + s*0.6f);
            break;
        case 'K': case 'k':
            glVertex2f(x, y); glVertex2f(x, y + s);
            glVertex2f(x + s, y); glVertex2f(x, y + s*0.5f);
            glVertex2f(x, y + s*0.5f); glVertex2f(x + s, y + s);
            break;
        case 'L': case 'l':
            glVertex2f(x, y); glVertex2f(x, y + s);
            glVertex2f(x, y + s); glVertex2f(x + s*0.8f, y + s);
            break;
        case 'M': case 'm':
            glVertex2f(x, y + s); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x + s*0.5f, y + s*0.5f);
            glVertex2f(x + s*0.5f, y + s*0.5f); glVertex2f(x + s, y);
            glVertex2f(x + s, y); glVertex2f(x + s, y + s);
            break;
        case 'N': case 'n':
            glVertex2f(x, y + s); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x + s, y + s);
            glVertex2f(x + s, y + s); glVertex2f(x + s, y);
            break;
        case 'O': case 'o':
            glVertex2f(x, y); glVertex2f(x + s, y);
            glVertex2f(x + s, y); glVertex2f(x + s, y + s);
            glVertex2f(x + s, y + s); glVertex2f(x, y + s);
            glVertex2f(x, y + s); glVertex2f(x, y);
            break;
        case 'P': case 'p':
            glVertex2f(x, y); glVertex2f(x, y + s);
            glVertex2f(x, y); glVertex2f(x + s*0.8f, y);
            glVertex2f(x + s*0.8f, y); glVertex2f(x + s*0.8f, y + s*0.5f);
            glVertex2f(x + s*0.8f, y + s*0.5f); glVertex2f(x, y + s*0.5f);
            break;
        case 'Q': case 'q':
            glVertex2f(x, y); glVertex2f(x + s, y);
            glVertex2f(x + s, y); glVertex2f(x + s, y + s);
            glVertex2f(x + s, y + s); glVertex2f(x, y + s);
            glVertex2f(x, y + s); glVertex2f(x, y);
            glVertex2f(x + s*0.6f, y + s*0.6f); glVertex2f(x + s*1.1f, y + s*1.1f);
            break;
        case 'R': case 'r':
            glVertex2f(x, y); glVertex2f(x, y + s);
            glVertex2f(x, y); glVertex2f(x + s*0.8f, y);
            glVertex2f(x + s*0.8f, y); glVertex2f(x + s*0.8f, y + s*0.5f);
            glVertex2f(x + s*0.8f, y + s*0.5f); glVertex2f(x, y + s*0.5f);
            glVertex2f(x, y + s*0.5f); glVertex2f(x + s, y + s);
            break;
        case 'S': case 's':
            glVertex2f(x + s, y); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x, y + s*0.5f);
            glVertex2f(x, y + s*0.5f); glVertex2f(x + s, y + s*0.5f);
            glVertex2f(x + s, y + s*0.5f); glVertex2f(x + s, y + s);
            glVertex2f(x + s, y + s); glVertex2f(x, y + s);
            break;
        case 'T': case 't':
            glVertex2f(x, y); glVertex2f(x + s, y);
            glVertex2f(x + s*0.5f, y); glVertex2f(x + s*0.5f, y + s);
            break;
        case 'U': case 'u':
            glVertex2f(x, y); glVertex2f(x, y + s);
            glVertex2f(x, y + s); glVertex2f(x + s, y + s);
            glVertex2f(x + s, y + s); glVertex2f(x + s, y);
            break;
        case 'V': case 'v':
            glVertex2f(x, y); glVertex2f(x + s*0.5f, y + s);
            glVertex2f(x + s*0.5f, y + s); glVertex2f(x + s, y);
            break;
        case 'W': case 'w':
            glVertex2f(x, y); glVertex2f(x + s*0.25f, y + s);
            glVertex2f(x + s*0.25f, y + s); glVertex2f(x + s*0.5f, y + s*0.4f);
            glVertex2f(x + s*0.5f, y + s*0.4f); glVertex2f(x + s*0.75f, y + s);
            glVertex2f(x + s*0.75f, y + s); glVertex2f(x + s, y);
            break;
        case 'X': case 'x':
            glVertex2f(x, y); glVertex2f(x + s, y + s);
            glVertex2f(x + s, y); glVertex2f(x, y + s);
            break;
        case 'Y': case 'y':
            glVertex2f(x, y); glVertex2f(x + s*0.5f, y + s*0.5f);
            glVertex2f(x + s, y); glVertex2f(x + s*0.5f, y + s*0.5f);
            glVertex2f(x + s*0.5f, y + s*0.5f); glVertex2f(x + s*0.5f, y + s);
            break;
        case 'Z': case 'z':
            glVertex2f(x, y); glVertex2f(x + s, y);
            glVertex2f(x + s, y); glVertex2f(x, y + s);
            glVertex2f(x, y + s); glVertex2f(x + s, y + s);
            break;
        case '0':
            glVertex2f(x, y); glVertex2f(x + s, y);
            glVertex2f(x + s, y); glVertex2f(x + s, y + s);
            glVertex2f(x + s, y + s); glVertex2f(x, y + s);
            glVertex2f(x, y + s); glVertex2f(x, y);
            break;
        case '1':
            glVertex2f(x + s*0.2f, y + s*0.3f); glVertex2f(x + s*0.5f, y);
            glVertex2f(x + s*0.5f, y); glVertex2f(x + s*0.5f, y + s);
            glVertex2f(x + s*0.2f, y + s); glVertex2f(x + s*0.8f, y + s);
            break;
        case '2':
            glVertex2f(x, y); glVertex2f(x + s, y);
            glVertex2f(x + s, y); glVertex2f(x + s, y + s*0.5f);
            glVertex2f(x + s, y + s*0.5f); glVertex2f(x, y + s*0.5f);
            glVertex2f(x, y + s*0.5f); glVertex2f(x, y + s);
            glVertex2f(x, y + s); glVertex2f(x + s, y + s);
            break;
        case '3':
            glVertex2f(x, y); glVertex2f(x + s, y);
            glVertex2f(x + s, y); glVertex2f(x + s, y + s);
            glVertex2f(x, y + s*0.5f); glVertex2f(x + s, y + s*0.5f);
            glVertex2f(x, y + s); glVertex2f(x + s, y + s);
            break;
        case '4':
            glVertex2f(x, y); glVertex2f(x, y + s*0.5f);
            glVertex2f(x, y + s*0.5f); glVertex2f(x + s, y + s*0.5f);
            glVertex2f(x + s*0.75f, y); glVertex2f(x + s*0.75f, y + s);
            break;
        case '5':
            glVertex2f(x + s, y); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x, y + s*0.5f);
            glVertex2f(x, y + s*0.5f); glVertex2f(x + s, y + s*0.5f);
            glVertex2f(x + s, y + s*0.5f); glVertex2f(x + s, y + s);
            glVertex2f(x + s, y + s); glVertex2f(x, y + s);
            break;
        case '6':
            glVertex2f(x + s, y); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x, y + s);
            glVertex2f(x, y + s); glVertex2f(x + s, y + s);
            glVertex2f(x + s, y + s); glVertex2f(x + s, y + s*0.5f);
            glVertex2f(x + s, y + s*0.5f); glVertex2f(x, y + s*0.5f);
            break;
        case '7':
            glVertex2f(x, y); glVertex2f(x + s, y);
            glVertex2f(x + s, y); glVertex2f(x + s*0.2f, y + s);
            break;
        case '8':
            glVertex2f(x, y); glVertex2f(x + s, y);
            glVertex2f(x + s, y); glVertex2f(x + s, y + s);
            glVertex2f(x, y + s); glVertex2f(x + s, y + s);
            glVertex2f(x, y + s); glVertex2f(x, y);
            glVertex2f(x, y + s*0.5f); glVertex2f(x + s, y + s*0.5f);
            break;
        case '9':
            glVertex2f(x + s, y + s*0.5f); glVertex2f(x, y + s*0.5f);
            glVertex2f(x, y + s*0.5f); glVertex2f(x, y);
            glVertex2f(x, y); glVertex2f(x + s, y);
            glVertex2f(x + s, y); glVertex2f(x + s, y + s);
            glVertex2f(x, y + s); glVertex2f(x + s, y + s);
            break;
        case '+':
            glVertex2f(x, y + s*0.5f); glVertex2f(x + s, y + s*0.5f);
            glVertex2f(x + s*0.5f, y); glVertex2f(x + s*0.5f, y + s);
            break;
        case '-':
            glVertex2f(x, y + s*0.5f); glVertex2f(x + s, y + s*0.5f);
            break;
        case '*':
            glVertex2f(x + s*0.2f, y + s*0.2f); glVertex2f(x + s*0.8f, y + s*0.8f);
            glVertex2f(x + s*0.8f, y + s*0.2f); glVertex2f(x + s*0.2f, y + s*0.8f);
            glVertex2f(x + s*0.5f, y);           glVertex2f(x + s*0.5f, y + s);
            break;
        case '/':
            glVertex2f(x, y + s); glVertex2f(x + s, y);
            break;
        case '=':
            glVertex2f(x, y + s*0.35f); glVertex2f(x + s, y + s*0.35f);
            glVertex2f(x, y + s*0.65f); glVertex2f(x + s, y + s*0.65f);
            break;
        case '?':
            glVertex2f(x, y); glVertex2f(x + s, y);
            glVertex2f(x + s, y); glVertex2f(x + s, y + s*0.5f);
            glVertex2f(x + s, y + s*0.5f); glVertex2f(x + s*0.5f, y + s*0.5f);
            glVertex2f(x + s*0.5f, y + s*0.5f); glVertex2f(x + s*0.5f, y + s*0.75f);
            glVertex2f(x + s*0.5f, y + s*0.9f); glVertex2f(x + s*0.5f, y + s);
            break;
        case '!':
            glVertex2f(x + s*0.5f, y); glVertex2f(x + s*0.5f, y + s*0.7f);
            glVertex2f(x + s*0.5f, y + s*0.85f); glVertex2f(x + s*0.5f, y + s);
            break;
        case '[':
            glVertex2f(x + s*0.8f, y); glVertex2f(x + s*0.2f, y);
            glVertex2f(x + s*0.2f, y); glVertex2f(x + s*0.2f, y + s);
            glVertex2f(x + s*0.2f, y + s); glVertex2f(x + s*0.8f, y + s);
            break;
        case ']':
            glVertex2f(x + s*0.2f, y); glVertex2f(x + s*0.8f, y);
            glVertex2f(x + s*0.8f, y); glVertex2f(x + s*0.8f, y + s);
            glVertex2f(x + s*0.8f, y + s); glVertex2f(x + s*0.2f, y + s);
            break;
        case '(':
            glVertex2f(x + s*0.8f, y); glVertex2f(x + s*0.4f, y + s*0.5f);
            glVertex2f(x + s*0.4f, y + s*0.5f); glVertex2f(x + s*0.8f, y + s);
            break;
        case ')':
            glVertex2f(x + s*0.2f, y); glVertex2f(x + s*0.6f, y + s*0.5f);
            glVertex2f(x + s*0.6f, y + s*0.5f); glVertex2f(x + s*0.2f, y + s);
            break;
        case ':':
            glVertex2f(x + s*0.5f, y + s*0.3f); glVertex2f(x + s*0.5f, y + s*0.4f);
            glVertex2f(x + s*0.5f, y + s*0.7f); glVertex2f(x + s*0.5f, y + s*0.8f);
            break;
        case '.':
            glVertex2f(x + s*0.4f, y + s); glVertex2f(x + s*0.6f, y + s);
            break;
        default:
            break;
    }
    glEnd();
}

static void hud_draw_text(const char* text, float x, float y, float size, float r, float g, float b) {
    glColor3f(r, g, b);
    glLineWidth(1.6f);
    float cur_x = x;
    size_t len = strlen(text);
    for (size_t i = 0; i < len; i++) {
        char c = text[i];
        if (c != ' ') {
            hud_draw_char(c, cur_x, y, size);
        }
        cur_x += size * 1.25f;
    }
}

static void hud_draw_panel(float x, float y, float w, float h, float r, float g, float b, float a) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x, y); glVertex2f(x + w, y);
    glVertex2f(x + w, y + h); glVertex2f(x, y + h);
    glEnd();

    glColor4f(r * 2.0f, g * 2.0f, b * 2.0f, 0.75f);
    glLineWidth(1.4f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y); glVertex2f(x + w, y);
    glVertex2f(x + w, y + h); glVertex2f(x, y + h);
    glEnd();
    glDisable(GL_BLEND);
}

// Render complete game HUD
static void hud_render(int screen_w, int screen_h, Calculator* calc, MathGame* game, ZombieHorde* horde, int hovered_btn_id, int focus_mode) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (double)screen_w, (double)screen_h, 0.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // 1. EMP Blast Screen Flash
    if (game->emp_flash_timer > 0.0f) {
        float alpha = game->emp_flash_timer / 0.85f * 0.45f;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.1f, 0.95f, 1.0f, alpha);
        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f); glVertex2f((float)screen_w, 0.0f);
        glVertex2f((float)screen_w, (float)screen_h); glVertex2f(0.0f, (float)screen_h);
        glEnd();
        glDisable(GL_BLEND);

        // Flash text
        float fw = 520.0f;
        float fx = ((float)screen_w - fw) * 0.5f;
        hud_draw_panel(fx, (float)screen_h * 0.4f, fw, 50.0f, 0.0f, 0.4f, 0.6f, 0.88f);
        hud_draw_text("EMP SHOCKWAVE ACTIVATED! HORDE VAPORIZED!", fx + 16.0f, (float)screen_h * 0.4f + 18.0f, 12.0f, 1.0f, 1.0f, 1.0f);
    }

    // 2. Health Bar (Bottom-Left)
    float hp_panel_w = 260.0f;
    float hp_panel_h = 60.0f;
    float hp_x = 24.0f;
    float hp_y = (float)screen_h - 85.0f;
    hud_draw_panel(hp_x, hp_y, hp_panel_w, hp_panel_h, 0.06f, 0.08f, 0.12f, 0.82f);
    
    char hp_str[32];
    snprintf(hp_str, sizeof(hp_str), "HEALTH: %d / %d", game->player_hp, game->max_hp);
    hud_draw_text(hp_str, hp_x + 14.0f, hp_y + 12.0f, 10.0f, 0.9f, 0.9f, 0.9f);

    // HP Bar Fill
    float hp_ratio = (float)game->player_hp / (float)game->max_hp;
    if (hp_ratio < 0.0f) hp_ratio = 0.0f;
    float bar_w = (hp_panel_w - 28.0f) * hp_ratio;
    
    // Background bar
    glColor4f(0.2f, 0.05f, 0.05f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(hp_x + 14.0f, hp_y + 32.0f);
    glVertex2f(hp_x + hp_panel_w - 14.0f, hp_y + 32.0f);
    glVertex2f(hp_x + hp_panel_w - 14.0f, hp_y + 46.0f);
    glVertex2f(hp_x + 14.0f, hp_y + 46.0f);
    glEnd();

    // Filled health
    if (hp_ratio > 0.4f) glColor3f(0.1f, 0.88f, 0.35f);
    else glColor3f(0.95f, 0.2f, 0.15f);
    glBegin(GL_QUADS);
    glVertex2f(hp_x + 14.0f, hp_y + 32.0f);
    glVertex2f(hp_x + 14.0f + bar_w, hp_y + 32.0f);
    glVertex2f(hp_x + 14.0f + bar_w, hp_y + 46.0f);
    glVertex2f(hp_x + 14.0f, hp_y + 46.0f);
    glEnd();

    // 3. Current Mission / Math Question Card (Top-Center Banner)
    float q_card_w = 640.0f;
    float q_card_h = 78.0f;
    float q_card_x = ((float)screen_w - q_card_w) * 0.5f;
    float q_card_y = 16.0f;

    hud_draw_panel(q_card_x, q_card_y, q_card_w, q_card_h, 0.05f, 0.14f, 0.20f, 0.85f);
    
    hud_draw_text("DEFENSE OBJECTIVE: SOLVE TO TRIGGER EMP NUKE", q_card_x + 18.0f, q_card_y + 12.0f, 9.5f, 0.2f, 0.9f, 1.0f);
    
    char q_display[80];
    snprintf(q_display, sizeof(q_display), "SOLVE: [ %s = ? ]", game->question_str);
    hud_draw_text(q_display, q_card_x + 18.0f, q_card_y + 32.0f, 16.0f, 1.0f, 0.92f, 0.2f);

    hud_draw_text("PRESS [E] OR [TAB/F] ON CALCULATOR TO SOLVE | LMB SHOOTS ZOMBIES", q_card_x + 18.0f, q_card_y + 58.0f, 8.5f, 0.8f, 0.85f, 0.9f);

    // 4. Stats & Wave Card (Top-Right)
    float stat_w = 260.0f;
    float stat_h = 100.0f;
    float stat_x = (float)screen_w - stat_w - 24.0f;
    float stat_y = 16.0f;
    hud_draw_panel(stat_x, stat_y, stat_w, stat_h, 0.08f, 0.06f, 0.12f, 0.85f);

    char wave_str[32], score_str[32], kill_str[32], disp_mirror[64];
    snprintf(wave_str, sizeof(wave_str), "WAVE: %d", horde->wave);
    snprintf(score_str, sizeof(score_str), "SCORE: %d", game->score);
    snprintf(kill_str, sizeof(kill_str), "ZOMBIES KILLED: %d", horde->zombies_killed);
    snprintf(disp_mirror, sizeof(disp_mirror), "CALC: [ %s ]", calc->display);

    hud_draw_text(wave_str, stat_x + 16.0f, stat_y + 14.0f, 11.0f, 1.0f, 0.35f, 0.35f);
    hud_draw_text(score_str, stat_x + 16.0f, stat_y + 34.0f, 11.0f, 0.2f, 0.95f, 0.5f);
    hud_draw_text(kill_str, stat_x + 16.0f, stat_y + 54.0f, 9.5f, 0.85f, 0.85f, 0.85f);
    hud_draw_text(disp_mirror, stat_x + 16.0f, stat_y + 74.0f, 11.0f, 0.2f, 0.98f, 0.8f);

    // 5. Controls Info (Top-Left)
    hud_draw_panel(24.0f, 16.0f, 250.0f, 120.0f, 0.06f, 0.08f, 0.12f, 0.75f);
    hud_draw_text("SURVIVAL COMBAT", 36.0f, 26.0f, 10.0f, 0.2f, 0.9f, 0.85f);
    hud_draw_text("WASD : MOVE AROUND", 36.0f, 44.0f, 9.0f, 0.8f, 0.82f, 0.85f);
    hud_draw_text("MOUSE : AIM / LOOK", 36.0f, 60.0f, 9.0f, 0.8f, 0.82f, 0.85f);
    hud_draw_text("LMB : SHOOT ZOMBIES", 36.0f, 76.0f, 9.5f, 1.0f, 0.4f, 0.3f);
    hud_draw_text("E : PRESS CALC KEY", 36.0f, 92.0f, 9.5f, 1.0f, 0.85f, 0.2f);
    hud_draw_text("TAB / F : DESK FOCUS", 36.0f, 108.0f, 9.0f, 0.2f, 0.9f, 0.85f);

    // 6. Crosshair & Hover Tooltip
    if (!focus_mode) {
        float cx = (float)screen_w * 0.5f;
        float cy = (float)screen_h * 0.5f;

        glLineWidth(2.2f);
        if (hovered_btn_id >= 0) {
            // Hovering calculator button
            glColor3f(1.0f, 0.95f, 0.2f);
            float sz = 12.0f;
            glBegin(GL_LINES);
            glVertex2f(cx - sz, cy); glVertex2f(cx - 3.0f, cy);
            glVertex2f(cx + 3.0f, cy); glVertex2f(cx + sz, cy);
            glVertex2f(cx, cy - sz); glVertex2f(cx, cy - 3.0f);
            glVertex2f(cx, cy + 3.0f); glVertex2f(cx, cy + sz);
            glEnd();

            char tip[64];
            snprintf(tip, sizeof(tip), "PRESS [ %s ] (PRESS E)", calc->buttons[hovered_btn_id].label);
            float tip_w = (float)strlen(tip) * 11.0f * 1.25f;
            hud_draw_panel(cx - tip_w * 0.5f - 10.0f, cy + 24.0f, tip_w + 20.0f, 28.0f, 0.08f, 0.08f, 0.12f, 0.85f);
            hud_draw_text(tip, cx - tip_w * 0.5f, cy + 32.0f, 11.0f, 1.0f, 0.9f, 0.2f);
        } else {
            // Tactical combat reticle (Red center cross with dot)
            glColor3f(1.0f, 0.25f, 0.25f);
            float sz = 8.0f;
            glBegin(GL_LINES);
            glVertex2f(cx - sz, cy); glVertex2f(cx - 3.0f, cy);
            glVertex2f(cx + 3.0f, cy); glVertex2f(cx + sz, cy);
            glVertex2f(cx, cy - sz); glVertex2f(cx, cy - 3.0f);
            glVertex2f(cx, cy + 3.0f); glVertex2f(cx, cy + sz);
            glEnd();

            glPointSize(3.0f);
            glBegin(GL_POINTS);
            glVertex2f(cx, cy);
            glEnd();
        }
    } else {
        // Focus Mode Top Banner
        float banner_w = 540.0f;
        float banner_x = ((float)screen_w - banner_w) * 0.5f;
        hud_draw_panel(banner_x, (float)screen_h - 55.0f, banner_w, 38.0f, 0.12f, 0.08f, 0.04f, 0.90f);
        hud_draw_text("DESK FOCUS: CLICK BUTTONS OR USE NUMPAD | [F] / [TAB] TO EXIT", banner_x + 18.0f, (float)screen_h - 43.0f, 9.5f, 1.0f, 0.85f, 0.2f);
    }

    // 7. Game Over Screen Overlay
    if (game->game_over) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.15f, 0.0f, 0.0f, 0.85f);
        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f); glVertex2f((float)screen_w, 0.0f);
        glVertex2f((float)screen_w, (float)screen_h); glVertex2f(0.0f, (float)screen_h);
        glEnd();
        glDisable(GL_BLEND);

        float go_w = 560.0f;
        float go_h = 160.0f;
        float go_x = ((float)screen_w - go_w) * 0.5f;
        float go_y = ((float)screen_h - go_h) * 0.5f;

        hud_draw_panel(go_x, go_y, go_w, go_h, 0.25f, 0.04f, 0.04f, 0.95f);
        hud_draw_text("CRITICAL FAILURE - OVERWHELMED", go_x + 36.0f, go_y + 24.0f, 16.0f, 1.0f, 0.2f, 0.2f);
        
        char fin_score[64], fin_q[64];
        snprintf(fin_score, sizeof(fin_score), "FINAL SCORE: %d", game->score);
        snprintf(fin_q, sizeof(fin_q), "MATH OBJECTIVES SOLVED: %d", game->questions_solved);
        hud_draw_text(fin_score, go_x + 36.0f, go_y + 60.0f, 12.0f, 1.0f, 0.9f, 0.9f);
        hud_draw_text(fin_q, go_x + 36.0f, go_y + 85.0f, 12.0f, 0.3f, 0.9f, 1.0f);
        hud_draw_text("PRESS [R] TO RESTART MISSION", go_x + 36.0f, go_y + 120.0f, 12.0f, 1.0f, 0.9f, 0.2f);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

#endif // HUD_H
