/*
 *
 * Copyright (C) 2004 Texas Instruments.
 *
 * ----------------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ----------------------------------------------------------------------------
 Modifications:
 ver. 1.0: Oct 2005, Swaminathan S
 *
 */

#include <common.h>
#include <i2c.h>
#if (CONFIG_COMMANDS & CFG_CMD_NAND)
#include <linux/mtd/nand.h>
extern struct nand_chip nand_dev_desc[CFG_MAX_NAND_DEVICE];
#endif

#if 0
void flash__init (void);
void ether__init (void);
#endif
#define PLL1_PLLM   *(volatile unsigned int *)0x01c40910
#define PLL2_PLLM   *(volatile unsigned int *)0x01c40D10
#define PLL2_DIV2   *(volatile unsigned char *)0x01c40D1C

void davinci_psc_all_enable(void);

/*******************************************
 Routine: delay
 Description:  Delay function
*******************************************/
static inline void delay (unsigned long loops)
{
	       __asm__ volatile ("1:\n"
		     "subs %0, %1, #1\n"
		     "bne 1b":"=r" (loops):"0" (loops));
}

/*******************************************
 Routine: board_init
 Description:  Board Initialization routine
*******************************************/
int board_init (void)
{
    	  DECLARE_GLOBAL_DATA_PTR;

        /* arch number of DaVinci DVDP-Board */
        gd->bd->bi_arch_number = 901;

      	/* adress of boot parameters */
       	gd->bd->bi_boot_params = LINUX_BOOT_PARAM_ADDR;
        /* Configure MUX settings */

      /* Power on required peripherals */
    	davinci_psc_all_enable();
#if 0
      	/* this speeds up your boot a quite a bit.  However to make it
 	 *  work, you need make sure your kernel startup flush bug is fixed.
	 *  ... rkw ...
	 */
	icache_enable ();
#endif
/*     	inittimer (); */
      	return 0;
}

/* PSC Domains */
#define LPSC_VPSSMSTR       0       // VPSS Master LPSC
#define LPSC_VPSSSLV        1       // VPSS Slave LPSC
#define LPSC_TPCC           2       // TPCC LPSC
#define LPSC_TPTC0          3       // TPTC0 LPSC
#define LPSC_TPTC1          4       // TPTC1 LPSC
#define LPSC_EMAC           5       // EMAC LPSC
#define LPSC_EMAC_WRAPPER   6       // EMAC WRAPPER LPSC
#define LPSC_MDIO           7       // MDIO LPSC
#define LPSC_IEEE1394       8       // IEEE1394 LPSC
#define LPSC_USB            9       // USB LPSC
#define LPSC_ATA            10      // ATA LPSC
#define LPSC_VLYNQ          11      // VLYNQ LPSC
#define LPSC_UHPI           12      // UHPI LPSC
#define LPSC_DDR_EMIF       13      // DDR_EMIF LPSC
#define LPSC_AEMIF          14      // AEMIF LPSC
#define LPSC_MMC_SD         15      // MMC_SD LPSC
#define LPSC_MEMSTICK       16      // MEMSTICK LPSC
#define LPSC_McBSP          17      // McBSP LPSC
#define LPSC_I2C            18      // I2C LPSC
#define LPSC_UART0          19      // UART0 LPSC
#define LPSC_UART1          20      // UART1 LPSC
#define LPSC_UART2          21      // UART2 LPSC
#define LPSC_SPI            22      // SPI LPSC
#define LPSC_PWM0           23      // PWM0 LPSC
#define LPSC_PWM1           24      // PWM1 LPSC
#define LPSC_PWM2           25      // PWM2 LPSC
#define LPSC_GPIO           26      // GPIO LPSC
#define LPSC_TIMER0         27      // TIMER0 LPSC
#define LPSC_TIMER1         28      // TIMER1 LPSC
#define LPSC_TIMER2         29      // TIMER2 LPSC
#define LPSC_SYSTEM_SUBSYS  30      // SYSTEM SUBSYSTEM LPSC
#define LPSC_ARM            31      // ARM LPSC
#define LPSC_SCR2           32      // SCR2 LPSC
#define LPSC_SCR3           33      // SCR3 LPSC
#define LPSC_SCR4           34      // SCR4 LPSC
#define LPSC_CROSSBAR       35      // CROSSBAR LPSC
#define LPSC_CFG27          36      // CFG27 LPSC
#define LPSC_CFG3           37      // CFG3 LPSC
#define LPSC_CFG5           38      // CFG5 LPSC
#define LPSC_GEM            39      // GEM LPSC
#define LPSC_IMCOP          40      // IMCOP LPSC

