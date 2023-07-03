#include "inimigos.h"
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "coisas.h"
#include "tiro.h"

void GamePlay(Game* g) {
    while (!WindowShouldClose())
    {
        UpdateDrawFrame(g);
        if (g->gameover) {
            break;
        }

    }
}

void GameOver(Game* g) {
    while (!IsKeyDown(KEY_ENTER) && !WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("GAME OVER", GetScreenWidth() / 2 - MeasureText("GAME OVER", 20) / 2, GetScreenHeight() / 2 - 50, 20, BLACK);
        EndDrawing();
    }

}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    Game game;
    game.screenWidth = 1000;
    game.screenHeight = 600;

    InitWindow(game.screenWidth, game.screenHeight, "o jogo");
    SetTargetFPS(60);

    InitGame(&game);

    GamePlay(&game);
    GameOver(&game);
    
    return 0;
}
//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(Game *g)
{
    g->sprite_inimigo = LoadTexture("assets/inimigo.png");
    g->sprite_player = LoadTexture("assets/nave.png");
    g->curr_map = 0;
    g->num_maps = 10;
    // Se o jogador nao tiver se mexido antes, atira pra direita
    g->player.direcaox = 1;
    g->player.direcaoy = 0;
    g->player.pos = (Rectangle){150, 300, STD_SIZE_X, STD_SIZE_Y};
    g->player.color = BLUE;
    g->player.speed = 6;
    g->player.special = 0;
    g->player.numero_tiros = 2;
    g->player.tiros[0].existe = false;
    g->player.tiros[0].color = RED;
    g->player.tiros[1].existe = false;
    g->player.tiros[0].color = RED;
    g->gameover = 0;
    map0_setup(g);
    map1_setup(g);
    map2_setup(g);
    map3_setup(g);
    map4_setup(g);
    map5_setup(g);
    map6_setup(g);
    map7_setup(g);
}

// Mata o inimigo, e se ele tiver a chave, abre a porta
void ataca_inimigo(Enemy* inimigo, Map* map) {
    if(inimigo->armadura > 0) {
        inimigo->armadura--;
        return;
    }
    inimigo->draw_enemy = 0;
    if (inimigo->has_key)
    {
        map->door_locked = 0;
    }
}

