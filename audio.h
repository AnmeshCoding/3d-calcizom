#ifndef AUDIO_H
#define AUDIO_H

#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <stdlib.h>

#pragma pack(push, 1)
typedef struct {
    char     riff[4];
    DWORD    file_size;
    char     wave[4];
    char     fmt[4];
    DWORD    fmt_size;
    WORD     audio_format;
    WORD     num_channels;
    DWORD    sample_rate;
    DWORD    byte_rate;
    WORD     block_align;
    WORD     bits_per_sample;
    char     data[4];
    DWORD    data_size;
} WavHeader;
#pragma pack(pop)

typedef struct {
    unsigned char* wav_num;
    unsigned char* wav_op;
    unsigned char* wav_eq;
    unsigned char* wav_clear;
    unsigned char* wav_gunshot;
    unsigned char* wav_zombie_hit;
    unsigned char* wav_zombie_groan;
    unsigned char* wav_player_hurt;
    unsigned char* wav_emp;
} SoundSystem;

static SoundSystem g_sounds = {0};

static unsigned char* audio_create_tone_wav(float freq1, float freq2, float duration_sec, float decay_rate, int is_click) {
    const DWORD sample_rate = 44100;
    DWORD num_samples = (DWORD)(sample_rate * duration_sec);
    DWORD data_size = num_samples * sizeof(short);
    DWORD total_size = sizeof(WavHeader) + data_size;

    unsigned char* buffer = (unsigned char*)malloc(total_size);
    if (!buffer) return NULL;

    WavHeader* hdr = (WavHeader*)buffer;
    memcpy(hdr->riff, "RIFF", 4);
    hdr->file_size = total_size - 8;
    memcpy(hdr->wave, "WAVE", 4);
    memcpy(hdr->fmt, "fmt ", 4);
    hdr->fmt_size = 16;
    hdr->audio_format = 1; // PCM
    hdr->num_channels = 1; // Mono
    hdr->sample_rate = sample_rate;
    hdr->bits_per_sample = 16;
    hdr->byte_rate = sample_rate * sizeof(short);
    hdr->block_align = sizeof(short);
    memcpy(hdr->data, "data", 4);
    hdr->data_size = data_size;

    short* samples = (short*)(buffer + sizeof(WavHeader));
    for (DWORD i = 0; i < num_samples; i++) {
        float t = (float)i / (float)sample_rate;
        float env = expf(-decay_rate * t);
        if (env > 1.0f) env = 1.0f;
        
        float val = 0.0f;
        if (is_click) {
            float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * expf(-180.0f * t);
            val = (sinf(2.0f * 3.14159265f * freq1 * t) + 0.5f * sinf(2.0f * 3.14159265f * freq2 * t)) * env + noise * 0.4f;
        } else {
            val = (0.7f * sinf(2.0f * 3.14159265f * freq1 * t) + 0.3f * sinf(2.0f * 3.14159265f * freq2 * t)) * env;
        }
        
        if (val > 1.0f) val = 1.0f;
        if (val < -1.0f) val = -1.0f;
        samples[i] = (short)(val * 24000.0f);
    }

    return buffer;
}

static unsigned char* audio_create_gunshot_wav() {
    const DWORD sample_rate = 44100;
    float duration = 0.22f;
    DWORD num_samples = (DWORD)(sample_rate * duration);
    DWORD data_size = num_samples * sizeof(short);
    DWORD total_size = sizeof(WavHeader) + data_size;

    unsigned char* buffer = (unsigned char*)malloc(total_size);
    if (!buffer) return NULL;

    WavHeader* hdr = (WavHeader*)buffer;
    memcpy(hdr->riff, "RIFF", 4);
    hdr->file_size = total_size - 8;
    memcpy(hdr->wave, "WAVE", 4);
    memcpy(hdr->fmt, "fmt ", 4);
    hdr->fmt_size = 16;
    hdr->audio_format = 1;
    hdr->num_channels = 1;
    hdr->sample_rate = sample_rate;
    hdr->bits_per_sample = 16;
    hdr->byte_rate = sample_rate * sizeof(short);
    hdr->block_align = sizeof(short);
    memcpy(hdr->data, "data", 4);
    hdr->data_size = data_size;

    short* samples = (short*)(buffer + sizeof(WavHeader));
    for (DWORD i = 0; i < num_samples; i++) {
        float t = (float)i / (float)sample_rate;
        // Explosive noise + punchy bass drop
        float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
        float bass_freq = 160.0f * expf(-18.0f * t);
        float bass = sinf(2.0f * 3.14159265f * bass_freq * t);
        float env = expf(-16.0f * t);
        float val = (noise * 0.7f + bass * 0.8f) * env;
        if (val > 1.0f) val = 1.0f;
        if (val < -1.0f) val = -1.0f;
        samples[i] = (short)(val * 28000.0f);
    }
    return buffer;
}