#define EPCPR               *(( volatile unsigned int* )( 0x01C41070 ))
#define PTCMD               *(( volatile unsigned int* )( 0x01C41120 ))
#define PTSTAT              *(( volatile unsigned int* )( 0x01C41128 ))
#define PDSTAT              *(( volatile unsigned int* )( 0x01C41200 ))
#define PDSTAT1             *(( volatile unsigned int* )( 0x01C41204 ))
#define PDCTL               *(( volatile unsigned int* )( 0x01C41300 ))
#define PDCTL1              *(( volatile unsigned int* )( 0x01C41304 ))
#define VBPR                *(( volatile unsigned int* )( 0x20000020 ))

/**************************************
 Routine: board_setup_psc_on
 Description:  Enable a PSC domain
**************************************/
void board_setup_psc_on( unsigned int domain, unsigned int id )
{
        volatile unsigned int* mdstat = ( unsigned int* )( 0x01C41800 + 4 * id );
	      volatile unsigned int* mdctl  = ( unsigned int* )( 0x01C41A00 + 4 * id );

	      *mdctl |= 0x00000003; // Set PowerDomain to turn on

    	  if ( ( PDSTAT & 0x00000001 ) == 0 )
    	  {
                PDCTL1 |= 0x1;
        	      PTCMD = ( 1 << domain );
        	      while ( ( ( ( EPCPR >> domain ) & 1 ) == 0 ) );

        	      PDCTL1 |= 0x100 ;
        	      while( ! ( ( ( PTSTAT >> domain ) & 1 ) == 0 ) );
    	  }
    	  else
    	  {
        	      PTCMD = ( 1<<domain );
        	      while( ! ( ( ( PTSTAT >> domain ) & 1 ) == 0 ) );
    	  }

    	  while( ! ( ( *mdstat & 0x0000001F ) == 0x3 ) );
}

