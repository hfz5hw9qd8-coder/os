#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080
#define BYTES_PER_PIXEL 4

#define WINDOW_X 160
#define WINDOW_Y 100
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 880
#define TITLE_HEIGHT 40
#define CONTENT_X 200
#define CONTENT_Y 160

void print(const char* message);
void print_color(const char* message, unsigned char color);
void clear();
unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char value);

int cursor = 0;
int shift = 0;

char command[80];
int command_length = 0;

int text_x = 80;
int text_y = 100;

const unsigned char keyboard_map[64] = {
    [0x02] = '&', [0x03] = 'e', [0x04] = '"', [0x05] = '\'',
    [0x06] = '(', [0x07] = '-', [0x08] = 'e', [0x09] = '_',
    [0x0A] = 'c', [0x0B] = 'a', [0x0C] = ')', [0x0D] = '=',
    [0x10] = 'a', [0x11] = 'z', [0x12] = 'e', [0x13] = 'r',
    [0x14] = 't', [0x15] = 'y', [0x16] = 'u', [0x17] = 'i',
    [0x18] = 'o', [0x19] = 'p', [0x1A] = '^', [0x1B] = '$',
    [0x1C] = '\n',
    [0x1E] = 'q', [0x1F] = 's', [0x20] = 'd', [0x21] = 'f',
    [0x22] = 'g', [0x23] = 'h', [0x24] = 'j', [0x25] = 'k',
    [0x26] = 'l', [0x27] = 'm',
    [0x2C] = 'w', [0x2D] = 'x', [0x2E] = 'c', [0x2F] = 'v',
    [0x30] = 'b', [0x31] = 'n', [0x32] = ',', [0x33] = ';',
    [0x34] = ':', [0x35] = '!', [0x39] = ' '
};

const unsigned char keyboard_shift_map[64] = {
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0', [0x0C] = '0', [0x0D] = '+',
    [0x10] = 'A', [0x11] = 'Z', [0x12] = 'E', [0x13] = 'R',
    [0x14] = 'T', [0x15] = 'Y', [0x16] = 'U', [0x17] = 'I',
    [0x18] = 'O', [0x19] = 'P', [0x1C] = '\n',
    [0x1E] = 'Q', [0x1F] = 'S', [0x20] = 'D', [0x21] = 'F',
    [0x22] = 'G', [0x23] = 'H', [0x24] = 'J', [0x25] = 'K',
    [0x26] = 'L', [0x27] = 'M',
    [0x2C] = 'W', [0x2D] = 'X', [0x2E] = 'C', [0x2F] = 'V',
    [0x30] = 'B', [0x31] = 'N', [0x32] = '?', [0x33] = '.',
    [0x34] = '/', [0x39] = ' '
};

