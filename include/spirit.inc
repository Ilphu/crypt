 ; @file spirit.inc
; @authors Jack Kertscher and Garrett Rhoads
; @brief Spirit logic
; @date 2024-11-02

if !def(SPIRIT_INC)
def SPIRIT_INC equ 1

include "include/pointers.inc"
include "include/graphics.inc"
include "include/sprites.inc"
include "include/character.inc"

; Spirit animation constants
def INIT_SPIRIT_ANIMATION_DELAY     equ ($0F)
def INIT_SPIRIT_MOVEMENT_DELAY      equ ($03)

def SPIRIT_SPRITE_COLLISION_RANGE_X equ ($07)
def SPIRIT_SPRITE_COLLISION_RANGE_Y equ ($0F)

def SPIRIT_DEFAULT_MOVEB_X          equ ($00)
def SPIRIT_DEFAULT_MOVEB_Y          equ ($01)

; Relative memory position of spirit traits from \1
rsreset
def SPIRIT_FIRST_ANI_FRAME_HEAD      rb 1 
def SPIRIT_FIRST_ANI_FRAME_LEGS      rb 1 
def SPIRIT_LAST_ANI_FRAME_HEAD       rb 1
def SPIRIT_ANI_DELAY                 rb 1
def SPIRIT_ANI_DELAY_STATE           rb 1
def SPIRIT_SPRITE_WORLD_X            rb 1
def SPIRIT_SPRITE_WORLD_Y            rb 1
def SPIRIT_SPIRIT_DISTANCE_TO_CHAR_X rb 1
def SPIRIT_SPIRIT_DISTANCE_TO_CHAR_Y rb 1
def SPIRIT_SPRITE_ANIMATION          rb 1
def PREV_SPIRIT_SPRITE_ANIMATION     rb 1
def SPIRIT_MOVEMENT_DELAY            rb 1
def SPIRIT_MOVEMENT_DELAY_STATE      rb 1
def SPIRIT_INTENDED_MOVEMENT         rb 1
def SPIRIT_ANIM_CHANGED              rb 1
def SPIRIT_COLLISION_FLAGS           rb 1
def SPIRIT_DEFAULT_PATHING_FLAGS     rb 1

def SPIRIT_ATTRIBUTES_SIZE           equ (17)

; Clears the WRAM bank of spirit attributes starting at \1
macro ClearSpiritWram
    ld c, SPIRIT_ATTRIBUTES_SIZE
    ld hl, \1
    .clear_spirit_wram\@
        ld [hl], 0
        inc hl
        dec c
        jr nz, .clear_spirit_wram\@
endm

