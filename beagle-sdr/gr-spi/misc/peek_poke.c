/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov[AT]ru.mvista[DOT]com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <endian.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define SWAP (BYTE_ORDER != BIG_ENDIAN)

static void pabort(const char *s)
{
perror(s);
abort();
}

static const char *device = "/dev/spidev1.1";
static uint8_t mode = 0;
static uint8_t bits = 16;
static uint32_t speed = 12000000;
static uint16_t delay;

static void transfer(int fd,int poke, unsigned short reg, unsigned short data)
{
 int ret;
 uint8_t word[2];

//printf(poke?"Poke":"Peek");
reg &= 63;
data &= 0xff;

//printf(" register %02x, data byte %02x\n",reg,data);

word[SWAP] = reg | (poke ? 0 : 0x80);
word[!SWAP] = data;
//word = (reg << 8) | data;  // this wouldn't guarantee byte order
//if (!poke) word |= 0x8000;

struct spi_ioc_transfer tr = {
.tx_buf = (uint64_t) &word,
.rx_buf = (uint64_t) &word,
.len = 2,
.delay_usecs = delay,
.speed_hz = speed,
.bits_per_word = bits,
};

//printf("Transmitting 0x%04x\n",word);

ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
if (ret == 1)
pabort("can't send spi message");

if (!poke) printf(" Read 0x%02x = %3d = %d%d%d%d %d%d%d%d from register %2d\n",
  word[1],word[1],
 (word[1])/0x80,
 (word[1]%0x80)/0x40,
 (word[1]%0x40)/0x20,
 (word[1]%0x20)/0x10,
 (word[1]%0x10)/0x08,
 (word[1]%0x08)/0x04,
 (word[1]%0x04)/0x02,
 (word[1]%0x02),
  reg);
else printf("Wrote 0x%02x = %3d =  %d%d%d%d %d%d%d%d   to register %2d\n",
  data,data,
 (data)/0x80,
 (data%0x80)/0x40,
 (data%0x40)/0x20,
 (data%0x20)/0x10,
 (data%0x10)/0x08,
 (data%0x08)/0x04,
 (data%0x04)/0x02,
 (data%0x02),
  reg);
}

void print_usage(const char *prog)
{
printf("Usage: %s [-DsbdlHOLC3]\n", prog);
puts("  -D --device   device to use (default /dev/spidev1.1)\n"
     "  -s --speed    max speed (Hz)\n"
     "  -d --delay    delay (usec)\n"
     "  -b --bpw      bits per word \n"
     "  -l --loop     loopback\n"
     "  -H --cpha     clock phase\n"
     "  -O --cpol     clock polarity\n"
     "  -L --lsb      least significant bit first\n"
     "  -C --cs-high  chip select active high\n"
     "  -3 --3wire    SI/SO signals shared\n");
exit(1);
}

void parse_opts(int argc, char *argv[])
{
while (1) {
static const struct option lopts[] = {
{ "device",  1, 0, 'D' },
{ "speed",   1, 0, 's' },
{ "delay",   1, 0, 'd' },
{ "bpw",     1, 0, 'b' },
{ "loop",    0, 0, 'l' },
{ "cpha",    0, 0, 'H' },
{ "cpol",    0, 0, 'O' },
{ "lsb",     0, 0, 'L' },
{ "cs-high", 0, 0, 'C' },
{ "3wire",   0, 0, '3' },
{ NULL, 0, 0, 0 },
};
int c;

c = getopt_long(argc, argv, "D:s:d:b:lHOLC3", lopts, NULL);

if (c == -1)
break;

switch (c) {
case 'D':
device = optarg;
break;
case 's':
speed = atoi(optarg);
break;
case 'd':
delay = atoi(optarg);
break;
case 'b':
bits = atoi(optarg);
break;
case 'l':
mode |= SPI_LOOP;
break;
case 'H':
mode |= SPI_CPHA;
break;
case 'O':
mode |= SPI_CPOL;
break;
case 'L':
mode |= SPI_LSB_FIRST;
break;
case 'C':
mode |= SPI_CS_HIGH;
break;
case '3':
mode |= SPI_3WIRE;
break;
default:
print_usage(argv[0]);
break;
}
}
}

int main(int argc, char *argv[])
{
int ret = 0;
int fd;
char foo;
char line[80];
int poke;
unsigned short reg, data;

parse_opts(argc, argv);

fd = open(device, O_RDWR);
if (fd < 0)
pabort("can't open device");

/*
 * spi mode
 */
ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
if (ret == -1)
pabort("can't set spi mode");

ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
if (ret == -1)
pabort("can't get spi mode");

/*
 * bits per word
 */
ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
if (ret == -1)
pabort("can't set bits per word");

ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
if (ret == -1)
pabort("can't get bits per word");

/*
 * max speed hz
 */
ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
if (ret == -1)
pabort("can't set max speed hz");

ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
if (ret == -1)
pabort("can't get max speed hz");

printf("spi mode: %d\n", mode);
printf("bits per word: %d\n", bits);
printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

while (1) {
printf("Enter P/K/Q, address, data: ");
fgets(line,sizeof(line),stdin);
sscanf(line,"%c %hi %hi",&foo, &reg, &data);

if (foo == 'q' || foo == 'Q') break;
poke = (foo == 'k' || foo == 'K');

transfer(fd,poke,reg,data);
}
close(fd);

return ret;
}
