#include <stdio.h>
#include <inttypes.h>

#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
# define SET_BINARY_MODE(stream) setmode(fileno(stream), O_BINARY)
#else
# define SET_BINARY_MODE(stream) ((void)0)
#endif

#define BUFSIZE 64*1024

unsigned char buf[BUFSIZE];

int main()
{
    SET_BINARY_MODE(stdin);
    SET_BINARY_MODE(stdout);

    unsigned char addrbytes[2];
    uint16_t startaddress;

    if (fread(addrbytes, 1, 2, stdin)!=2)
    {
        fputs("Error reading start address\n", stderr);
        return 1;
    }
    startaddress = addrbytes[1] << 8 | addrbytes[0];

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

    uint16_t endaddress = startaddress + (uint16_t)nread - 1;

    unsigned char *r = buf;
    int remaining = 79 - printf("0 fora=%" PRIu16 "to%" PRIu16
            ":readb:pokea,b:next:sys%" PRIu16 ":data%u", startaddress,
            endaddress, startaddress, (unsigned)*r++);

    --nread;
    int lineno = 0;
    while (nread--)
    {
        int needed=2;
        if (*r>9)++needed;
        if (*r>99)++needed;
        if (remaining - needed < 0)
        {
            ++lineno;
            remaining = 74-needed;
            if (lineno>9) --remaining;
            if (lineno>99) --remaining;
            if (lineno>999) --remaining;
            if (lineno>9999) --remaining;
            printf("\n%d data%u", lineno, (unsigned)*r++);
        }
        else
        {
            remaining -= needed;
            printf(",%u", (unsigned)*r++);
        }
    }
    putchar('\n');

    return 0;
}
