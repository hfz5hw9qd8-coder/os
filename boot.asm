bits 16
org 0x7C00

start:
    ; Sauvegarder le disque de boot
    mov [boot_drive], dl

    ; -------------------------------------------------
    ; Charger le kernel à l'adresse 0x1000
    ; Kernel = 11 secteurs
    ; Secteurs disque : 2 -> 12
    ; -------------------------------------------------

    xor ax, ax
    mov es, ax
    mov bx, 0x1000

    mov ah, 0x02
    mov al, 0x0B
    mov ch, 0x00
    mov cl, 0x02
    mov dh, 0x00
    mov dl, [boot_drive]

    int 0x13

    jc disk_error


    ; -------------------------------------------------
    ; Récupérer les informations VBE du mode 0x192
    ; 1920x1080x32
    ; -------------------------------------------------

    mov ax, 0x4F01
    mov cx, 0x0192

    xor bx, bx
    mov es, bx
    mov di, 0x8000

    int 0x10

    cmp ax, 0x004F
    jne vbe_error


    ; -------------------------------------------------
    ; Sauvegarder les informations framebuffer
    ;
    ; 0x7000 = adresse framebuffer
    ; 0x7004 = pitch
    ; 0x7008 = bits par pixel
    ; -------------------------------------------------

    mov eax, [0x8028]
    mov [0x7000], eax

    mov ax, [0x8010]
    mov [0x7004], ax

    mov al, [0x8019]
    mov [0x7008], al


    ; -------------------------------------------------
    ; Activer le mode 1920x1080x32
    ;
    ; Mode VBE = 0x0192
    ; LFB      = 0x4000
    ;
    ; BX = 0x4192
    ; -------------------------------------------------

    mov ax, 0x4F02
    mov bx, 0x4192

    int 0x10

    cmp ax, 0x004F
    jne vbe_error


    ; -------------------------------------------------
    ; Passer en protected mode
    ; -------------------------------------------------

    cli

    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump obligatoire pour charger CS
    jmp dword 0x08:protected_mode


; =====================================================
; ERREUR DISQUE
; =====================================================

disk_error:
    cli

disk_error_loop:
    hlt
    jmp disk_error_loop


; =====================================================
; ERREUR VBE
; =====================================================

vbe_error:
    cli

vbe_error_loop:
    hlt
    jmp vbe_error_loop


; =====================================================
; PROTECTED MODE
; =====================================================

bits 32

protected_mode:

    ; -------------------------------------------------
    ; Charger le segment de données
    ; -------------------------------------------------

    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax


    ; -------------------------------------------------
    ; Stack
    ; -------------------------------------------------

    mov esp, 0x90000


    ; -------------------------------------------------
    ; Saut ABSOLU vers le kernel
    ; Kernel chargé à l'adresse physique 0x1000
    ; -------------------------------------------------

    mov eax, 0x1000
    jmp eax


; =====================================================
; GDT
; =====================================================

gdt_start:

    ; -------------------------------------------------
    ; Null descriptor
    ; -------------------------------------------------

    dq 0


    ; -------------------------------------------------
    ; Code segment
    ; Selector = 0x08
    ; -------------------------------------------------

gdt_code:

    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00


    ; -------------------------------------------------
    ; Data segment
    ; Selector = 0x10
    ; -------------------------------------------------

gdt_data:

    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00


gdt_end:


    ; -------------------------------------------------
    ; GDTR
    ; -------------------------------------------------

gdt_descriptor:

    dw gdt_end - gdt_start - 1
    dd gdt_start


; =====================================================
; DISQUE DE BOOT
; =====================================================

boot_drive:
    db 0


; =====================================================
; SIGNATURE BIOS
; =====================================================

times 510 - ($ - $$) db 0

dw 0xAA55