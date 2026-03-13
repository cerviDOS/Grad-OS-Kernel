;;; Real Mode Instructions ;;;
[bits 16]
[org 0x7C00]

;; Load Kernel into Memory

; Store drive number
mov [boot_drive], DL

; Get registers ready for disk read
mov AH, 02h
mov AL, NUM_SECTORS
mov EBX, KERNEL_ADDRESS
mov CH, 0x00
mov CL, 0x02
mov DH, 0x00
mov DL, [boot_drive]

; Perform disk read
int 13h

; Check for disk read error
jc .disk_read_error

; Check if correct number of sectors have been read
cmp AL, NUM_SECTORS
jnz .incorrect_sector_count_error

; Disk read successful, continue on
mov SI, DISK_READ_SUCCESS_MSG
call print_str_real_mode

jmp load_gdt

.disk_read_error:
mov SI, DISK_READ_ERR_MSG
jmp .print_error_and_hang 

.incorrect_sector_count_error:
mov SI, INCORRECT_SECTOR_COUNT_ERR_MSG
jmp .print_error_and_hang

.print_error_and_hang:
call print_str_real_mode
jmp $

;; Load gdt and turn on protected flag in CR0
load_gdt:
cli
lgdt [gdt_descriptor]
mov EAX, CR0
or EAX, 0x01
mov CR0, EAX

; Perform far jump
jmp CODE_SEGMENT_INDEX:begin_32_bit_mode

;; Displays the string at the address stored in the SI register
print_str_real_mode:
pusha
mov AH, 0x0E

.print_str_lp:
lodsb
cmp AL, 0
je .print_str_exit

int 10h
jmp .print_str_lp

.print_str_exit:
popa
ret

;;; Protected Mode Instructions ;;;
[bits 32]

begin_32_bit_mode:
; Reset segment registers
mov AX, DATA_SEGMENT_INDEX
mov DS, AX
mov SS, AX
mov ES, AX
mov FS, AX
mov GS, AX

; Update stack pointers to allow larger stack space
mov EBP, 0x90000
mov ESP, EBP

; If we're actually in protected mode, we should
; be able to modify the VGA buffer directly
;;        mov SI, PROTECTED_MODE_MSG
;;        call print_str_protected_mode

; Jump to 0x1000 --- where we have loaded our kernel
; to begin executing C code
jmp KERNEL_ADDRESS

print_str_protected_mode:
pusha
mov AH, 0x0E
mov EBX, VGA_BUFFER
mov CL, 0x05

.print_str_lp:
lodsb

cmp AL, 0
jz .print_str_exit

mov [EBX], AL
mov [EBX+1], CL

add EBX, 2

jmp .print_str_lp
.print_str_exit:
popa
ret

;;; Constants and Variables  ;;;
; Definitions for the global descriptor table
%include "gdt.asm"

; Address for kernel to be loaded
KERNEL_ADDRESS equ 0x8000

; Number of sectors to read
NUM_SECTORS equ ((FILE_SIZE-1)/512)+1

; Drive number for disk reading
boot_drive db 0

; Address of VGA Buffer
VGA_BUFFER equ 0xb8000

; Address of message for print_string to display
message dw 0

; Messages
DISK_READ_ERR_MSG db "BOOT-ERR: a disk read error has occured!", 0
INCORRECT_SECTOR_COUNT_ERR_MSG db "BOOT-ERR: incorrect number of sectors read!", 0
DISK_READ_SUCCESS_MSG db "BOOT-INFO: successfully read kernel from disk", 0
PROTECTED_MODE_MSG db "BOOT-INFO: successfully entered protected mode", 0

; Pad file until it reaches 510 bytes, then set last 2 bytes to boot signature
times 510-($-$$) db 0	
dw 0xAA55
