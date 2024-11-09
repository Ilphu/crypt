; @file game.asm
; @authors Garrett Rhoads, Jack Kertscher, Darren Strash, and Maximilien Dagois
; @breif Crypt: Game loop code
; @date 2024-11-02

include "include/hardware.inc"
include "include/utils.inc"
include "include/pointers.inc"
include "include/graphics.inc"
include "include/sprites.inc"
include "include/animations.inc"
include "include/printing.inc"
include "include/joypad.inc"
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

section "vblank_interrupt", rom0[$0040]
    reti

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

section "game", rom0

; Initialize the palette, vblank interrupt, window, and LCD.
init_game:
    InitGraphics
    ret

; Update the everything needed for the game loop
update_game:
    UpdateJoypad
    call update_character_data
    call update_enemies_data
    halt
    
    UpdateTimer
    call update_character_visuals
    call update_enemies_visuals

    ;call update_enemies
    call update_window
    ret

export init_game, update_game

