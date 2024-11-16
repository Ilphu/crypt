 ; @file enemy.inc
; @authors Jack Kertscher and Garrett Rhoads
; @brief Enemy logic
; @date 2024-11-02

if !def(ENEMY_INC)
def ENEMY_INC equ 1

include "include/pointers.inc"
include "include/graphics.inc"
include "include/sprites.inc"
include "include/character.inc"

; Enemy animation constants
def INIT_ENEMY_ANIMATION_DELAY      equ ($0F)
def INIT_ENEMY_MOVEMENT_DELAY       equ ($03)

def SPRITE_COLLISION_RANGE_X        equ ($07)
def SPRITE_COLLISION_RANGE_Y        equ ($0F)

def DEFAULT_MOVEB_X                 equ ($00)
def DEFAULT_MOVEB_Y                 equ ($01)

; Relative memory position of enemy traits from \1
rsreset
def ENEMY_FIRST_ANI_FRAME_HEAD      rb 1 
def ENEMY_FIRST_ANI_FRAME_LEGS      rb 1 
def ENEMY_LAST_ANI_FRAME_HEAD       rb 1
def ENEMY_ANI_DELAY                 rb 1
def ENEMY_ANI_DELAY_STATE           rb 1
def ENEMY_SPRITE_WORLD_X            rb 1
def ENEMY_SPRITE_WORLD_Y            rb 1
def DISTANCE_TO_CHAR_X              rb 1
def DISTANCE_TO_CHAR_Y              rb 1
def ENEMY_SPRITE_ANIMATION          rb 1
def PREV_ENEMY_SPRITE_ANIMATION     rb 1
def ENEMY_MOVEMENT_DELAY            rb 1
def ENEMY_MOVEMENT_DELAY_STATE      rb 1
def ENEMY_INTENDED_MOVEMENT         rb 1
def ENEMY_ANIM_CHANGED              rb 1
def ENEMY_COLLISION_FLAGS           rb 1
def ENEMY_DEFAULT_PATHING_FLAGS     rb 1

def ENEMY_ATTRIBUTES_SIZE           equ (17)

; Clears the WRAM bank of enemy attributes starting at \1
macro ClearEnemyWram
    ld c, ENEMY_ATTRIBUTES_SIZE
    ld hl, \1
    .clear_enemy_wram\@
        ld [hl], 0
        inc hl
        dec c
        jr nz, .clear_enemy_wram\@
endm

