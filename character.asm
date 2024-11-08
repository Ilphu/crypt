; @file character.asm
; @authors Jack Kertscher and Garrett Rhoads
; @brief Controls the character
; @date 2024-11-02

include "include/hardware.inc"
include "include/utils.inc"
include "include/pointers.inc"
include "include/sprites.inc"
include "include/joypad.inc"
include "include/character.inc"

section "character", rom0

update_character_data:
    UpdateCharacterMovement
    UpdateAnimationState
    ret

; update the characters movement, animations and positions
update_character_visuals:
    UpdateSpritePos16 CHARACTER_SPRITE_HEAD, CHARACTER_SPRITE_LEGS, CHARACTER_SPRITE_WORLD_X, CHARACTER_SPRITE_WORLD_Y
    AnimateCharacter
    ret

export update_character_visuals, update_character_data