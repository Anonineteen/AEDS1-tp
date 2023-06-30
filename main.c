#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define STD_SIZE_X 30
#define STD_SIZE_Y 30
#define SCREEN_BORDER 4

typedef struct Tiro {

    Rectangle pos;
    Color color;
    int speed;
    int direcaox;
    int direcaoy;
} Tiro;

typedef struct Player
{
    Rectangle pos;
    Color color;
    int speed;
    int special;
    int direcaox;
    int direcaoy;
    Tiro tiro;
} Player;

typedef struct Enemy
{
    Rectangle pos;
    Color color;
    int speed;
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
void UpdateDrawFrame(Game *g); // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Auxiliar Functions Declaration
//------------------------------------------------------------------------------------
void draw_borders(Game *g);
void draw_map(Game *g);
void update_enemy_pos(Game *g, Enemy *e);
void update_hero_pos(Game *g);

int barrier_collision(Map *m, Rectangle *t);
void map0_setup(Game *g);
void map1_setup(Game *g);
void map2_setup(Game *g);
void desenha_tiro(Game *g);
void cria_tiro (Game *g);
void movimenta_tiro (Game *g);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    Game game;
    game.screenWidth = 800;
    game.screenHeight = 450;

    InitWindow(game.screenWidth, game.screenHeight, "o jogo");
    SetTargetFPS(60);

    InitGame(&game);

    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        UpdateDrawFrame(&game);
        if (game.gameover)
            break;
    }
    while (!IsKeyDown(KEY_ENTER) && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("GAME OVER", GetScreenWidth() / 2 - MeasureText("GAME OVER", 20) / 2, GetScreenHeight() / 2 - 50, 20, BLACK);
        EndDrawing();
    }
    return 0;
}
//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(Game *g)
{

    g->curr_map = 0;
    g->num_maps = 10;
    g->player.pos = (Rectangle){150, 300, STD_SIZE_X, STD_SIZE_Y};
    g->player.color = BLACK;
    g->player.speed = 6;
    g->player.special = 0;
    g->player.direcaox = 1;
    g->gameover = 0;
    map0_setup(g);
    map1_setup(g);
    map2_setup(g);
}

// Update game (one frame)
void UpdateGame(Game *g)
{
    update_hero_pos(g);
    cria_tiro(g);
    movimenta_tiro(g);

    Map *map = &g->maps[g->curr_map];
    for (int i; i < map->num_enemies; i++)
    {
        if (!map->enemies[i].draw_enemy)
            continue;
        update_enemy_pos(g, &map->enemies[i]);
        if (!CheckCollisionRecs(g->player.pos, map->enemies[i].pos))
            continue;

        if (g->player.special)
        {
            map->enemies[i].draw_enemy = 0;
            if (map->enemies[i].has_key)
            {
                map->door_locked = 0;
            }
            continue;
        }
        g->gameover = 1;
    }


    if (CheckCollisionRecs(g->player.pos, map->special_item) && map->draw_special_item)
    {
        g->player.pos.width += 10;
        g->player.pos.height += 10;
        g->player.special = 1;
        map->draw_special_item = 0;
    }

    if (CheckCollisionRecs(g->player.pos, map->door) && !map->door_locked)
    {
        g->curr_map = map->next_map;
        g->player.pos = (Rectangle){50, 200, STD_SIZE_X, STD_SIZE_Y};
        g->player.special = 0;
    }

    if (map->prev_map != -1 && CheckCollisionRecs(g->player.pos, map->prev_door))
    {
        g->curr_map = map->prev_map;
        g->player.pos = (Rectangle){g->screenWidth - 50, g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
        g->player.special = 0;
    }
}

// Draw game (one frame)
void DrawGame(Game *g)
{
    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawRectangle(0, 0, g->screenWidth, g->screenHeight, GRAY);
    draw_borders(g);
    draw_map(g);
    desenha_tiro(g);

    DrawRectangleRec(g->player.pos, g->player.color);

    EndDrawing();
}

// Update and Draw (one frame)
void UpdateDrawFrame(Game *g)
{
    UpdateGame(g);
    DrawGame(g);
}

void draw_borders(Game *g)
{
    DrawRectangle(0, 0, SCREEN_BORDER, g->screenHeight, BLACK);
    DrawRectangle(0, 0, g->screenWidth, SCREEN_BORDER, BLACK);
    DrawRectangle(g->screenWidth - SCREEN_BORDER, 0, g->screenWidth, g->screenHeight, BLACK);
    DrawRectangle(0, g->screenHeight - SCREEN_BORDER, g->screenWidth, g->screenHeight, BLACK);
}

void draw_map(Game *g)
{
    Map *map = &g->maps[g->curr_map];
    for (int i = 0; i < map->num_barriers; i++)
    {
        DrawRectangleRec(map->barriers[i], BLACK);
    }

    if (map->door_locked)
        DrawRectangleRec(map->door, RED);
    else
        DrawRectangleRec(map->door, GREEN);

    if (map->prev_map != -1)
    {
        DrawRectangleRec(map->prev_door, GREEN);
    }

    for (int i = 0; i < map->num_enemies; i++)
    {
        if (!map->enemies[i].draw_enemy)
            continue;
        DrawRectangleRec(map->enemies[i].pos, PINK);
    }

    if (map->draw_special_item)
        DrawRectangleRec(map->special_item, PURPLE);
}

void update_hero_pos(Game *g)
{

    Player *p = &g->player;
    Map *m = &g->maps[g->curr_map];

    int direcaox = 0, direcaoy = 0;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        direcaox += -1;
    }
   else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        direcaox += 1;
    }
  else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
    {
        direcaoy += -1;
    }
  else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
    {
        direcaoy += 1;
    }
    if(!(direcaox==0 && direcaoy==0)){
        p->direcaoy = direcaoy;
        p->direcaox = direcaox;
    }

    int novo_x = p->pos.x + p->speed * direcaox;
    int novo_y = p->pos.y + p->speed * direcaoy;
    Rectangle nova_posicao = {.x=novo_x, .y=novo_y, .width=p->pos.width, .height=p->pos.height};
    int coliding = barrier_collision(m, &nova_posicao);
    
    if(coliding) return;
    if (novo_y >= SCREEN_BORDER && novo_y + p->pos.height <= g->screenHeight - SCREEN_BORDER) {
        p->pos.y = novo_y;
    }
    if (novo_x >= SCREEN_BORDER && novo_x + p->pos.width <= g->screenWidth - SCREEN_BORDER) {
        p->pos.x = novo_x;
    }
}

