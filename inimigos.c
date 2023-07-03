#include "inimigos.h"

// Inicializa algumas propriedades do inimigo
void zerar_inimigo(Enemy* en) {
    en->draw_enemy = 1;
    en->has_key = 0;
    for(int i = 0; i < 2; i++) {
        en->tiros[i].existe = false;
    }
    en->armadura = 0;
} 

Enemy inimigo_comum() {
    Enemy e = {.speed = 6,
        .color = ORANGE,
        .pos = (Rectangle){
            .width = STD_SIZE_X,
            .height = STD_SIZE_Y},
        };
    zerar_inimigo(&e);
    return e;
}

Enemy inimigo_encouracado(int armadura) {
    Enemy e = {.speed = 4,
        .color = PURPLE,
        .pos = (Rectangle){
            .width = STD_SIZE_X,
            .height = STD_SIZE_Y},
        };
    zerar_inimigo(&e);
    e.armadura = armadura;
    return e;
}

Enemy inimigo_dificil() {
    Enemy e = {.speed = 10,
        .color = GREEN,
        .pos = (Rectangle){
            .width = STD_SIZE_X*0.8,
            .height = STD_SIZE_Y*0.8},
        };
    zerar_inimigo(&e);
    return e;
}
