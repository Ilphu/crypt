; Crypt: Main file
; @file main.asm
; @authors Darren Strash, Maximilien Dagois, Jack Kertscher, and Garrett Rhoads

include "include/hardware.inc"
include "include/utils.inc"
include "include/pointers.inc"
include "include/graphics.inc"
include "include/sprites.inc"
include "include/animations.inc"
include "include/printing.inc"
include "include/joypad.inc"
include "include/main.inc"
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

section "header", rom0[$0100]
entrypoint:
    di
    jp main
    ds ($0150 - @), 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Initializes the sample, then runs the sample update loop.
section "main", rom0
main:
    copy [rGSF], rGSF_TITLE
    .main_title
        DisableLCD
        call init_title_screen
        TitleLoop
    .lvl_1
        DisableLCD
        call init_game
        Lvl1Loop
    ld a, [rGSF]
    bit rGSB_TITLE, a
    jp nz, .main_title
    .lvl_2_title
        DisableLCD
        call init_title_screen
        Lvl2TitleLoop
    .lvl_2
        halt
        call init_game
        Lvl2Loop
    ld a, [rGSF]
    bit rGSB_TITLE, a
    jp nz, .main_title
    .lvl_3_title
        DisableLCD
        call init_title_screen
        Lvl2TitleLoop
    .lvl_3
        halt
        call init_game
        Lvl2Loop
    jp .main_title
    
        