void cria_tiro (Game *g){

    if(IsKeyPressed(KEY_SPACE)){
        Tiro novo_tiro;
        novo_tiro.pos.x = g->player.pos.x;
        novo_tiro.pos.y = g->player.pos.y;
        novo_tiro.pos.width = 10;
        novo_tiro.pos.height = 10;
        novo_tiro.color = BLUE;
        novo_tiro.speed = g->player.speed*2;
        novo_tiro.direcaox = g->player.direcaox;
        novo_tiro.direcaoy = g->player.direcaoy;
        g->player.tiro = novo_tiro;
    }
}

void movimenta_tiro (Game *g){

    Tiro* tiro = &g->player.tiro;
    int distancia1 = tiro->direcaox * tiro->speed;
    int distancia2 = tiro->direcaoy * tiro->speed;
    tiro->pos.x += distancia1;
    tiro->pos.y += distancia2;
}

void atualiza_tiro (Game *g){

    Tiro* tiro = &g->player.tiro;
}

void desenha_tiro(Game *g){

    Tiro tiro = g->player.tiro;

    DrawRectangle(tiro.pos.x, tiro.pos.y,tiro.pos.width,tiro.pos.height,tiro.color);
}

void update_enemy_pos(Game *g, Enemy *e)
{
    Map *m = &g->maps[g->curr_map];

    if (e->direction == KEY_LEFT)
    {
        if (e->pos.x > SCREEN_BORDER)
            e->pos.x -= e->speed;
        else
        {
            e->direction = KEY_RIGHT + (rand() % 4);
        }
        if (barrier_collision(m, &e->pos))
        {
            e->pos.x += e->speed;
            e->direction = KEY_RIGHT + (rand() % 4);
        }
    }
    else if (e->direction == KEY_RIGHT)
    {
        if (e->pos.x + e->pos.width < g->screenWidth - SCREEN_BORDER)
            e->pos.x += e->speed;
        else
            e->direction = KEY_RIGHT + (rand() % 4);

        if (barrier_collision(m, &e->pos))
        {
            e->pos.x -= e->speed;
            e->direction = KEY_RIGHT + (rand() % 4);
        }
    }
    else if (e->direction == KEY_UP)
    {
        if (e->pos.y > SCREEN_BORDER)
            e->pos.y -= e->speed;
        else
            e->direction = KEY_RIGHT + (rand() % 4);

        if (barrier_collision(m, &e->pos))
        {
            e->pos.y += e->speed;
            e->direction = KEY_RIGHT + (rand() % 4);
        }
    }
    else if (e->direction == KEY_DOWN)
    {
        if (e->pos.y + e->pos.height < g->screenHeight - SCREEN_BORDER)
            e->pos.y += e->speed;
        else
            e->direction = KEY_RIGHT + (rand() % 4);

        if (barrier_collision(m, &e->pos))
        {
            e->pos.y -= e->speed;
            e->direction = KEY_RIGHT + (rand() % 4);
        }
    }
    if (rand() % 100 == 1) // 1% de chance do inimigo mudar de direcao
        e->direction = KEY_RIGHT + (rand() % 4);
}

