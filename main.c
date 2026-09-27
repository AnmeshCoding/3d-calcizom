#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "math3d.h"
#include "audio.h"
#include "calculator.h"
#include "world.h"
#include "camera.h"
#include "zombie.h"
#include "weapon.h"
#include "math_game.h"
#include "hud.h"

// Window state
static int g_window_w = 1280;
static int g_window_h = 720;
static HWND g_hwnd = NULL;
static HDC g_hdc = NULL;
static HGLRC g_hrc = NULL;
static int g_running = 1;
static int g_mouse_captured = 1;

// Input state
static int g_key_w = 0;
static int g_key_s = 0;
static int g_key_a = 0;
static int g_key_d = 0;
static int g_key_space = 0;
static int g_key_e_pressed = 0;
static int g_mouse_lmb_pressed = 0;

static int g_cursor_x = 0;
static int g_cursor_y = 0;

// Game systems
static Camera g_cam;
static Calculator g_calc;
static ZombieHorde g_horde;
static Weapon g_weapon;
static MathGame g_game;
static int g_hovered_btn = -1;

static void set_perspective(double fovy_deg, double aspect, double zNear, double zFar) {
    double fH = tan(fovy_deg / 360.0 * 3.14159265358979323846) * zNear;
    double fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

static void set_lookat(Vec3 eye, Vec3 target, Vec3 up) {
    Vec3 f = vec3_normalize(vec3_sub(target, eye));
    Vec3 s = vec3_normalize(vec3_cross(f, up));
    Vec3 u = vec3_cross(s, f);

    float m[16] = {
        s.x,  u.x, -f.x, 0.0f,
        s.y,  u.y, -f.y, 0.0f,
        s.z,  u.z, -f.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    glMultMatrixf(m);
    glTranslatef(-eye.x, -eye.y, -eye.z);
}

static void center_mouse() {
    if (!g_mouse_captured || g_cam.focus_mode) return;
    RECT rect;
    GetClientRect(g_hwnd, &rect);
    POINT pt = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
    ClientToScreen(g_hwnd, &pt);
    SetCursorPos(pt.x, pt.y);
}

// Map keyboard character to scientific calculator button
static void handle_calc_keyboard_input(char ch) {
    int btn_map[10] = {21, 16, 17, 18, 11, 12, 13, 6, 7, 8}; // 0..9 button indices
    if (ch >= '0' && ch <= '9') {
        calc_press_button(&g_calc, btn_map[ch - '0']);
    } else if (ch == '.') {
        calc_press_button(&g_calc, 22); // '.'
    } else if (ch == '+') {
        calc_press_button(&g_calc, 19); // '+'
    } else if (ch == '-') {
        calc_press_button(&g_calc, 14); // '-'
    } else if (ch == '*') {
        calc_press_button(&g_calc, 9);  // '*'
    } else if (ch == '/') {
        calc_press_button(&g_calc, 4);  // '/'
    } else if (ch == '=' || ch == 13) { // Enter or '='
        calc_press_button(&g_calc, 24); // '='
        math_game_check_answer(&g_game, &g_calc, &g_horde);
    } else if (ch == 'c' || ch == 'C') {
        calc_press_button(&g_calc, 2);  // 'C'
    } else if (ch == 8) {               // Backspace
        calc_press_button(&g_calc, 3);  // 'CE'
    } else if (ch == 's' || ch == 'S') {
        if (g_cam.focus_mode) calc_press_button(&g_calc, 0); // SIN
    } else if (ch == 'q' || ch == 'Q') {
        calc_press_button(&g_calc, 15); // SQRT
    } else if (ch == 'p' || ch == 'P') {
        calc_press_button(&g_calc, 1);  // PI
    }

    // Always check if current result solves question
    math_game_check_answer(&g_game, &g_calc, &g_horde);
}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE:
            g_running = 0;
            return 0;

        case WM_SIZE:
            g_window_w = LOWORD(lParam);
            g_window_h = HIWORD(lParam);
            if (g_window_h == 0) g_window_h = 1;
            glViewport(0, 0, g_window_w, g_window_h);
            return 0;

        case WM_SETFOCUS:
            if (!g_cam.focus_mode) {
                g_mouse_captured = 1;
                ShowCursor(FALSE);
                center_mouse();
            }
            return 0;

        case WM_KILLFOCUS:
            g_mouse_captured = 0;
            ShowCursor(TRUE);
            return 0;

        case WM_LBUTTONDOWN:
            if (!g_mouse_captured && !g_cam.focus_mode) {
                g_mouse_captured = 1;
                ShowCursor(FALSE);
                center_mouse();
            } else {
                g_mouse_lmb_pressed = 1;
            }
            return 0;

        case WM_MOUSEMOVE: {
            int cur_x = LOWORD(lParam);
            int cur_y = HIWORD(lParam);
            g_cursor_x = cur_x;
            g_cursor_y = cur_y;

            if (g_mouse_captured && !g_cam.focus_mode) {
                RECT rect;
                GetClientRect(hwnd, &rect);
                int mid_x = (rect.right - rect.left) / 2;
                int mid_y = (rect.bottom - rect.top) / 2;
                int dx = cur_x - mid_x;
                int dy = cur_y - mid_y;

                if (dx != 0 || dy != 0) {
                    float sens = 0.0024f;
                    g_cam.yaw += (float)dx * sens;
                    g_cam.pitch -= (float)dy * sens;

                    float max_pitch = 85.0f * DEG2RAD;
                    if (g_cam.pitch > max_pitch) g_cam.pitch = max_pitch;
                    if (g_cam.pitch < -max_pitch) g_cam.pitch = -max_pitch;

                    center_mouse();
                }
            }
            return 0;
        }

        case WM_KEYDOWN:
            switch (wParam) {
                case 'W': g_key_w = 1; break;
                case 'S': g_key_s = 1; break;
                case 'A': g_key_a = 1; break;
                case 'D': g_key_d = 1; break;
                case VK_SPACE: g_key_space = 1; break;
                case 'E': g_key_e_pressed = 1; break;

                case 'R':
                    if (g_game.game_over) {
                        math_game_init(&g_game);
                        zombie_horde_init(&g_horde);
                        calc_init(&g_calc);
                    }
                    break;

                case 'F':
                case VK_TAB:
                    g_cam.focus_mode = !g_cam.focus_mode;
                    if (g_cam.focus_mode) {
                        ShowCursor(TRUE);
                    } else {
                        if (g_mouse_captured) {
                            ShowCursor(FALSE);
                            center_mouse();
                        }
                    }
                    break;

                case VK_ESCAPE:
                    if (g_cam.focus_mode) {
                        g_cam.focus_mode = 0;
                        if (g_mouse_captured) {
                            ShowCursor(FALSE);
                            center_mouse();
                        }
                    } else {
                        g_mouse_captured = !g_mouse_captured;
                        ShowCursor(!g_mouse_captured);
                        if (g_mouse_captured) center_mouse();
                    }
                    break;

                // Numpad Keys
                case VK_NUMPAD0: handle_calc_keyboard_input('0'); break;
                case VK_NUMPAD1: handle_calc_keyboard_input('1'); break;
                case VK_NUMPAD2: handle_calc_keyboard_input('2'); break;
                case VK_NUMPAD3: handle_calc_keyboard_input('3'); break;
                case VK_NUMPAD4: handle_calc_keyboard_input('4'); break;
                case VK_NUMPAD5: handle_calc_keyboard_input('5'); break;
                case VK_NUMPAD6: handle_calc_keyboard_input('6'); break;
                case VK_NUMPAD7: handle_calc_keyboard_input('7'); break;
                case VK_NUMPAD8: handle_calc_keyboard_input('8'); break;
                case VK_NUMPAD9: handle_calc_keyboard_input('9'); break;
                case VK_DECIMAL: handle_calc_keyboard_input('.'); break;
                case VK_ADD:      handle_calc_keyboard_input('+'); break;
                case VK_SUBTRACT: handle_calc_keyboard_input('-'); break;
                case VK_MULTIPLY: handle_calc_keyboard_input('*'); break;
                case VK_DIVIDE:   handle_calc_keyboard_input('/'); break;
                case VK_RETURN:   handle_calc_keyboard_input('='); break;
                case VK_BACK:     handle_calc_keyboard_input(8);   break;
                case VK_DELETE:   handle_calc_keyboard_input('C'); break;
            }
            return 0;

        case WM_CHAR:
            if (wParam >= '0' && wParam <= '9') {
                handle_calc_keyboard_input((char)wParam);
            } else if (wParam == '+' || wParam == '-' || wParam == '*' || wParam == '/' || wParam == '=' || wParam == '.' || wParam == 'c' || wParam == 'C') {
                handle_calc_keyboard_input((char)wParam);
            }
            return 0;

        case WM_KEYUP:
            switch (wParam) {
                case 'W': g_key_w = 0; break;
                case 'S': g_key_s = 0; break;
                case 'A': g_key_a = 0; break;
                case 'D': g_key_d = 0; break;
                case VK_SPACE: g_key_space = 0; break;
            }
            return 0;
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

static int init_window_and_gl(HINSTANCE hInstance) {
    WNDCLASSEXA wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "3DWorldMathZombieClass";

    if (!RegisterClassExA(&wc)) {
        MessageBoxA(NULL, "Failed to register window class.", "Error", MB_OK);
        return 0;
    }

    int screen_w = GetSystemMetrics(SM_CXSCREEN);
    int screen_h = GetSystemMetrics(SM_CYSCREEN);
    int win_x = (screen_w - g_window_w) / 2;
    int win_y = (screen_h - g_window_h) / 2;

    RECT wr = {0, 0, g_window_w, g_window_h};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    g_hwnd = CreateWindowExA(
        0,
        wc.lpszClassName,
        "3D Scientific Calculator vs Zombie Horde (Pure C)",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        win_x, win_y,
        wr.right - wr.left, wr.bottom - wr.top,
        NULL, NULL, hInstance, NULL
    );

    if (!g_hwnd) return 0;
    g_hdc = GetDC(g_hwnd);

    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;

    int format = ChoosePixelFormat(g_hdc, &pfd);
    SetPixelFormat(g_hdc, format, &pfd);

    g_hrc = wglCreateContext(g_hdc);
    wglMakeCurrent(g_hdc, g_hrc);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glDisable(GL_CULL_FACE);
    glClearColor(0.04f, 0.05f, 0.07f, 1.0f);

    return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hPrevInstance; (void)lpCmdLine; (void)nCmdShow;

    if (!init_window_and_gl(hInstance)) return 1;

    audio_init();
    calc_init(&g_calc);
    camera_init(&g_cam);
    zombie_horde_init(&g_horde);
    weapon_init(&g_weapon);
    math_game_init(&g_game);

    ShowCursor(FALSE);
    center_mouse();

    LARGE_INTEGER freq, last_time, cur_time;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last_time);

    MSG msg;
    while (g_running) {
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                g_running = 0;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        if (!g_running) break;

        QueryPerformanceCounter(&cur_time);
        float dt = (float)(cur_time.QuadPart - last_time.QuadPart) / (float)freq.QuadPart;
        last_time = cur_time;
        if (dt > 0.05f) dt = 0.05f;

        // Update physics & AI if alive
        if (!g_game.game_over) {
            camera_update(&g_cam, dt, g_key_w, g_key_s, g_key_a, g_key_d, g_key_space);
            zombie_horde_update(&g_horde, g_cam.pos, dt, &g_game.player_hp);
        }
        calc_update(&g_calc, dt);
        weapon_update(&g_weapon, dt);
        math_game_update(&g_game, dt);

        // Raycasting for button interaction
        int use_mouse = g_cam.focus_mode;
        Ray3 pick_ray = camera_generate_ray(&g_cam, g_window_w, g_window_h, g_cursor_x, g_cursor_y, use_mouse);
        float hit_dist = 0.0f;
        g_hovered_btn = calc_raycast(&g_calc, pick_ray, 5.0f, &hit_dist);

        // COMBAT & INTERACTION HANDLING:
        if (g_cam.focus_mode) {
            // In Focus Mode: Left Click presses calculator keys
            if (g_mouse_lmb_pressed) {
                if (g_hovered_btn >= 0) {
                    calc_press_button(&g_calc, g_hovered_btn);
                    math_game_check_answer(&g_game, &g_calc, &g_horde);
                }
                g_mouse_lmb_pressed = 0;
            }
        } else {
            // In First-Person Walk Mode:
            // 1. Left Click FIRES THE GUN!
            if (g_mouse_lmb_pressed && !g_game.game_over) {
                if (weapon_fire(&g_weapon)) {
                    Vec3 hit_pos;
                    zombie_horde_raycast_shoot(&g_horde, pick_ray, 1, &hit_pos);
                }
                g_mouse_lmb_pressed = 0;
            }

            // 2. 'E' Key INTERACTS WITH CALCULATOR!
            if (g_key_e_pressed) {
                if (g_hovered_btn >= 0) {
                    calc_press_button(&g_calc, g_hovered_btn);
                    math_game_check_answer(&g_game, &g_calc, &g_horde);
                }
                g_key_e_pressed = 0;
            }
        }

        // RENDER FRAME
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 3D Perspective Projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        double aspect = (double)g_window_w / (double)g_window_h;
        set_perspective(60.0, aspect, 0.1, 100.0);

        // 3D Camera View
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        Vec3 eye, target, up;
        camera_get_view(&g_cam, &eye, &target, &up);
        set_lookat(eye, target, up);

        // Render World
        world_setup_lighting();
        world_render();
        zombie_horde_render(&g_horde);
        calc_render(&g_calc);

        // Render First-Person Combat Gun
        weapon_render(&g_weapon, g_cam.focus_mode);

        // Render 2D HUD & Mission Status
        hud_render(g_window_w, g_window_h, &g_calc, &g_game, &g_horde, g_hovered_btn, g_cam.focus_mode);

        SwapBuffers(g_hdc);
        Sleep(1);
    }

    audio_cleanup();
    wglMakeCurrent(NULL, NULL);
    if (g_hrc) wglDeleteContext(g_hrc);
    if (g_hdc && g_hwnd) ReleaseDC(g_hwnd, g_hdc);
    if (g_hwnd) DestroyWindow(g_hwnd);

    return 0;
}
