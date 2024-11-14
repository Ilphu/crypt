; @file utils.inc
; @authors Jack Kertscher and Maximilien Dagois
; @brief copy macro
; @date 2024-11-02

if !def(UTILS_RGBINC)
def UTILS_RGBINC equ 1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include "include/hardware.inc"
include "include/pointers.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

def INITIAL_DELAY                   equ ($00)
def ALL_BITS_FLIP                   equ ($FF)

; Find the absolute difference between (a) and (b) and store it in (a)
macro SubAbsolute
    cp a, b
    jr c, .less_than_a\@
        ; a >= b
        sub b 
        jr .end\@
    .less_than_a\@
        ld c, a
        ld a, b
        ld b, c
        sub b
    .end\@
endm

; Swaps (a) and (b) through (c)
macro SwapAB
    ld c, a
    ld a, b
    ld b, c
endm

macro copy
    ld a, \2
    ld \1, a
endm

macro vBit
    ld b, a
    ld a, [hl]
    inc b
    .count_down\@
        srl a
        dec b
        jr nz, .count_down\@
endm
endc