; Initializing the spirit
; \1: Spirit data memory chunk
; \2: Spirit head OAM location, \3: Spirit legs OAM location
macro InitSpirit
    InitSpiritAnimation \1, \2, \3

    copy [\1 + SPIRIT_SPRITE_WORLD_X], \4
    copy [\1 + SPIRIT_SPRITE_WORLD_Y], \5
    copy [\1 + SPIRIT_DISTANCE_TO_CHAR_X], CHARACTER_SPRITE_HEAD_X_START - \4
    copy [\1 + SPIRIT_DISTANCE_TO_CHAR_Y], CHARACTER_SPRITE_HEAD_Y_START - \5

    copy [\1 + SPIRIT_MOVEMENT_DELAY], INIT_SPIRIT_MOVEMENT_DELAY
    copy [\1 + SPIRIT_MOVEMENT_DELAY_STATE], INITIAL_DELAY

    ; Init the head and legs of our SPIRIT sprite
    copy [\2 + OAMA_X], \4
    copy [\2 + OAMA_Y], \5
    copy [\2 + OAMA_TILEID], [\1 + SPIRIT_FIRST_ANI_FRAME_HEAD]
    copy [\2 + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP
    
    copy [\3 + OAMA_X], \6
    copy [\3 + OAMA_Y], \7
    copy [\3 + OAMA_TILEID], [\1 + SPIRIT_FIRST_ANI_FRAME_LEGS]
    copy [\3 + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP
endm

; Initializing the animations for the spirit
; \1: Spirit data memory chunk
; \2: Spirit head OAM location, \3: Spirit legs OAM location
macro InitSpiritAnimation
    copy [\1 + SPIRIT_FIRST_ANI_FRAME_HEAD],    (SIDE_ANIMATION_START + SPIRIT_ANIMATION_OFFSET)
    copy [\1 + SPIRIT_FIRST_ANI_FRAME_LEGS],    (SIDE_ANIMATION_START + LEGS_TILE_OFFSET + SPIRIT_ANIMATION_OFFSET)
    copy [\1 + SPIRIT_LAST_ANI_FRAME_HEAD],     (SIDE_ANIMATION_END + SPIRIT_ANIMATION_OFFSET)
    
    copy [\1 + SPIRIT_SPRITE_ANIMATION],        (RIGHT_ANIMATION_FLAG)
    copy [\1 + PREV_SPIRIT_SPRITE_ANIMATION],   (IDLE_ANIMATION_FLAG)
    copy [\1 + SPIRIT_ANI_DELAY],               (INIT_SPIRIT_ANIMATION_DELAY)
    copy [\1 + SPIRIT_ANI_DELAY_STATE],         (INITIAL_DELAY)
endm

; updates the distance from spirit to the character in the x direction
; \1: Spirit data memory chunk
; \2: Spirit head OAM location, \3: Spirit legs OAM location
; data only
macro UpdateSpiritDistanceToCharX
    ld a, [CHARACTER_SPRITE_WORLD_X]
    ld b, a
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    cp a, b
    jr c, .less_than_x\@
        ; a >= b
        sub b
        ld [\1 + SPIRIT_DISTANCE_TO_CHAR_X], a
        jr .end_update_distance_x\@
    .less_than_x\@
        ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
        ld b, a
        ld a, [CHARACTER_SPRITE_WORLD_X]
        sub b
        ld [\1 + SPIRIT_DISTANCE_TO_CHAR_X], a
    .end_update_distance_x\@
endm

; updates the distance from spirit to the character in the y direction
; \1: Spirit data memory chunk
; \2: Spirit head OAM location, \3: Spirit legs OAM location
; data only
macro UpdateSpiritDistanceToCharY
    ld a, [CHARACTER_SPRITE_WORLD_Y]
    ld b, a
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    cp a, b
    jr c, .less_than_y\@
        ; a >= b
        sub b
        ld [\1 + SPIRIT_DISTANCE_TO_CHAR_Y], a
        jr .end_update_distance_y\@
    .less_than_y\@
        ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
        ld b, a
        ld a, [CHARACTER_SPRITE_WORLD_Y]
        sub b
        ld [\1 + SPIRIT_DISTANCE_TO_CHAR_Y], a
    .end_update_distance_y\@
endm

macro IntendedSpiritMovementX
        ld a, [CHARACTER_SPRITE_WORLD_X]
        ld b, a
        ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
        cp a, b
        jr c, .move_right\@
        ; move left
            ld a, [\1 + SPIRIT_INTENDED_MOVEMENT]
            set PADB_LEFT, a 
            ld [\1 + SPIRIT_INTENDED_MOVEMENT], a
            jr .check_diagonal\@
        .move_right\@
            ld a, [\1 + SPIRIT_INTENDED_MOVEMENT]
            set PADB_RIGHT, a 
            ld [\1 + SPIRIT_INTENDED_MOVEMENT], a
            
    .check_diagonal\@
endm

macro IntendedSpiritMovementY
    ld a, [CHARACTER_SPRITE_WORLD_Y]
    ld b, a
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    cp a, b
    jr c, .move_down\@
    ; move_up
        ld a, [\1 + SPIRIT_INTENDED_MOVEMENT]
        set PADB_UP, a
        ld [\1 + SPIRIT_INTENDED_MOVEMENT], a
        jr .end\@
    .move_down\@
        ld a, [\1 + SPIRIT_INTENDED_MOVEMENT]
        set PADB_DOWN, a
        ld [\1 + SPIRIT_INTENDED_MOVEMENT], a
    .end\@
endm

macro UpdateSpiritIntendedMovement
    xor a
    ld [\1 + SPIRIT_INTENDED_MOVEMENT], a
    
    UpdateDistanceToCharXSpirit \1, \2, \3
    UpdateDistanceToCharYSpirit \1, \2, \3

    ld a, [\1 + SPIRIT_DISTANCE_TO_CHAR_Y]
    ld b, a
    ld a, [\1 + SPIRIT_DISTANCE_TO_CHAR_X]
    ; move in which ever axis is furthest away from the character
    ; If x < y, move y
    cp a, b
    jp c, .move_y\@
        IntendedMovementXSpirit \1, \2, \3
        ld a, [\1 + SPIRIT_DISTANCE_TO_CHAR_Y]
        ld b, a
        ld a, [\1 + SPIRIT_DISTANCE_TO_CHAR_X]
        cp a, b
        jr nz, .end\@
    .move_y\@ 
        IntendedMovementYSpirit \1, \2, \3
    .end\@
endm

; top left collision point (1 px above the leftmost pixel of the top edge)
macro UpdateSpiritTLCollisionFlag
    ; Y position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    dec a
    add a, TOP_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY

    ; X position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    add a, LEFT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit ; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_tl_collide\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        set COLLB_TOP_LEFT, a
        ld [\1 + SPIRIT_COLLISION_FLAGS], a
    .no_tl_collide\@
endm

; Top right collision point (1px above the rightmost pixel of the top edge)
macro UpdateSpiritTRCollisionFlag
    ; Y position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    dec a
    add a, TOP_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    ; X position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    add a, TILE_WIDTH
    add a, RIGHT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_tr_collide\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        set COLLB_TOP_RIGHT, a
        ld [\1 + SPIRIT_COLLISION_FLAGS], a
    .no_tr_collide\@
endm

; Right top collision point (1px to the right of the topmost pixel of the right edge)
macro UpdateSpiritRTCollisionFlag
    ; Y position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    add a, TOP_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    ; X position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    add a, TILE_WIDTH
    inc a
    add a, RIGHT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_rt_collide\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        set COLLB_RIGHT_TOP, a
        ld [\1 + SPIRIT_COLLISION_FLAGS], a
    .no_rt_collide\@
endm

; Right bottom collision point (1px to the right of the bottommost pixel of the right edge)
macro UpdateSpiritRBCollisionFlag
    ; Y position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    add a, TILE_WIDTH
    add a, BOTTOM_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    ; X position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    add a, TILE_WIDTH
    inc a
    add a, RIGHT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_rb_collide\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        set COLLB_RIGHT_BOTTOM, a
        ld [\1 + SPIRIT_COLLISION_FLAGS], a
    .no_rb_collide\@
endm

; Bottom right collision point (1px below the rightmost pixel of the bottom edge)
macro UpdateSpiritBRCollisionFlag
    ; Y position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    add a, TILE_WIDTH
    inc a
    add a, BOTTOM_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    ; X position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    add a, TILE_WIDTH
    add a, RIGHT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_br_collide\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        set COLLB_BOTTOM_RIGHT, a
        ld [\1 + SPIRIT_COLLISION_FLAGS], a
    .no_br_collide\@
endm

; Bottom left collision point (1px below the leftmost pixel of the bottom edge)
macro UpdateSpiritBLCollisionFlag
    ; Y position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    add a, TILE_WIDTH
    inc a
    add a, BOTTOM_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    ; X position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    add a, LEFT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_bl_collide\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        set COLLB_BOTTOM_LEFT, a
        ld [\1 + SPIRIT_COLLISION_FLAGS], a
    .no_bl_collide\@
endm

; Left bottom collision point (1px to the left of the bottommost pixel of the left edge)
macro UpdateSpiritLBCollisionFlag
    ; Y position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    add a, TILE_WIDTH
    add a, BOTTOM_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    ; X position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    dec a
    add a, LEFT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_lb_collide\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        set COLLB_LEFT_BOTTOM, a
        ld [\1 + SPIRIT_COLLISION_FLAGS], a
    .no_lb_collide\@
endm

; Left top collision point (1px to the left of the topmost pixel of the left edge)
macro UpdateSpiritLTCollisionFlag
    ; Y position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    add a, TOP_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    
    ; X position of the point
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    dec a
    add a, LEFT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_lt_collide\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        set COLLB_LEFT_TOP, a
        ld [\1 + SPIRIT_COLLISION_FLAGS], a
    .no_lt_collide\@
endm

macro CheckRightSpiritSpriteCollision
    ld a, [\4 + SPIRIT_SPRITE_WORLD_X]
    ld b, a
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    add a, SPIRIT_SPRITE_COLLISION_RANGE_X
    inc a
    cp a, b
    jp c, .no_right_collision\@
    SwapAB
    add a, SPIRIT_SPRITE_COLLISION_RANGE_X
    SwapAB
    cp a, b
    jp nc, .no_right_collision\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        set COLLB_RIGHT_TOP, a
        set COLLB_RIGHT_BOTTOM, a
        ld [\1 + SPIRIT_COLLISION_FLAGS], a
    .no_right_collision\@
endm

macro CheckLeftSpiritSpriteCollision
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    ld b, a
    ld a, [\4 + SPIRIT_SPRITE_WORLD_X]
    add a, SPIRIT_SPRITE_COLLISION_RANGE_X
    inc a
    cp a, b
    jp c, .no_right_collision\@
    SwapAB
    add a, SPIRIT_SPRITE_COLLISION_RANGE_X
    SwapAB
    cp a, b
    jp nc, .no_right_collision\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        set COLLB_LEFT_TOP, a
        set COLLB_LEFT_BOTTOM, a
        ld [\1 + SPIRIT_COLLISION_FLAGS], a
    .no_right_collision\@
endm

macro CheckBottomSpiritSpriteCollision
    ld a, [\4 + SPIRIT_SPRITE_WORLD_Y]
    ld b, a
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    add a, SPIRIT_SPRITE_COLLISION_RANGE_Y
    inc a
    cp a, b
    jp c, .no_right_collision\@
    SwapAB
    add a, SPIRIT_SPRITE_COLLISION_RANGE_Y
    SwapAB
    cp a, b
    jp nc, .no_right_collision\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        set COLLB_BOTTOM_LEFT, a
        set COLLB_BOTTOM_RIGHT, a
        ld [\1 + SPIRIT_COLLISION_FLAGS], a
    .no_right_collision\@
endm

macro CheckTopSpiritSpriteCollision
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    ld b, a
    ld a, [\4 + SPIRIT_SPRITE_WORLD_Y]
    add a, SPIRIT_SPRITE_COLLISION_RANGE_Y
    inc a
    cp a, b
    jp c, .no_top_collision\@
    SwapAB
    add a, SPIRIT_SPRITE_COLLISION_RANGE_Y
    SwapAB
    cp a, b
    jp nc, .no_top_collision\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        set COLLB_TOP_LEFT, a
        set COLLB_TOP_RIGHT, a
        ld [\1 + SPIRIT_COLLISION_FLAGS], a
    .no_top_collision\@
endm

; \1, \2, \3: This spirit's data, head, legs
; \4: The other spirit's data
macro UpdateSpiritSpriteCollisionFlags
    ld a, [\4 + SPIRIT_SPRITE_WORLD_X]
    ld b, a
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    SubAbsolute

    cp a, SPIRIT_SPRITE_COLLISION_RANGE_X
    jp nc, .no_collision\@
    ld a, [\4 + SPIRIT_SPRITE_WORLD_Y]
    ld b, a
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    SubAbsolute

    cp a, SPIRIT_SPRITE_COLLISION_RANGE_Y
    jp nc, .no_collision\@

    ; collision!!!!!!
        CheckRightSpriteCollision \1, \2, \3, \4
        CheckLeftSpriteCollision \1, \2, \3, \4
        CheckBottomSpriteCollision \1, \2, \3, \4

    .no_collision\@
endm

macro UpdateSpiritCollisionFlags
    xor a
    ld [\1 + SPIRIT_COLLISION_FLAGS], a

    UpdateSpiritTLCollisionFlag \1, \2, \3
    UpdateSpiritTRCollisionFlag \1, \2, \3
    UpdateSpiritRTCollisionFlag \1, \2, \3
    UpdateSpiritRBCollisionFlag \1, \2, \3
    UpdateSpiritBRCollisionFlag \1, \2, \3
    UpdateSpiritBLCollisionFlag \1, \2, \3
    UpdateSpiritLBCollisionFlag \1, \2, \3
    UpdateSpiritLTCollisionFlag \1, \2, \3
    UpdateSpiritSpriteCollisionFlags \1, \2, \3, \4
endm

macro ApplyIntendedXSpirit
    ; Move left
    ld a, [\1 + SPIRIT_INTENDED_MOVEMENT]
    bit PADB_LEFT, a
    jp z, .not_left_move\@
        ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
        dec a
        ld [\1 + SPIRIT_SPRITE_WORLD_X], a
    .not_left_move\@
    ; Move right
    ld a, [\1 + SPIRIT_INTENDED_MOVEMENT]
    bit PADB_RIGHT, a
    jp z, .not_right_move\@
        ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
        inc a
        ld [\1 + SPIRIT_SPRITE_WORLD_X], a
    .not_right_move\@
endm

macro ApplyIntendedYSpirit
    ; Move up
    ld a, [\1 + SPIRIT_INTENDED_MOVEMENT]
    bit PADB_UP, a
    jp z, .not_up_move\@
        ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
        dec a
        ld [\1 + SPIRIT_SPRITE_WORLD_Y], a
    .not_up_move\@
    ; Move down
    ld a, [\1 + SPIRIT_INTENDED_MOVEMENT]
    bit PADB_DOWN, a
    jp z, .end_movement\@
        ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
        inc a
        ld [\1 + SPIRIT_SPRITE_WORLD_Y], a
    .end_movement\@
endm

; Combines the intended movement and collision data into (a) such that
; the collision flags are only set if we are also moving in that direction 
macro CombineMoveCollideSpirit
    ld a, [\1 + SPIRIT_INTENDED_MOVEMENT]
    or a, $0F
    ld b, a
    ld a, [\1 + SPIRIT_COLLISION_FLAGS]
    
    and a, b
    SwapAB
    srl a
    srl a
    srl a
    srl a
    or a, $F0
    SwapAB
    and a, b
    
endm

macro YCollisionPathfindingSpirit
    ld a, [\1 + SPIRIT_DEFAULT_PATHING_FLAGS]
    bit SPIRIT_DEFAULT_MOVEB_X, a
    jp nz, .defaulting_right\@
    ; Defaulting left
        ; if default movement left and wall left: Change default to right
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        bit COLLB_LEFT_BOTTOM, a
        jp nz, .change_default_to_right\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        bit COLLB_LEFT_TOP, a
        jp z, .set_intended_movement\@
    .change_default_to_right\@
        ; Changing default to right
        ld a, [\1 + SPIRIT_DEFAULT_PATHING_FLAGS]
        set SPIRIT_DEFAULT_MOVEB_X, a
        ld [\1 + SPIRIT_DEFAULT_PATHING_FLAGS], a
        jp .set_intended_movement\@
    .defaulting_right\@
        ; if default movement right and wall right: Change default to left
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        bit COLLB_RIGHT_BOTTOM, a
        jp nz, .change_default_to_left\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        bit COLLB_RIGHT_TOP, a
        jp z, .set_intended_movement\@
    .change_default_to_left\@
        ; changing default to left
        ld a, [\1 + SPIRIT_DEFAULT_PATHING_FLAGS]
        res SPIRIT_DEFAULT_MOVEB_X, a
        ld [\1 + SPIRIT_DEFAULT_PATHING_FLAGS], a
        jp .set_intended_movement\@
    .set_intended_movement\@
        ; Set the intended movement to the default X movement
        ld a, [\1 + SPIRIT_DEFAULT_PATHING_FLAGS]
        bit SPIRIT_DEFAULT_MOVEB_X, a
        jp nz, .set_move_right\@
        ; Set move left
            copy [\1 + SPIRIT_INTENDED_MOVEMENT], PADF_LEFT
            jp .done\@
        .set_move_right\@
            copy [\1 + SPIRIT_INTENDED_MOVEMENT], PADF_RIGHT
    .done\@
endm

macro XCollisionPathfindingSpirit
    ld a, [\1 + SPIRIT_DEFAULT_PATHING_FLAGS]
    bit SPIRIT_DEFAULT_MOVEB_Y, a
    jp nz, .defaulting_up\@
    ; Defaulting down
        ; if default movement down and wall down: Change default to up
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        bit COLLB_BOTTOM_LEFT, a
        jp nz, .change_default_to_up\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        bit COLLB_BOTTOM_RIGHT, a
        jp z, .set_intended_movement\@
    .change_default_to_up\@
        ; Changing default to right
        ld a, [\1 + SPIRIT_DEFAULT_PATHING_FLAGS]
        set SPIRIT_DEFAULT_MOVEB_Y, a
        ld [\1 + SPIRIT_DEFAULT_PATHING_FLAGS], a
        jp .set_intended_movement\@
    .defaulting_up\@
        ; if default movement right and wall right: Change default to left
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        bit COLLB_TOP_RIGHT, a
        jp nz, .change_default_to_down\@
        ld a, [\1 + SPIRIT_COLLISION_FLAGS]
        bit COLLB_TOP_LEFT, a
        jp z, .set_intended_movement\@
    .change_default_to_down\@
        ; changing default to up
        ld a, [\1 + SPIRIT_DEFAULT_PATHING_FLAGS]
        res SPIRIT_DEFAULT_MOVEB_Y, a
        ld [\1 + SPIRIT_DEFAULT_PATHING_FLAGS], a
        jp .set_intended_movement\@
    .set_intended_movement\@
        ; Set the intended movement to the default Y movement
        ld a, [\1 + SPIRIT_DEFAULT_PATHING_FLAGS]
        bit SPIRIT_DEFAULT_MOVEB_Y, a
        jp nz, .set_move_up\@
        ; Set move down
            copy [\1 + SPIRIT_INTENDED_MOVEMENT], PADF_DOWN
            jp .done\@
        .set_move_up\@
            copy [\1 + SPIRIT_INTENDED_MOVEMENT], PADF_UP
    .done\@
endm

macro ResetXPathfindingSpirit
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    ld b, a
    ld a, [CHARACTER_SPRITE_WORLD_Y]
    cp a, b
    jp c, .spirit_below_char\@
    ; spirit above char
        ld a, [\1 + SPIRIT_DEFAULT_PATHING_FLAGS]
        res SPIRIT_DEFAULT_MOVEB_Y, a
        ld [\1 + SPIRIT_DEFAULT_PATHING_FLAGS], a
        jp .done\@
    .spirit_below_char\@
        ld a, [\1 + SPIRIT_DEFAULT_PATHING_FLAGS]
        set SPIRIT_DEFAULT_MOVEB_Y, a
        ld [\1 + SPIRIT_DEFAULT_PATHING_FLAGS], a
    .done\@
endm

macro ResetSpiritYPathfinding
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    ld b, a
    ld a, [CHARACTER_SPRITE_WORLD_X]
    cp a, b
    jp c, .spirit_right_char\@
    ; spirit left of char
        ld a, [\1 + SPIRIT_DEFAULT_PATHING_FLAGS]
        set SPIRIT_DEFAULT_MOVEB_X, a
        ld [\1 + SPIRIT_DEFAULT_PATHING_FLAGS], a
        jp .done\@
    .spirit_right_char\@
        ld a, [\1 + SPIRIT_DEFAULT_PATHING_FLAGS]
        res SPIRIT_DEFAULT_MOVEB_X, a
        ld [\1 + SPIRIT_DEFAULT_PATHING_FLAGS], a
    .done\@
endm

macro UpdateIntendedCollisionSpirit
    CombineMoveCollideSpirit \1, \2, \3
    jp z, .no_change\@
        bit COLLB_BOTTOM_LEFT, a
        jp nz, .colliding_Y\@
        bit COLLB_TOP_LEFT, a
        jp nz, .colliding_Y\@
        bit COLLB_BOTTOM_RIGHT, a
        jp nz, .colliding_Y\@
        bit COLLB_TOP_RIGHT, a
        jp z, .no_colliding_Y\@
    .colliding_Y\@
        ;CombineMoveCollide \1, \2, \3
        YCollisionPathfindingSpirit \1, \2, \3
        jp .done\@
    .no_colliding_Y\@
        ResetYPathfindingSpirit \1, \2, \3
    
        CombineMoveCollideSpirit \1, \2, \3
        bit COLLB_LEFT_BOTTOM, a
        jp nz, .colliding_X\@
        bit COLLB_RIGHT_BOTTOM, a
        jp nz, .colliding_X\@
        bit COLLB_LEFT_TOP, a
        jp nz, .colliding_X\@
        bit COLLB_RIGHT_TOP, a
        jp z, .no_colliding_X\@
    .colliding_X\@
        ;CombineMoveCollide \1, \2, \3
        XCollisionPathfindingSpirit \1, \2, \3
        jp .done\@
    .no_colliding_X\@
        ResetXPathfindingSpirit \1, \2, \3
        jp .done\@
    .no_change\@
        ResetXPathfindingSpirit \1, \2, \3
        ResetYPathfindingSpirit \1, \2, \3
    .done\@
endm

macro ApplyIntendedMovementSpirit
    ; Does the spirit move this framme?
    ld a, [\1 + SPIRIT_MOVEMENT_DELAY]
    ld b, a
    ld a, [\1 + SPIRIT_MOVEMENT_DELAY_STATE]
    inc a
    and a, b
    ld [\1 + SPIRIT_MOVEMENT_DELAY_STATE], a
    jp nz, .end_movement\@
        ApplyIntendedXSpirit \1, \2, \3
        ApplyIntendedYSpirit \1, \2, \3
    .end_movement\@
endm

; Handles collisions with the character
; \1: Spirit data memory chunk
; \2: Spirit head OAM location, \3: Spirit legs OAM location
macro SpiritCharacterCollision
    ld a, [\1 + SPIRIT_DISTANCE_TO_CHAR_X]
    cp a, SPIRIT_SPRITE_COLLISION_RANGE_X
    jr nc, .no_collision_spirit\@
    ld a, [\1 + SPIRIT_DISTANCE_TO_CHAR_Y]
    cp a, SPIRIT_SPRITE_COLLISION_RANGE_Y
    jr nc, .no_collision_spirit\@
    ; collision!!!!!!
    copy [rGSF], rGSF_TITLE
    .no_collision_spirit\@
endm

; spirit left animation
; \1: Spirit data memory chunk
; \2: Spirit head OAM location, \3: Spirit legs OAM location
macro SpiritSetLeftAnim
    copy [\1 + SPIRIT_FIRST_ANI_FRAME_HEAD], SIDE_ANIMATION_START + SPIRIT_ANIMATION_OFFSET
    copy [\1 + SPIRIT_FIRST_ANI_FRAME_LEGS], SIDE_ANIMATION_START + LEGS_TILE_OFFSET + SPIRIT_ANIMATION_OFFSET
    copy [\1 + SPIRIT_LAST_ANI_FRAME_HEAD], SIDE_ANIMATION_END + SPIRIT_ANIMATION_OFFSET
    ;copy [\2 + OAMA_FLAGS], OAMF_PAL0
    ;copy [\3 + OAMA_FLAGS], OAMF_PAL0
endm

; spirit right animation
; \1: Spirit data memory chunk
; \2: Spirit head OAM location, \3: Spirit legs OAM location
macro SpiritSetRightAnim
    copy [\1 + SPIRIT_FIRST_ANI_FRAME_HEAD], SIDE_ANIMATION_START + SPIRIT_ANIMATION_OFFSET
    copy [\1 + SPIRIT_FIRST_ANI_FRAME_LEGS], SIDE_ANIMATION_START + LEGS_TILE_OFFSET + SPIRIT_ANIMATION_OFFSET
    copy [\1 + SPIRIT_LAST_ANI_FRAME_HEAD], SIDE_ANIMATION_END + SPIRIT_ANIMATION_OFFSET
    ;copy [\2 + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP
    ;copy [\3 + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP
endm

; spirit up animation
; \1: Spirit data memory chunk
; \2: Spirit head OAM location, \3: Spirit legs OAM location
macro SpiritSetUpAnim
    copy [\1 + SPIRIT_FIRST_ANI_FRAME_HEAD], UP_ANIMATION_START + SPIRIT_ANIMATION_OFFSET
    copy [\1 + SPIRIT_FIRST_ANI_FRAME_LEGS], UP_ANIMATION_START + LEGS_TILE_OFFSET + SPIRIT_ANIMATION_OFFSET
    copy [\1 + SPIRIT_LAST_ANI_FRAME_HEAD], UP_ANIMATION_END + SPIRIT_ANIMATION_OFFSET
endm

; spirit down animation
; \1: Spirit data memory chunk
; \2: Spirit head OAM location, \3: Spirit legs OAM location
macro SpiritSetDownAnim
    copy [\1 + SPIRIT_FIRST_ANI_FRAME_HEAD], DOWN_ANIMATION_START + SPIRIT_ANIMATION_OFFSET
    copy [\1 + SPIRIT_FIRST_ANI_FRAME_LEGS], DOWN_ANIMATION_START + LEGS_TILE_OFFSET + SPIRIT_ANIMATION_OFFSET
    copy [\1 + SPIRIT_LAST_ANI_FRAME_HEAD], DOWN_ANIMATION_END + SPIRIT_ANIMATION_OFFSET
endm

macro UpdateSpiritAnimationState
    copy [\1 + PREV_SPIRIT_SPRITE_ANIMATION], [\1 + SPIRIT_SPRITE_ANIMATION]

    ; Check if we are moving in the y direction, prioritize that animation
    ld a, [\1 + SPIRIT_INTENDED_MOVEMENT]
    bit PADB_LEFT, a
    jr nz, .moving_y\@
    bit PADB_RIGHT, a
    jr z, .not_moving_y\@
    .moving_y\@
        and ISOLATE_LEFT_RIGHT
    .not_moving_y\@
        ld [\1 + SPIRIT_SPRITE_ANIMATION], a
endm

macro ApplySpiritAnimationState
    ld a, [\1 + PREV_SPIRIT_SPRITE_ANIMATION]
    ld b, a
    ld a, [\1 + SPIRIT_SPRITE_ANIMATION]
    cp a, b
    jr z, .no_spirit_update_animation\@
    cp a, DOWN_ANIMATION_FLAG
    jr nz, .not_down_anim\@
        SpiritSetDownAnim \1, \2, \3
        jp .set_spirit_animation\@
    .not_down_anim\@
        ld a, [\1 + SPIRIT_SPRITE_ANIMATION]
        cp a, UP_ANIMATION_FLAG
    jr nz, .not_up_anim\@
        SpiritSetUpAnim \1, \2, \3
        jp .set_spirit_animation\@
    .not_up_anim\@
        ld a, [\1 + SPIRIT_SPRITE_ANIMATION]
        cp a, LEFT_ANIMATION_FLAG
    jr nz, .not_left_anim\@
        SpiritSetLeftAnim \1, \2, \3
        jp .set_spirit_animation\@
    .not_left_anim\@
        ld a, [\1 + SPIRIT_SPRITE_ANIMATION]
        cp a, RIGHT_ANIMATION_FLAG
    jr nz, .no_spirit_update_animation\@
        SpiritSetRightAnim \1, \2, \3
        jp .set_spirit_animation\@
    .set_spirit_animation\@
        copy [\1 + SPIRIT_ANIM_CHANGED], ANIM_CHANGED_FLAG
    .no_spirit_update_animation\@
endm

; updates the spirit animation in the x direction
; \1: Spirit data memory chunk
; \2: Spirit head OAM location, \3: Spirit legs OAM location
macro UpdateSpiritAnimationX
    ld a, [CHARACTER_SPRITE_WORLD_X]
    ld b, a 
    ld a, [\1 + SPIRIT_SPRITE_WORLD_X]
    cp a, b
    jr nc, .spirit_left_anim\@
    .spirit_right_anim\@
        ld a, [\1 + PREV_SPIRIT_SPRITE_ANIMATION]
        cp RIGHT_ANIMATION_FLAG
        jp z, .no_x_anim_update\@

        SpiritSetRightAnim \1, \2, \3
        jr .reset_spirit_animation_x\@
    .spirit_left_anim\@
        ld a, [\1 + PREV_SPIRIT_SPRITE_ANIMATION]
        cp LEFT_ANIMATION_FLAG
        jp z, .no_x_anim_update\@

        SpiritSetLeftAnim \1, \2, \3
    .reset_spirit_animation_x\@
        copy [\2 + OAMA_TILEID], [\1 + SPIRIT_FIRST_ANI_FRAME_HEAD]
        copy [\3 + OAMA_TILEID], [\1 + SPIRIT_FIRST_ANI_FRAME_LEGS]
    .no_x_anim_update\@
endm

; updates the spirit animation in the y direction
; \1: Spirit data memory chunk
; \2: Spirit head OAM location, \3: Spirit legs OAM location
macro UpdateSpiritAnimationY
    ld a, [CHARACTER_SPRITE_WORLD_Y]
    ld b, a
    ld a, [\1 + SPIRIT_SPRITE_WORLD_Y]
    cp a, b
    jr c, .spirit_down_anim\@
    .spirit_up_anim\@
        ld a, [\1 + PREV_SPIRIT_SPRITE_ANIMATION]
        cp UP_ANIMATION_FLAG
        jp z, .no_y_anim_update\@

        SpiritSetUpAnim \1, \2, \3
        jr .reset_spirit_animation_y\@
    .spirit_down_anim\@
        ld a, [\1 + PREV_SPIRIT_SPRITE_ANIMATION]
        cp DOWN_ANIMATION_FLAG
        jp z, .no_y_anim_update\@

        SpiritSetDownAnim \1, \2, \3
    .reset_spirit_animation_y\@
        copy [\2 + OAMA_TILEID], [\1 + SPIRIT_FIRST_ANI_FRAME_HEAD]
        copy [\3 + OAMA_TILEID], [\1 + SPIRIT_FIRST_ANI_FRAME_LEGS]
    .no_y_anim_update\@
endm

; animates the spirit
; \1: Spirit data memory chunk
; \2: Spirit head OAM location, \3: Spirit legs OAM location
macro AnimateSpirit
    ld a, [\1 + SPIRIT_ANIM_CHANGED]
    cp a, ANIM_CHANGED_FLAG
    jp z, .reset\@
    ; checks wether or not we want to animate on this v-blank
    ld a, [\1 + SPIRIT_ANI_DELAY]
    ld b, a
    ld a, [\1 + SPIRIT_ANI_DELAY_STATE]
    inc a
    and a, b
    ld [\1 + SPIRIT_ANI_DELAY_STATE], a
    jp nz, .end_animate\@
    ; if we want to animate check if we need to reset to the begining of the 
    ; sprite sheet
    ld a, [\1 + SPIRIT_LAST_ANI_FRAME_HEAD]
    ld b, a
    ld a, [\2 + OAMA_TILEID]
    cp a, b
    jp z, .reset\@
    ; move the head to the next animation
    inc a
    inc a
    ld [\2 + OAMA_TILEID], a
    ; move the legs to next animation
    ld a, [\3 + OAMA_TILEID]
    inc a
    inc a
    ld [\3 + OAMA_TILEID], a
    jp .end_animate\@
    ; if we needed to reset set the sprite back to their first animations in
    ; the sprite sheet
    .reset\@
        copy [\2 + OAMA_TILEID], [\1 + SPIRIT_FIRST_ANI_FRAME_HEAD]
        copy [\3 + OAMA_TILEID], [\1 + SPIRIT_FIRST_ANI_FRAME_LEGS]
        copy [\1 + SPIRIT_ANIM_CHANGED], ANIM_UNCHANGED_FLAG

        ld a, [\1 + SPIRIT_SPRITE_ANIMATION]
        cp a, LEFT_ANIMATION_FLAG
        jp nz, .reset_right_anim\@
            copy [\2 + OAMA_FLAGS], OAMF_PAL0
            copy [\3 + OAMA_FLAGS], OAMF_PAL0
            jr .end_animate\@
        .reset_right_anim\@
            copy [\2 + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP
            copy [\3 + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP
    .end_animate\@
endm

macro UpdateSpiritData
    UpdateSpiritIntendedMovement \1, \2, \3
    UpdateSpiritCollisionFlags \1, \2, \3, \4
    UpdateIntendedCollisionSpirit \1, \2, \3
    ApplyIntendedMovementSpirit \1, \2, \3
    UpdateSpiritAnimationState \1, \2, \3
    ApplySpiritAnimationState \1, \2, \3
endm

endc