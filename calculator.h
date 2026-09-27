#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <windows.h>
#include <GL/gl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "math3d.h"
#include "audio.h"

#define NUM_BUTTONS 25
#define CALC_WORLD_X 0.0f
#define CALC_WORLD_Y 1.15f   // Elevated high above the table (tabletop is at 0.88m)
#define CALC_WORLD_Z 0.0f
#define CALC_TILT_X  12.0f  // 12 degree ergonomic tilt; lowest front edge sits at y = 0.99m (11cm above desk)

typedef struct {
    int id;
    char label[8];
    float x, y, z;      // Local center coordinates
    float w, h, d;      // Width, height, depth
    float r, g, b;      // Button color
    float press_anim;   // Press depression animation
    int hovered;
} CalcButton;

typedef struct {
    char display[32];
    char history[32];
    double operand1;
    double operand2;
    char pending_op;
    int entering_new;
    int has_decimal;
    int error_state;
    
    CalcButton buttons[NUM_BUTTONS];
} Calculator;

// 7-segment bitmasks: bits 0..6 = a, b, c, d, e, f, g
static unsigned char get_7segment_mask(char ch) {
    switch (ch) {
        case '0': return 0x3F;
        case '1': return 0x06;
        case '2': return 0x5B;
        case '3': return 0x4F;
        case '4': return 0x66;
        case '5': return 0x6D;
        case '6': return 0x7D;
        case '7': return 0x07;
        case '8': return 0x7F;
        case '9': return 0x6F;
        case '-': return 0x40;
        case 'E': return 0x79;
        case 'r': return 0x50;
        case 'o': return 0x5C;
        case 'C': return 0x39;
        case 'L': return 0x38;
        default: return 0x00;
    }
}

// Render a single 7-segment character in local space (x, y, z) with width w, height h
static void draw_7segment_char(char ch, float x, float y, float z, float w, float h, float r, float g, float b) {
    unsigned char mask = get_7segment_mask(ch);
    float t = 0.0035f;
    float hw = w * 0.5f;
    float hh = h * 0.5f;

    float faint_r = r * 0.08f;
    float faint_g = g * 0.08f;
    float faint_b = b * 0.08f;

    #define DRAW_SEG(bit, cond, x0, z0, x1, z1) \
        if (cond) glColor3f(r, g, b); \
        else glColor3f(faint_r, faint_g, faint_b); \
        glBegin(GL_LINES); \
        glVertex3f(x + x0, y + 0.001f, z + z0); \
        glVertex3f(x + x1, y + 0.001f, z + z1); \
        glEnd();

    glLineWidth(2.5f);
    DRAW_SEG(0, mask & 0x01, -hw + t, -hh, hw - t, -hh);       // a
    DRAW_SEG(1, mask & 0x02, hw, -hh + t, hw, -t);             // b
    DRAW_SEG(2, mask & 0x04, hw, t, hw, hh - t);               // c
    DRAW_SEG(3, mask & 0x08, -hw + t, hh, hw - t, hh);         // d
    DRAW_SEG(4, mask & 0x10, -hw, t, -hw, hh - t);             // e
    DRAW_SEG(5, mask & 0x20, -hw, -hh + t, -hw, -t);           // f
    DRAW_SEG(6, mask & 0x40, -hw + t, 0.0f, hw - t, 0.0f);     // g
    #undef DRAW_SEG
}

