C_FILES = ${wildcard *.c ./**/*.c}
O_FILES = ${C_FILES:.c=.o}
ASM_O_FILES = ./device/outb.o ./device/inb.o

all: qemu_launch

qemu_launch: os.bin
	qemu-system-i386 -drive format=raw,file=$<

os.bin: boot.bin kernel.bin
	cat $^ > $@

boot.bin: kernel.bin boot.asm
	FILE_SIZE=$$(ls -l $< | sed 's/  */ /g' | cut -d' ' -f5); \
	sed "s/FILE_SIZE/$$FILE_SIZE/" boot.asm > withFileSize.asm
	nasm withFileSize.asm -f bin -o $@
	rm withFileSize.asm

kernel.bin: kernel_entry.o ${ASM_O_FILES} ${O_FILES}
	x86_64-linux-gnu-ld -m elf_i386 -s -o $@ -Ttext 0x8000 $^ --oformat binary

kernel_entry.o: kernel_entry.elf
	nasm $< -f elf -o $@

${ASM_O_FILES}:
	nasm ${@:.o=.asm} -f elf -o $@

${O_FILES}:
	x86_64-elf-gcc -fno-pie -m32 -ffreestanding -Iinclude -Wall -c ${@:.o=.c} -o $@

clean:
	$(RM) *.bin
	find . -name \*.o | xargs --no-run-if-empty rm

