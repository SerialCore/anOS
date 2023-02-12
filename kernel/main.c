void Start_Kernel(void)
{
    // address of frame buffer
    int* addr = (int*)0xffff800000a00000;
	int i;

    // with resolution 1440*90
	for (i = 0; i < 1440 * 20; i++)
	{
		// #00ff0000 RED
		*((char*)addr + 0) = (char)0x00;
		*((char*)addr + 1) = (char)0x00;
		*((char*)addr + 2) = (char)0xff;
		*((char*)addr + 3) = (char)0x00;
		addr += 1;	
	}
	for (i = 0; i < 1440 * 20; i++)
	{
		// #0000ff00 GREEN
		*((char*)addr + 0) = (char)0x00;
		*((char*)addr + 1) = (char)0xff;
		*((char*)addr + 2) = (char)0x00;
		*((char*)addr + 3) = (char)0x00;
		addr += 1;	
	}
	for (i = 0; i < 1440 * 20; i++)
	{
		// #000000ff BLUE
		*((char*)addr + 0) = (char)0xff;
		*((char*)addr + 1) = (char)0x00;
		*((char*)addr + 2) = (char)0x00;
		*((char*)addr + 3) = (char)0x00;
		addr += 1;	
	}
	for (i = 0; i < 1440 * 20; i++)
	{
		// #00ffffff WHITE
		*((char*)addr + 0) = (char)0xff;
		*((char*)addr + 1) = (char)0xff;
		*((char*)addr + 2) = (char)0xff;
		*((char*)addr + 3) = (char)0x00;
		addr += 1;	
	}

    while(1)
        ;
}