// Draw vector characters for button legends
static void draw_button_label(const char* label, float cx, float cy, float cz, float scale) {
    glColor3f(0.96f, 0.96f, 0.96f);
    glLineWidth(2.2f);

    float len = (float)strlen(label);
    float start_x = cx - (len * scale * 0.8f) * 0.5f;
    float y = cy + 0.001f;

    for (size_t i = 0; i < strlen(label); i++) {
        char c = label[i];
        float x = start_x + (float)i * scale * 0.95f;
        float z = cz;
        float s = scale * 0.38f;

        glBegin(GL_LINES);
        if (c >= '0' && c <= '9') {
            unsigned char m = get_7segment_mask(c);
            if (m & 0x01) { glVertex3f(x - s, y, z - s); glVertex3f(x + s, y, z - s); }
            if (m & 0x02) { glVertex3f(x + s, y, z - s); glVertex3f(x + s, y, z); }
            if (m & 0x04) { glVertex3f(x + s, y, z);     glVertex3f(x + s, y, z + s); }
            if (m & 0x08) { glVertex3f(x - s, y, z + s); glVertex3f(x + s, y, z + s); }
            if (m & 0x10) { glVertex3f(x - s, y, z);     glVertex3f(x - s, y, z + s); }
            if (m & 0x20) { glVertex3f(x - s, y, z - s); glVertex3f(x - s, y, z); }
            if (m & 0x40) { glVertex3f(x - s, y, z);     glVertex3f(x + s, y, z); }
        } else if (c == '+') {
            glVertex3f(x - s, y, z); glVertex3f(x + s, y, z);
            glVertex3f(x, y, z - s); glVertex3f(x, y, z + s);
        } else if (c == '-') {
            glVertex3f(x - s, y, z); glVertex3f(x + s, y, z);
        } else if (c == '*') {
            glVertex3f(x - s*0.7f, y, z - s*0.7f); glVertex3f(x + s*0.7f, y, z + s*0.7f);
            glVertex3f(x - s*0.7f, y, z + s*0.7f); glVertex3f(x + s*0.7f, y, z - s*0.7f);
            glVertex3f(x, y, z - s);               glVertex3f(x, y, z + s);
        } else if (c == '/') {
            glVertex3f(x - s, y, z + s); glVertex3f(x + s, y, z - s);
        } else if (c == '=') {
            glVertex3f(x - s, y, z - s*0.35f); glVertex3f(x + s, y, z - s*0.35f);
            glVertex3f(x - s, y, z + s*0.35f); glVertex3f(x + s, y, z + s*0.35f);
        } else if (c == '.') {
            glVertex3f(x - s*0.2f, y, z + s); glVertex3f(x + s*0.2f, y, z + s);
        } else if (c == 'C') {
            glVertex3f(x + s, y, z - s); glVertex3f(x - s, y, z - s);
            glVertex3f(x - s, y, z - s); glVertex3f(x - s, y, z + s);
            glVertex3f(x - s, y, z + s); glVertex3f(x + s, y, z + s);
        } else if (c == 'E') {
            glVertex3f(x + s, y, z - s); glVertex3f(x - s, y, z - s);
            glVertex3f(x - s, y, z - s); glVertex3f(x - s, y, z + s);
            glVertex3f(x - s, y, z + s); glVertex3f(x + s, y, z + s);
            glVertex3f(x - s, y, z);     glVertex3f(x + s*0.6f, y, z);
        } else if (c == 'S' || c == 's') {
            glVertex3f(x + s, y, z - s); glVertex3f(x - s, y, z - s);
            glVertex3f(x - s, y, z - s); glVertex3f(x - s, y, z);
            glVertex3f(x - s, y, z);     glVertex3f(x + s, y, z);
            glVertex3f(x + s, y, z);     glVertex3f(x + s, y, z + s);
            glVertex3f(x + s, y, z + s); glVertex3f(x - s, y, z + s);
        } else if (c == 'I' || c == 'i') {
            glVertex3f(x, y, z - s); glVertex3f(x, y, z + s);
        } else if (c == 'N' || c == 'n') {
            glVertex3f(x - s, y, z + s); glVertex3f(x - s, y, z - s);
            glVertex3f(x - s, y, z - s); glVertex3f(x + s, y, z + s);
            glVertex3f(x + s, y, z + s); glVertex3f(x + s, y, z - s);
        } else if (c == 'O' || c == 'o') {
            glVertex3f(x - s, y, z - s); glVertex3f(x + s, y, z - s);
            glVertex3f(x + s, y, z - s); glVertex3f(x + s, y, z + s);
            glVertex3f(x + s, y, z + s); glVertex3f(x - s, y, z + s);
            glVertex3f(x - s, y, z + s); glVertex3f(x - s, y, z - s);
        } else if (c == 'T' || c == 't') {
            glVertex3f(x - s, y, z - s); glVertex3f(x + s, y, z - s);
            glVertex3f(x, y, z - s);     glVertex3f(x, y, z + s);
        } else if (c == 'A' || c == 'a') {
            glVertex3f(x - s, y, z + s); glVertex3f(x, y, z - s);
            glVertex3f(x, y, z - s);     glVertex3f(x + s, y, z + s);
            glVertex3f(x - s*0.5f, y, z); glVertex3f(x + s*0.5f, y, z);
        } else if (c == 'Q' || c == 'q') {
            glVertex3f(x - s, y, z - s); glVertex3f(x + s, y, z - s);
            glVertex3f(x + s, y, z - s); glVertex3f(x + s, y, z + s);
            glVertex3f(x + s, y, z + s); glVertex3f(x - s, y, z + s);
            glVertex3f(x - s, y, z + s); glVertex3f(x - s, y, z - s);
            glVertex3f(x, y, z);         glVertex3f(x + s*1.1f, y, z + s*1.1f);
        } else if (c == 'R' || c == 'r') {
            glVertex3f(x - s, y, z + s); glVertex3f(x - s, y, z - s);
            glVertex3f(x - s, y, z - s); glVertex3f(x + s, y, z - s);
            glVertex3f(x + s, y, z - s); glVertex3f(x + s, y, z);
            glVertex3f(x + s, y, z);     glVertex3f(x - s, y, z);
            glVertex3f(x, y, z);         glVertex3f(x + s, y, z + s);
        } else if (c == 'P' || c == 'p') {
            glVertex3f(x - s, y, z + s); glVertex3f(x - s, y, z - s);
            glVertex3f(x - s, y, z - s); glVertex3f(x + s, y, z - s);
            glVertex3f(x + s, y, z - s); glVertex3f(x + s, y, z);
            glVertex3f(x + s, y, z);     glVertex3f(x - s, y, z);
        } else if (c == '^') {
            glVertex3f(x - s, y, z); glVertex3f(x, y, z - s);
            glVertex3f(x, y, z - s); glVertex3f(x + s, y, z);
        } else if (c == 'X' || c == 'x') {
            glVertex3f(x - s, y, z - s); glVertex3f(x + s, y, z + s);
            glVertex3f(x + s, y, z - s); glVertex3f(x - s, y, z + s);
        } else {
            glVertex3f(x - s, y, z); glVertex3f(x + s, y, z);
        }
        glEnd();
    }
}

