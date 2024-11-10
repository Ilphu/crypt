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

macro copy
    ld a, \2
    ld \1, a
endm

macro vBit
    ld c, \1
    inc c
    ld a, \2

    .count_down\@
        srl a
        dec c
        jr nz, .count_down\@
endm
endc

