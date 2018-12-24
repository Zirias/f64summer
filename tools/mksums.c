#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
# define SET_BINARY_MODE(stream) setmode(fileno(stream), O_BINARY)
#else
# define SET_BINARY_MODE(stream) ((void)0)
#endif

#define BUFSIZE 64*1024
#define MAXLINES 16*1024

unsigned char buf[BUFSIZE];
uint16_t warnlines[MAXLINES];

int main(int argc, char **argv)
{
    int allowshiftspace = 0;

    if (argc > 1)
    {
	if (argc == 2 && !strcmp(argv[1], "-s"))
	{
	    allowshiftspace = 1;
	}
	else
	{
	    if (!argv[0]) argv[0] = "mksums";
	    fprintf(stderr, "USAGE: %s [-s] < input.prg > checksums.txt\n"
		    "       -s:   allow shifted spaces in input\n", argv[0]);
	    return 1;
	}
    }

    SET_BINARY_MODE(stdin);

    unsigned char addrbytes[2];
    uint16_t startaddress;

    if (fread(addrbytes, 1, 2, stdin)!=2)
    {
        fputs("Error reading start address\n", stderr);
        return 1;
    }
    startaddress = addrbytes[1] << 8 | addrbytes[0];

    switch (startaddress)
    {
	case 0x0801:
	    fputs("BASIC program at $0801 (C64)\n", stderr);
	    break;

	case 0x1001:
	    fputs("BASIC program at $1001 (C16/116/+4 or VIC20)\n", stderr);
	    break;
	
	case 0x1201:
	    fputs("BASIC program at $1201 (VIC20 /w memory expansion)\n",
		    stderr);
	    break;

	case 0x1c01:
	    fputs("BASIC program at $1C01 (C128)\n", stderr);
	    break;

	case 0x4001:
	    fputs("BASIC program at $4001 (C128 /w bitmap gfx)\n", stderr);
	    break;

	default:
	    fputs("!!!!!!!! WARNING !!!!!!!!\n", stderr);
	    fprintf(stderr, "Load address is $%04X, this is unusual!\n",
		    (unsigned)startaddress);
	    fputs("The following output may be bogus, make sure this is "
		    "indeed a BASIC PRG file\nfor one of the supported "
		    "systems (VIC-20, C64, C16/116/+4 or C128)!\n\n", stderr);
    }

    size_t nread = fread(buf, 1, BUFSIZE, stdin);
    if (!nread)
    {
        fputs("Error: couldn't read any data\n", stderr);
        return 1;
    }

    size_t rembuf = BUFSIZE;
    unsigned char *bufptr = buf;
    size_t nchunk = nread;
    while (nchunk && nchunk < rembuf)
    {
        bufptr += nchunk;
        rembuf -= nchunk;
        nchunk = fread(bufptr, 1, rembuf, stdin);
        nread += nchunk;
    }

    size_t pos = 0;
    int retval = 0;
    size_t warnlinesnum = 0;

    for (;;)
    {
	if (pos > nread-2)
	{
	    retval = 1;
	    goto end;
	}
	if (!buf[pos] && !buf[pos+1]) goto end;

	pos += 2;

	if (pos > nread-2)
	{
	    retval = 1;
	    goto end;
	}

	uint16_t lineno = 0;
	lineno += buf[pos++];
	lineno += buf[pos++] << 8;

	printf("%u:\t", (unsigned)lineno);

	uint16_t lfsr = lineno ^ 0xffff;
	int inquot = 0;
	unsigned char c;
	while ((c = buf[pos++]))
	{
	    if (pos > nread)
	    {
		retval = 1;
		goto end;
	    }
	    if (c==0xa0 && !allowshiftspace)
	    {
		if (warnlinesnum < MAXLINES)
		{
		    warnlines[warnlinesnum++] = lineno;
		}
		c = ' ';
	    }
	    if (!inquot && c==' ') continue;
	    if (c=='"') inquot = !inquot;
	    for (int b = 0; b < 8; ++b)
	    {
		unsigned char tmp = !!(c & 0x80);
		c <<= 1;
		tmp ^= (lfsr & 1);
		lfsr >>= 1;
		if (tmp) lfsr ^= 0xb400;
	    }
	}
	printf("%04X\n", (unsigned)lfsr);
    }

end:
    if (warnlinesnum)
    {
	fputs("!!!!!!!! WARNING !!!!!!!!\n", stderr);
	fputs("Shifted spaces found on line(s)", stderr);
	for (size_t i = 0; i < warnlinesnum; ++i)
	{
	    fputc(" ,"[!!i], stderr);
	    fprintf(stderr, "%u", (unsigned)warnlines[i]);
	}
	fputs(".\n", stderr);
	fputs("They were automatically replaced by normal spaces.\n", stderr);
	fputs("If this program really needs shifted spaces, use the -s "
	       "option.\n", stderr);
    }

    if (retval)
    {
	fputs("Unknown error parsing BASIC program\n", stderr);
    }
    return retval;
}