// Update game (one frame)
void UpdateGame(Game *g)
{
    update_hero_pos(g);
    jogador_atira(&g->player);
    Map *map = &g->maps[g->curr_map];
    for(int i = 0; i < g->player.numero_tiros; i++) {
        movimenta_tiro(&g->player.tiros[i]);
        atualiza_tiro(&g->player.tiros[i], map, g->screenWidth, g->screenHeight);
        colide_inimigo_tiro(&g->player.tiros[i], map);
    }



    for (int i; i < map->num_enemies; i++)
    {
        if (!map->enemies[i].draw_enemy)
            continue;
        update_enemy_pos(g, &map->enemies[i]);
        if (!CheckCollisionRecs(g->player.pos, map->enemies[i].pos))
            continue;

        // Se o player tiver comido a bolinha, mata o inimigo
        if (g->player.special)
        {
            ataca_inimigo(&map->enemies[i], map);
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
    draw_map(g);
    draw_borders(g);
    for(int i = 0; i < g->player.numero_tiros; i++) {
        desenha_tiro(&g->player.tiros[i]);
    }
    // Desenha o sprite do jogador proporcionalmente ao tamanho do retangulo de pos,
    // multiplicado por 1.5 pra encaixar melhor no tamanho do player
    DrawTextureEx(g->sprite_player, (Vector2){g->player.pos.x, g->player.pos.y}, 0.0, g->player.pos.width / 506.0 , WHITE);

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
    DrawRectangle(0, 0, g->screenWidth, g->screenHeight, map->color);
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

    // Desenha os inimigos do mapa
    for (int i = 0; i < map->num_enemies; i++)
    {
        Enemy* ini = &map->enemies[i];
        if (!ini->draw_enemy)
            continue;
        // Armadura
        if (ini->armadura > 0) {
            int grossura = 8*ini->armadura;
            Rectangle armadura_pos = ini->pos;
            armadura_pos.height += grossura*2;
            armadura_pos.width += grossura*2;
            armadura_pos.x -= grossura;
            armadura_pos.y -= grossura;
            Color cor_inimigo = ini->color;
            cor_inimigo.a = 100;
            DrawRectangleRounded(armadura_pos, 4, 5, cor_inimigo);
        }
        DrawTextureEx(g->sprite_inimigo, (Vector2){ini->pos.x, ini->pos.y}, 0, ini->pos.width/250.0, ini->color);
    }

    if (map->draw_special_item)
        DrawRectangleRec(map->special_item, PURPLE);
}

// Movimenta o jogador de acordo com a tecla pressionada
void update_hero_pos(Game *g)
{

    Player *p = &g->player;
    Map *m = &g->maps[g->curr_map];

    // Direcao 0 significa que nao esta se mexendo nessa direcao
    int direcaox = 0, direcaoy = 0;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        direcaox = -1;
    }
    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        {
        direcaox = 1;
    }
    else if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        {
        direcaoy = -1;
    }
    else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
    {
        direcaoy = 1;
    }
    // Não atualiza as ultimas direcoes se o jogador estiver parado
    // Precisamos disso para o tiro nao ficar parado no lugar
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

// retorna 1 se o retangulo estiver encostando em alguma parede do mapa.
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

// Criação de mapas
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
        g->maps[0].enemies[i] = inimigo_comum();
        g->maps[0].enemies[i].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[0].enemies[i].direction = KEY_RIGHT + (rand() % 4);
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
        g->maps[1].enemies[i] = inimigo_comum();
        g->maps[1].enemies[i].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[1].enemies[i].direction = KEY_RIGHT + (rand() % 4);
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
        g->maps[2].enemies[i] = inimigo_comum();
        g->maps[2].enemies[i].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[2].enemies[i].direction = KEY_RIGHT + (rand() % 4);
    }
    g->maps[2].enemies[0].has_key = 1;
    g->maps[2].special_item = (Rectangle){4 * g->screenWidth / 5, 20, 15, 15};
    g->maps[2].draw_special_item = 1;
    g->maps[2].prev_map = 1;
    g->maps[2].next_map = 3;
}

void map3_setup(Game *g)
{
    Map* mapa = &g->maps[3];
    mapa->num_barriers = 2;
    // Barra vertical da cruz
    mapa->barriers[0] = (Rectangle){.x = g->screenWidth / 2, .height = g->screenHeight/2, .width = 2, .y = g->screenHeight/4};
    // Barra horizontal da cruz
    mapa->barriers[1] = (Rectangle){.y = g->screenHeight / 2, .width = g->screenWidth/2, .height = 2, .x = g->screenWidth/4};
    mapa->color = GRAY;
    mapa->door = (Rectangle){g->screenWidth - (SCREEN_BORDER + 5), g->screenHeight / 3, SCREEN_BORDER, 50};
    mapa->prev_door = (Rectangle){SCREEN_BORDER, g->screenHeight / 3, 5, 50};
    mapa->num_enemies = 4;
    mapa->door_locked = 1;

    for (int i = 0; i < mapa->num_enemies-1; i++)
    {
        mapa->enemies[i] = inimigo_comum();
        mapa->enemies[i].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
        mapa->enemies[i].direction = KEY_RIGHT + (rand() % 4);
    }
    // Inimigo mais dificil de matar
    mapa->enemies[mapa->num_enemies-1] = inimigo_dificil();
    mapa->enemies[mapa->num_enemies-1].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
    mapa->enemies[mapa->num_enemies-1].direction = KEY_RIGHT + (rand() % 4);
    mapa->enemies[mapa->num_enemies-1].has_key = 1;

    mapa->special_item = (Rectangle){4 * g->screenWidth / 5, 20, 15, 15};
    mapa->draw_special_item = 1;
    mapa->prev_map = 2;
    mapa->next_map = 4;
}

