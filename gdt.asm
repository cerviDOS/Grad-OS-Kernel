;;; Global Descriptor Table Definition ;;;
gdt_start_segment:                   
        dq 0x0                  ; 8 bytes of zeros for null sector
gdt_code_segment:
        dw 0xFFFF               ; First 16 bits of limit
        dw 0x0000               ; First 16 bits of base
        db 0x00                 ; Second 8 bits of base
        db 0x9A                 ; Access byte (1001 1010)
        db 0xCF                 ; Flag nibble + last 4 bits of limit
        db 0x00                 ; Last 8 bits of base
gdt_data_segment:
        dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x92                 ; Access byte (1001 0010)
        db 0xCF
        db 0x00
gdt_end:
gdt_descriptor:
        dw gdt_end - gdt_start_segment - 1 ; Address of last byte
        dd gdt_start_segment               ; Base address      	

        CODE_SEGMENT_INDEX equ gdt_code_segment - gdt_start_segment
        DATA_SEGMENT_INDEX equ gdt_data_segment - gdt_start_segment
