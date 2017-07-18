#define GPKCON (volatile unsigned long*)0x7f008800
#define GPKDAT (volatile unsigned long*)0x7f008808

int gboot_main()
{
    *(GPKCON) = 0x11110000;
    *(GPKDAT) = 0xa0;
    
    return 0;    
}