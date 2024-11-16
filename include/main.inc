; @file main.inc
; @authors Jack Kertscher and Garrett Rhoads
; @brief macros and constants for main game loop 
; @date 2024-11-02

if !def(MAIN_INC)
def MAIN_INC equ 1

include "include/hardware.inc"
include "include/utils.inc"
include "include/pointers.inc"
include "include/graphics.inc"
include "include/sprites.inc"
include "include/animations.inc"
include "include/printing.inc"
include "include/joypad.inc"

; game state constatns
def rGSF_TITLE          equ %00000001
def rGSF_LVL1           equ %00000010
def rGSF_LVL2           equ %00000100
def rGSF_LVL3           equ %00001000
def rGSF_END            equ %00010000
def rGSF_CHAR_DEATH     equ %10000000

def rGSB_TITLE          equ ($00)
def rGSB_LVL1           equ ($01)
def rGSB_LVL2           equ ($02)
def rGSB_LVL3           equ ($03)
def rGSB_END            equ ($04)
def rGSB_CHAR_DEATH     equ ($07)

; logic for the title 
macro TitleLoop
    .title_screen\@
        halt
        UpdateJoypad
        ld a, [JOYPAD_CURRENT]
        bit PADB_A, a
        jr nz, .title_screen\@
        copy [rGSF], rGSF_LVL1
endm

; logic for the first level
macro Lvl1Loop
    .lvl_1_loop\@
        call update_game
        ld a, [rGSF]
        bit rGSB_LVL1, a
        jr nz, .lvl_1_loop\@
endm

; logic for level 2 title
macro Lvl2TitleLoop
    .lvl_2_title_screen\@
        halt
        UpdateJoypad
        ld a, [JOYPAD_CURRENT]
        bit PADB_A, a
        jr nz, .lvl_2_title_screen\@
        copy [rGSF], rGSF_LVL2
endm

; logic for second level
macro Lvl2Loop
    .lvl_2_loop\@
        call update_game
        ld a, [rGSF]
        bit rGSB_LVL2, a
        jr nz, .lvl_2_loop\@
endm

macro Lvl3TitleLoop
    .lvl_3_title_screen\@
        halt
        UpdateJoypad
        ld a, [JOYPAD_CURRENT]
        bit PADB_A, a
        jr nz, .lvl_3_title_screen\@
        copy [rGSF], rGSF_LVL3
endm

; logic for second level
macro Lvl3Loop
    .lvl_3_loop\@
        call update_game
        ld a, [rGSF]
        bit rGSB_LVL3, a
        jr nz, .lvl_3_loop\@
endm

macro CharDeathAnimation
    .char_death_anim\@
        halt
        ld a, [CHAR_DEATH_ANIM_STATE]
        and a, CHAR_DEATH_ANIM_DELAY
        jr nz, .wait\@
            ld a, [CHARACTER_SPRITE_HEAD + OAMA_FLAGS]
            bit OAMB_PAL1, a
            jp z, .to_pal1\@
                copy [CHARACTER_SPRITE_HEAD + OAMA_FLAGS], OAMF_PAL0
                copy [CHARACTER_SPRITE_LEGS + OAMA_FLAGS], OAMF_PAL0
                jr .skip\@
            .to_pal1\@
                copy [CHARACTER_SPRITE_HEAD + OAMA_FLAGS], OAMF_PAL1
                copy [CHARACTER_SPRITE_LEGS + OAMA_FLAGS], OAMF_PAL1
            .skip\@
            ld a, [CHAR_DEATH_NUM_FLASHES_STATE]
            inc a
            ld [CHAR_DEATH_NUM_FLASHES_STATE], a
        .wait\@
        ld a, [CHAR_DEATH_ANIM_STATE]
        inc a
        ld [CHAR_DEATH_ANIM_STATE], a

        ld a, [CHAR_DEATH_NUM_FLASHES_STATE]
        cp a, CHAR_DEATH_NUM_FLASHES
        jp c, .char_death_anim\@
        copy [rGSF], rGSF_TITLE
endm

endc