// Initialize Scientific Calculator with clean 5x5 keypad layout
static void calc_init(Calculator* calc) {
    memset(calc, 0, sizeof(Calculator));
    strcpy(calc->display, "0");
    strcpy(calc->history, "READY");
    calc->entering_new = 1;

    // Palette
    float c_sci[3]  = {0.18f, 0.40f, 0.52f}; // Sci-Fi Cyan Blue
    float c_num[3]  = {0.24f, 0.25f, 0.28f}; // Charcoal slate
    float c_op[3]   = {0.88f, 0.52f, 0.12f}; // Amber / Orange
    float c_eq[3]   = {0.10f, 0.75f, 0.40f}; // Emerald Green
    float c_clr[3]  = {0.85f, 0.18f, 0.22f}; // Crimson Red
    float c_fn[3]   = {0.35f, 0.38f, 0.44f}; // Light steel

    // Button sizes
    float bw = 0.185f;
    float bh = 0.040f;
    float bd = 0.105f;

    // 5 Rows (Z coords along the tilted face, strictly below LCD screen at z = -0.34)
    float r0 = -0.14f;
    float r1 = -0.01f;
    float r2 =  0.12f;
    float r3 =  0.25f;
    float r4 =  0.38f;

    // 5 Columns (X coords across the calculator)
    float c0 = -0.44f; // Scientific Functions
    float c1 = -0.22f; // Numbers & Clear
    float c2 =  0.00f; // Numbers & Clear
    float c3 =  0.22f; // Numbers & Clear
    float c4 =  0.44f; // Math Operators

    #define DEF_BTN(idx, lbl, px, pz, width, cr, cg, cb) \
        calc->buttons[idx].id = idx; \
        strncpy(calc->buttons[idx].label, lbl, sizeof(calc->buttons[idx].label)-1); \
        calc->buttons[idx].x = px; \
        calc->buttons[idx].y = bh * 0.5f; \
        calc->buttons[idx].z = pz; \
        calc->buttons[idx].w = width; \
        calc->buttons[idx].h = bh; \
        calc->buttons[idx].d = bd; \
        calc->buttons[idx].r = cr; \
        calc->buttons[idx].g = cg; \
        calc->buttons[idx].b = cb; \
        calc->buttons[idx].press_anim = 0.0f; \
        calc->buttons[idx].hovered = 0;

    // Row 0: SIN, PI, C, CE, /
    DEF_BTN(0,  "SIN", c0, r0, bw, c_sci[0], c_sci[1], c_sci[2]);
    DEF_BTN(1,  "PI",  c1, r0, bw, c_sci[0], c_sci[1], c_sci[2]);
    DEF_BTN(2,  "C",   c2, r0, bw, c_clr[0], c_clr[1], c_clr[2]);
    DEF_BTN(3,  "CE",  c3, r0, bw, c_fn[0],  c_fn[1],  c_fn[2]);
    DEF_BTN(4,  "/",   c4, r0, bw, c_op[0],  c_op[1],  c_op[2]);

    // Row 1: COS, 7, 8, 9, *
    DEF_BTN(5,  "COS", c0, r1, bw, c_sci[0], c_sci[1], c_sci[2]);
    DEF_BTN(6,  "7",   c1, r1, bw, c_num[0], c_num[1], c_num[2]);
    DEF_BTN(7,  "8",   c2, r1, bw, c_num[0], c_num[1], c_num[2]);
    DEF_BTN(8,  "9",   c3, r1, bw, c_num[0], c_num[1], c_num[2]);
    DEF_BTN(9,  "*",   c4, r1, bw, c_op[0],  c_op[1],  c_op[2]);

    // Row 2: TAN, 4, 5, 6, -
    DEF_BTN(10, "TAN", c0, r2, bw, c_sci[0], c_sci[1], c_sci[2]);
    DEF_BTN(11, "4",   c1, r2, bw, c_num[0], c_num[1], c_num[2]);
    DEF_BTN(12, "5",   c2, r2, bw, c_num[0], c_num[1], c_num[2]);
    DEF_BTN(13, "6",   c3, r2, bw, c_num[0], c_num[1], c_num[2]);
    DEF_BTN(14, "-",   c4, r2, bw, c_op[0],  c_op[1],  c_op[2]);

    // Row 3: SQRT, 1, 2, 3, +
    DEF_BTN(15, "SQRT", c0, r3, bw, c_sci[0], c_sci[1], c_sci[2]);
    DEF_BTN(16, "1",    c1, r3, bw, c_num[0], c_num[1], c_num[2]);
    DEF_BTN(17, "2",    c2, r3, bw, c_num[0], c_num[1], c_num[2]);
    DEF_BTN(18, "3",    c3, r3, bw, c_num[0], c_num[1], c_num[2]);
    DEF_BTN(19, "+",    c4, r3, bw, c_op[0],  c_op[1],  c_op[2]);

    // Row 4: X^2, 0, ., +/-, =
    DEF_BTN(20, "X^2",  c0, r4, bw, c_sci[0], c_sci[1], c_sci[2]);
    DEF_BTN(21, "0",    c1, r4, bw, c_num[0], c_num[1], c_num[2]);
    DEF_BTN(22, ".",    c2, r4, bw, c_num[0], c_num[1], c_num[2]);
    DEF_BTN(23, "+/-",  c3, r4, bw, c_fn[0],  c_fn[1],  c_fn[2]);
    DEF_BTN(24, "=",    c4, r4, bw, c_eq[0],  c_eq[1],  c_eq[2]);

    #undef DEF_BTN
}

