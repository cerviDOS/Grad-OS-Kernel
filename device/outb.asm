global outb

; extern void outb(uint16_t port, uint8_t val)
outb:
push EAX                ; Don't clobber registers!
push EDX

mov EAX, [ESP+16]       ; After pushing above registers, the "port"
mov EDX, [ESP+12]       ; will be found at [ESP+16], and the value at [ESP+12}

out DX, AL

pop EAX
pop EDX

ret
