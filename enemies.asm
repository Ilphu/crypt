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

section "enemy", rom0

; updates the enemies' movement, animations and positions
update_enemies:
    UpdateEnemyMovement ENEMY1_DATA, ENEMY1_SPRITE_HEAD, ENEMY1_SPRITE_LEGS
    AnimateEnemy ENEMY1_DATA, ENEMY1_SPRITE_HEAD, ENEMY1_SPRITE_LEGS
    UpdateSpritePos16 ENEMY1_SPRITE_HEAD, ENEMY1_SPRITE_LEGS, ENEMY1_DATA + ENEMY_SPRITE_WORLD_X, ENEMY1_DATA + ENEMY_SPRITE_WORLD_Y

    ; only have a second enemy if we are on the second level
    ld a, [rGSF]
    bit rGSB_LVL2, a
    jp z, .no_update_enemy_2
        UpdateEnemyMovement ENEMY2_DATA, ENEMY2_SPRITE_HEAD, ENEMY2_SPRITE_LEGS
        AnimateEnemy ENEMY2_DATA, ENEMY2_SPRITE_HEAD, ENEMY2_SPRITE_LEGS
        UpdateSpritePos16 ENEMY2_SPRITE_HEAD, ENEMY2_SPRITE_LEGS, ENEMY2_DATA + ENEMY_SPRITE_WORLD_X, ENEMY2_DATA + ENEMY_SPRITE_WORLD_Y
    .no_update_enemy_2
    
    ret

export update_enemies