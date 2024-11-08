; @file printing.inc
; @author Jack Kertscher
; @brief Printing constants
; @date 2024-11-02

if !def(PRINTING_INC)
def PRINTING_INC equ 1

; Printing constants
def ASCII_TO_TILESET_OFFSET         equ ($20)

def LEVEL_2_PRINT_LOC_1           equ ($9C22)
def LEVEL_2_PRINT_LOC_2           equ ($9C45)
def CLEAR_PRINT_LOC_1             equ ($9C21)
def CLEAR_PRINT_LOC_2             equ ($9C41)
def LOSS_PRINT_LOC                equ ($9C21)

endc