// Compute binary calculation
static void calc_execute_op(Calculator* calc) {
    if (!calc->pending_op) return;
    double val2 = atof(calc->display);
    double res = 0.0;
    int err = 0;

    switch (calc->pending_op) {
        case '+': res = calc->operand1 + val2; break;
        case '-': res = calc->operand1 - val2; break;
        case '*': res = calc->operand1 * val2; break;
        case '/':
            if (fabs(val2) < 1e-12) err = 1;
            else res = calc->operand1 / val2;
            break;
    }

    if (err) {
        strcpy(calc->display, "Error");
        strcpy(calc->history, "Div by 0");
        calc->error_state = 1;
        calc->entering_new = 1;
        calc->pending_op = 0;
    } else {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.8g", res);
        snprintf(calc->display, sizeof(calc->display), "%s", buf);
        calc->operand1 = res;
        calc->entering_new = 1;
        calc->has_decimal = (strchr(calc->display, '.') != NULL);
    }
}

// Process scientific & standard calculator button press
static void calc_press_button(Calculator* calc, int btn_id) {
    if (btn_id < 0 || btn_id >= NUM_BUTTONS) return;
    CalcButton* btn = &calc->buttons[btn_id];
    btn->press_anim = 0.16f;

    // 0-9
    if (strcmp(btn->label, "0") >= 0 && strcmp(btn->label, "9") <= 0 && strlen(btn->label) == 1) {
        audio_play_number();
        if (calc->error_state) calc_init(calc);
        if (calc->entering_new) {
            strcpy(calc->display, btn->label);
            calc->entering_new = 0;
            calc->has_decimal = 0;
        } else {
            if (strlen(calc->display) < 11) {
                if (strcmp(calc->display, "0") == 0) strcpy(calc->display, btn->label);
                else strcat(calc->display, btn->label);
            }
        }
        return;
    }

    // Decimal '.'
    if (strcmp(btn->label, ".") == 0) {
        audio_play_number();
        if (calc->error_state) calc_init(calc);
        if (calc->entering_new) {
            strcpy(calc->display, "0.");
            calc->entering_new = 0;
            calc->has_decimal = 1;
        } else if (!calc->has_decimal) {
            if (strlen(calc->display) < 11) {
                strcat(calc->display, ".");
                calc->has_decimal = 1;
            }
        }
        return;
    }

    // Negate '+/-'
    if (strcmp(btn->label, "+/-") == 0) {
        audio_play_number();
        if (calc->error_state) return;
        double v = -atof(calc->display);
        char buf[32];
        snprintf(buf, sizeof(buf), "%.8g", v);
        snprintf(calc->display, sizeof(calc->display), "%s", buf);
        return;
    }

    // Clear 'C'
    if (strcmp(btn->label, "C") == 0) {
        audio_play_clear();
        calc_init(calc);
        return;
    }

    // Clear Entry 'CE'
    if (strcmp(btn->label, "CE") == 0) {
        audio_play_clear();
        strcpy(calc->display, "0");
        calc->entering_new = 1;
        calc->has_decimal = 0;
        calc->error_state = 0;
        return;
    }

    // Scientific: PI
    if (strcmp(btn->label, "PI") == 0) {
        audio_play_operator();
        if (calc->error_state) calc_init(calc);
        strcpy(calc->display, "3.1415927");
        calc->entering_new = 1;
        calc->has_decimal = 1;
        return;
    }

    // Scientific: SIN (degrees)
    if (strcmp(btn->label, "SIN") == 0) {
        audio_play_operator();
        if (calc->error_state) return;
        double deg = atof(calc->display);
        double rad = deg * (3.14159265358979323846 / 180.0);
        double res = sin(rad);
        if (fabs(res) < 1e-15) res = 0.0;
        char buf[32];
        snprintf(buf, sizeof(buf), "%.8g", res);
        snprintf(calc->display, sizeof(calc->display), "%s", buf);
        snprintf(calc->history, sizeof(calc->history), "sin(%.5g)", deg);
        calc->entering_new = 1;
        return;
    }

    // Scientific: COS (degrees)
    if (strcmp(btn->label, "COS") == 0) {
        audio_play_operator();
        if (calc->error_state) return;
        double deg = atof(calc->display);
        double rad = deg * (3.14159265358979323846 / 180.0);
        double res = cos(rad);
        if (fabs(res) < 1e-15) res = 0.0;
        char buf[32];
        snprintf(buf, sizeof(buf), "%.8g", res);
        snprintf(calc->display, sizeof(calc->display), "%s", buf);
        snprintf(calc->history, sizeof(calc->history), "cos(%.5g)", deg);
        calc->entering_new = 1;
        return;
    }

    // Scientific: TAN (degrees)
    if (strcmp(btn->label, "TAN") == 0) {
        audio_play_operator();
        if (calc->error_state) return;
        double deg = atof(calc->display);
        if (fabs(fmod(deg - 90.0, 180.0)) < 1e-5) {
            strcpy(calc->display, "Error");
            strcpy(calc->history, "tan undef");
            calc->error_state = 1;
        } else {
            double rad = deg * (3.14159265358979323846 / 180.0);
            double res = tan(rad);
            char buf[32];
            snprintf(buf, sizeof(buf), "%.8g", res);
            snprintf(calc->display, sizeof(calc->display), "%s", buf);
            snprintf(calc->history, sizeof(calc->history), "tan(%.5g)", deg);
        }
        calc->entering_new = 1;
        return;
    }

    // Scientific: SQRT
    if (strcmp(btn->label, "SQRT") == 0) {
        audio_play_operator();
        if (calc->error_state) return;
        double val = atof(calc->display);
        if (val < 0.0) {
            strcpy(calc->display, "Error");
            strcpy(calc->history, "sqrt neg");
            calc->error_state = 1;
        } else {
            double res = sqrt(val);
            char buf[32];
            snprintf(buf, sizeof(buf), "%.8g", res);
            snprintf(calc->display, sizeof(calc->display), "%s", buf);
            snprintf(calc->history, sizeof(calc->history), "sqrt(%.5g)", val);
        }
        calc->entering_new = 1;
        return;
    }

    // Scientific: X^2
    if (strcmp(btn->label, "X^2") == 0) {
        audio_play_operator();
        if (calc->error_state) return;
        double val = atof(calc->display);
        double res = val * val;
        char buf[32];
        snprintf(buf, sizeof(buf), "%.8g", res);
        snprintf(calc->display, sizeof(calc->display), "%s", buf);
        snprintf(calc->history, sizeof(calc->history), "(%.5g)^2", val);
        calc->entering_new = 1;
        return;
    }

    // Binary operators '+', '-', '*', '/'
    if (strcmp(btn->label, "+") == 0 || strcmp(btn->label, "-") == 0 ||
        strcmp(btn->label, "*") == 0 || strcmp(btn->label, "/") == 0) {
        audio_play_operator();
        if (calc->error_state) return;
        
        if (calc->pending_op && !calc->entering_new) {
            calc_execute_op(calc);
        } else {
            calc->operand1 = atof(calc->display);
        }

        calc->pending_op = btn->label[0];
        snprintf(calc->history, sizeof(calc->history), "%.6g %c", calc->operand1, calc->pending_op);
        calc->entering_new = 1;
        calc->has_decimal = 0;
        return;
    }

    // Equals '='
    if (strcmp(btn->label, "=") == 0) {
        audio_play_equals();
        if (calc->error_state) return;
        if (calc->pending_op) {
            snprintf(calc->history, sizeof(calc->history), "%.6g %c %.6g =", 
                     calc->operand1, calc->pending_op, atof(calc->display));
            calc_execute_op(calc);
            calc->pending_op = 0;
        }
        return;
    }
}

