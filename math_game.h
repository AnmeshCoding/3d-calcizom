#ifndef MATH_GAME_H
#define MATH_GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "calculator.h"
#include "zombie.h"
#include "audio.h"

typedef struct {
    char question_str[64];
    double expected_answer;
    int player_hp;
    int max_hp;
    int score;
    int questions_solved;
    float emp_flash_timer;
    int game_over;
} MathGame;

static void math_game_generate_question(MathGame* game) {
    int op_type = rand() % 5;
    int a = 0, b = 0;
    double ans = 0.0;

    switch (op_type) {
        case 0: // Addition
            a = rand() % 50 + 5;
            b = rand() % 50 + 5;
            ans = (double)(a + b);
            snprintf(game->question_str, sizeof(game->question_str), "%d + %d", a, b);
            break;
        case 1: // Subtraction
            a = rand() % 60 + 20;
            b = rand() % (a - 5) + 1;
            ans = (double)(a - b);
            snprintf(game->question_str, sizeof(game->question_str), "%d - %d", a, b);
            break;
        case 2: // Multiplication
            a = rand() % 12 + 3;
            b = rand() % 12 + 2;
            ans = (double)(a * b);
            snprintf(game->question_str, sizeof(game->question_str), "%d * %d", a, b);
            break;
        case 3: { // Division
            b = rand() % 10 + 2;
            int q = rand() % 10 + 2;
            a = b * q;
            ans = (double)q;
            snprintf(game->question_str, sizeof(game->question_str), "%d / %d", a, b);
            break;
        }
        case 4: { // Square Root
            int roots[] = {4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144};
            int idx = rand() % 11;
            a = roots[idx];
            ans = sqrt((double)a);
            snprintf(game->question_str, sizeof(game->question_str), "SQRT(%d)", a);
            break;
        }
    }

    game->expected_answer = ans;
}

static void math_game_init(MathGame* game) {
    game->player_hp = 100;
    game->max_hp = 100;
    game->score = 0;
    game->questions_solved = 0;
    game->emp_flash_timer = 0.0f;
    game->game_over = 0;
    math_game_generate_question(game);
}

// Check if calculator current result matches current math question
static int math_game_check_answer(MathGame* game, Calculator* calc, ZombieHorde* horde) {
    if (game->game_over) return 0;

    double user_val = atof(calc->display);
    if (fabs(user_val - game->expected_answer) < 1e-4) {
        // MATCH! Trigger EMP Nuke Blast!
        audio_play_emp();
        game->emp_flash_timer = 0.85f;
        int wiped = zombie_horde_emp_blast(horde);
        game->score += 500 + wiped * 100;
        game->questions_solved++;

        // Heal player by +35 HP
        game->player_hp += 35;
        if (game->player_hp > game->max_hp) game->player_hp = game->max_hp;

        // Advance zombie wave
        horde->wave++;
        horde->spawn_timer = 1.0f;

        // Set calculator history
        snprintf(calc->history, sizeof(calc->history), "EMP BLAST! +500 PTS");

        // Generate next question
        math_game_generate_question(game);
        return 1;
    }
    return 0;
}

static void math_game_update(MathGame* game, float dt) {
    if (game->emp_flash_timer > 0.0f) {
        game->emp_flash_timer -= dt;
        if (game->emp_flash_timer < 0.0f) game->emp_flash_timer = 0.0f;
    }
    if (game->player_hp <= 0) {
        game->player_hp = 0;
        game->game_over = 1;
    }
}

#endif // MATH_GAME_H
