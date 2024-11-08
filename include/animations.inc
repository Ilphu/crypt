; @file animations.inc
; @authors Jack Kertscher and Garrett Rhoads
; @brief animations constants
; @date 2024-11-02

if !def(ANIMATIONS_INC)
def ANIMATIONS_INC equ 1

include "include/pointers.inc"

def IDLE_ANIMATION_FLAG             equ ($00)
def DOWN_ANIMATION_FLAG             equ (%10000000)
def UP_ANIMATION_FLAG               equ (%01000000)
def LEFT_ANIMATION_FLAG             equ (%00100000)
def RIGHT_ANIMATION_FLAG            equ (%00010000)

def ANIM_CHANGED_FLAG               equ ($01)
def ANIM_UNCHANGED_FLAG             equ ($00)

def LEGS_TILE_OFFSET                equ ($01)

def DOWN_IDLE_ANIMATION_START       equ ($00)
def DOWN_IDLE_ANIMATION_END         equ ($00)
def UP_IDLE_ANIMATION_START         equ ($0C)
def UP_IDLE_ANIMATION_END           equ ($0C)
def SIDE_IDLE_ANIMATION_START       equ ($06)
def SIDE_IDLE_ANIMATION_END         equ ($06)

def DOWN_ANIMATION_START            equ ($02)
def DOWN_ANIMATION_END              equ ($04)

def UP_ANIMATION_START              equ ($0E)
def UP_ANIMATION_END                equ ($10)

def SIDE_ANIMATION_START            equ ($08)
def SIDE_ANIMATION_END              equ ($0A)

def ENEMY_ANIMATION_OFFSET          equ ($1B)

endc