#ifndef TIRO_H
#define TIRO_H

#include "coisas.h"
#include "raylib.h"

void movimenta_tiro(Tiro *tiro);
void desenha_tiro(Tiro *tiro);
void jogador_atira(Player* jogador);
void atualiza_tiro(Tiro* tiro, Map* map, int largura, int altura);
void colide_inimigo_tiro(Tiro* tiro, Map* map);

#endif
