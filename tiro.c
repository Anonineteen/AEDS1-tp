#include "coisas.h"
#include "tiro.h"


void movimenta_tiro(Tiro *tiro) {
    if(!tiro->existe) return;
    tiro->pos.x += tiro->direcaox * tiro->speed;
    tiro->pos.y += tiro->direcaoy * tiro->speed;
}

void desenha_tiro(Tiro *tiro) {
    if(!tiro->existe) return;
    DrawRectangleRec(tiro->pos, tiro->color);
}

void atualiza_tiro(Tiro* tiro, Map* map, int largura, int altura) {
    if(!tiro->existe) return;
    if(tiro->pos.x > largura || tiro->pos.x < 0 ||
       tiro->pos.y > altura || tiro->pos.y < 0 ||
        barrier_collision(map, &tiro->pos)) {
        tiro->existe = false;
    }
}

void colide_inimigo_tiro(Tiro* tiro, Map* map) {
    if(!tiro->existe) return;
    for(int i = 0; i < map->num_enemies; i++) {
        Enemy* enemy = &map->enemies[i];
        if(enemy->draw_enemy) {
            if(CheckCollisionRecs(enemy->pos, tiro->pos)) {
                mata_inimigo(enemy, map);
                tiro->existe = false;
                return;
            }
        }
    }
}

void jogador_atira(Player* player){
    if(IsKeyPressed(KEY_SPACE) && !player->tiro.existe){
        // Cria um novo tiro na mesma posição do jogador,
        // com duas vezes a velocidade do jogador e na mesma direção do jogador
        Tiro novo_tiro;
        novo_tiro.pos.x = player->pos.x;
        novo_tiro.pos.y = player->pos.y;
        novo_tiro.pos.width = 10;
        novo_tiro.pos.height = 10;
        novo_tiro.color = BLUE;
        novo_tiro.speed = player->speed * 1.5;
        novo_tiro.direcaox = player->direcaox;
        novo_tiro.direcaoy = player->direcaoy;
        if(player->direcaox == 0 && player->direcaoy == 0) {
            novo_tiro.direcaox = 1;
        }
        novo_tiro.existe = true;

        player->tiro = novo_tiro;
    }
}