static unsigned char* audio_create_emp_wav() {
    const DWORD sample_rate = 44100;
    float duration = 0.65f;
    DWORD num_samples = (DWORD)(sample_rate * duration);
    DWORD data_size = num_samples * sizeof(short);
    DWORD total_size = sizeof(WavHeader) + data_size;

    unsigned char* buffer = (unsigned char*)malloc(total_size);
    if (!buffer) return NULL;

    WavHeader* hdr = (WavHeader*)buffer;
    memcpy(hdr->riff, "RIFF", 4);
    hdr->file_size = total_size - 8;
    memcpy(hdr->wave, "WAVE", 4);
    memcpy(hdr->fmt, "fmt ", 4);
    hdr->fmt_size = 16;
    hdr->audio_format = 1;
    hdr->num_channels = 1;
    hdr->sample_rate = sample_rate;
    hdr->bits_per_sample = 16;
    hdr->byte_rate = sample_rate * sizeof(short);
    hdr->block_align = sizeof(short);
    memcpy(hdr->data, "data", 4);
    hdr->data_size = data_size;

    short* samples = (short*)(buffer + sizeof(WavHeader));
    for (DWORD i = 0; i < num_samples; i++) {
        float t = (float)i / (float)sample_rate;
        // Rising frequency sweep then explosive shockwave
        float sweep = sinf(2.0f * 3.14159265f * (300.0f + 1400.0f * t) * t);
        float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
        float env = (t < 0.2f) ? (t / 0.2f) : expf(-6.0f * (t - 0.2f));
        float val = (sweep * 0.5f + noise * 0.6f) * env;
        if (val > 1.0f) val = 1.0f;
        if (val < -1.0f) val = -1.0f;
        samples[i] = (short)(val * 28000.0f);
    }
    return buffer;
}

static unsigned char* audio_create_impact_wav(float freq, float duration, float decay) {
    const DWORD sample_rate = 44100;
    DWORD num_samples = (DWORD)(sample_rate * duration);
    DWORD data_size = num_samples * sizeof(short);
    DWORD total_size = sizeof(WavHeader) + data_size;

    unsigned char* buffer = (unsigned char*)malloc(total_size);
    if (!buffer) return NULL;

    WavHeader* hdr = (WavHeader*)buffer;
    memcpy(hdr->riff, "RIFF", 4);
    hdr->file_size = total_size - 8;
    memcpy(hdr->wave, "WAVE", 4);
    memcpy(hdr->fmt, "fmt ", 4);
    hdr->fmt_size = 16;
    hdr->audio_format = 1;
    hdr->num_channels = 1;
    hdr->sample_rate = sample_rate;
    hdr->bits_per_sample = 16;
    hdr->byte_rate = sample_rate * sizeof(short);
    hdr->block_align = sizeof(short);
    memcpy(hdr->data, "data", 4);
    hdr->data_size = data_size;

    short* samples = (short*)(buffer + sizeof(WavHeader));
    for (DWORD i = 0; i < num_samples; i++) {
        float t = (float)i / (float)sample_rate;
        float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
        float tone = sinf(2.0f * 3.14159265f * freq * t);
        float env = expf(-decay * t);
        float val = (tone * 0.6f + noise * 0.4f) * env;
        if (val > 1.0f) val = 1.0f;
        if (val < -1.0f) val = -1.0f;
        samples[i] = (short)(val * 24000.0f);
    }
    return buffer;
}

