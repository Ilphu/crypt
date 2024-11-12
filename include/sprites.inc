; @file sprites.inc
; @authors Jack Kertscher and Garrett Rhoads
; @brief Constants and generic functionality of all sprites
; @date 2024-11-02

if !def(SPRITES_INC)
def SPRITES_INC equ 1

include "include/pointers.inc"
include "include/sprites.inc"
include "include/graphics.inc"
include "include/enemy.inc"

; Sprite Constants 
def SPRITE_X_OFFSET                 equ (8)
def SPRITE_Y_OFFSET                 equ (16)

; Sprite Collision Flag Constants
def COLLF_TOP_LEFT                  equ $80
def COLLF_TOP_RIGHT                 equ $40
def COLLF_RIGHT_TOP                 equ $20
def COLLF_RIGHT_BOTTOM              equ $10
def COLLF_BOTTOM_RIGHT              equ $08
def COLLF_BOTTOM_LEFT               equ $04
def COLLF_LEFT_BOTTOM               equ $02
def COLLF_LEFT_TOP                  equ $01

def COLLB_TOP_LEFT                  equ $07
def COLLB_TOP_RIGHT                 equ $06
def COLLB_RIGHT_TOP                 equ $05
def COLLB_RIGHT_BOTTOM              equ $04
def COLLB_BOTTOM_RIGHT              equ $03
def COLLB_BOTTOM_LEFT               equ $02
def COLLB_LEFT_BOTTOM               equ $01
def COLLB_LEFT_TOP                  equ $00

; Bounding box offsets
def TOP_BOUNDING_BOX_OFFSET         equ 3
def BOTTOM_BOUNDING_BOX_OFFSET      equ -1
def LEFT_BOUNDING_BOX_OFFSET        equ $00
def RIGHT_BOUNDING_BOX_OFFSET       equ -1

; character sprite constants
def CHARACTER_SPRITE_HEAD equ (_OAMRAM)
def CHARACTER_SPRITE_LEGS equ (_OAMRAM + sizeof_OAM_ATTRS)

def CHARACTER_SPRITE_HEAD_X_START          equ (9 * TILE_WIDTH) 
def CHARACTER_SPRITE_HEAD_Y_START          equ (8 * TILE_WIDTH)
def CHARACTER_SPRITE_LEGS_X_START          equ CHARACTER_SPRITE_HEAD_X_START
def CHARACTER_SPRITE_LEGS_Y_START          equ (CHARACTER_SPRITE_HEAD_Y_START + TILE_WIDTH)

; first enemy sprite constants
def ENEMY1_SPRITE_HEAD equ (_OAMRAM + (2 * sizeof_OAM_ATTRS))
def ENEMY1_SPRITE_LEGS equ (_OAMRAM + (3 * sizeof_OAM_ATTRS))

def ENEMY1_SPRITE_HEAD_X_START          equ (0 * TILE_WIDTH) 
def ENEMY1_SPRITE_HEAD_Y_START          equ (10 * TILE_WIDTH)
def ENEMY1_SPRITE_LEGS_X_START          equ ENEMY1_SPRITE_HEAD_X_START
def ENEMY1_SPRITE_LEGS_Y_START          equ (ENEMY1_SPRITE_HEAD_Y_START + TILE_WIDTH)

; second enemy sprite constants
def ENEMY2_SPRITE_HEAD equ (_OAMRAM + (4 * sizeof_OAM_ATTRS))
def ENEMY2_SPRITE_LEGS equ (_OAMRAM + (5 * sizeof_OAM_ATTRS))

def ENEMY2_SPRITE_HEAD_X_START          equ (31 * TILE_WIDTH) 
def ENEMY2_SPRITE_HEAD_Y_START          equ (24 * TILE_WIDTH)
def ENEMY2_SPRITE_LEGS_X_START          equ ENEMY2_SPRITE_HEAD_X_START
def ENEMY2_SPRITE_LEGS_Y_START          equ (ENEMY2_SPRITE_HEAD_Y_START + TILE_WIDTH)

; timer sprite constants
def TIMER_SPRITE_TENS equ (_OAMRAM + (6 * sizeof_OAM_ATTRS))
def TIMER_SPRITE_ONES equ (_OAMRAM + (7 * sizeof_OAM_ATTRS))

def TIMER_SPRITE_TENS_X_START          equ (4 + SPRITE_X_OFFSET) 
def TIMER_SPRITE_TENS_Y_START          equ (4 + SPRITE_Y_OFFSET)
def TIMER_SPRITE_ONES_X_START          equ (TIMER_SPRITE_TENS_X_START + TILE_WIDTH)
def TIMER_SPRITE_ONES_Y_START          equ TIMER_SPRITE_TENS_Y_START

; initializes all neccessary sprites
macro InitSprites
    InitEnemy ENEMY1_DATA, ENEMY1_SPRITE_HEAD, ENEMY1_SPRITE_LEGS, \
              ENEMY1_SPRITE_HEAD_X_START, ENEMY1_SPRITE_HEAD_Y_START, \
              ENEMY1_SPRITE_LEGS_X_START, ENEMY1_SPRITE_LEGS_Y_START
    ld a, [rGSF]
    bit rGSB_LVL2, a
    jr z, .no_init_enemy_2
        InitEnemy ENEMY2_DATA, ENEMY2_SPRITE_HEAD, ENEMY2_SPRITE_LEGS, \
                  ENEMY2_SPRITE_HEAD_X_START, ENEMY2_SPRITE_HEAD_Y_START, \
                  ENEMY2_SPRITE_LEGS_X_START, ENEMY2_SPRITE_LEGS_Y_START
    .no_init_enemy_2
        InitCharacter
        InitTimer
endm

; UpdateSpritePos OAMLocHead, OAMLocLegs, GlobalX, GlobalY
; Update the location of an 8x16 sprite using its head OAM address, legs OAM
; address, and world X and Y position.
macro UpdateSpritePos16
    ; Calculate screen X position
    ld a, [rSCX]
    ld b, a
    ld a, [\3]
    add SPRITE_X_OFFSET
    sub b

    ld [\1 + OAMA_X], a
    ld [\2 + OAMA_X], a

    ; Calculate screen Y position
    ld a, [rSCY]
    ld b, a
    ld a, [\4]
    add SPRITE_Y_OFFSET
    sub b
    ld [\1 + OAMA_Y], a
    add TILE_WIDTH
    ld [\2 + OAMA_Y], a
endm

macro TileIndexY
    srl a
    and %11111100

    ; Point (hl) to the start of the 4-byte tile row
    ld hl, COLLISION_MAP_ADDRESS_START
    ld de, 0
    ld e, a
    add hl, de
endm

macro TileIndexX
    srl a
    srl a
    srl a
    ld b, a
    srl a
    srl a
    srl a

    ; Point (hl) to the byte in which the desired tile bit is held
    ld e, a
    add hl, de
    
    ; Pick out the desired tile bit
    ld a, b
    and a, %00000111
    xor a, %00000111
endm

endc