/**************************************
 Routine: davinci_psc_all_enable
 Description:  Enable all PSC domains
**************************************/
void davinci_psc_all_enable(void)
{
#define PSC_ADDR            0x01C41000

        unsigned int alwaysOnPdNum = 0 ;

    	  /* This function turns on all clocks in the ALWAYSON and DSP Power
	       * Domains.  Note this function assumes that the Power Domains are
	       * already on.
	       */
#if 0
	unsigned int dspPdNum = 1, i;
    	  /* Write ENABLE (0x3) to all 41 MDCTL[i].NEXT bit fields. */
    	  for( i = 0; i < 41; i++){
                *(volatile unsigned int*) (PSC_ADDR+0xA00+4*i) =
				        *(volatile unsigned int*) (PSC_ADDR+0xA00+4*i) | 0x3;
    	  }

    	  /* For special workaround:  Set MDCTL[i].EMURSTIE to 0x1 for all of the
	       * following Modules.  VPSSSLV, EMAC, EMACCTRL, MDIO, USB, ATA, VLYNQ,
         * HPI, DDREMIF, AEMIF, MMCSD, MEMSTICK, ASP, GPIO, IMCOP.
         */
        /**(unsigned int*) (PSC_ADDR+0xA00+4*1) = *(unsigned int*) (PSC_ADDR+0xA00+4*1) | 0x203;*/
        *(unsigned int*) (PSC_ADDR+0xA00+4*5) = *(unsigned int*) (PSC_ADDR+0xA00+4*5) | 0x203;
        *(unsigned int*) (PSC_ADDR+0xA00+4*6) = *(unsigned int*) (PSC_ADDR+0xA00+4*6) | 0x203;
        *(unsigned int*) (PSC_ADDR+0xA00+4*7) = *(unsigned int*) (PSC_ADDR+0xA00+4*7) | 0x203;
        /**(unsigned int*) (PSC_ADDR+0xA00+4*9) = *(unsigned int*) (PSC_ADDR+0xA00+4*9) | 0x203;
        *(unsigned int*) (PSC_ADDR+0xA00+4*10) = *(unsigned int*) (PSC_ADDR+0xA00+4*10) | 0x203;
        *(unsigned int*) (PSC_ADDR+0xA00+4*11) = *(unsigned int*) (PSC_ADDR+0xA00+4*11) | 0x203;
        *(unsigned int*) (PSC_ADDR+0xA00+4*12) = *(unsigned int*) (PSC_ADDR+0xA00+4*12) | 0x203;*/
        *(unsigned int*) (PSC_ADDR+0xA00+4*13) = *(unsigned int*) (PSC_ADDR+0xA00+4*13) | 0x203;
        *(unsigned int*) (PSC_ADDR+0xA00+4*14) = *(unsigned int*) (PSC_ADDR+0xA00+4*14) | 0x203;
        /**(unsigned int*) (PSC_ADDR+0xA00+4*15) = *(unsigned int*) (PSC_ADDR+0xA00+4*15) | 0x203;
        *(unsigned int*) (PSC_ADDR+0xA00+4*16) = *(unsigned int*) (PSC_ADDR+0xA00+4*16) | 0x203;
        *(unsigned int*) (PSC_ADDR+0xA00+4*17) = *(unsigned int*) (PSC_ADDR+0xA00+4*17) | 0x203;*/
        *(unsigned int*) (PSC_ADDR+0xA00+4*19) = *(unsigned int*) (PSC_ADDR+0xA00+4*19) | 0x203;
        /**(unsigned int*) (PSC_ADDR+0xA00+4*26) = *(unsigned int*) (PSC_ADDR+0xA00+4*26) | 0x203;
        *(unsigned int*) (PSC_ADDR+0xA00+4*40) = *(unsigned int*) (PSC_ADDR+0xA00+4*40) | 0x203;*/
#endif

        /* For special workaround:  Clear MDCTL[i].EMURSTIE to 0x0 for all of the following Modules.
         *  VPSSSLV, EMAC, EMACCTRL, MDIO, USB, ATA, VLYNQ,
         *  HPI, DDREMIF, AEMIF, MMCSD, MEMSTICK, ASP, GPIO, IMCOP.
         */
        /**(unsigned int*) (PSC_ADDR+0xA00+4*1) = *(unsigned int*) (PSC_ADDR+0xA00+4*1) & 0x003;*/
        *(unsigned int*) (PSC_ADDR+0xA00+4*5) = *(unsigned int*) (PSC_ADDR+0xA00+4*5) | 0x003;
        *(unsigned int*) (PSC_ADDR+0xA00+4*6) = *(unsigned int*) (PSC_ADDR+0xA00+4*6) | 0x003;
        *(unsigned int*) (PSC_ADDR+0xA00+4*7) = *(unsigned int*) (PSC_ADDR+0xA00+4*7) | 0x003;
        /**(unsigned int*) (PSC_ADDR+0xA00+4*9) = *(unsigned int*) (PSC_ADDR+0xA00+4*9) & 0x003;
        *(unsigned int*) (PSC_ADDR+0xA00+4*10) = *(unsigned int*) (PSC_ADDR+0xA00+4*10) & 0x003;
        *(unsigned int*) (PSC_ADDR+0xA00+4*11) = *(unsigned int*) (PSC_ADDR+0xA00+4*11) & 0x003;
        *(unsigned int*) (PSC_ADDR+0xA00+4*12) = *(unsigned int*) (PSC_ADDR+0xA00+4*12) & 0x003;*/
        *(unsigned int*) (PSC_ADDR+0xA00+4*13) = *(unsigned int*) (PSC_ADDR+0xA00+4*13) | 0x003;
        *(unsigned int*) (PSC_ADDR+0xA00+4*14) = *(unsigned int*) (PSC_ADDR+0xA00+4*14) | 0x003;
        /**(unsigned int*) (PSC_ADDR+0xA00+4*15) = *(unsigned int*) (PSC_ADDR+0xA00+4*15) & 0x003;
        *(unsigned int*) (PSC_ADDR+0xA00+4*16) = *(unsigned int*) (PSC_ADDR+0xA00+4*16) & 0x003;
        *(unsigned int*) (PSC_ADDR+0xA00+4*17) = *(unsigned int*) (PSC_ADDR+0xA00+4*17) & 0x003;*/
        *(unsigned int*) (PSC_ADDR+0xA00+4*19) = *(unsigned int*) (PSC_ADDR+0xA00+4*19) | 0x003;
        *(unsigned int*) (PSC_ADDR+0xA00+4*18) = *(unsigned int*) (PSC_ADDR+0xA00+4*18) | 0x003;
        *(unsigned int*) (PSC_ADDR+0xA00+4*28) = *(unsigned int*) (PSC_ADDR+0xA00+4*28) | 0x003;
        /**(unsigned int*) (PSC_ADDR+0xA00+4*26) = *(unsigned int*) (PSC_ADDR+0xA00+4*26) & 0x003;
        *(unsigned int*) (PSC_ADDR+0xA00+4*40) = *(unsigned int*) (PSC_ADDR+0xA00+4*40) & 0x003;*/

    	  /* Set PTCMD.GO0 to 0x1 to initiate the state transtion for Modules in
	       * the ALWAYSON Power Domain
	       */
	PTCMD = (1<<alwaysOnPdNum);

    	  /* Wait for PTSTAT.GOSTAT0 to clear to 0x0 */
    	  while(! ((( PTSTAT >> alwaysOnPdNum) & 0x00000001) == 0));


        /* Bringup UART out of reset here since NS16650 code that we are using from uBoot
         * will not do it
         */
#define UARTPWREMU_MGMT     0x01c20030
        *(volatile unsigned int*) UARTPWREMU_MGMT = 0x0000E003;
        
	/* Enable GIO3.3V cells used for EMAC */
#define VDD3P3V_PWDN        0x01c40048
        *(volatile unsigned int*) VDD3P3V_PWDN = 0;

#define PINMUX0     0x01C40000
#define PINMUX4     0x01C40004

        /* Enable UART0 MUX lines */
        *(volatile unsigned int *)PINMUX4 |= 1;
        /* Enable EMAC and AEMIF pins */
        *(volatile unsigned int*) PINMUX0 = 0x80000C1F;

	/* Enable I2C pin Mux */
        *(volatile unsigned int *)PINMUX4 |= (1 << 7);

	/* Set the Bus Priority Register to appropriate value */
	VBPR = 0x20;
}

