#pragma once
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#define MAP_WIDTH 32
#define MAP_HEIGHT 32
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MAX_ENEMIES 15
#define MAX_PROJECTILES 20
#define MAX_ITEMS 15
#define MAX_RENDER_SPRITES 50

// Режимы игры
typedef enum { 
    MODE_MENU, 
    MODE_GAME_SOLO, 
    MODE_GAME_COOP 
} GameMode;

// Структура ввода
typedef struct { 
    bool w, s, a, d, space; 
    bool up, down, left, right, enter; 
    bool reset, k1, k2; 
} InputState;

typedef struct { 
    int wall_height; 
    int hit_type; 
    bool is_dark; 
    float texture_x; 
} RenderColumn;

typedef struct { 
    float x; 
    float y; 
    int tex_id; 
    float dist; 
} RenderSprite;

typedef struct { 
    float x; 
    float y; 
    int type; 
    int hp; 
    float cooldown; 
    bool alive; 
    float rx; 
    float ry; 
    float roam_timer; 
} Enemy;

typedef struct { 
    float x; 
    float y; 
    float vx; 
    float vy; 
    bool active; 
} Projectile;

typedef struct { 
    float x; 
    float y; 
    int type; 
    bool active; 
} Item;

// Структура одного игрока
typedef struct {
    float x, y;
    float dir_x, dir_y;
    float plane_x, plane_y;
    int hp, ammo;
    float weapon_cooldown;
    float weapon_anim_time;
    float damage_flash_time;
    bool alive;
} Player;

// Буфер отрисовки одного экрана
typedef struct {
    RenderColumn render_buffer[SCREEN_WIDTH];
    float z_buffer[SCREEN_WIDTH];
    RenderSprite render_sprites[MAX_RENDER_SPRITES];
    int num_sprites;
} RenderView;

// Главное состояние
typedef struct {
    GameMode mode;
    Player players[2]; 
    RenderView views[2]; 
    
    bool has_won;
    bool play_damage_sound; 

    int map[MAP_HEIGHT][MAP_WIDTH]; 
    Enemy enemies[MAX_ENEMIES];
    Projectile projectiles[MAX_PROJECTILES];
    Item items[MAX_ITEMS];
} GameState;

void Core_Init(GameState* state);
void Core_Update(GameState* state, const InputState* input, float delta_time);