static void calc_update(Calculator* calc, float dt) {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (calc->buttons[i].press_anim > 0.0f) {
            calc->buttons[i].press_anim -= dt;
            if (calc->buttons[i].press_anim < 0.0f) {
                calc->buttons[i].press_anim = 0.0f;
            }
        }
    }
}

// Raycast against calculator buttons
static int calc_raycast(Calculator* calc, Ray3 world_ray, float max_dist, float* out_dist) {
    Vec3 p = vec3_sub(world_ray.orig, vec3_create(CALC_WORLD_X, CALC_WORLD_Y, CALC_WORLD_Z));
    Vec3 d = world_ray.dir;

    float rad = -CALC_TILT_X * DEG2RAD;
    float cosA = cosf(rad);
    float sinA = sinf(rad);

    Vec3 local_p;
    local_p.x = p.x;
    local_p.y = p.y * cosA - p.z * sinA;
    local_p.z = p.y * sinA + p.z * cosA;

    Vec3 local_d;
    local_d.x = d.x;
    local_d.y = d.y * cosA - d.z * sinA;
    local_d.z = d.y * sinA + d.z * cosA;

    Ray3 local_ray = {local_p, local_d};

    int best_id = -1;
    float best_t = max_dist;

    for (int i = 0; i < NUM_BUTTONS; i++) {
        CalcButton* btn = &calc->buttons[i];
        BoundingBox3 aabb;
        aabb.min = vec3_create(btn->x - btn->w*0.5f, btn->y - btn->h*0.5f, btn->z - btn->d*0.5f);
        aabb.max = vec3_create(btn->x + btn->w*0.5f, btn->y + btn->h*0.5f, btn->z + btn->d*0.5f);

        float hit_t = 0.0f;
        if (ray_intersect_aabb(local_ray, aabb, &hit_t)) {
            if (hit_t > 0.0f && hit_t < best_t) {
                best_t = hit_t;
                best_id = i;
            }
        }
    }

    for (int i = 0; i < NUM_BUTTONS; i++) {
        calc->buttons[i].hovered = (i == best_id);
    }

    if (best_id != -1 && out_dist) *out_dist = best_t;
    return best_id;
}

