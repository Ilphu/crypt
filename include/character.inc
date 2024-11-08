; @file character.inc
; @authors Jack Kertscher and Garrett Rhoads
; @brief Character logic
; @date 2024-11-02

if !def(CHARACTER_INC)
def CHARACTER_INC equ 1

include "include/pointers.inc"
include "include/graphics.inc"
include "include/sprites.inc"
include "include/main.inc"

; character specific constants
def INIT_CHARACTER_ANIMATION_DELAY  equ ($0F)
def SPRINT_CHARACTER_ANI_DELAY      equ ($07)
def INIT_CHARACTER_MOVEMENT_DELAY   equ ($03)
def SPRINT_CHARACTER_MOVEMENT_DELAY equ ($01)
def FULL_CHARACTER_STAMINA_BAR      equ ($80)
def CHARACTER_SPRINT_DELAY          equ ($20)
def CHARACTER_Y_MOVE_MIN            equ ($00)
def CHARACTER_Y_MOVE_MAX            equ ($F1)
def CHARACTER_X_MOVE_MIN            equ ($00)
def CHARACTER_X_MOVE_MAX            equ ($F8)

; Initializes the character
macro InitCharacter
    InitCharacterAnimation
    copy [CHARACTER_SPRITE_WORLD_X], CHARACTER_SPRITE_HEAD_X_START
    copy [CHARACTER_SPRITE_WORLD_Y], CHARACTER_SPRITE_HEAD_Y_START
    copy [CHARACTER_SPRITE_ANIMATION], IDLE_ANIMATION_FLAG

    copy [CHARACTER_MOVEMENT_DELAY], INIT_CHARACTER_MOVEMENT_DELAY
    copy [CHARACTER_MOVEMENT_DELAY_STATE], INITIAL_DELAY

    ; Init the head and legs of our character sprite
    copy [CHARACTER_SPRITE_HEAD + OAMA_X], CHARACTER_SPRITE_HEAD_X_START
    copy [CHARACTER_SPRITE_HEAD + OAMA_Y], CHARACTER_SPRITE_HEAD_Y_START
    copy [CHARACTER_SPRITE_HEAD + OAMA_TILEID], [CHARACTER_FIRST_ANI_FRAME_HEAD]
    copy [CHARACTER_SPRITE_HEAD + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP
    
    copy [CHARACTER_SPRITE_LEGS + OAMA_X], CHARACTER_SPRITE_LEGS_X_START
    copy [CHARACTER_SPRITE_LEGS + OAMA_Y], CHARACTER_SPRITE_LEGS_Y_START
    copy [CHARACTER_SPRITE_LEGS + OAMA_TILEID], [CHARACTER_FIRST_ANI_FRAME_LEGS]
    copy [CHARACTER_SPRITE_LEGS + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP

    ; initializes the stamina bar
    copy [CHARACTER_STAMINA_BAR], FULL_CHARACTER_STAMINA_BAR
    copy [CHARACTER_CAN_SPRINT], SPRINT_CHARACTER_MOVEMENT_DELAY
    copy [SPRINT_RECHARGE_DELAY_STATE], SPRINT_CHARACTER_MOVEMENT_DELAY
endm

; Initializes the character's  animations
macro InitCharacterAnimation
    copy [CHARACTER_FIRST_ANI_FRAME_HEAD],    (DOWN_IDLE_ANIMATION_START)
    copy [CHARACTER_FIRST_ANI_FRAME_LEGS],    (DOWN_IDLE_ANIMATION_START + LEGS_TILE_OFFSET)
    copy [CHARACTER_LAST_ANI_FRAME_HEAD],     (DOWN_IDLE_ANIMATION_END)
    copy [CHARACTER_ANI_DELAY],               (INIT_CHARACTER_ANIMATION_DELAY)
    copy [CHARACTER_ANI_DELAY_STATE],         (INITIAL_DELAY)
endm

; Updates the character in the x direction
macro UpdateCharacterX
    ; Move left
    ld a, [JOYPAD_CURRENT]
    bit PADB_LEFT, a
    jp nz, .not_left_move
        ld a, [CHARACTER_SPRITE_WORLD_X]
        cp a, CHARACTER_X_MOVE_MIN
        jp z, .not_left_move

        dec a
        ld [CHARACTER_SPRITE_WORLD_X], a
    .not_left_move
    ; Move right
    ld a, [JOYPAD_CURRENT]
    bit PADB_RIGHT, a
    jp nz, .not_right_move
        ld a, [CHARACTER_SPRITE_WORLD_X]
        cp a, CHARACTER_X_MOVE_MAX
        jp z, .not_right_move

        inc a
        ld [CHARACTER_SPRITE_WORLD_X], a
    .not_right_move
endm

; Updates the character in the y direction
macro UpdateCharacterY
    ; Move down
    ld a, [JOYPAD_CURRENT]
    bit PADB_DOWN, a
    jp nz, .not_down_move
        ld a, [CHARACTER_SPRITE_WORLD_Y]
        cp a, CHARACTER_Y_MOVE_MAX
        jp z, .not_down_move

        inc a
        ld [CHARACTER_SPRITE_WORLD_Y], a
    .not_down_move
    ; Move up
    ld a, [JOYPAD_CURRENT]
    bit PADB_UP, a
    jp nz, .not_up_move
        ld a, [CHARACTER_SPRITE_WORLD_Y]
        cp a, CHARACTER_Y_MOVE_MIN
        jp z, .not_up_move

        dec a
        ld [CHARACTER_SPRITE_WORLD_Y], a
    .not_up_move
endm

; Handles stamina logic for when the character is walking
macro CharacterWalkingStamina
    copy [CHARACTER_MOVEMENT_DELAY], INIT_CHARACTER_MOVEMENT_DELAY
    ; is stamina bar full?
    ld a, FULL_CHARACTER_STAMINA_BAR
    ld b, a
    ld a, [CHARACTER_STAMINA_BAR]
    cp a, b
    ; If not, recharge it
    jr nc, .end_sprint
        ld a, [SPRINT_RECHARGE_DELAY_STATE]
        inc a
        and INIT_CHARACTER_MOVEMENT_DELAY
        ld [SPRINT_RECHARGE_DELAY_STATE], a
        jr nz, .end_sprint
            ld a, [CHARACTER_STAMINA_BAR]
            inc a
            ld [CHARACTER_STAMINA_BAR], a
            ld b, a
            ld a, CHARACTER_SPRINT_DELAY
            cp a, b
            jr nc, .end_sprint
                copy [CHARACTER_CAN_SPRINT], SPRINT_CHARACTER_MOVEMENT_DELAY
endm

; Character sprinting logic
macro UpdateSprint
    ; If the character can sprint and (A) is being held, sprint
    ld a, [CHARACTER_CAN_SPRINT]
    bit JOYPADB_A, a
    jr z, .walk
            xor a
            ld b, a
            ld a, [CHARACTER_STAMINA_BAR]
            cp a, b
            jr z, .cant_sprint
                ld a, [JOYPAD_CURRENT]
                bit PADB_B, a
                jr nz, .walk
                    ; If the character is sprinting, drain stamina
                    ld a, [CHARACTER_STAMINA_BAR]
                    dec a
                    ld [CHARACTER_STAMINA_BAR], a
                    copy [CHARACTER_MOVEMENT_DELAY], SPRINT_CHARACTER_MOVEMENT_DELAY
                    jr .end_sprint
            .cant_sprint
                xor a
                ld [CHARACTER_CAN_SPRINT], a
        .walk
            CharacterWalkingStamina     
    .end_sprint
endm

; Sprinting sped up animations
macro UpdateSprintAnimation
    ld a, [CHARACTER_MOVEMENT_DELAY]
    ld b, a
    ld a, SPRINT_CHARACTER_MOVEMENT_DELAY
    cp a, b
    jr z, .sprint_ani
        copy [CHARACTER_ANI_DELAY], INIT_CHARACTER_ANIMATION_DELAY
        jr .end_update_sprint_animation
    .sprint_ani
        copy [CHARACTER_ANI_DELAY], SPRINT_CHARACTER_ANI_DELAY
    .end_update_sprint_animation
endm

; Over arching character movement logic
macro UpdateCharacterMovement
    ; Check if we're sprinting
    UpdateSprint
    UpdateSprintAnimation
    ; Check if the character is moving this frame
    ld a, [CHARACTER_MOVEMENT_DELAY]
    ld b, a
    ld a, [CHARACTER_MOVEMENT_DELAY_STATE]
    inc a
    and a, b
    ld [CHARACTER_MOVEMENT_DELAY_STATE], a
    ; If so, move the character
    jp nz, .end_movement
        UpdateCharacterY
        UpdateCharacterX
    .end_movement
endm

; Animates the character
macro AnimateCharacter
    ld a, [CHARACTER_ANIM_CHANGED]
    cp a, ANIM_CHANGED_FLAG
    jp z, .reset
    ; checks whether or not we want to animate on this v-blank
    ld a, [CHARACTER_ANI_DELAY]
    ld b, a
    ld a, [CHARACTER_ANI_DELAY_STATE]
    inc a
    and a, b
    ld [CHARACTER_ANI_DELAY_STATE], a
    jp nz, .end_animate
    ; if we want to animate, check if we need to reset to the begining of the animation
    ld a, [CHARACTER_LAST_ANI_FRAME_HEAD]
    ld b, a
    ld a, [CHARACTER_SPRITE_HEAD + OAMA_TILEID]
    cp a, b
    jp z, .reset
    ; move the head to the next animation
    inc a
    inc a
    ld [CHARACTER_SPRITE_HEAD + OAMA_TILEID], a
    ; move the legs to next animation
    ld a, [CHARACTER_SPRITE_LEGS + OAMA_TILEID]
    inc a
    inc a
    ld [CHARACTER_SPRITE_LEGS + OAMA_TILEID], a
    jp .end_animate
    ; if we needed to reset set the sprite back to their first animations in
    ; the sprite sheet
    .reset
        copy [CHARACTER_SPRITE_HEAD + OAMA_TILEID], [CHARACTER_FIRST_ANI_FRAME_HEAD]
        copy [CHARACTER_SPRITE_LEGS + OAMA_TILEID], [CHARACTER_FIRST_ANI_FRAME_LEGS]
        copy [CHARACTER_ANIM_CHANGED], ANIM_UNCHANGED_FLAG
    .end_animate
endm

; Updates the animations state registers
macro UpdateAnimationState
    copy [PREV_CHARACTER_SPRITE_ANIMATION], [CHARACTER_SPRITE_ANIMATION]
    ; Check if controls have changed
    ld a, [JOYPAD_PRESSED]
    ld b, a
    ld a, [JOYPAD_RELEASED]
    and b
    cp a, NO_BUTTONS_PRESSED
    jp z, .no_animation_update
    ld a, [JOYPAD_CURRENT]
    xor ALL_BITS_FLIP
    ; Check if we are moving in the y direction, prioritize that animation
    bit JOYPADB_LEFT, a
    jr nz, .moving_y
    bit JOYPADB_RIGHT, a
    jr z, .not_moving_y
    .moving_y
        and ISOLATE_LEFT_RIGHT
    .not_moving_y
        and ISOLATE_DPAD
        ld [CHARACTER_SPRITE_ANIMATION], a
    UpdateAnimationData
    .no_animation_update
endm

; Sets pointers for down animation
macro SetDownAnim
    copy [CHARACTER_FIRST_ANI_FRAME_HEAD],    (DOWN_ANIMATION_START)
    copy [CHARACTER_FIRST_ANI_FRAME_LEGS],    (DOWN_ANIMATION_START + LEGS_TILE_OFFSET)
    copy [CHARACTER_LAST_ANI_FRAME_HEAD],     (DOWN_ANIMATION_END)
endm

; Sets pointers for up animation
macro SetUpAnim
    copy [CHARACTER_FIRST_ANI_FRAME_HEAD],    (UP_ANIMATION_START)
    copy [CHARACTER_FIRST_ANI_FRAME_LEGS],    (UP_ANIMATION_START + LEGS_TILE_OFFSET)
    copy [CHARACTER_LAST_ANI_FRAME_HEAD],     (UP_ANIMATION_END)
endm

; Sets pointers for left animation
macro SetLeftAnim
    copy [CHARACTER_FIRST_ANI_FRAME_HEAD],    (SIDE_ANIMATION_START)
    copy [CHARACTER_FIRST_ANI_FRAME_LEGS],    (SIDE_ANIMATION_START + LEGS_TILE_OFFSET)
    copy [CHARACTER_LAST_ANI_FRAME_HEAD],     (SIDE_ANIMATION_END)
    copy [CHARACTER_SPRITE_HEAD + OAMA_FLAGS], OAMF_PAL0
    copy [CHARACTER_SPRITE_LEGS + OAMA_FLAGS], OAMF_PAL0
endm

; Sets pointers for right animation
macro SetRightAnim
    copy [CHARACTER_FIRST_ANI_FRAME_HEAD],    (SIDE_ANIMATION_START)
    copy [CHARACTER_FIRST_ANI_FRAME_LEGS],    (SIDE_ANIMATION_START + LEGS_TILE_OFFSET)
    copy [CHARACTER_LAST_ANI_FRAME_HEAD],     (SIDE_ANIMATION_END)
    copy [CHARACTER_SPRITE_HEAD + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP
    copy [CHARACTER_SPRITE_LEGS + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP
endm

; Character idle animation logic
macro SetIdleAnim
    ld a, [PREV_CHARACTER_SPRITE_ANIMATION]
    cp a, DOWN_ANIMATION_FLAG
    jr nz, .not_down_idle
        copy [CHARACTER_FIRST_ANI_FRAME_HEAD],    (DOWN_IDLE_ANIMATION_START)
        copy [CHARACTER_FIRST_ANI_FRAME_LEGS],    (DOWN_IDLE_ANIMATION_START + LEGS_TILE_OFFSET)
        copy [CHARACTER_LAST_ANI_FRAME_HEAD],     (DOWN_IDLE_ANIMATION_END)
    .not_down_idle
    ld a, [PREV_CHARACTER_SPRITE_ANIMATION]
    cp a, UP_ANIMATION_FLAG
    jr nz, .not_up_idle
        copy [CHARACTER_FIRST_ANI_FRAME_HEAD],    (UP_IDLE_ANIMATION_START)
        copy [CHARACTER_FIRST_ANI_FRAME_LEGS],    (UP_IDLE_ANIMATION_START + LEGS_TILE_OFFSET)
        copy [CHARACTER_LAST_ANI_FRAME_HEAD],     (UP_IDLE_ANIMATION_END)
    .not_up_idle
    ld a, [PREV_CHARACTER_SPRITE_ANIMATION]
    cp a, LEFT_ANIMATION_FLAG
    jr z, .side_idle
    cp a, RIGHT_ANIMATION_FLAG
    jr nz, .not_side_idle
    .side_idle
        copy [CHARACTER_FIRST_ANI_FRAME_HEAD],    (SIDE_IDLE_ANIMATION_START)
        copy [CHARACTER_FIRST_ANI_FRAME_LEGS],    (SIDE_IDLE_ANIMATION_START + LEGS_TILE_OFFSET)
        copy [CHARACTER_LAST_ANI_FRAME_HEAD],     (SIDE_IDLE_ANIMATION_END)
    .not_side_idle
endm

; Updates animation pointers
macro UpdateAnimationData
    ld a, [CHARACTER_SPRITE_ANIMATION]
    cp a, DOWN_ANIMATION_FLAG
    jr nz, .not_down_anim
    .down_anim
        SetDownAnim
        jp .anim_set
    .not_down_anim
        ld a, [CHARACTER_SPRITE_ANIMATION]
        cp a, UP_ANIMATION_FLAG
        jr nz, .not_up_anim
    .up_anim
        SetUpAnim
        jp .anim_set
    .not_up_anim
        ld a, [CHARACTER_SPRITE_ANIMATION]
        cp a, LEFT_ANIMATION_FLAG
        jr nz, .not_left_anim
    .left_anim
        SetLeftAnim
        jp .anim_set
    .not_left_anim
        ld a, [CHARACTER_SPRITE_ANIMATION]
        cp a, RIGHT_ANIMATION_FLAG
        jr nz, .not_right_anim
    .right_anim
        SetRightAnim
        jp .anim_set
    .not_right_anim
        SetIdleAnim
    .anim_set
        copy [CHARACTER_ANIM_CHANGED], ANIM_CHANGED_FLAG
endm

endc