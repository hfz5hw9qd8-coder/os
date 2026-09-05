CC = /opt/llvm-mingw/bin/clang
LD = /opt/llvm-mingw/bin/ld.lld
OBJCOPY = /opt/llvm-mingw/bin/llvm-objcopy
NASM = nasm
QEMU = qemu-system-i386

CFLAGS = --target=i386-unknown-none \
         -m32 \
         -Oz \
         -ffreestanding \
         -fno-pie \
         -fno-stack-protector

all: os.img

boot.bin: boot.asm
	$(NASM) -f bin boot.asm -o boot.bin

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

kernel.elf: kernel.o linker.ld
	$(LD) -m elf_i386 -T linker.ld -o kernel.elf kernel.o

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary \
		--only-section=.text \
		--only-section=.rodata \
		kernel.elf kernel.bin

os.img: boot.bin kernel.bin
	@test $$(stat -f%z kernel.bin) -le 1536 || (echo "ERREUR: kernel.bin dépasse 1536 octets"; exit 1)
	dd if=/dev/zero of=os.img bs=512 count=4
	dd if=boot.bin of=os.img bs=512 count=1 conv=notrunc
	dd if=kernel.bin of=os.img bs=512 seek=1 conv=notrunc

run: os.img
	$(QEMU) -drive format=raw,file=os.img -k fr

size: kernel.bin
	stat -f%z kernel.bin

clean:
	rm -f boot.bin kernel.o kernel.elf kernel.bin os.img