/******************************
 Routine: misc_init_r
 Description:  Misc. init
******************************/
int misc_init_r (void)
{
	char temp[20];
	char emac_read_addr [10] = { 0x7f, 0 } ;
	int  i= 0;
        int clk = 0;

        clk = ((PLL2_PLLM + 1) * 27) / (PLL2_DIV2 + 1);

        printf ("ARM Clock :- %dMHz\n", ((((PLL1_PLLM + 1) * 27 ) / 2)) );
        printf ("DDR Clock :- %dMHz\n", (clk/2));

#if 0 
	i2c_write (0x50, 0x00, 1, emac_read_addr, 2);
	i2c_read (0x50, 0x00, 1, emac_read_addr, 6);
        temp[0] = (emac_read_addr[0] & 0xF0) >> 4;
        temp[1] = (emac_read_addr[0] & 0x0F);
        temp[2] = ':';
        temp[3] = (emac_read_addr[1] & 0xF0) >> 4;
        temp[4] = (emac_read_addr[1] & 0x0F);
        temp[5] = ':';
        temp[6] = (emac_read_addr[2] & 0xF0) >> 4;
        temp[7] = (emac_read_addr[2] & 0x0F);
        temp[8] = ':';
        temp[9] = (emac_read_addr[3] & 0xF0) >> 4;
        temp[10]= (emac_read_addr[3] & 0x0F);
        temp[11]= ':';
        temp[12]= (emac_read_addr[4] & 0xF0) >> 4;
        temp[13]= (emac_read_addr[4] & 0x0F);
        temp[14]= ':';
        temp[15]= (emac_read_addr[5] & 0xF0) >> 4;
        temp[16]= (emac_read_addr[5] & 0x0F);
                                                                               
        for (i = 0; i < 17; i++)
        {
        if (temp[i] == ':')
                continue;
        else if (temp[i] >= 0 && temp[i] <= 9)
                temp[i] = temp[i] + 48;
        else
                temp[i] = temp[i] + 87;
        }
                   
	temp [17] = 0;                                                            
	if ((emac_read_addr [0] != 0xFF) ||
	    (emac_read_addr [1] != 0xFF) ||	
	    (emac_read_addr [2] != 0xFF) ||	
	    (emac_read_addr [3] != 0xFF) ||	
	    (emac_read_addr [4] != 0xFF) ||	
	    (emac_read_addr [5] != 0xFF))
	{ 
		setenv ("ethaddr", temp);
	}

	i2c_read (0x39, 0x00, 1, &i, 1);	
	setenv ("videostd", ((i  & 0x80)?"pal":"ntsc"));
#else
	setenv ("ethaddr", "00:0e:99:02:57:7a");
#endif	
        return (0);
}

/******************************
 Routine: dram_init
 Description:  Memory Info
******************************/
int dram_init (void)
{
        DECLARE_GLOBAL_DATA_PTR;

	      gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	      gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

        return 0;
}

#if (CONFIG_COMMANDS & CFG_CMD_NAND)
void nand_init(void)
{
    extern flash_info_t flash_info[];


    nand_probe(CFG_NAND_ADDR);
    if (nand_dev_desc[0].ChipID != NAND_ChipID_UNKNOWN) {
                print_size(nand_dev_desc[0].totlen, "\n");
    }
}
#endif

