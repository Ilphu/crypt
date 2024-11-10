; @file graphics.inc
; @authors Jack Kertscher, Garrett Rhoads, and Maximilien Dagois
; @brief Graphics constants and initialization
; @date 2024-11-02

if !def(GRAPHICS_INC)
def GRAPHICS_INC equ 1

; Graphics constants 
def TILE_WIDTH                      equ (8)
def TILES_COUNT                     equ (384)
def BYTES_PER_TILE                  equ (16)
def TILES_BYTE_SIZE                 equ (TILES_COUNT * BYTES_PER_TILE)

def TILEMAPS_COUNT                  equ (2)
def BYTES_PER_TILEMAP               equ (1024)
def TILEMAPS_BYTE_SIZE              equ (TILEMAPS_COUNT * BYTES_PER_TILEMAP)

def GRAPHICS_DATA_SIZE              equ (TILES_BYTE_SIZE + TILEMAPS_BYTE_SIZE)
def GRAPHICS_DATA_ADDRESS_END       equ ($8000)
def GRAPHICS_DATA_ADDRESS_START \
    equ (GRAPHICS_DATA_ADDRESS_END - GRAPHICS_DATA_SIZE)

def BACKGROUND_PALETTE              equ (%11100100)
def SPRITE_PALETTE_1                equ (%11010000)

def BG_START_X                      equ (0)
def BG_START_Y                      equ (0)

def WINDOW_STARTING_X               equ (7)
def WINDOW_STARTING_Y               equ (0)
def WINDOW_HIDDEN_X                 equ (7)
def WINDOW_HIDDEN_Y                 equ (255)

def SCREEN_STILL_X_MIN              equ (5 * TILE_WIDTH)
def SCREEN_STILL_X_MAX              equ (15 * TILE_WIDTH)
def SCREEN_STILL_Y_MIN              equ (4 * TILE_WIDTH) + (TILE_WIDTH / 2)
def SCREEN_STILL_Y_MAX              equ (13 * TILE_WIDTH) + (TILE_WIDTH / 2)

def ROM_COLLISION_MAP_ADDRESS_START equ ($5F7F)
def ROM_COLLISION_MAP_ADDRESS_END   equ (ROM_COLLISION_MAP_ADDRESS_START + $80)
def COLLISION_MAP_ADDRESS_START     equ ($DF7F)
def COLLISION_MAP_ADDRESS_END       equ (COLLISION_MAP_ADDRESS_START + $80)

include "include/character.inc"
include "include/timer.inc"

; load the graphics data from ROM to VRAM
macro LoadGraphicsDataIntoVRAM
    ld de, GRAPHICS_DATA_ADDRESS_START
    ld hl, _VRAM8000
    .load_tile\@
        ld a, [de]
        inc de
        ld [hli], a
        ld a, d
        cp a, high(GRAPHICS_DATA_ADDRESS_END)
        jr nz, .load_tile\@
endm

; Initialize the OAM
macro InitOAM
    ld c, OAM_COUNT
    ld hl, _OAMRAM + OAMA_Y
    ld de, sizeof_OAM_ATTRS
    .init_oam\@
        ld [hl], 0
        add hl, de
        dec c
        jr nz, .init_oam\@
endm

macro InitCollisionMap
    ld de, ROM_COLLISION_MAP_ADDRESS_START
    ld hl, COLLISION_MAP_ADDRESS_START
    .load_byte\@
        ld a, [de]
        inc de
        ld [hli], a
        ld a, e
        cp a, low(ROM_COLLISION_MAP_ADDRESS_END)
        jr nz, .load_byte\@
endm

macro PrintLvl2Text
    ld de, CLEAR_PRINT_LOC_1
    ld bc, CLEAR_STRING
    call print_text
    ld de, CLEAR_PRINT_LOC_2
    ld bc, CLEAR_STRING
    call print_text
    ld de, LEVEL_2_PRINT_LOC_1
    ld bc, LEVEL_2_STRING_1
    call print_text
    ld de, LEVEL_2_PRINT_LOC_2
    ld bc, LEVEL_2_STRING_2
    call print_text
endm

; initialze the title
macro InitTitle
    ld a, BACKGROUND_PALETTE
    ld [rBGP], a

    LoadGraphicsDataIntoVRAM

    ld a, IEF_VBLANK
    ld [rIE], a
    ei

    ld a, LCDCF_ON | LCDCF_WIN9C00 | LCDCF_BG8800 | LCDCF_BG9800 | LCDCF_OBJ8 | LCDCF_OBJOFF | LCDCF_BGON | LCDCF_WINON
    ld [rLCDC], a

    ; cover the background with the window for start screen
    ld a, WINDOW_STARTING_X
    ld [rWX], a
    ld a, WINDOW_STARTING_Y
    ld [rWY], a

    ; loads the collision map to the last 128 bytes of WRAM
    InitCollisionMap

    ; level 2 text anouncement
    ld a, [rGSF]
    bit rGSB_LVL2, a
    jr z, .not_lvl_2
        PrintLvl2Text
    .not_lvl_2
endm

; initialize the graphics for the game loop
macro InitGraphics
    InitOAM

    ld a, SPRITE_PALETTE_1
    ld [rOBP0], a

    copy [rSCX], BG_START_X
    copy [rSCY], BG_START_Y

    InitSprites

    ; set the graphics parameters and turn back LCD on
    ld a, LCDCF_ON | LCDCF_WIN9C00 | LCDCF_BG8800 | LCDCF_BG9800 | LCDCF_OBJ8 | LCDCF_OBJON | LCDCF_BGON | LCDCF_WINOFF
    ld [rLCDC], a
endm

endc