static void audio_init() {
    g_sounds.wav_num = audio_create_tone_wav(1200.0f, 600.0f, 0.045f, 50.0f, 1);
    g_sounds.wav_op = audio_create_tone_wav(1600.0f, 800.0f, 0.055f, 40.0f, 1);
    g_sounds.wav_eq = audio_create_tone_wav(880.0f, 1320.0f, 0.14f, 18.0f, 0);
    g_sounds.wav_clear = audio_create_tone_wav(520.0f, 260.0f, 0.06f, 45.0f, 1);
    g_sounds.wav_gunshot = audio_create_gunshot_wav();
    g_sounds.wav_emp = audio_create_emp_wav();
    g_sounds.wav_zombie_hit = audio_create_impact_wav(220.0f, 0.12f, 30.0f);
    g_sounds.wav_player_hurt = audio_create_impact_wav(95.0f, 0.18f, 22.0f);
    g_sounds.wav_zombie_groan = audio_create_tone_wav(110.0f, 135.0f, 0.28f, 8.0f, 0);
}

static void audio_play_number() {
    if (g_sounds.wav_num) PlaySoundA((LPCSTR)g_sounds.wav_num, NULL, SND_MEMORY | SND_ASYNC);
}

static void audio_play_operator() {
    if (g_sounds.wav_op) PlaySoundA((LPCSTR)g_sounds.wav_op, NULL, SND_MEMORY | SND_ASYNC);
}

static void audio_play_equals() {
    if (g_sounds.wav_eq) PlaySoundA((LPCSTR)g_sounds.wav_eq, NULL, SND_MEMORY | SND_ASYNC);
}

static void audio_play_clear() {
    if (g_sounds.wav_clear) PlaySoundA((LPCSTR)g_sounds.wav_clear, NULL, SND_MEMORY | SND_ASYNC);
}

static void audio_play_gunshot() {
    if (g_sounds.wav_gunshot) PlaySoundA((LPCSTR)g_sounds.wav_gunshot, NULL, SND_MEMORY | SND_ASYNC);
}

static void audio_play_emp() {
    if (g_sounds.wav_emp) PlaySoundA((LPCSTR)g_sounds.wav_emp, NULL, SND_MEMORY | SND_ASYNC);
}

static void audio_play_zombie_hit() {
    if (g_sounds.wav_zombie_hit) PlaySoundA((LPCSTR)g_sounds.wav_zombie_hit, NULL, SND_MEMORY | SND_ASYNC);
}

static void audio_play_player_hurt() {
    if (g_sounds.wav_player_hurt) PlaySoundA((LPCSTR)g_sounds.wav_player_hurt, NULL, SND_MEMORY | SND_ASYNC);
}

static void audio_play_zombie_groan() {
    if (g_sounds.wav_zombie_groan) PlaySoundA((LPCSTR)g_sounds.wav_zombie_groan, NULL, SND_MEMORY | SND_ASYNC);
}

static void audio_cleanup() {
    if (g_sounds.wav_num) free(g_sounds.wav_num);
    if (g_sounds.wav_op) free(g_sounds.wav_op);
    if (g_sounds.wav_eq) free(g_sounds.wav_eq);
    if (g_sounds.wav_clear) free(g_sounds.wav_clear);
    if (g_sounds.wav_gunshot) free(g_sounds.wav_gunshot);
    if (g_sounds.wav_emp) free(g_sounds.wav_emp);
    if (g_sounds.wav_zombie_hit) free(g_sounds.wav_zombie_hit);
    if (g_sounds.wav_player_hurt) free(g_sounds.wav_player_hurt);
    if (g_sounds.wav_zombie_groan) free(g_sounds.wav_zombie_groan);
}

#endif // AUDIO_H
