cpu::cpu()
{
	memset(memory, 0, sizeof(memory));
	printf("hello\n");
	FILE* f = fopen("hello.txt", "r");
	FILE *fp = fopen("DMG_ROM.bin", "rb");
	// Read bootstrap program from binary
	fread(bootloader, sizeof(uint8_t), 256, fp);
	fclose(fp);
	printf("hi\n");
	// Tetris has no banking so we can read it all the way
	FILE *rom = fopen("Tetris (World).gb", "rb");
	fread(memory, sizeof(uint8_t), 32768, rom);
	fclose(rom);
	pc = 0x0000;
	time = 0;
	zero = carry = half_carry = subtract = 0;
	booting = 1;
	sp = 0xfffe;
}

void cpu::status()
{
	printf("a = %02x\n", a);
	printf("b = %02x\n", b);
	printf("c = %02x\n", c);
	printf("d = %02x\n", d);
	printf("e = %02x\n", e);
	printf("h = %02x\n", h);
	printf("l = %02x\n", l);
	printf("sp = %04x\n", sp);
	printf("pc = %04x\n", pc);
	printf("carry = %u\n", carry);
	printf("half_carry = %u\n", half_carry);
	printf("subtract = %u\n", subtract);
	printf("zero = %u\n", zero);
}

uint8_t cpu::read(uint16_t addr)
{
	// TODO: fix read i.e make banking work
	if (booting)
	{
		if (addr < 0x0100)
			return bootloader[addr];
		else if (pc == 0x0100)
			booting = 0;
	}
	return memory[addr];
}

void cpu::write(uint16_t addr, uint8_t val)
{
	// TODO: make banking work

	// no writing to read only memory
	if (addr < 0x8000)
		return;
	else if (addr >= 0xe000 && addr < 0xfe00) // echo ram
	{
		memory[addr] = val;
		write(addr - 0x2000, val);
	}
	else if (addr >= 0xfea0 && addr < 0xfeff) // restricted area
		return;
	else if (addr == 0xff44) // tries to write to the scanline register
		memory[addr] = 0;
	else if (addr >= 0x8000 && addr <= 0x9fff)
	{
		//printf("write val %02x to address %04x\n", val, addr);
		memory[addr] = val;
	}
	else
		memory[addr] = val;
}

// flag register format is the following
// Z N H C X X X

// convert the flag variables into the actual flag register
// value and return it;
uint8_t cpu::get_f()
{

	uint8_t val = 0;
	val |= (zero << 7);
	val |= (subtract << 6);
	val |= (half_carry << 5);
	val |= (carry << 4);
	return val;
}

// set the flag register to the given value
void cpu::set_f(uint8_t val)
{
	zero       = (val >> 7) & 1;
	subtract   = (val >> 6) & 1;
	half_carry = (val >> 5) & 1;
	carry      = (val >> 4) & 1;
}