int barrier_collision(Map *map, Rectangle *target)
{
    for (int i = 0; i < map->num_barriers; i++)
    {
        if (CheckCollisionRecs(*target, map->barriers[i]))
        {
            return 1;
        }
    }
    return 0;
}

// Maps Setup
void map0_setup(Game *g)
{
    g->maps[0].num_barriers = 1;
    g->maps[0].barriers[0] = (Rectangle){g->screenWidth / 2, 0, 2, 0.8 * g->screenHeight};
    g->maps[0].color = GRAY;
    g->maps[0].door = (Rectangle){g->screenWidth - (SCREEN_BORDER + 5), g->screenHeight / 3, SCREEN_BORDER, 50};
    g->maps[0].num_enemies = 2;
    g->maps[0].door_locked = 1;

    for (int i = 0; i < g->maps[0].num_enemies; i++)
    {
        g->maps[0].enemies[i].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[0].enemies[i].color = BLACK;
        g->maps[0].enemies[i].speed = 6;
        g->maps[0].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[0].enemies[i].draw_enemy = 1;
        g->maps[0].enemies[i].has_key = 0;
    }
    g->maps[0].enemies[0].has_key = 1;
    g->maps[0].special_item = (Rectangle){2 * g->screenWidth / 3, 20, 15, 15};
    g->maps[0].draw_special_item = 1;
    g->maps[0].prev_map = -1;
    g->maps[0].next_map = 1;
}

void map1_setup(Game *g)
{
    g->maps[1].num_barriers = 2;
    g->maps[1].barriers[0] = (Rectangle){3 * g->screenWidth / 4, 0, 2, 0.6 * g->screenHeight};
    g->maps[1].barriers[1] = (Rectangle){g->screenWidth / 4, 0.4 * g->screenHeight, 2, g->screenHeight};
    g->maps[1].color = GRAY;
    g->maps[1].door = (Rectangle){g->screenWidth - (SCREEN_BORDER + 5), g->screenHeight / 3, SCREEN_BORDER, 50};
    g->maps[1].prev_door = (Rectangle){SCREEN_BORDER, g->screenHeight / 3, 5, 50};
    g->maps[1].num_enemies = 3;
    g->maps[1].door_locked = 1;

    for (int i = 0; i < g->maps[1].num_enemies; i++)
    {
        g->maps[1].enemies[i].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[1].enemies[i].color = BLACK;
        g->maps[1].enemies[i].speed = 6;
        g->maps[1].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[1].enemies[i].draw_enemy = 1;
        g->maps[1].enemies[i].has_key = 0;
    }
    g->maps[1].enemies[0].has_key = 1;
    g->maps[1].special_item = (Rectangle){4 * g->screenWidth / 5, 20, 15, 15};
    g->maps[1].draw_special_item = 1;
    g->maps[1].prev_map = 0;
    g->maps[1].next_map = 2;
}

void map2_setup(Game *g)
{
    g->maps[2].num_barriers = 3;
    g->maps[2].barriers[0] = (Rectangle){g->screenWidth / 4, 0, 2, 0.6 * g->screenHeight};
    g->maps[2].barriers[1] = (Rectangle){2 * g->screenWidth / 4, 0, 2, 0.6 * g->screenHeight};
    g->maps[2].barriers[2] = (Rectangle){3 * g->screenWidth / 4, 0, 2, 0.6 * g->screenHeight};
    g->maps[2].color = GRAY;
    g->maps[2].door = (Rectangle){g->screenWidth - (SCREEN_BORDER + 5), g->screenHeight / 3, SCREEN_BORDER, 50};
    g->maps[2].prev_door = (Rectangle){SCREEN_BORDER, g->screenHeight / 3, 5, 50};
    g->maps[2].num_enemies = 4;
    g->maps[2].door_locked = 1;

    for (int i = 0; i < g->maps[2].num_enemies; i++)
    {
        g->maps[2].enemies[i].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[2].enemies[i].color = BLACK;
        g->maps[2].enemies[i].speed = 6;
        g->maps[2].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[2].enemies[i].draw_enemy = 1;
        g->maps[2].enemies[i].has_key = 0;
    }
    g->maps[2].enemies[0].has_key = 1;
    g->maps[2].special_item = (Rectangle){4 * g->screenWidth / 5, 20, 15, 15};
    g->maps[2].draw_special_item = 1;
    g->maps[2].prev_map = 1;
    g->maps[2].next_map = 3;
}
