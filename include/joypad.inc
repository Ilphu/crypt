; @file joypad.inc
; @authors Darren Strash, with minor edits from Garrett Rhoads and Jack Kertsher
; @brief Joypad logic
; @date 2024-11-02

if !def(JOYPAD_INC)
def JOYPAD_INC equ 1

include "include/pointers.inc"

def NO_BUTTONS_PRESSED          equ ($FF)
def JOYPADB_LEFT                equ (5)
def JOYPADB_RIGHT               equ (4)
def JOYPADB_A                   equ (0)
def ISOLATE_LEFT_RIGHT          equ (%00110000)
def ISOLATE_DPAD                equ ($F0)
def ISOLATE_POLLED_VALUES       equ ($0F)

macro InitJoypad
    copy [JOYPAD_CURRENT], NO_BUTTONS_PRESSED
    copy [JOYPAD_PREVIOUS], NO_BUTTONS_PRESSED
    copy [JOYPAD_PRESSED], NO_BUTTONS_PRESSED
    copy [JOYPAD_RELEASED], NO_BUTTONS_PRESSED
endm

macro UpdateJoypad

    ld a, [JOYPAD_CURRENT]
    ld [JOYPAD_PREVIOUS], a
    ld c, a ; (c) contains previous joypad

    ld a, P1F_GET_BTN ; that's %00010000
    ld [rP1], a

    ; wait!
    ld a, [rP1]
    ld a, [rP1]
    ld a, [rP1]
    ld a, [rP1]
    ld a, [rP1]

    ; read button poll result
    ld a, [rP1]

    ; save button result in b
    and ISOLATE_POLLED_VALUES
    ld b, a

    ld a, P1F_GET_DPAD
    ld [rP1], a

    ; wait
    ld a, [rP1]

    ; read dpad poll result
    ld a, [rP1]

    ; move dpad result and load button result into (a)
    and ISOLATE_POLLED_VALUES
    swap a
    or a, b

    ; store in our custom joypad byte
    ld [JOYPAD_CURRENT], a
    ld b, a ; (b) contains current joypad byte

    ; update pressed
    ; for each bit: if 1 before, 0 now then it was pressed, set to 0

    ; load previous joypad byte (from (c))
    ld a, c
    cpl ; now previous holds are 1's
    or b ; or with current joypad byte
    
    ; now any that were previous held or currently not held are 1's
    ; and all presses are 0's
    ld [JOYPAD_PRESSED], a

    ; update released
    ; for each bit, if 0 before, and 1 now, then it was released

    ; load current joypad byte (in (b))
    ld a, b
    cpl ; now current holds are 1's
    or c ; or previous joypad byte

    ; now any that were previous not hold and current hold are 1's
    ; and all releases are 0's
    ld [JOYPAD_RELEASED], a

    ld a, P1F_GET_NONE
    ld [rP1], a

endm

endc