void map4_setup(Game *g)
{
    Map* mapa = &g->maps[4];
    mapa->num_barriers = 8;
    // comprimento vertical
    float cv = g->screenHeight/6.0;
    // comprimento horizontal
    float ch = g->screenWidth/6.0;
    // L superior esquerdo
    // Barra vertical
    mapa->barriers[0] = (Rectangle){.x = ch, .height = cv, .y = cv, .width=2 };
    // Barra horizontal
    mapa->barriers[1] = (Rectangle){.x = ch, .width = ch, .y = cv, .height=2 };

    // L superior direito
    // Barra vertical
    mapa->barriers[2] = (Rectangle){.x = g->screenWidth - ch, .height = cv, .y = cv, .width=2 };
    // Barra horizontal
    mapa->barriers[3] = (Rectangle){.x = g->screenWidth - 2*ch, .width = ch, .y = cv, .height=2 };

    // L inferior direito
    // Barra vertical
    mapa->barriers[4] = (Rectangle){.x = g->screenWidth - ch, .height = cv, .y = g->screenHeight - 2*cv, .width=2 };
    // Barra horizontal
    mapa->barriers[5] = (Rectangle){.x = g->screenWidth - 2*ch, .width = ch, .y = g->screenHeight - cv, .height=2 };

    // L inferior esquerdo
    // Barra vertical
    mapa->barriers[6] = (Rectangle){.x = ch, .height = cv, .y = g->screenHeight - 2*cv, .width=2 };
    // Barra horizontal
    mapa->barriers[7] = (Rectangle){.x = ch, .width = ch, .y = g->screenHeight - cv, .height=2 };

    mapa->color = GRAY;
    mapa->door = (Rectangle){g->screenWidth - (SCREEN_BORDER + 5), g->screenHeight / 3, SCREEN_BORDER, 50};
    mapa->prev_door = (Rectangle){SCREEN_BORDER, g->screenHeight / 3, 5, 50};
    mapa->num_enemies = 3;
    mapa->door_locked = 1;

    for (int i = 0; i < mapa->num_enemies-1; i++)
    {
        mapa->enemies[i] = inimigo_comum();
        mapa->enemies[i].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
        mapa->enemies[i].direction = KEY_RIGHT + (rand() % 4);
    }
    // Inimigo mais dificil de matar
    mapa->enemies[mapa->num_enemies-1] = inimigo_dificil();
    mapa->enemies[mapa->num_enemies-1].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
    mapa->enemies[mapa->num_enemies-1].direction = KEY_RIGHT + (rand() % 4);
    mapa->enemies[mapa->num_enemies-1].has_key = 1;

    mapa->special_item = (Rectangle){4 * g->screenWidth / 5, 20, 15, 15};
    mapa->draw_special_item = 1;
    mapa->prev_map = 4;
    mapa->next_map = 5;
}

void map5_setup(Game *g)
{
    Map* mapa = &g->maps[5];
    mapa->num_barriers = 8;
    // comprimento vertical
    float cv = g->screenHeight/6.0;
    // comprimento horizontal
    float ch = g->screenWidth/6.0;
    // L superior esquerdo
    // Barra vertical
    mapa->barriers[0] = (Rectangle){.x = 2*ch, .height = cv, .y = cv, .width=2 };
    // Barra horizontal
    mapa->barriers[1] = (Rectangle){.x = ch, .width = ch, .y = cv, .height=2 };

    // L superior direito
    // Barra vertical
    mapa->barriers[2] = (Rectangle){.x = g->screenWidth - 2*ch, .height = cv, .y = cv, .width=2 };
    // Barra horizontal
    mapa->barriers[3] = (Rectangle){.x = g->screenWidth - 2*ch, .width = ch, .y = cv, .height=2 };

    // L inferior direito
    // Barra vertical
    mapa->barriers[4] = (Rectangle){.x = g->screenWidth - 2*ch, .height = cv, .y = g->screenHeight - 2*cv, .width=2 };
    // Barra horizontal
    mapa->barriers[5] = (Rectangle){.x = g->screenWidth - 2*ch, .width = ch, .y = g->screenHeight - cv, .height=2 };

    // L inferior esquerdo
    // Barra vertical
    mapa->barriers[6] = (Rectangle){.x = 2*ch, .height = cv, .y = g->screenHeight - 2*cv, .width=2 };
    // Barra horizontal
    mapa->barriers[7] = (Rectangle){.x = ch, .width = ch, .y = g->screenHeight - cv, .height=2 };

    mapa->color = GRAY;
    mapa->door = (Rectangle){g->screenWidth - (SCREEN_BORDER + 5), g->screenHeight / 3, SCREEN_BORDER, 50};
    mapa->prev_door = (Rectangle){SCREEN_BORDER, g->screenHeight / 3, 5, 50};
    mapa->num_enemies = 3;
    mapa->door_locked = 1;

    for (int i = 0; i < mapa->num_enemies-1; i++)
    {
        mapa->enemies[i] = inimigo_comum();
        mapa->enemies[i].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
        mapa->enemies[i].direction = KEY_RIGHT + (rand() % 4);
    }
    // Inimigo encouracado
    mapa->enemies[mapa->num_enemies-1] = inimigo_encouracado(1);
    mapa->enemies[mapa->num_enemies-1].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
    mapa->enemies[mapa->num_enemies-1].direction = KEY_RIGHT + (rand() % 4);
    mapa->enemies[mapa->num_enemies-1].has_key = 1;

    mapa->special_item = (Rectangle){4 * g->screenWidth / 5, 20, 15, 15};
    mapa->draw_special_item = 1;
    mapa->prev_map = 5;
    mapa->next_map = 6;
}