// Draw a colored 3D box
static void draw_box(float x, float y, float z, float w, float h, float d) {
    float hw = w * 0.5f;
    float hh = h * 0.5f;
    float hd = d * 0.5f;

    glBegin(GL_QUADS);
    // Top
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(x - hw, y + hh, z - hd);
    glVertex3f(x - hw, y + hh, z + hd);
    glVertex3f(x + hw, y + hh, z + hd);
    glVertex3f(x + hw, y + hh, z - hd);

    // Bottom
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(x - hw, y - hh, z - hd);
    glVertex3f(x + hw, y - hh, z - hd);
    glVertex3f(x + hw, y - hh, z + hd);
    glVertex3f(x - hw, y - hh, z + hd);

    // Front
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(x - hw, y - hh, z + hd);
    glVertex3f(x + hw, y - hh, z + hd);
    glVertex3f(x + hw, y + hh, z + hd);
    glVertex3f(x - hw, y + hh, z + hd);

    // Back
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(x - hw, y - hh, z - hd);
    glVertex3f(x - hw, y + hh, z - hd);
    glVertex3f(x + hw, y + hh, z - hd);
    glVertex3f(x + hw, y - hh, z - hd);

    // Left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(x - hw, y - hh, z - hd);
    glVertex3f(x - hw, y - hh, z + hd);
    glVertex3f(x - hw, y + hh, z + hd);
    glVertex3f(x - hw, y + hh, z - hd);

    // Right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(x + hw, y - hh, z - hd);
    glVertex3f(x + hw, y + hh, z - hd);
    glVertex3f(x + hw, y + hh, z + hd);
    glVertex3f(x + hw, y - hh, z + hd);
    glEnd();
}

