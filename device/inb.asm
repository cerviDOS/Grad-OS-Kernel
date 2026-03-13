global inb

; extern uint8_t int(uint16_t port)
inb:
push EDX
mov DX, [ESP+8]

mov EAX, 0x0
in AL, DX

pop EDX
ret