void map6_setup(Game *g)
{
    Map* mapa = &g->maps[6];
    mapa->num_barriers = 8;
    // comprimento vertical
    float cv = g->screenHeight/6.0;
    // comprimento horizontal
    float ch = g->screenWidth/6.0;
    // Primeiro L
    // Barra vertical
    mapa->barriers[0] = (Rectangle){.x = 2*ch, .height = 2*cv, .y = g->screenHeight/2, .width=2 };
    // Barra horizontal
    mapa->barriers[1] = (Rectangle){.x = ch, .width = ch, .y = cv, .height=2 };

    // L superior direito
    // Barra vertical
    mapa->barriers[2] = (Rectangle){.x = g->screenWidth - 2*ch, .height = cv, .y = cv, .width=2 };
    // Barra horizontal
    mapa->barriers[3] = (Rectangle){.x = g->screenWidth - 2*ch, .width = ch, .y = cv, .height=2 };

    // L inferior direito
    // Barra vertical
    mapa->barriers[4] = (Rectangle){.x = g->screenWidth - 2*ch, .height = cv, .y = g->screenHeight - 2*cv, .width=2 };
    // Barra horizontal
    mapa->barriers[5] = (Rectangle){.x = g->screenWidth - 2*ch, .width = ch, .y = g->screenHeight - cv, .height=2 };

    // L inferior esquerdo
    // Barra vertical
    mapa->barriers[6] = (Rectangle){.x = 2*ch, .height = cv, .y = g->screenHeight - 2*cv, .width=2 };
    // Barra horizontal
    mapa->barriers[7] = (Rectangle){.x = ch, .width = ch, .y = g->screenHeight - cv, .height=2 };

    mapa->color = GRAY;
    mapa->door = (Rectangle){g->screenWidth - (SCREEN_BORDER + 5), g->screenHeight / 3, SCREEN_BORDER, 50};
    mapa->prev_door = (Rectangle){SCREEN_BORDER, g->screenHeight / 3, 5, 50};
    mapa->num_enemies = 3;
    mapa->door_locked = 1;

    for (int i = 0; i < mapa->num_enemies-1; i++)
    {
        mapa->enemies[i] = inimigo_dificil();
        mapa->enemies[i].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
        mapa->enemies[i].direction = KEY_RIGHT + (rand() % 4);
    }
    // Inimigo encouracado
    mapa->enemies[mapa->num_enemies-1] = inimigo_encouracado(1);
    mapa->enemies[mapa->num_enemies-1].pos = (Rectangle){2 * g->screenWidth / 3, 2 * g->screenHeight / 3, STD_SIZE_X, STD_SIZE_Y};
    mapa->enemies[mapa->num_enemies-1].direction = KEY_RIGHT + (rand() % 4);
    mapa->enemies[mapa->num_enemies-1].has_key = 1;

    mapa->special_item = (Rectangle){4 * g->screenWidth / 5, 20, 15, 15};
    mapa->draw_special_item = 1;
    mapa->prev_map = 5;
    mapa->next_map = 6;
}

void map7_setup(Game *g) {}