; Initializing the enemy
; \1: Enemy data memory chunk
; \2: Enemy head OAM location, \3: Enemy legs OAM location
macro InitEnemy
    InitEnemyAnimation \1, \2, \3

    copy [\1 + ENEMY_SPRITE_WORLD_X], \4
    copy [\1 + ENEMY_SPRITE_WORLD_Y], \5
    copy [\1 + DISTANCE_TO_CHAR_X], CHARACTER_SPRITE_HEAD_X_START - \4
    copy [\1 + DISTANCE_TO_CHAR_Y], CHARACTER_SPRITE_HEAD_Y_START - \5

    copy [\1 + ENEMY_MOVEMENT_DELAY], INIT_ENEMY_MOVEMENT_DELAY
    copy [\1 + ENEMY_MOVEMENT_DELAY_STATE], INITIAL_DELAY

    ; Init the head and legs of our ENEMY sprite
    copy [\2 + OAMA_X], \4
    copy [\2 + OAMA_Y], \5
    copy [\2 + OAMA_TILEID], [\1 + ENEMY_FIRST_ANI_FRAME_HEAD]
    copy [\2 + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP
    
    copy [\3 + OAMA_X], \6
    copy [\3 + OAMA_Y], \7
    copy [\3 + OAMA_TILEID], [\1 + ENEMY_FIRST_ANI_FRAME_LEGS]
    copy [\3 + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP
endm

; Initializing the animations for the enemy
; \1: Enemy data memory chunk
; \2: Enemy head OAM location, \3: Enemy legs OAM location
macro InitEnemyAnimation
    copy [\1 + ENEMY_FIRST_ANI_FRAME_HEAD],    (SIDE_ANIMATION_START + ENEMY_ANIMATION_OFFSET)
    copy [\1 + ENEMY_FIRST_ANI_FRAME_LEGS],    (SIDE_ANIMATION_START + LEGS_TILE_OFFSET + ENEMY_ANIMATION_OFFSET)
    copy [\1 + ENEMY_LAST_ANI_FRAME_HEAD],     (SIDE_ANIMATION_END + ENEMY_ANIMATION_OFFSET)
    
    copy [\1 + ENEMY_SPRITE_ANIMATION],        (RIGHT_ANIMATION_FLAG)
    copy [\1 + PREV_ENEMY_SPRITE_ANIMATION],   (IDLE_ANIMATION_FLAG)
    copy [\1 + ENEMY_ANI_DELAY],               (INIT_ENEMY_ANIMATION_DELAY)
    copy [\1 + ENEMY_ANI_DELAY_STATE],         (INITIAL_DELAY)
endm

; updates the distance from enemy to the character in the x direction
; \1: Enemy data memory chunk
; \2: Enemy head OAM location, \3: Enemy legs OAM location
; data only
macro UpdateDistanceToCharX
    ld a, [CHARACTER_SPRITE_WORLD_X]
    ld b, a
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    cp a, b
    jr c, .less_than_x\@
        ; a >= b
        sub b
        ld [\1 + DISTANCE_TO_CHAR_X], a
        jr .end_update_distance_x\@
    .less_than_x\@
        ld a, [\1 + ENEMY_SPRITE_WORLD_X]
        ld b, a
        ld a, [CHARACTER_SPRITE_WORLD_X]
        sub b
        ld [\1 + DISTANCE_TO_CHAR_X], a
    .end_update_distance_x\@
endm

; updates the distance from enemy to the character in the y direction
; \1: Enemy data memory chunk
; \2: Enemy head OAM location, \3: Enemy legs OAM location
; data only
macro UpdateDistanceToCharY
    ld a, [CHARACTER_SPRITE_WORLD_Y]
    ld b, a
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    cp a, b
    jr c, .less_than_y\@
        ; a >= b
        sub b
        ld [\1 + DISTANCE_TO_CHAR_Y], a
        jr .end_update_distance_y\@
    .less_than_y\@
        ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
        ld b, a
        ld a, [CHARACTER_SPRITE_WORLD_Y]
        sub b
        ld [\1 + DISTANCE_TO_CHAR_Y], a
    .end_update_distance_y\@
endm

macro IntendedMovementX
        ld a, [CHARACTER_SPRITE_WORLD_X]
        ld b, a
        ld a, [\1 + ENEMY_SPRITE_WORLD_X]
        cp a, b
        jr c, .move_right\@
        ; move left
            ld a, [\1 + ENEMY_INTENDED_MOVEMENT]
            set PADB_LEFT, a 
            ld [\1 + ENEMY_INTENDED_MOVEMENT], a
            jr .check_diagonal\@
        .move_right\@
            ld a, [\1 + ENEMY_INTENDED_MOVEMENT]
            set PADB_RIGHT, a 
            ld [\1 + ENEMY_INTENDED_MOVEMENT], a
            
    .check_diagonal\@
endm

macro IntendedMovementY
    ld a, [CHARACTER_SPRITE_WORLD_Y]
    ld b, a
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    cp a, b
    jr c, .move_down\@
    ; move_up
        ld a, [\1 + ENEMY_INTENDED_MOVEMENT]
        set PADB_UP, a
        ld [\1 + ENEMY_INTENDED_MOVEMENT], a
        jr .end\@
    .move_down\@
        ld a, [\1 + ENEMY_INTENDED_MOVEMENT]
        set PADB_DOWN, a
        ld [\1 + ENEMY_INTENDED_MOVEMENT], a
    .end\@
endm

macro UpdateIntendedMovement
    xor a
    ld [\1 + ENEMY_INTENDED_MOVEMENT], a
    
    UpdateDistanceToCharX \1, \2, \3
    UpdateDistanceToCharY \1, \2, \3

    ld a, [\1 + DISTANCE_TO_CHAR_Y]
    ld b, a
    ld a, [\1 + DISTANCE_TO_CHAR_X]
    ; move in which ever axis is furthest away from the character
    ; If x < y, move y
    cp a, b
    jp c, .move_y\@
        IntendedMovementX \1, \2, \3
        ld a, [\1 + DISTANCE_TO_CHAR_Y]
        ld b, a
        ld a, [\1 + DISTANCE_TO_CHAR_X]
        cp a, b
        jr nz, .end\@
    .move_y\@ 
        IntendedMovementY \1, \2, \3
    .end\@
endm

; top left collision point (1 px above the leftmost pixel of the top edge)
macro UpdateEnemyTLCollisionFlag
    ; Y position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    dec a
    add a, TOP_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY

    ; X position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    add a, LEFT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit ; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_tl_collide\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        set COLLB_TOP_LEFT, a
        ld [\1 + ENEMY_COLLISION_FLAGS], a
    .no_tl_collide\@
endm

; Top right collision point (1px above the rightmost pixel of the top edge)
macro UpdateEnemyTRCollisionFlag
    ; Y position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    dec a
    add a, TOP_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    ; X position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    add a, TILE_WIDTH
    add a, RIGHT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_tr_collide\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        set COLLB_TOP_RIGHT, a
        ld [\1 + ENEMY_COLLISION_FLAGS], a
    .no_tr_collide\@
endm

; Right top collision point (1px to the right of the topmost pixel of the right edge)
macro UpdateEnemyRTCollisionFlag
    ; Y position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    add a, TOP_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    ; X position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    add a, TILE_WIDTH
    inc a
    add a, RIGHT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_rt_collide\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        set COLLB_RIGHT_TOP, a
        ld [\1 + ENEMY_COLLISION_FLAGS], a
    .no_rt_collide\@
endm

; Right bottom collision point (1px to the right of the bottommost pixel of the right edge)
macro UpdateEnemyRBCollisionFlag
    ; Y position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    add a, TILE_WIDTH
    add a, BOTTOM_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    ; X position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    add a, TILE_WIDTH
    inc a
    add a, RIGHT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_rb_collide\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        set COLLB_RIGHT_BOTTOM, a
        ld [\1 + ENEMY_COLLISION_FLAGS], a
    .no_rb_collide\@
endm

; Bottom right collision point (1px below the rightmost pixel of the bottom edge)
macro UpdateEnemyBRCollisionFlag
    ; Y position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    add a, TILE_WIDTH
    inc a
    add a, BOTTOM_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    ; X position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    add a, TILE_WIDTH
    add a, RIGHT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_br_collide\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        set COLLB_BOTTOM_RIGHT, a
        ld [\1 + ENEMY_COLLISION_FLAGS], a
    .no_br_collide\@
endm

; Bottom left collision point (1px below the leftmost pixel of the bottom edge)
macro UpdateEnemyBLCollisionFlag
    ; Y position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    add a, TILE_WIDTH
    inc a
    add a, BOTTOM_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    ; X position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    add a, LEFT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_bl_collide\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        set COLLB_BOTTOM_LEFT, a
        ld [\1 + ENEMY_COLLISION_FLAGS], a
    .no_bl_collide\@
endm

; Left bottom collision point (1px to the left of the bottommost pixel of the left edge)
macro UpdateEnemyLBCollisionFlag
    ; Y position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    add a, TILE_WIDTH
    add a, BOTTOM_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    ; X position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    dec a
    add a, LEFT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_lb_collide\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        set COLLB_LEFT_BOTTOM, a
        ld [\1 + ENEMY_COLLISION_FLAGS], a
    .no_lb_collide\@
endm

; Left top collision point (1px to the left of the topmost pixel of the left edge)
macro UpdateEnemyLTCollisionFlag
    ; Y position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    add a, TILE_WIDTH
    add a, TOP_BOUNDING_BOX_OFFSET
    ; Y portion of tile indexing
    TileIndexY
    
    ; X position of the point
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    dec a
    add a, LEFT_BOUNDING_BOX_OFFSET
    ; X portion of tile indexing
    TileIndexX
    ; Sets the carry flag to 1 if the (a)th bit of [hl] is a 1, else 0.
    vBit; a, [hl]
    ; Set that collision flag if the desired tile bit is a 1
    jr nc, .no_lt_collide\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        set COLLB_LEFT_TOP, a
        ld [\1 + ENEMY_COLLISION_FLAGS], a
    .no_lt_collide\@
endm

macro CheckRightSpriteCollision
    ld a, [\4 + ENEMY_SPRITE_WORLD_X]
    ld b, a
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    add a, SPRITE_COLLISION_RANGE_X
    inc a
    cp a, b
    jp c, .no_right_collision\@
    SwapAB
    add a, SPRITE_COLLISION_RANGE_X
    SwapAB
    cp a, b
    jp nc, .no_right_collision\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        set COLLB_RIGHT_TOP, a
        set COLLB_RIGHT_BOTTOM, a
        ld [\1 + ENEMY_COLLISION_FLAGS], a
    .no_right_collision\@
endm

macro CheckLeftSpriteCollision
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    ld b, a
    ld a, [\4 + ENEMY_SPRITE_WORLD_X]
    add a, SPRITE_COLLISION_RANGE_X
    inc a
    cp a, b
    jp c, .no_right_collision\@
    SwapAB
    add a, SPRITE_COLLISION_RANGE_X
    SwapAB
    cp a, b
    jp nc, .no_right_collision\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        set COLLB_LEFT_TOP, a
        set COLLB_LEFT_BOTTOM, a
        ld [\1 + ENEMY_COLLISION_FLAGS], a
    .no_right_collision\@
endm

macro CheckBottomSpriteCollision
    ld a, [\4 + ENEMY_SPRITE_WORLD_Y]
    ld b, a
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    add a, SPRITE_COLLISION_RANGE_Y
    inc a
    cp a, b
    jp c, .no_right_collision\@
    SwapAB
    add a, SPRITE_COLLISION_RANGE_Y
    SwapAB
    cp a, b
    jp nc, .no_right_collision\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        set COLLB_BOTTOM_LEFT, a
        set COLLB_BOTTOM_RIGHT, a
        ld [\1 + ENEMY_COLLISION_FLAGS], a
    .no_right_collision\@
endm

macro CheckTopSpriteCollision
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    ld b, a
    ld a, [\4 + ENEMY_SPRITE_WORLD_Y]
    add a, SPRITE_COLLISION_RANGE_Y
    inc a
    cp a, b
    jp c, .no_top_collision\@
    SwapAB
    add a, SPRITE_COLLISION_RANGE_Y
    SwapAB
    cp a, b
    jp nc, .no_top_collision\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        set COLLB_TOP_LEFT, a
        set COLLB_TOP_RIGHT, a
        ld [\1 + ENEMY_COLLISION_FLAGS], a
    .no_top_collision\@
endm

; \1, \2, \3: This enemy's data, head, legs
; \4: The other enemy's data
macro UpdateEnemySpriteCollisionFlags
    ld a, [\4 + ENEMY_SPRITE_WORLD_X]
    ld b, a
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    SubAbsolute

    cp a, SPRITE_COLLISION_RANGE_X
    jp nc, .no_collision\@
    ld a, [\4 + ENEMY_SPRITE_WORLD_Y]
    ld b, a
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    SubAbsolute

    cp a, SPRITE_COLLISION_RANGE_Y
    jp nc, .no_collision\@

    ; collision!!!!!!
        CheckRightSpriteCollision \1, \2, \3, \4
        CheckLeftSpriteCollision \1, \2, \3, \4
        CheckBottomSpriteCollision \1, \2, \3, \4

    .no_collision\@
endm

macro UpdateEnemyCollisionFlags
    xor a
    ld [\1 + ENEMY_COLLISION_FLAGS], a

    UpdateEnemyTLCollisionFlag \1, \2, \3
    UpdateEnemyTRCollisionFlag \1, \2, \3
    UpdateEnemyRTCollisionFlag \1, \2, \3
    UpdateEnemyRBCollisionFlag \1, \2, \3
    UpdateEnemyBRCollisionFlag \1, \2, \3
    UpdateEnemyBLCollisionFlag \1, \2, \3
    UpdateEnemyLBCollisionFlag \1, \2, \3
    UpdateEnemyLTCollisionFlag \1, \2, \3
    UpdateEnemySpriteCollisionFlags \1, \2, \3, \4
endm

macro ApplyIntendedX
    ; Move left
    ld a, [\1 + ENEMY_INTENDED_MOVEMENT]
    bit PADB_LEFT, a
    jp z, .not_left_move\@
        ld a, [\1 + ENEMY_SPRITE_WORLD_X]
        dec a
        ld [\1 + ENEMY_SPRITE_WORLD_X], a
    .not_left_move\@
    ; Move right
    ld a, [\1 + ENEMY_INTENDED_MOVEMENT]
    bit PADB_RIGHT, a
    jp z, .not_right_move\@
        ld a, [\1 + ENEMY_SPRITE_WORLD_X]
        inc a
        ld [\1 + ENEMY_SPRITE_WORLD_X], a
    .not_right_move\@
endm

macro ApplyIntendedY
    ; Move up
    ld a, [\1 + ENEMY_INTENDED_MOVEMENT]
    bit PADB_UP, a
    jp z, .not_up_move\@
        ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
        dec a
        ld [\1 + ENEMY_SPRITE_WORLD_Y], a
    .not_up_move\@
    ; Move down
    ld a, [\1 + ENEMY_INTENDED_MOVEMENT]
    bit PADB_DOWN, a
    jp z, .end_movement\@
        ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
        inc a
        ld [\1 + ENEMY_SPRITE_WORLD_Y], a
    .end_movement\@
endm

; Combines the intended movement and collision data into (a) such that
; the collision flags are only set if we are also moving in that direction 
macro CombineMoveCollide
    ld a, [\1 + ENEMY_INTENDED_MOVEMENT]
    or a, $0F
    ld b, a
    ld a, [\1 + ENEMY_COLLISION_FLAGS]

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

macro YCollisionPathfinding
    ld a, [\1 + ENEMY_DEFAULT_PATHING_FLAGS]
    bit DEFAULT_MOVEB_X, a
    jp nz, .defaulting_right\@
    ; Defaulting left
        ; if default movement left and wall left: Change default to right
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        bit COLLB_LEFT_BOTTOM, a
        jp nz, .change_default_to_right\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        bit COLLB_LEFT_TOP, a
        jp z, .set_intended_movement\@
    .change_default_to_right\@
        ; Changing default to right
        ld a, [\1 + ENEMY_DEFAULT_PATHING_FLAGS]
        set DEFAULT_MOVEB_X, a
        ld [\1 + ENEMY_DEFAULT_PATHING_FLAGS], a
        jp .set_intended_movement\@
    .defaulting_right\@
        ; if default movement right and wall right: Change default to left
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        bit COLLB_RIGHT_BOTTOM, a
        jp nz, .change_default_to_left\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        bit COLLB_RIGHT_TOP, a
        jp z, .set_intended_movement\@
    .change_default_to_left\@
        ; changing default to left
        ld a, [\1 + ENEMY_DEFAULT_PATHING_FLAGS]
        res DEFAULT_MOVEB_X, a
        ld [\1 + ENEMY_DEFAULT_PATHING_FLAGS], a
        jp .set_intended_movement\@
    .set_intended_movement\@
        ; Set the intended movement to the default X movement
        ld a, [\1 + ENEMY_DEFAULT_PATHING_FLAGS]
        bit DEFAULT_MOVEB_X, a
        jp nz, .set_move_right\@
        ; Set move left
            copy [\1 + ENEMY_INTENDED_MOVEMENT], PADF_LEFT
            jp .done\@
        .set_move_right\@
            copy [\1 + ENEMY_INTENDED_MOVEMENT], PADF_RIGHT
    .done\@
endm

macro XCollisionPathfinding
    ld a, [\1 + ENEMY_DEFAULT_PATHING_FLAGS]
    bit DEFAULT_MOVEB_Y, a
    jp nz, .defaulting_up\@
    ; Defaulting down
        ; if default movement down and wall down: Change default to up
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        bit COLLB_BOTTOM_LEFT, a
        jp nz, .change_default_to_up\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        bit COLLB_BOTTOM_RIGHT, a
        jp z, .set_intended_movement\@
    .change_default_to_up\@
        ; Changing default to right
        ld a, [\1 + ENEMY_DEFAULT_PATHING_FLAGS]
        set DEFAULT_MOVEB_Y, a
        ld [\1 + ENEMY_DEFAULT_PATHING_FLAGS], a
        jp .set_intended_movement\@
    .defaulting_up\@
        ; if default movement right and wall right: Change default to left
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        bit COLLB_TOP_RIGHT, a
        jp nz, .change_default_to_down\@
        ld a, [\1 + ENEMY_COLLISION_FLAGS]
        bit COLLB_TOP_LEFT, a
        jp z, .set_intended_movement\@
    .change_default_to_down\@
        ; changing default to up
        ld a, [\1 + ENEMY_DEFAULT_PATHING_FLAGS]
        res DEFAULT_MOVEB_Y, a
        ld [\1 + ENEMY_DEFAULT_PATHING_FLAGS], a
        jp .set_intended_movement\@
    .set_intended_movement\@
        ; Set the intended movement to the default Y movement
        ld a, [\1 + ENEMY_DEFAULT_PATHING_FLAGS]
        bit DEFAULT_MOVEB_Y, a
        jp nz, .set_move_up\@
        ; Set move down
            copy [\1 + ENEMY_INTENDED_MOVEMENT], PADF_DOWN
            jp .done\@
        .set_move_up\@
            copy [\1 + ENEMY_INTENDED_MOVEMENT], PADF_UP
    .done\@
endm

macro ResetXPathfinding
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    ld b, a
    ld a, [CHARACTER_SPRITE_WORLD_Y]
    cp a, b
    jp c, .enemy_below_char\@
    ; enemy above char
        ld a, [\1 + ENEMY_DEFAULT_PATHING_FLAGS]
        res DEFAULT_MOVEB_Y, a
        ld [\1 + ENEMY_DEFAULT_PATHING_FLAGS], a
        jp .done\@
    .enemy_below_char\@
        ld a, [\1 + ENEMY_DEFAULT_PATHING_FLAGS]
        set DEFAULT_MOVEB_Y, a
        ld [\1 + ENEMY_DEFAULT_PATHING_FLAGS], a
    .done\@
endm

macro ResetYPathfinding
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    ld b, a
    ld a, [CHARACTER_SPRITE_WORLD_X]
    cp a, b
    jp c, .enemy_right_char\@
    ; enemy left of char
        ld a, [\1 + ENEMY_DEFAULT_PATHING_FLAGS]
        set DEFAULT_MOVEB_X, a
        ld [\1 + ENEMY_DEFAULT_PATHING_FLAGS], a
        jp .done\@
    .enemy_right_char\@
        ld a, [\1 + ENEMY_DEFAULT_PATHING_FLAGS]
        res DEFAULT_MOVEB_X, a
        ld [\1 + ENEMY_DEFAULT_PATHING_FLAGS], a
    .done\@
endm

macro UpdateIntendedCollision
    CombineMoveCollide \1, \2, \3
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
        YCollisionPathfinding \1, \2, \3
        jp .done\@
    .no_colliding_Y\@
        ResetYPathfinding \1, \2, \3
    
        CombineMoveCollide \1, \2, \3
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
        XCollisionPathfinding \1, \2, \3
        jp .done\@
    .no_colliding_X\@
        ResetXPathfinding \1, \2, \3
        jp .done\@
    .no_change\@
        ResetXPathfinding \1, \2, \3
        ResetYPathfinding \1, \2, \3
    .done\@
endm

macro ApplyIntendedMovement
    ; Does the enemy move this framme?
    ld a, [\1 + ENEMY_MOVEMENT_DELAY]
    ld b, a
    ld a, [\1 + ENEMY_MOVEMENT_DELAY_STATE]
    inc a
    and a, b
    ld [\1 + ENEMY_MOVEMENT_DELAY_STATE], a
    jp nz, .end_movement\@
        ApplyIntendedX \1, \2, \3
        ApplyIntendedY \1, \2, \3
    .end_movement\@
endm

; Handles collisions with the character
; \1: Enemy data memory chunk
; \2: Enemy head OAM location, \3: Enemy legs OAM location
macro CharacterCollision
    ld a, [\1 + DISTANCE_TO_CHAR_X]
    cp a, SPRITE_COLLISION_RANGE_X
    jr nc, .no_collision_enemy\@
    ld a, [\1 + DISTANCE_TO_CHAR_Y]
    cp a, SPRITE_COLLISION_RANGE_Y
    jr nc, .no_collision_enemy\@
    ; collision!!!!!!
    copy [rGSF], rGSF_TITLE
    .no_collision_enemy\@
endm

; enemy left animation
; \1: Enemy data memory chunk
; \2: Enemy head OAM location, \3: Enemy legs OAM location
macro EnemySetLeftAnim
    copy [\1 + ENEMY_FIRST_ANI_FRAME_HEAD], SIDE_ANIMATION_START + ENEMY_ANIMATION_OFFSET
    copy [\1 + ENEMY_FIRST_ANI_FRAME_LEGS], SIDE_ANIMATION_START + LEGS_TILE_OFFSET + ENEMY_ANIMATION_OFFSET
    copy [\1 + ENEMY_LAST_ANI_FRAME_HEAD], SIDE_ANIMATION_END + ENEMY_ANIMATION_OFFSET
    ;copy [\2 + OAMA_FLAGS], OAMF_PAL0
    ;copy [\3 + OAMA_FLAGS], OAMF_PAL0
endm

; enemy right animation
; \1: Enemy data memory chunk
; \2: Enemy head OAM location, \3: Enemy legs OAM location
macro EnemySetRightAnim
    copy [\1 + ENEMY_FIRST_ANI_FRAME_HEAD], SIDE_ANIMATION_START + ENEMY_ANIMATION_OFFSET
    copy [\1 + ENEMY_FIRST_ANI_FRAME_LEGS], SIDE_ANIMATION_START + LEGS_TILE_OFFSET + ENEMY_ANIMATION_OFFSET
    copy [\1 + ENEMY_LAST_ANI_FRAME_HEAD], SIDE_ANIMATION_END + ENEMY_ANIMATION_OFFSET
    ;copy [\2 + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP
    ;copy [\3 + OAMA_FLAGS], OAMF_PAL0 | OAMF_XFLIP
endm

; enemy up animation
; \1: Enemy data memory chunk
; \2: Enemy head OAM location, \3: Enemy legs OAM location
macro EnemySetUpAnim
    copy [\1 + ENEMY_FIRST_ANI_FRAME_HEAD], UP_ANIMATION_START + ENEMY_ANIMATION_OFFSET
    copy [\1 + ENEMY_FIRST_ANI_FRAME_LEGS], UP_ANIMATION_START + LEGS_TILE_OFFSET + ENEMY_ANIMATION_OFFSET
    copy [\1 + ENEMY_LAST_ANI_FRAME_HEAD], UP_ANIMATION_END + ENEMY_ANIMATION_OFFSET
endm

; enemy down animation
; \1: Enemy data memory chunk
; \2: Enemy head OAM location, \3: Enemy legs OAM location
macro EnemySetDownAnim
    copy [\1 + ENEMY_FIRST_ANI_FRAME_HEAD], DOWN_ANIMATION_START + ENEMY_ANIMATION_OFFSET
    copy [\1 + ENEMY_FIRST_ANI_FRAME_LEGS], DOWN_ANIMATION_START + LEGS_TILE_OFFSET + ENEMY_ANIMATION_OFFSET
    copy [\1 + ENEMY_LAST_ANI_FRAME_HEAD], DOWN_ANIMATION_END + ENEMY_ANIMATION_OFFSET
endm

macro UpdateEnemyAnimationState
    copy [\1 + PREV_ENEMY_SPRITE_ANIMATION], [\1 + ENEMY_SPRITE_ANIMATION]

    ; Check if we are moving in the y direction, prioritize that animation
    ld a, [\1 + ENEMY_INTENDED_MOVEMENT]
    bit PADB_LEFT, a
    jr nz, .moving_y\@
    bit PADB_RIGHT, a
    jr z, .not_moving_y\@
    .moving_y\@
        and ISOLATE_LEFT_RIGHT
    .not_moving_y\@
        ld [\1 + ENEMY_SPRITE_ANIMATION], a
endm

macro ApplyEnemyAnimationState
    ld a, [\1 + PREV_ENEMY_SPRITE_ANIMATION]
    ld b, a
    ld a, [\1 + ENEMY_SPRITE_ANIMATION]
    cp a, b
    jr z, .no_enemy_update_animation\@
    cp a, DOWN_ANIMATION_FLAG
    jr nz, .not_down_anim\@
        EnemySetDownAnim \1, \2, \3
        jp .set_enemy_animation\@
    .not_down_anim\@
        ld a, [\1 + ENEMY_SPRITE_ANIMATION]
        cp a, UP_ANIMATION_FLAG
    jr nz, .not_up_anim\@
        EnemySetUpAnim \1, \2, \3
        jp .set_enemy_animation\@
    .not_up_anim\@
        ld a, [\1 + ENEMY_SPRITE_ANIMATION]
        cp a, LEFT_ANIMATION_FLAG
    jr nz, .not_left_anim\@
        EnemySetLeftAnim \1, \2, \3
        jp .set_enemy_animation\@
    .not_left_anim\@
        ld a, [\1 + ENEMY_SPRITE_ANIMATION]
        cp a, RIGHT_ANIMATION_FLAG
    jr nz, .no_enemy_update_animation\@
        EnemySetRightAnim \1, \2, \3
        jp .set_enemy_animation\@
    .set_enemy_animation\@
        copy [\1 + ENEMY_ANIM_CHANGED], ANIM_CHANGED_FLAG
    .no_enemy_update_animation\@
endm

; updates the enemy animation in the x direction
; \1: Enemy data memory chunk
; \2: Enemy head OAM location, \3: Enemy legs OAM location
macro UpdateEnemyAnimationX
    ld a, [CHARACTER_SPRITE_WORLD_X]
    ld b, a 
    ld a, [\1 + ENEMY_SPRITE_WORLD_X]
    cp a, b
    jr nc, .enemy_left_anim\@
    .enemy_right_anim\@
        ld a, [\1 + PREV_ENEMY_SPRITE_ANIMATION]
        cp RIGHT_ANIMATION_FLAG
        jp z, .no_x_anim_update\@

        EnemySetRightAnim \1, \2, \3
        jr .reset_enemy_animation_x\@
    .enemy_left_anim\@
        ld a, [\1 + PREV_ENEMY_SPRITE_ANIMATION]
        cp LEFT_ANIMATION_FLAG
        jp z, .no_x_anim_update\@

        EnemySetLeftAnim \1, \2, \3
    .reset_enemy_animation_x\@
        copy [\2 + OAMA_TILEID], [\1 + ENEMY_FIRST_ANI_FRAME_HEAD]
        copy [\3 + OAMA_TILEID], [\1 + ENEMY_FIRST_ANI_FRAME_LEGS]
    .no_x_anim_update\@
endm

; updates the enemy animation in the y direction
; \1: Enemy data memory chunk
; \2: Enemy head OAM location, \3: Enemy legs OAM location
macro UpdateEnemyAnimationY
    ld a, [CHARACTER_SPRITE_WORLD_Y]
    ld b, a
    ld a, [\1 + ENEMY_SPRITE_WORLD_Y]
    cp a, b
    jr c, .enemy_down_anim\@
    .enemy_up_anim\@
        ld a, [\1 + PREV_ENEMY_SPRITE_ANIMATION]
        cp UP_ANIMATION_FLAG
        jp z, .no_y_anim_update\@

        EnemySetUpAnim \1, \2, \3
        jr .reset_enemy_animation_y\@
    .enemy_down_anim\@
        ld a, [\1 + PREV_ENEMY_SPRITE_ANIMATION]
        cp DOWN_ANIMATION_FLAG
        jp z, .no_y_anim_update\@

        EnemySetDownAnim \1, \2, \3
    .reset_enemy_animation_y\@
        copy [\2 + OAMA_TILEID], [\1 + ENEMY_FIRST_ANI_FRAME_HEAD]
        copy [\3 + OAMA_TILEID], [\1 + ENEMY_FIRST_ANI_FRAME_LEGS]
    .no_y_anim_update\@
endm

; animates the enemy
; \1: Enemy data memory chunk
; \2: Enemy head OAM location, \3: Enemy legs OAM location
macro AnimateEnemy
    ld a, [\1 + ENEMY_ANIM_CHANGED]
    cp a, ANIM_CHANGED_FLAG
    jp z, .reset\@
    ; checks wether or not we want to animate on this v-blank
    ld a, [\1 + ENEMY_ANI_DELAY]
    ld b, a
    ld a, [\1 + ENEMY_ANI_DELAY_STATE]
    inc a
    and a, b
    ld [\1 + ENEMY_ANI_DELAY_STATE], a
    jp nz, .end_animate\@
    ; if we want to animate check if we need to reset to the begining of the 
    ; sprite sheet
    ld a, [\1 + ENEMY_LAST_ANI_FRAME_HEAD]
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
        copy [\2 + OAMA_TILEID], [\1 + ENEMY_FIRST_ANI_FRAME_HEAD]
        copy [\3 + OAMA_TILEID], [\1 + ENEMY_FIRST_ANI_FRAME_LEGS]
        copy [\1 + ENEMY_ANIM_CHANGED], ANIM_UNCHANGED_FLAG

        ld a, [\1 + ENEMY_SPRITE_ANIMATION]
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

macro UpdateEnemyData
    UpdateIntendedMovement \1, \2, \3
    UpdateEnemyCollisionFlags \1, \2, \3, \4
    UpdateEnemyCollisionFlags \1, \2, \3, \5
    UpdateEnemyCollisionFlags \1, \2, \3, \6
    UpdateIntendedCollision \1, \2, \3
    ApplyIntendedMovement \1, \2, \3
    UpdateEnemyAnimationState \1, \2, \3
    ApplyEnemyAnimationState \1, \2, \3
    CharacterCollision \1, \2, \3
endm

endc