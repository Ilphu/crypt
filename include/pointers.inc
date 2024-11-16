; @file pointers.inc
; @authors Jack Kertscher and Garrett Rhoads
; @brief Custom variable pointers
; @date 2024-11-02
if !def(POINTERS_INC)
def POINTERS_INC equ 1

include "include/animations.inc"
include "include/joypad.inc"
include "include/graphics.inc"
include "include/sprites.inc"

RSSET _RAM
; pointers start at the beginning of WRAM

; Game state flags
def rGSF                            rb 1

; Character animation values
def CHARACTER_FIRST_ANI_FRAME_HEAD  rb 1 
def CHARACTER_FIRST_ANI_FRAME_LEGS  rb 1 
def CHARACTER_LAST_ANI_FRAME_HEAD   rb 1
def CHARACTER_ANI_DELAY             rb 1
def CHARACTER_ANI_DELAY_STATE       rb 1

; Character trait values
def CHARACTER_SPRITE_WORLD_X        rb 1
def CHARACTER_SPRITE_WORLD_Y        rb 1
def CHARACTER_SPRITE_ANIMATION      rb 1
def PREV_CHARACTER_SPRITE_ANIMATION rb 1
def CHARACTER_MOVEMENT_DELAY        rb 1
def CHARACTER_MOVEMENT_DELAY_STATE  rb 1
def CHARACTER_STAMINA_BAR           rb 1
def CHARACTER_CAN_SPRINT            rb 1
def SPRINT_RECHARGE_DELAY_STATE     rb 1
def CHARACTER_ANIM_CHANGED          rb 1
def CHARACTER_COLLISION_FLAGS       rb 1
def CHAR_DEATH_ANIM_STATE           rb 1
def CHAR_DEATH_NUM_FLASHES_STATE    rb 1

; enemy data chunks
def ENEMY1_DATA                     rb 17
def ENEMY2_DATA                     rb 17
def ENEMY3_DATA                     rb 17
def ENEMY4_DATA                     rb 17
def SPIRIT_DATA                     rb 17

; Timer values
def CURRENT_TIMER_TENS_VALUE        rb 1
def CURRENT_TIMER_ONES_VALUE        rb 1
def TIMER_DELAY                     rb 1
def TIMER_DELAY_STATE               rb 1

; Joypad values
def JOYPAD_CURRENT                  rb 1
def JOYPAD_PREVIOUS                 rb 1
def JOYPAD_PRESSED                  rb 1
def JOYPAD_RELEASED                 rb 1

endc