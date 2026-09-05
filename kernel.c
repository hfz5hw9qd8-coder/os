void print(const char* message);
void print_color(const char* message, unsigned char color);
void clear();
unsigned char inb(unsigned short port);

int cursor = 0;
int shift = 0;

char command[80];
int command_length = 0;

const unsigned char keyboard_map[64] = {
    [0x02] = '&',
    [0x03] = 'e',
    [0x04] = '"',
    [0x05] = '\'',
    [0x06] = '(',
    [0x07] = '-',
    [0x08] = 'e',
    [0x09] = '_',
    [0x0A] = 'c',
    [0x0B] = 'a',
    [0x0C] = ')',
    [0x0D] = '=',
    [0x0E] = '\b',
    [0x0F] = '\t',

    [0x10] = 'a',
    [0x11] = 'z',
    [0x12] = 'e',
    [0x13] = 'r',
    [0x14] = 't',
    [0x15] = 'y',
    [0x16] = 'u',
    [0x17] = 'i',
    [0x18] = 'o',
    [0x19] = 'p',
    [0x1A] = '^',
    [0x1B] = '$',
    [0x1C] = '\n',

    [0x1E] = 'q',
    [0x1F] = 's',
    [0x20] = 'd',
    [0x21] = 'f',
    [0x22] = 'g',
    [0x23] = 'h',
    [0x24] = 'j',
    [0x25] = 'k',
    [0x26] = 'l',
    [0x27] = 'm',

    [0x2C] = 'w',
    [0x2D] = 'x',
    [0x2E] = 'c',
    [0x2F] = 'v',
    [0x30] = 'b',
    [0x31] = 'n',
    [0x32] = ',',
    [0x33] = ';',
    [0x34] = ':',
    [0x35] = '!',
    [0x39] = ' ',
};

const unsigned char keyboard_shift_map[64] = {
    [0x02] = '1',
    [0x03] = '2',
    [0x04] = '3',
    [0x05] = '4',
    [0x06] = '5',
    [0x07] = '6',
    [0x08] = '7',
    [0x09] = '8',
    [0x0A] = '9',
    [0x0B] = '0',
    [0x0C] = '0',
    [0x0D] = '+',
    [0x0E] = '\b',
    [0x0F] = '\t',

    [0x10] = 'A',
    [0x11] = 'Z',
    [0x12] = 'E',
    [0x13] = 'R',
    [0x14] = 'T',
    [0x15] = 'Y',
    [0x16] = 'U',
    [0x17] = 'I',
    [0x18] = 'O',
    [0x19] = 'P',

    [0x1C] = '\n',

    [0x1E] = 'Q',
    [0x1F] = 'S',
    [0x20] = 'D',
    [0x21] = 'F',
    [0x22] = 'G',
    [0x23] = 'H',
    [0x24] = 'J',
    [0x25] = 'K',
    [0x26] = 'L',
    [0x27] = 'M',

    [0x2C] = 'W',
    [0x2D] = 'X',
    [0x2E] = 'C',
    [0x2F] = 'V',
    [0x30] = 'B',
    [0x31] = 'N',
    [0x32] = '?',
    [0x33] = '.',
    [0x34] = '/',

    [0x39] = ' ',
};

__attribute__((section(".text.start")))
void _start()
{
    clear();

    print("Bienvenue dans mon OS !\n");
    print_color("Version 0.1\n", 0x0A);
    print("Systeme pret.\n\n");
    print("> ");

    while (1)
    {
        if (inb(0x64) & 1)
        {
            unsigned char scancode = inb(0x60);

            

            if (scancode == 0x2A || scancode == 0x36)
            {
                shift = 1;

                volatile unsigned char* video =
                    (volatile unsigned char*)0xB8000;

                video[0] = 'S';
                video[1] = 0x0A;

                continue;
            }

            if (scancode == 0xAA || scancode == 0xB6)
            {
                shift = 0;

                volatile unsigned char* video =
                    (volatile unsigned char*)0xB8000;

                video[0] = 's';
                video[1] = 0x07;

                continue;
            }

            if (scancode & 0x80)
                continue;

            if (scancode < sizeof(keyboard_map))
            {
                unsigned char key;

                if (shift)
                    key = keyboard_shift_map[scancode];
                else
                    key = keyboard_map[scancode];

                if (key == '\b')
                {
                    if (cursor > 2)
                    {
                        cursor--;

                        volatile unsigned char* video =
                            (volatile unsigned char*)0xB8000;

                        video[cursor * 2] = ' ';
                        video[cursor * 2 + 1] = 0x07;
                    }
                }
                else if (key == '\n')
                {
                    cursor = ((cursor / 80) + 1) * 80;
                    print("> ");
                }
                else if (key != 0)
                {
                    volatile unsigned char* video =
                        (volatile unsigned char*)0xB8000;

                    video[cursor * 2] = key;
                    video[cursor * 2 + 1] = 0x07;

                    if (command_length < 79)
                    {
                        command[command_length] = key;
                        command_length++;
                    }

                    cursor++;
                }
            }
        }
    }
}

unsigned char inb(unsigned short port)
{
    unsigned char value;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

void clear()
{
    volatile unsigned char* video =
        (volatile unsigned char*)0xB8000;

    for (int i = 0; i < 80 * 25; i++)
    {
        video[i * 2] = ' ';
        video[i * 2 + 1] = 0x07;
    }

    cursor = 0;
}

void print(const char* message)
{
    print_color(message, 0x07);
}

void print_color(const char* message, unsigned char color)
{
    volatile unsigned char* video =
        (volatile unsigned char*)0xB8000;

    while (*message != '\0')
    {
        if (*message == '\n')
        {
            cursor = ((cursor / 80) + 1) * 80;
        }
        else
        {
            video[cursor * 2] = *message;
            video[cursor * 2 + 1] = color;
            cursor++;
        }

        message++;
    }
}