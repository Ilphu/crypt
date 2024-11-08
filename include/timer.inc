; @file timer.inc
; @author Jack Kertscher
; @brief Timer logic
; @date 2024-11-02

if !def(TIMER_INC)
def TIMER_INC equ 1

; timer constants
def INIT_TIMER_DELAY        equ ($3F)
def TIMER_TO_TILE_OFFSET    equ ($30)
def ONES_RESET_VALUE        equ ($09)
def TIMER_ROLLOVER_VLAUE    equ ($FF)

def INIT_TIMER_ONES         equ ($00)
def INIT_TIMER_TENS         equ ($03)

; initializing the timer sprites
macro InitTimer
    copy [CURRENT_TIMER_TENS_VALUE], INIT_TIMER_TENS
    copy [CURRENT_TIMER_ONES_VALUE], INIT_TIMER_ONES
    copy [TIMER_DELAY], INIT_TIMER_DELAY
    copy [TIMER_DELAY_STATE], INITIAL_DELAY

    copy [TIMER_SPRITE_TENS + OAMA_X], TIMER_SPRITE_TENS_X_START
    copy [TIMER_SPRITE_TENS + OAMA_Y], TIMER_SPRITE_TENS_Y_START
    copy [TIMER_SPRITE_TENS + OAMA_TILEID], INIT_TIMER_TENS + TIMER_TO_TILE_OFFSET
    copy [TIMER_SPRITE_TENS + OAMA_FLAGS], OAMF_PAL0

    copy [TIMER_SPRITE_ONES + OAMA_X], TIMER_SPRITE_ONES_X_START
    copy [TIMER_SPRITE_ONES + OAMA_Y], TIMER_SPRITE_ONES_Y_START
    copy [TIMER_SPRITE_ONES + OAMA_TILEID], INIT_TIMER_ONES + TIMER_TO_TILE_OFFSET
    copy [TIMER_SPRITE_ONES + OAMA_FLAGS], OAMF_PAL0
endm 

; displaying the current number
macro DisplayTimerValue
    ld a, [CURRENT_TIMER_ONES_VALUE]
    add TIMER_TO_TILE_OFFSET
    ld [TIMER_SPRITE_ONES + OAMA_TILEID], a

    ld a, [CURRENT_TIMER_TENS_VALUE]
    add TIMER_TO_TILE_OFFSET
    ld [TIMER_SPRITE_TENS + OAMA_TILEID], a
endm

; updating the timer to count down, raise proper flag when over
macro UpdateTimer
    ; Check if the timer updates this frame
    ld a, [TIMER_DELAY]
    ld b, a
    ld a, [TIMER_DELAY_STATE]
    inc a
    and a, b
    ld [TIMER_DELAY_STATE], a
    jp nz, .done_updating_timer
    ; Update the ones place
    ld a, [CURRENT_TIMER_ONES_VALUE]
    dec a
    ld [CURRENT_TIMER_ONES_VALUE], a
    cp a, TIMER_ROLLOVER_VLAUE
    jp nz, .display_timer
    ; Update the tens place if needed
    copy  [CURRENT_TIMER_ONES_VALUE], ONES_RESET_VALUE
    ld a, [CURRENT_TIMER_TENS_VALUE]
    dec a
    ld [CURRENT_TIMER_TENS_VALUE], a
    cp a, TIMER_ROLLOVER_VLAUE
    jp nz, .display_timer
    ; If the timer is over, raise the appropriate flag
    ld a, [rGSF]
    bit rGSB_LVL2, a
    jr z, .next_lvl
    copy [rGSF], rGSF_TITLE
    jp .display_timer
    .next_lvl
        copy [rGSF], rGSF_LVL2
    .display_timer
        DisplayTimerValue
    .done_updating_timer
endm

endc