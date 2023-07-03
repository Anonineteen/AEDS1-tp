#ifndef COISAS_H
#define COISAS_H

#include <stdbool.h>
#include "raylib.h"

typedef struct Pontuacao {
    char nome[20];
    int pontos;
} Pontuacao;

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define STD_SIZE_X 50
#define STD_SIZE_Y 50
#define SCREEN_BORDER 4

#define INIMIGO_DIFICIL (Enemy) {.speed=10, .pos = (Rectangle){.width=STD_SIZE_X*0.8,.height=STD_SIZE_Y*0.8}, .color=GREEN}
#define INIMIGO_COMUM (Enemy) {.speed=6, .pos = (Rectangle){.width=STD_SIZE_X,.height=STD_SIZE_Y}, .color=BLACK}

typedef struct Tiro {
    Rectangle pos;
    Color color;
    bool existe;
    int speed;
    int direcaox;
    int direcaoy;
} Tiro;

typedef struct Player
{
    Rectangle pos;
    Color color;
    int pontos;
    int speed;
    int special;
    int direcaox;
    int direcaoy;
    Tiro tiros[2];
    int numero_tiros;
} Player;

typedef struct Enemy
{
    Rectangle pos;
    Color color;
    Tiro tiros[2];
    int speed;
    int armadura;
    int direction;
    int draw_enemy;
    int has_key;
} Enemy;

typedef struct Map
{
    Rectangle barriers[10];
    int num_barriers;
    Rectangle door;
    Rectangle prev_door;
    Color color;
    Enemy enemies[10];
    int num_enemies;
    Rectangle special_item;
    int draw_special_item;
    int door_locked;
    int next_map;
    int prev_map;
} Map;

typedef struct Game
{
    Map maps[10];
    Texture2D sprite_player;
    Texture2D sprite_inimigo;
    int num_maps;
    int curr_map;
    Player player;
    int screenWidth;
    int screenHeight;
    int gameover;
} Game;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
void InitGame(Game *g);        // Initialize game
void UpdateGame(Game *g);      // Update game (one frame)
void DrawGame(Game *g);        // Draw game (one frame)

//------------------------------------------------------------------------------------
// Auxiliar Functions Declaration
//------------------------------------------------------------------------------------
void draw_borders(Game *g);
void draw_map(Game *g);
void update_enemy_pos(Game *g, Enemy *e);
void update_hero_pos(Game *g);
void ataca_inimigo(Enemy* inimigo, Map* map);

int barrier_collision(Map *m, Rectangle *t);
void map0_setup(Game *g);
void map1_setup(Game *g);
void map2_setup(Game *g);
void map3_setup(Game *g);
void map4_setup(Game *g);
void map5_setup(Game *g);
void map6_setup(Game *g);
void map7_setup(Game *g);

#endif