int strcmp(const char* a, const char* b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

const unsigned char font[][5] = {
    {0x1E,0x05,0x05,0x1E,0x00},{0x1F,0x15,0x15,0x0A,0x00},
    {0x0E,0x11,0x11,0x11,0x00},{0x1F,0x11,0x11,0x0E,0x00},
    {0x1F,0x15,0x15,0x11,0x00},{0x1F,0x05,0x05,0x01,0x00},
    {0x0E,0x11,0x15,0x1D,0x00},{0x1F,0x04,0x04,0x1F,0x00},
    {0x11,0x1F,0x11,0x00,0x00},{0x08,0x10,0x10,0x0F,0x00},
    {0x1F,0x04,0x0A,0x11,0x00},{0x1F,0x10,0x10,0x10,0x00},
    {0x1F,0x02,0x04,0x02,0x1F},{0x1F,0x02,0x04,0x08,0x1F},
    {0x0E,0x11,0x11,0x0E,0x00},{0x1F,0x05,0x05,0x02,0x00},
    {0x0E,0x11,0x19,0x1E,0x00},{0x1F,0x05,0x0D,0x12,0x00},
    {0x12,0x15,0x15,0x09,0x00},{0x01,0x1F,0x01,0x01,0x00},
    {0x0F,0x10,0x10,0x0F,0x00},{0x07,0x08,0x10,0x08,0x07},
    {0x0F,0x10,0x0C,0x10,0x0F},{0x11,0x0A,0x04,0x0A,0x11},
    {0x03,0x04,0x18,0x04,0x03},{0x19,0x15,0x13,0x00,0x00},
    {0x0E,0x11,0x11,0x0E,0x00},{0x12,0x1F,0x10,0x00,0x00},
    {0x19,0x15,0x15,0x12,0x00},{0x11,0x15,0x15,0x0A,0x00},
    {0x07,0x04,0x1F,0x04,0x00},{0x17,0x15,0x15,0x09,0x00},
    {0x0E,0x15,0x15,0x08,0x00},{0x01,0x01,0x1D,0x03,0x00},
    {0x0A,0x15,0x15,0x0A,0x00},{0x02,0x15,0x15,0x0E,0x00}
};

void putpixel(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
    unsigned int framebuffer=*(unsigned int*)0x7000;
    unsigned short pitch=*(unsigned short*)0x7004;
    volatile unsigned char* video=(volatile unsigned char*)framebuffer;
    if (x<0 || x>=SCREEN_WIDTH || y<0 || y>=SCREEN_HEIGHT) return;
    unsigned int offset=y*pitch+x*BYTES_PER_PIXEL;
    video[offset+0]=b; video[offset+1]=g; video[offset+2]=r;
}

void fill_rect(int x,int y,int width,int height,unsigned char r,unsigned char g,unsigned char b)
{
    unsigned int framebuffer=*(unsigned int*)0x7000;
    unsigned short pitch=*(unsigned short*)0x7004;
    volatile unsigned char* video=(volatile unsigned char*)framebuffer;
    for (int py=y;py<y+height;py++) {
        unsigned int offset=py*pitch+x*BYTES_PER_PIXEL;
        for (int px=0;px<width;px++) {
            video[offset+0]=b; video[offset+1]=g; video[offset+2]=r;
            offset+=BYTES_PER_PIXEL;
        }
    }
}

void graphics_test()
{
    unsigned int framebuffer=*(unsigned int*)0x7000;
    unsigned short pitch=*(unsigned short*)0x7004;
    volatile unsigned char* video=(volatile unsigned char*)framebuffer;
    for (int y=0;y<SCREEN_HEIGHT;y++) {
        for (int x=0;x<SCREEN_WIDTH;x++) {
            unsigned int offset=y*pitch+x*BYTES_PER_PIXEL;
            video[offset+0]=30; video[offset+1]=30; video[offset+2]=35;
        }
    }
    for (int y=WINDOW_Y;y<WINDOW_Y+WINDOW_HEIGHT;y++) {
        for (int x=WINDOW_X;x<WINDOW_X+WINDOW_WIDTH;x++) {
            unsigned int offset=y*pitch+x*BYTES_PER_PIXEL;
            video[offset+0]=230; video[offset+1]=230; video[offset+2]=230;
        }
    }
    for (int y=WINDOW_Y;y<WINDOW_Y+TITLE_HEIGHT;y++) {
        for (int x=WINDOW_X;x<WINDOW_X+WINDOW_WIDTH;x++) {
            unsigned int offset=y*pitch+x*BYTES_PER_PIXEL;
            video[offset+0]=60; video[offset+1]=60; video[offset+2]=65;
        }
    }
    for (int y=WINDOW_Y+10;y<WINDOW_Y+28;y++) {
        for (int x=WINDOW_X+14;x<WINDOW_X+32;x++) {
            unsigned int offset=y*pitch+x*BYTES_PER_PIXEL;
            video[offset+0]=220; video[offset+1]=60; video[offset+2]=60;
        }
    }
    for (int y=WINDOW_Y+10;y<WINDOW_Y+28;y++) {
        for (int x=WINDOW_X+40;x<WINDOW_X+58;x++) {
            unsigned int offset=y*pitch+x*BYTES_PER_PIXEL;
            video[offset+0]=230; video[offset+1]=190; video[offset+2]=50;
        }
    }
    for (int y=WINDOW_Y+10;y<WINDOW_Y+28;y++) {
        for (int x=WINDOW_X+66;x<WINDOW_X+84;x++) {
            unsigned int offset=y*pitch+x*BYTES_PER_PIXEL;
            video[offset+0]=60; video[offset+1]=190; video[offset+2]=80;
        }
    }
}

void draw_char(int x,int y,char c,unsigned char r,unsigned char g,unsigned char b)
{
    const unsigned char* glyph;
    if (c>='a' && c<='z') c-='a'-'A';
    if (c>='A' && c<='Z') glyph=font[c-'A'];
    else if (c>='0' && c<='9') glyph=font[26+(c-'0')];
    else if (c=='.') {
        putpixel(x+2,y+6,r,g,b); putpixel(x+3,y+6,r,g,b);
        putpixel(x+2,y+7,r,g,b); putpixel(x+3,y+7,r,g,b); return;
    } else if (c=='>') {
        putpixel(x,y,r,g,b); putpixel(x+1,y+1,r,g,b);
        putpixel(x+2,y+2,r,g,b); putpixel(x+1,y+3,r,g,b);
        putpixel(x,y+4,r,g,b); return;
    } else return;
    for (int col=0;col<5;col++) {
        for (int row=0;row<7;row++) {
            if (glyph[col] & (1<<row)) {
                int px=x+col*2, py=y+row*2;
                putpixel(px,py,r,g,b); putpixel(px+1,py,r,g,b);
                putpixel(px,py+1,r,g,b); putpixel(px+1,py+1,r,g,b);
            }
        }
    }
}

void draw_string(int x,int y,const char* text,unsigned char r,unsigned char g,unsigned char b)
{
    while (*text!='\0') {
        if (*text==' ') x+=12;
        else { draw_char(x,y,*text,r,g,b); x+=12; }
        text++;
    }
}

void clear_terminal()
{
    fill_rect(WINDOW_X,WINDOW_Y+TITLE_HEIGHT,WINDOW_WIDTH,WINDOW_HEIGHT-TITLE_HEIGHT,230,230,230);
    text_x=CONTENT_X; text_y=CONTENT_Y;
}

void redraw_command()
{
    unsigned int framebuffer=*(unsigned int*)0x7000;
    unsigned short pitch=*(unsigned short*)0x7004;
    volatile unsigned char* video=(volatile unsigned char*)framebuffer;
    for (int y=text_y;y<text_y+18;y++) {
        unsigned int offset=y*pitch+WINDOW_X*BYTES_PER_PIXEL;
        for (int x=0;x<WINDOW_WIDTH;x++) {
            video[offset+0]=230; video[offset+1]=230; video[offset+2]=230;
            offset+=BYTES_PER_PIXEL;
        }
    }
    draw_string(CONTENT_X,text_y,"> ",20,20,20);
    draw_string(CONTENT_X+24,text_y,command,20,20,20);
    text_x=CONTENT_X+24+command_length*12;
}

void execute_command()
{
    if (strcmp(command,"help")) {
        text_y+=16;
        draw_string(CONTENT_X,text_y,"COMMANDES",20,20,20);
        text_y+=16;
        draw_string(CONTENT_X,text_y,"HELP ABOUT CLEAR",20,20,20);
    } else if (strcmp(command,"about")) {
        text_y+=16;
        draw_string(CONTENT_X,text_y,"MON OS VERSION 0.1",20,20,20);
    } else if (strcmp(command,"clear")) {
        clear_terminal();
    } else if (command_length>0) {
        text_y+=16;
        draw_string(CONTENT_X,text_y,"COMMANDE INCONNUE",20,20,20);
    }
    command_length=0; command[0]='\0';
    if (text_y>WINDOW_Y+WINDOW_HEIGHT-50) clear_terminal();
    draw_string(CONTENT_X,text_y,"> ",20,20,20);
    text_x=CONTENT_X+24;
}

int mouse_x=300,mouse_y=180;
int mouse_packet_index=0;
unsigned char mouse_packet[3];
unsigned char mouse_buttons=0;

void mouse_wait_write()
{
    int timeout=100000;
    while ((inb(0x64)&2) && timeout>0) timeout--;
}

void mouse_wait_read()
{
    int timeout=100000;
    while (!(inb(0x64)&1) && timeout>0) timeout--;
}

unsigned char mouse_command(unsigned char command)
{
    mouse_wait_write(); outb(0x64,0xD4);
    mouse_wait_write(); outb(0x60,command);
    mouse_wait_read(); return inb(0x60);
}

void mouse_init()
{
    mouse_wait_write(); outb(0x64,0xA8);
    mouse_command(0xF6); mouse_command(0xF4);
}

void mouse_cursor_xor(int x,int y)
{
    unsigned int framebuffer=*(unsigned int*)0x7000;
    unsigned short pitch=*(unsigned short*)0x7004;
    volatile unsigned char* video=(volatile unsigned char*)framebuffer;
    int points[24][2]={{0,0},{1,0},{0,1},{1,1},{0,2},{1,2},{2,2},{3,2},
        {0,3},{1,3},{2,3},{0,4},{1,4},{2,4},{0,5},{1,5},{3,5},
        {0,6},{1,6},{4,6},{0,7},{5,7},{0,8},{6,8}};
    for (int i=0;i<24;i++) {
        int px=x+points[i][0],py=y+points[i][1];
        if (px>=0 && px<SCREEN_WIDTH && py>=0 && py<SCREEN_HEIGHT) {
            unsigned int offset=py*pitch+px*BYTES_PER_PIXEL;
            video[offset+0]^=0xFF; video[offset+1]^=0xFF; video[offset+2]^=0xFF;
        }
    }
}

void mouse_handle_click(unsigned char buttons)
{
    int left_down=buttons&1;
    int was_left_down=mouse_buttons&1;
    if (left_down && !was_left_down) {
        int bx=WINDOW_X+14;
        int by=WINDOW_Y+10;
        if (mouse_x>=bx && mouse_x<bx+18 && mouse_y>=by && mouse_y<by+18) {
            /* Fermer visuellement la fenetre sans redessiner tout l'ecran. */
            mouse_cursor_xor(mouse_x,mouse_y);
            fill_rect(WINDOW_X,WINDOW_Y,WINDOW_WIDTH,WINDOW_HEIGHT,30,30,35);
            mouse_cursor_xor(mouse_x,mouse_y);
        }
    }
    mouse_buttons=buttons;
}

void mouse_process_byte(unsigned char value)
{
    mouse_packet[mouse_packet_index]=value;
    if (mouse_packet_index==0 && !(value&0x08)) return;
    mouse_packet_index++;
    if (mouse_packet_index<3) return;
    mouse_packet_index=0;
    if (mouse_packet[0]&0xC0) return;
    int dx=(int)(signed char)mouse_packet[1];
    int dy=(int)(signed char)mouse_packet[2];
    mouse_cursor_xor(mouse_x,mouse_y);
    mouse_x+=dx; mouse_y-=dy;
    if (mouse_x<0) mouse_x=0;
    if (mouse_y<0) mouse_y=0;
    if (mouse_x>SCREEN_WIDTH-8) mouse_x=SCREEN_WIDTH-8;
    if (mouse_y>SCREEN_HEIGHT-10) mouse_y=SCREEN_HEIGHT-10;
    mouse_buttons=mouse_packet[0]&0x07;
    mouse_cursor_xor(mouse_x,mouse_y);
}

__attribute__((section(".text.start")))
void _start()
{
    graphics_test();
    draw_string(CONTENT_X,190,"BIENVENUE DANS MON OS",20,20,20);
    draw_string(CONTENT_X,220,"VERSION 0.1",20,20,20);
    draw_string(CONTENT_X,250,"SYSTEME PRET",20,20,20);
    text_x=CONTENT_X; text_y=300;
    draw_string(CONTENT_X,text_y,"> ",20,20,20);
    text_x=CONTENT_X+24;
    mouse_init();
    mouse_cursor_xor(mouse_x,mouse_y);

    while (1) {
        if (inb(0x64)&1) {
            unsigned char status=inb(0x64);
            unsigned char value=inb(0x60);
            if (status&0x20) {
                unsigned char buttons=value;
                mouse_process_byte(buttons);
                continue;
            }
            unsigned char scancode=value;
            if (scancode==0x2A || scancode==0x36) { shift=1; continue; }
            if (scancode==0xAA || scancode==0xB6) { shift=0; continue; }
            if (scancode==0x0E) {
                if (command_length>0) {
                    command_length--;
                    command[command_length]='\0';
                    redraw_command();
                }
                continue;
            }
            if (scancode&0x80) continue;
            if (scancode<sizeof(keyboard_map)) {
                unsigned char key=shift?keyboard_shift_map[scancode]:keyboard_map[scancode];
                if (key==0) continue;
                if (key=='\n') {
                    command[command_length]='\0';
                    execute_command();
                    continue;
                }
                if (command_length<79) {
                    char temp[2]; temp[0]=key; temp[1]='\0';
                    draw_string(text_x,text_y,temp,20,20,20);
                    command[command_length]=key;
                    command_length++;
                    text_x+=12;
                }
            }
        }
    }
}

unsigned char inb(unsigned short port)
{
    unsigned char value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void outb(unsigned short port,unsigned char value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value),"Nd"(port));
}

void clear()
{
    volatile unsigned char* video=(volatile unsigned char*)0xB8000;
    for (int i=0;i<80*25;i++) { video[i*2]=' '; video[i*2+1]=0x07; }
    cursor=0;
}

void print(const char* message)
{
    print_color(message,0x07);
}

void print_color(const char* message,unsigned char color)
{
    volatile unsigned char* video=(volatile unsigned char*)0xB8000;
    while (*message!='\0') {
        if (*message=='\n') cursor=((cursor/80)+1)*80;
        else { video[cursor*2]=*message; video[cursor*2+1]=color; cursor++; }
        message++;
    }
}
