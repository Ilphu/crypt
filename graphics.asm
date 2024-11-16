; @file graphics.asm
; @authors Jack Kertscher and Garrett Rhoads
; @brief Controls the window
; @date 2024-11-02

include "include/hardware.inc"
include "include/utils.inc"
include "include/pointers.inc"
include "include/graphics.inc"
include "include/sprites.inc"
include "include/printing.inc"
include "include/timer.inc"

section "graphics", rom0

def BORDER_X        equ ($60)
def BORDER_Y        equ ($70)
def BORDER_0        equ ($00)

; Prints the text starting at (bc) to the position in the tilemap starting at (de)
print_text:
    push hl
    push de
    push bc
    push af
    halt
    ; Load the starting tilemap address into (hl)
    ld h, d
    ld l, e

    .write_text_loop
        ; Load the letter into (a), exit if it is the termination character
        ld a, [bc]
        cp a, BORDER_0
        jr z, .over

        ; Offset the ascii value to get a tile index and write that index to the screen
        add a, ASCII_TO_TILESET_OFFSET

        ld [hli], a
        inc bc
        jr .write_text_loop

    .over
        pop af
        pop bc
        pop de
        pop hl
        ret

export print_text

; Changes the posistion of the screen based on 
; the position of sprites and borders of the level
update_window:
    .check_window_moves
        ld a, [CHARACTER_MOVEMENT_DELAY]
        ld b, a
        ld a, [CHARACTER_MOVEMENT_DELAY_STATE]
        cp a, b
        jp nz, .no_move_screen

        ld a, [CHARACTER_SPRITE_HEAD + OAMA_X]
        cp a, SCREEN_STILL_X_MAX
        jr c, .no_move_screen_right

        ld a, [rSCX]
        cp a, BORDER_X
        jp z, .no_move_screen_right

        inc a
        ld [rSCX], a
        ld a, [CHARACTER_SPRITE_HEAD + OAMA_X]
    .no_move_screen_right
        ld a, [CHARACTER_SPRITE_HEAD + OAMA_X]
        cp a, SCREEN_STILL_X_MIN
        jr nc, .no_move_screen_left

        ld a, [rSCX]
        cp a, BORDER_0
        jp z, .no_move_screen_left

        dec a
        ld [rSCX], a
        ld a, [CHARACTER_SPRITE_HEAD + OAMA_X]
    .no_move_screen_left
        ld a, [CHARACTER_SPRITE_HEAD + OAMA_Y]
        cp a, SCREEN_STILL_Y_MIN
        jr nc, .no_move_screen_up

        ld a, [rSCY]
        cp a, BORDER_0
        jp z, .no_move_screen_up

        dec a
        ld [rSCY], a
        ld a, [CHARACTER_SPRITE_HEAD + OAMA_Y]
    .no_move_screen_up
        ld a, [CHARACTER_SPRITE_HEAD + OAMA_Y]
        cp a, SCREEN_STILL_Y_MAX
        jr c, .no_move_screen

        ld a, [rSCY]
        cp a, BORDER_Y
        jp z, .no_move_screen

        inc a
        ld [rSCY], a
        ld a, [CHARACTER_SPRITE_HEAD + OAMA_Y]
    .no_move_screen
        ret

export update_window

; initializes the title screen
init_title_screen:
    InitTitle
    ret
export init_title_screen

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Strings to print
LEVEL_3_STRING_1:
    db "  THE ZOMBIES   \0"
LEVEL_3_STRING_2:
    db "ARE COMING\0"
LEVEL_2_STRING_1:
    db "ANOTHER CREATURE\0"
LEVEL_2_STRING_2:
    db "APPROACHES\0"
CLEAR_STRING:
    db "                  \0"
LOSS_STRING:
    db "NOT FAST ENOUGH...\0"

; collision map
section "collision_map", rom0[ROM_COLLISION_MAP_ADDRESS_START]
ds 22, $FF
db $F8, $0F, $F0, $19, $F0, $0F, $F0, $03, $F0, $0F, $F0, $03, $F0, $0F, $F0, $03
db $F0, $0F, $00, $03, $F0, $00, $00, $01, $F0, $00, $00, $38, $E0, $00, $F0, $F8
db $00, $0F, $F0, $F8, $00, $0F, $F0, $F8, $00, $0F, $F0, $F8, $1F, $8F, $F0, $F8
db $1F, $9F, $F0, $F8, $1F, $9F, $F8, $00, $1F, $9F, $FC, $00, $1F, $9F, $FC, $00
db $07, $8F, $F8, $00, $00, $0F, $F0, $C0, $00, $0F, $00, $C0, $00, $00, $00, $00
db $00, $00, $00, $0F, $F8, $00, $F0, $0F, $F8, $0F, $F0, $0F, $F8, $0F
ds 12, $FF
; DungeionTilesetUpdated.png is a remix of the GB Studio Dungeon Tileset by 
; Rekkimaru and the GameBoy palette "Island" by PiiXL.
; GB Studio Dungeon Tileset:
;   Used for environmental tiles
;   Obtained from https://rekkimaru.itch.io/gb-studio-dungeon-tileset-8x8
;   Liscenced under free use
; Gameboy Palette "Island":
;   Used for text tiles
;   Obtained from https://piiixl.itch.io/island
;   Liscenced under CC by 4.0 https://creativecommons.org/licenses/by/4.0/deed.en
; All sprite tiles were created by Garrett Rhodes
; These were remixed together by Garrett Rhodes and Jack Kertscher

section "graphics_data", rom0[GRAPHICS_DATA_ADDRESS_START]
incbin "graphics/DungeonTilesetUpdated.chr"
incbin "graphics/Background.tlm"
incbin "graphics/Window.tlm"