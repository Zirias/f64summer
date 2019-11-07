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

int main(int argc, char **argv)
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

    if (argc < 2 || strcmp(argv[1], "-v"))
    {
        int remaining = 78 - printf("0 fora=%" PRIu16 "to%" PRIu16
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
                remaining = 73-needed;
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

    printf("0 s=5:l=4:for i=%" PRIu16 " to %" PRIu16
            ":read x:if x>255 then 2\n", startaddress, endaddress+1);
    puts("1 s=2*s-(s>4095)+x and 8191:poke i,x:next");
    puts("2 if s<>x-256 then print\"error in line\";l:end");
    printf("3 if i<%" PRIu16 " then l=l+1:i=i-1:next\n", endaddress+1);

    int lineno = 4;
    uint16_t sum = 5;
    sum = ((sum << 1) + !!(sum & 4096) + (unsigned)*r) & 8191;
    int remaining = 78 - printf("4 sys%" PRIu16  ":data%u", startaddress,
            (unsigned)*r++);
    --nread;

    while (nread--)
    {
        uint16_t nsum = ((sum << 1) + !!(sum & 4096) + (unsigned)*r) & 8191;
        int nsumlen = 4;
        if (nsum > (999-256)) ++nsumlen;
        int needed = 2 + nsumlen;
        if (*r>9)++needed;
        if (*r>99)++needed;
        if (remaining - needed < 0)
        {
            printf(",%u\n", sum+256);
            remaining = 73 - (needed - nsumlen);
            if (lineno>9) --remaining;
            if (lineno>99) --remaining;
            if (lineno>999) --remaining;
            if (lineno>9999) --remaining;
            printf("%d data%u", ++lineno, (unsigned)*r++);
        }
        else
        {
            remaining -= (needed - nsumlen);
            printf(",%u", (unsigned)*r++);
        }
        sum = nsum;
    }
    printf(",%u\n", sum+256);
    return 0;
}
