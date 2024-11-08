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
def rGSF_END            equ %00001000

def rGSB_TITLE          equ ($00)
def rGSB_LVL1           equ ($01)
def rGSB_LVL2           equ ($02)
def rGSB_END            equ ($03)

; logic for the title 
macro TitleLoop
    .title_screen
        halt
        UpdateJoypad
        ld a, [JOYPAD_CURRENT]
        bit PADB_A, a
        jr nz, .title_screen
        copy [rGSF], rGSF_LVL1
endm

; logic for the first level
macro Lvl1Loop
    .lvl_1_loop
        call update_game
        ld a, [rGSF]
        bit rGSB_LVL1, a
        jr nz, .lvl_1_loop
endm

; logic for level 2 title
macro Lvl2TitleLoop
    .lvl_2_title_screen
        halt
        UpdateJoypad
        ld a, [JOYPAD_CURRENT]
        bit PADB_A, a
        jr nz, .lvl_2_title_screen
        copy [rGSF], rGSF_LVL2
endm

; logic for second level
macro Lvl2Loop
    .lvl_2_loop
        call update_game
        ld a, [rGSF]
        bit rGSB_LVL2, a
        jr nz, .lvl_2_loop
endm

endc