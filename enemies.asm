; @file enemies.asm
; @authors Jack Kertscher and Garrett Rhoads
; @brief Controls the enemies
; @date 2024-11-02

include "include/hardware.inc"
include "include/utils.inc"
include "include/pointers.inc"
include "include/sprites.inc"
include "include/joypad.inc"
include "include/enemy.inc"
include "include/spirit.inc"

section "enemy", rom0

update_enemies_data:
    UpdateEnemyData ENEMY1_DATA, ENEMY1_SPRITE_HEAD, ENEMY1_SPRITE_LEGS, ENEMY2_DATA, ENEMY3_DATA, ENEMY4_DATA

    ld a, [rGSF]
    bit rGSB_LVL2, a
    jp z, .no_update_enemy_2
        UpdateEnemyData ENEMY2_DATA, ENEMY2_SPRITE_HEAD, ENEMY2_SPRITE_LEGS, ENEMY1_DATA, ENEMY3_DATA, ENEMY4_DATA
    .no_update_enemy_2
    ld a, [rGSF]
    bit rGSB_LVL3, a
    jp z, .no_update_enemy_3
        UpdateEnemyData ENEMY2_DATA, ENEMY2_SPRITE_HEAD, ENEMY2_SPRITE_LEGS, ENEMY1_DATA, ENEMY3_DATA, ENEMY4_DATA
        UpdateEnemyData ENEMY3_DATA, ENEMY3_SPRITE_HEAD, ENEMY3_SPRITE_LEGS, ENEMY1_DATA, ENEMY2_DATA, ENEMY4_DATA
        UpdateEnemyData ENEMY4_DATA, ENEMY4_SPRITE_HEAD, ENEMY4_SPRITE_LEGS, ENEMY1_DATA, ENEMY2_DATA, ENEMY3_DATA
    .no_update_enemy_3
    ret

update_enemies_visuals:
    AnimateEnemy ENEMY1_DATA, ENEMY1_SPRITE_HEAD, ENEMY1_SPRITE_LEGS
    UpdateSpritePos16 ENEMY1_SPRITE_HEAD, ENEMY1_SPRITE_LEGS, ENEMY1_DATA + ENEMY_SPRITE_WORLD_X, ENEMY1_DATA + ENEMY_SPRITE_WORLD_Y
    ld a, [rGSF]
    bit rGSB_LVL2, a
    jp z, .no_update_enemy_2
        AnimateEnemy ENEMY2_DATA, ENEMY2_SPRITE_HEAD, ENEMY2_SPRITE_LEGS
        UpdateSpritePos16 ENEMY2_SPRITE_HEAD, ENEMY2_SPRITE_LEGS, ENEMY2_DATA + ENEMY_SPRITE_WORLD_X, ENEMY2_DATA + ENEMY_SPRITE_WORLD_Y
    .no_update_enemy_2
    
    ld a, [rGSF]
    bit rGSB_LVL3, a
    jp z, .no_update_enemy_3
        AnimateEnemy ENEMY2_DATA, ENEMY2_SPRITE_HEAD, ENEMY2_SPRITE_LEGS
        UpdateSpritePos16 ENEMY2_SPRITE_HEAD, ENEMY2_SPRITE_LEGS, ENEMY2_DATA + ENEMY_SPRITE_WORLD_X, ENEMY2_DATA + ENEMY_SPRITE_WORLD_Y

        AnimateEnemy ENEMY3_DATA, ENEMY3_SPRITE_HEAD, ENEMY3_SPRITE_LEGS
        UpdateSpritePos16 ENEMY3_SPRITE_HEAD, ENEMY3_SPRITE_LEGS, ENEMY3_DATA + ENEMY_SPRITE_WORLD_X, ENEMY3_DATA + ENEMY_SPRITE_WORLD_Y

        AnimateEnemy ENEMY4_DATA, ENEMY4_SPRITE_HEAD, ENEMY4_SPRITE_LEGS
        UpdateSpritePos16 ENEMY4_SPRITE_HEAD, ENEMY4_SPRITE_LEGS, ENEMY4_DATA + ENEMY_SPRITE_WORLD_X, ENEMY4_DATA + ENEMY_SPRITE_WORLD_Y
    .no_update_enemy_3
    ret

export update_enemies_data, update_enemies_visuals