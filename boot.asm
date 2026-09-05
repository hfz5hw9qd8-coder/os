bits 16
org 0x7C00

start:
    ; Sauvegarder le numéro du disque fourni par le BIOS
    mov [boot_drive], dl

    ; Charger le kernel à l'adresse physique 0x1000
    xor ax, ax
    mov es, ax
    mov bx, 0x1000

    ; Lire le secteur 2
    mov ah, 0x02
    mov al, 0x02
    mov ch, 0x00
    mov cl, 0x02
    mov dh, 0x00
    mov dl, [boot_drive]

    int 0x13
    jc disk_error

    ; Maintenant seulement, désactiver les interruptions
    cli

    ; Charger la GDT
    lgdt [gdt_descriptor]

    ; Passer en protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump vers le code 32 bits
    jmp dword 0x08:protected_mode


disk_error:
    ; En cas d'erreur disque, rester ici
    jmp disk_error


bits 32

protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Initialiser la pile
    mov esp, 0x90000

    ; Afficher P
    mov byte [0xB8000], 'P'
    mov byte [0xB8001], 0x07

    ; Lancer le kernel chargé à 0x1000
    jmp 0x1000


; =========================
; GDT
; =========================

gdt_start:

    ; Entrée 0 : NULL
    dq 0

gdt_code:
    ; Segment code 32 bits
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

gdt_data:
    ; Segment données 32 bits
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

boot_drive:
    db 0

times 510 - ($ - $$) db 0
dw 0xAA55