// Render entire enlarged 3D Scientific Calculator
static void calc_render(Calculator* calc) {
    // 1. Solid Mounting Stand (drawn in world coordinates under the calculator, down to tabletop y = 0.88m)
    // Tabletop is at y = 0.88m. This base securely supports the calculator console.
    glColor3f(0.12f, 0.13f, 0.16f);
    glBegin(GL_QUADS);
    // Stand Box
    glVertex3f(-0.55f, 0.882f, -0.48f);
    glVertex3f( 0.55f, 0.882f, -0.48f);
    glVertex3f( 0.55f, 1.05f, -0.48f);
    glVertex3f(-0.55f, 1.05f, -0.48f);

    glVertex3f(-0.55f, 0.882f,  0.45f);
    glVertex3f( 0.55f, 0.882f,  0.45f);
    glVertex3f( 0.55f, 0.98f,   0.45f);
    glVertex3f(-0.55f, 0.98f,   0.45f);

    glVertex3f(-0.55f, 0.882f, -0.48f);
    glVertex3f(-0.55f, 0.882f,  0.45f);
    glVertex3f(-0.55f, 0.98f,   0.45f);
    glVertex3f(-0.55f, 1.05f,  -0.48f);

    glVertex3f(0.55f, 0.882f, -0.48f);
    glVertex3f(0.55f, 0.882f,  0.45f);
    glVertex3f(0.55f, 0.98f,   0.45f);
    glVertex3f(0.55f, 1.05f,  -0.48f);
    glEnd();

    glPushMatrix();
    glTranslatef(CALC_WORLD_X, CALC_WORLD_Y, CALC_WORLD_Z);
    glRotatef(CALC_TILT_X, 1.0f, 0.0f, 0.0f);

    // 2. Calculator Chassis (Obsidian steel frame)
    glColor3f(0.11f, 0.12f, 0.15f);
    draw_box(0.0f, -0.045f, 0.08f, 1.25f, 0.09f, 1.10f);

    // Top deck plate
    glColor3f(0.17f, 0.18f, 0.22f);
    draw_box(0.0f, 0.003f, 0.08f, 1.20f, 0.01f, 1.05f);

    // Glowing futuristic side accent stripes (cyan neon)
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.90f, 1.0f);
    draw_box(-0.59f, 0.006f, 0.08f, 0.02f, 0.012f, 1.02f);
    draw_box( 0.59f, 0.006f, 0.08f, 0.02f, 0.012f, 1.02f);
    glEnable(GL_LIGHTING);

    // 3. Wide Panoramic LCD Display
    // Bezel (sitting cleanly at z = -0.34, well above keys at z = -0.14)
    glColor3f(0.06f, 0.07f, 0.09f);
    draw_box(0.0f, 0.012f, -0.34f, 1.08f, 0.015f, 0.17f);

    // Screen Panel (Backlit glowing emerald/cyan)
    glColor3f(0.04f, 0.17f, 0.13f);
    draw_box(0.0f, 0.019f, -0.34f, 1.04f, 0.005f, 0.14f);

    // Screen Content
    glDisable(GL_LIGHTING);

    // Secondary line: History / mode
    if (strlen(calc->history) > 0) {
        draw_button_label(calc->history, 0.22f, 0.022f, -0.38f, 0.024f);
    }

    // Main 7-segment digits
    int disp_len = (int)strlen(calc->display);
    float char_w = 0.050f;
    float char_h = 0.082f;
    float char_spacing = 0.064f;
    float right_x = 0.44f;
    float pz = -0.32f;

    for (int i = disp_len - 1, slot = 0; i >= 0; i--, slot++) {
        char ch = calc->display[i];
        float px = right_x - (float)slot * char_spacing;

        if (ch == '.') {
            glColor3f(0.2f, 0.98f, 0.80f);
            glPointSize(5.0f);
            glBegin(GL_POINTS);
            glVertex3f(px + 0.015f, 0.022f, pz + char_h * 0.5f);
            glEnd();
            slot--;
        } else {
            draw_7segment_char(ch, px, 0.022f, pz, char_w, char_h, 0.2f, 0.98f, 0.80f);
        }
    }

    glEnable(GL_LIGHTING);

    // 4. Render 3D Keycaps
    for (int i = 0; i < NUM_BUTTONS; i++) {
        CalcButton* btn = &calc->buttons[i];
        float press_depth = (btn->press_anim > 0.0f) ? 0.018f : 0.0f;
        float cur_y = btn->y - press_depth;

        if (btn->hovered) {
            glColor3f(btn->r * 1.4f + 0.25f, btn->g * 1.4f + 0.25f, btn->b * 1.4f + 0.25f);
        } else {
            glColor3f(btn->r, btn->g, btn->b);
        }

        draw_box(btn->x, cur_y, btn->z, btn->w, btn->h, btn->d);

        // Hover outline / glow border
        if (btn->hovered) {
            glDisable(GL_LIGHTING);
            glColor3f(1.0f, 0.95f, 0.25f);
            glLineWidth(2.5f);
            float hw = btn->w * 0.52f;
            float hh = btn->h * 0.52f;
            float hd = btn->d * 0.52f;
            glBegin(GL_LINE_LOOP);
            glVertex3f(btn->x - hw, cur_y + hh, btn->z - hd);
            glVertex3f(btn->x + hw, cur_y + hh, btn->z - hd);
            glVertex3f(btn->x + hw, cur_y + hh, btn->z + hd);
            glVertex3f(btn->x - hw, cur_y + hh, btn->z + hd);
            glEnd();
            glEnable(GL_LIGHTING);
        }

        // Button label
        glDisable(GL_LIGHTING);
        draw_button_label(btn->label, btn->x, cur_y + btn->h * 0.5f + 0.001f, btn->z, 0.042f);
        glEnable(GL_LIGHTING);
    }

    glPopMatrix();
}

#endif // CALCULATOR_H
