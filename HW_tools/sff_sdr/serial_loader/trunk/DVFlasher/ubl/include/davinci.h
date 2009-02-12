/* davinci.h - common davinci processors definitions. */

#ifndef _DAVINCI_H_
#define _DAVINCI_H_

#include <stdint.h>

#include "tistdtypes.h"

//Return type defines 
#define E_PASS    0x00000000u
#define E_FAIL    0x00000001u
#define E_TIMEOUT 0x00000002u

#define SYSTEM ((sysModuleRegs*) 0x01C40000)

/* -------------------------------------------------------------------------- *
 *    ARM Interrupt Controller register structure - See sprue26.pdf for more  *
 *       details.                                                             *
 * -------------------------------------------------------------------------- */
typedef struct _aintc_regs_
{
	volatile uint32_t FIQ0;
	volatile uint32_t FIQ1;
	volatile uint32_t IRQ0;
	volatile uint32_t IRQ1;
	volatile uint32_t FIQENTRY;
	volatile uint32_t IRQENTRY;
	volatile uint32_t EINT0;
	volatile uint32_t EINT1;
	volatile uint32_t INTCTL;
	volatile uint32_t EABASE;
	volatile uint8_t  RSVD0[8];
	volatile uint32_t INTPRI0;
	volatile uint32_t INTPRI1;
	volatile uint32_t INTPRI2;
	volatile uint32_t INTPRI3;
	volatile uint32_t INTPRI4;
	volatile uint32_t INTPRI5;
	volatile uint32_t INTPRI6;
	volatile uint32_t INTPRI7;
} aintcRegs;

#define AINTC ((aintcRegs*) 0x01C48000)

/* -------------------------------------------------------------------------- *
 *    PLL Register structure - See sprue14.pdf, Chapter 6 for more details.   *
 * -------------------------------------------------------------------------- */
typedef struct _PLL_regs_
{
	volatile uint32_t PID;
	volatile uint8_t  RSVD0[224];
	volatile uint32_t RSTYPE;
	volatile uint8_t  RSVD1[24];	
	volatile uint32_t PLLCTL;
	volatile uint8_t  RSVD2[12];	
	volatile uint32_t PLLM;
	volatile uint8_t  RSVD3[4];
	volatile uint32_t PLLDIV1;
	volatile uint32_t PLLDIV2;
	volatile uint32_t PLLDIV3;	
	volatile uint8_t  RSVD4[4]; 
	volatile uint32_t POSTDIV;
	volatile uint32_t BPDIV;
	volatile uint8_t  RSVD5[8];	
	volatile uint32_t PLLCMD;
	volatile uint32_t PLLSTAT;
	volatile uint32_t ALNCTL;
	volatile uint32_t DCHANGE;
	volatile uint32_t CKEN;
	volatile uint32_t CKSTAT;
	volatile uint32_t SYSTAT;
	volatile uint8_t  RSVD6[12];
	volatile uint32_t PLLDIV4;
	volatile uint32_t PLLDIV5;
} PLLRegs;

#define PLL1 ((PLLRegs*) 0x01C40800)
#define PLL2 ((PLLRegs*) 0x01C40C00)

/* -------------------------------------------------------------------------- *
 *    Power/Sleep Ctrl Register structure - See sprue14.pdf, Chapter 7        * 
 *       for more details.                                                    *
 * -------------------------------------------------------------------------- */
typedef struct _PSC_regs_
{
	volatile uint32_t PID;        // 0x000
	volatile uint8_t  RSVD0[12];   // 0x004
	volatile uint32_t GBLCTL;     // 0x010 - NOT ON DM355
	volatile uint8_t  RSVD1[4];    // 0x014
	volatile uint32_t INTEVAL;    // 0x018
	volatile uint8_t  RSVD2[36];   // 0x01C
	volatile uint32_t MERRPR0;    // 0x040
	volatile uint32_t MERRPR1;    // 0x044
	volatile uint8_t  RSVD3[8];    // 0x048
	volatile uint32_t MERRCR0;    // 0x050
	volatile uint32_t MERRCR1;    // 0x054
	volatile uint8_t  RSVD4[8];    // 0x058
	volatile uint32_t PERRPR;     // 0x060
	volatile uint8_t  RSVD5[4];    // 0x064
	volatile uint32_t PERRCR;     // 0x068
	volatile uint8_t  RSVD6[4];    // 0x06C
	volatile uint32_t EPCPR;      // 0x070
	volatile uint8_t  RSVD7[4];    // 0x074
	volatile uint32_t EPCCR;      // 0x078
	volatile uint8_t  RSVD8[132];  // 0x07C
	volatile uint32_t RAILSTAT;   // 0x100 - NOT ON DM355
	volatile uint32_t RAILCTL;    // 0x104 - NOT ON DM355
	volatile uint32_t RAILSEL;    // 0x108 - NOT ON DM355
	volatile uint8_t  RSVD9[20];   // 0x10C
	volatile uint32_t PTCMD;      // 0x120
	volatile uint8_t  RSVD10[4];   // 0x124
	volatile uint32_t PTSTAT;     // 0x128
	volatile uint8_t  RSVD11[212]; // 0x12C
	volatile uint32_t PDSTAT0;    // 0x200
	volatile uint32_t PDSTAT1;    // 0x204
	volatile uint8_t  RSVD12[248]; // 0x208
	volatile uint32_t PDCTL0;     // 0x300
	volatile uint32_t PDCTL1;     // 0x304
	volatile uint8_t  RSVD13[536]; // 0x308
	volatile uint32_t MCKOUT0;    // 0x520
	volatile uint32_t MCKOUT1;    // 0x524
	volatile uint8_t  RSVD14[728]; // 0x528
	volatile uint32_t MDSTAT[41]; // 0x800
	volatile uint8_t  RSVD15[348]; // 0x8A4
	volatile uint32_t MDCTL[41];  // 0xA00
} PSCRegs;

#define PSC ((PSCRegs*) 0x01C41000)

#define PD0                 0

/* PSC constants */
#define LPSC_VPSS_MAST      0
#define LPSC_VPSS_SLV       1
#define LPSC_TPCC           2
#define LPSC_TPTC0          3
#define LPSC_TPTC1          4
#ifdef DM6446
#define LPSC_EMAC0          5
#define LPSC_EMAC1          6
#define LPSC_MDIO           7
#define LPSC_1394           8
#endif
#ifdef DM355
#define LPSC_TIMER3         5
#define LPSC_SPI1           6
#define LPSC_MMC_SD1        7
#define LPSC_ASP1           8
#endif
#define LPSC_USB            9
#ifdef DM6446
#define LPSC_ATA            10
#define LPSC_VLYNQ          11
#define LPSC_HPI            12
#endif
#ifdef DM355
#define LPSC_PWM3           10
#define LPSC_SPI2           11
#define LPSC_RTO            12
#endif
#define LPSC_DDR2           13
#define LPSC_AEMIF	    14
#define LPSC_MMC_SD0        15
#define LPSC_MEMSTK         16
#define LPSC_ASP0           17
#define LPSC_I2C            18
#define LPSC_UART0          19
#ifdef DM355
#define LPSC_UART1          20
#define LPSC_UART2          21
#define LPSC_SPIO           22
#define LPSC_PWM0           23
#define LPSC_PWM1           24
#define LPSC_PWM2           25
#endif
#define LPSC_GPIO           26
#define LPSC_TIMER0         27
#ifdef DM355
#define LPSC_TIMER1         28
#define LPSC_TIMER2         29
#define LPSC_SYSMOD         30
#endif
#define LPSC_ARM            31
#ifdef DM6446
#define LPSC_DSP            39
#define LPSC_IMCOP          40
#endif
#ifdef DM355
#define LPSC_VPSS_DAC       40
#endif

#define EMURSTIE_MASK       0x00000200

#define PSC_ENABLE          0x3
#define PSC_DISABLE         0x2
#define PSC_SYNCRESET       0x1
#define PSC_SWRSTDISABLE    0x0

/* -------------------------------------------------------------------------- *
 *    DDR2 Memory Ctrl Register structure - See sprue22b.pdf for more details.*
 * -------------------------------------------------------------------------- */
typedef struct _DDR2_MEM_CTL_REGS_
{
	volatile uint8_t  RSVD0[4];	//0x00
	volatile uint32_t SDRSTAT;	//0x04
	volatile uint32_t SDBCR;	//0x08
	volatile uint32_t SDRCR;	//0x0C
	volatile uint32_t SDTIMR;	//0x10
	volatile uint32_t SDTIMR2;	//0x14
	volatile uint8_t  RSVD1[4];	//0x18 
	volatile uint32_t SDBCR2;     //0x1C
	volatile uint32_t PBBPR;	//0x20
	volatile uint8_t  RSVD2[156];	//0x24 
	volatile uint32_t IRR;	//0xC0
	volatile uint32_t IMR;	//0xC4
	volatile uint32_t IMSR;	//0xC8
	volatile uint32_t IMCR;	//0xCC
	volatile uint8_t  RSVD3[20];	//0xD0
	volatile uint32_t DDRPHYCR;	//0xE4
	volatile uint8_t  RSVD4[8];	//0xE8
	volatile uint32_t VTPIOCR;	//0xF0 - NOT ON DM355
} DDR2Regs;

#define DDRVTPR (*((volatile uint32_t*) 0x01C42030))
#define DDR ((DDR2Regs*) 0x20000000)
#define DDR_TEST_PATTERN 0xA55AA55Au
#define DDR_RAM_SIZE 0x10000000u


/* -------------------------------------------------------------------------- *
 *    AEMIF Register structure - See sprue20a.pdf for more details.           *
 * -------------------------------------------------------------------------- */
typedef struct _emif_regs_
{
	volatile uint32_t ERCSR;           // 0x00
	volatile uint32_t AWCCR;           // 0x04
	volatile uint32_t SDBCR;           // 0x08 - NOT ON DM355
	volatile uint32_t SDRCR;           // 0x0C - NOT ON DM355
	volatile uint32_t A1CR;            // 0x10
	volatile uint32_t A2CR;            // 0x14
	volatile uint32_t A3CR;            // 0x18 - NOT ON DM355
	volatile uint32_t A4CR;            // 0x1C - NOT ON DM355
	volatile uint32_t SDTIMR;          // 0x20 - NOT ON DM355
	volatile uint32_t DDRSR;           // 0x24 - NOT ON DM355
	volatile uint32_t DDRPHYCR;        // 0x28 - NOT ON DM355
	volatile uint32_t DDRPHYSR;        // 0x2C - NOT ON DM355
	volatile uint32_t TOTAR;           // 0x30 - NOT ON DM355
	volatile uint32_t TOTACTR;         // 0x34 - NOT ON DM355
	volatile uint32_t DDRPHYID_REV;    // 0x38 - NOT ON DM355
	volatile uint32_t SDSRETR;         // 0x3C - NOT ON DM355
	volatile uint32_t EIRR;            // 0x40
	volatile uint32_t EIMR;
	volatile uint32_t EIMSR;
	volatile uint32_t EIMCR;
	volatile uint32_t IOCTRLR;         // 0x50 - NOT ON DM355
	volatile uint32_t IOSTATR;         // 0x54 - NOT ON DM355
	volatile uint32_t RSVD0;
	volatile uint32_t ONENANDCTL;      // 0x5C - ONLY ON DM355  
	volatile uint32_t NANDFCR;         // 0x60
	volatile uint32_t NANDFSR;         // 0x64
	volatile uint32_t RSVD1[2];
	volatile uint32_t NANDF1ECC;       // 0x70
	volatile uint32_t NANDF2ECC;       // 0x74
	volatile uint32_t NANDF3ECC;       // 0x78 - NOT ON DM355
	volatile uint32_t NANDF4ECC;       // 0x7C - NOT ON DM355
	volatile uint32_t RSVD2;           // 0x80
	volatile uint32_t IODFTECR;
	volatile uint32_t IODFTGCR;
	volatile uint32_t RSVD3;
	volatile uint32_t IODFTMRLR;       // 0x90
	volatile uint32_t IODFTMRMR;       // 0x94
	volatile uint32_t IODFTMRMSBR;     // 0x98
	volatile uint32_t RSVD4[5];
	volatile uint32_t MODRNR;          // 0xB0
	volatile uint32_t RSVD5[2];
	volatile uint32_t NAND4BITECCLOAD; // 0xBC - ONLY ON DM355  
	volatile uint32_t NAND4BITECC1;    // 0xC0 - ONLY ON DM355  
	volatile uint32_t NAND4BITECC2;    // 0xC4 - ONLY ON DM355  
	volatile uint32_t NAND4BITECC3;    // 0xC8 - ONLY ON DM355  
	volatile uint32_t NAND4BITECC4;    // 0xCC - ONLY ON DM355  
	volatile uint32_t NANDERRADD1;     // 0xD0 - ONLY ON DM355  
	volatile uint32_t NANDERRADD2;     // 0xD4 - ONLY ON DM355  
	volatile uint32_t NANDERRVAL1;     // 0xD8 - ONLY ON DM355  
	volatile uint32_t NANDERRVAL2;     // 0xDC - ONLY ON DM355
} emifRegs;

#define AEMIF ((emifRegs*) 0x01E00000)

/* -------------------------------------------------------------------------- *
 *    UART Register structure - See sprue33.pdf for more details.             *
 * -------------------------------------------------------------------------- */
typedef struct _uart_regs_
{
	volatile uint32_t RBR;
	volatile uint32_t IER;
	volatile uint32_t IIR;
	volatile uint32_t LCR;
	volatile uint32_t MCR;
	volatile uint32_t LSR;
	volatile uint32_t MSR; // NOT ON DM355
	volatile uint32_t SCR; // NOT ON DM355
	volatile uint8_t  DLL;
	volatile uint8_t  RSVDO[3];
	volatile uint8_t  DLH;
	volatile uint8_t  RSVD1[3];	
	volatile uint32_t PID1;
	volatile uint32_t PID2;
	volatile uint32_t PWREMU_MGNT;
} uartRegs;

#define THR RBR
#define FCR IIR

#define UART0 ((uartRegs*) 0x01C20000)

/* -------------------------------------------------------------------------- *
 *    Timer Register structure - See sprue26.pdf for more details.             *
 * -------------------------------------------------------------------------- */
typedef struct _timer_regs_
{
	volatile uint32_t PID12;
	volatile uint32_t EMUMGT_CLKSPD;
	volatile uint32_t GPINT_GPEN;    // NOT ON DM355
	volatile uint32_t GPTDAT_GPDIR;  // NOT ON DM355
	volatile uint32_t TIM12;
	volatile uint32_t TIM34;
	volatile uint32_t PRD12;
	volatile uint32_t PRD34;
	volatile uint32_t TCR;
	volatile uint32_t TGCR;
	volatile uint32_t WDTCR;
	volatile uint8_t  RSVD1[12];      // 0x2C - ONLY ON DM355
	volatile uint32_t REL12;          // 0x34 - ONLY ON DM355
	volatile uint32_t REL34;          // 0x38 - ONLY ON DM355
	volatile uint32_t CAP12;          // 0x3C - ONLY ON DM355
	volatile uint32_t CAP34;          // 0x40 - ONLY ON DM355
	volatile uint32_t INTCTL_STAT;    // 0x44 - ONLY ON DM355
} timerRegs;

#define TIMER0 ((timerRegs*) 0x01C21400)


struct gpio_controller {
	uint32_t     dir;
	uint32_t     out_data;
	uint32_t     set_data;
	uint32_t     clr_data;
	uint32_t     in_data;
	uint32_t     set_rising;
	uint32_t     clr_rising;
	uint32_t     set_falling;
	uint32_t     clr_falling;
	uint32_t     intstat;
};

#define DAVINCI_GPIO_BASE 0x01C67000

#define GPIOC ((volatile struct gpio_controller *) DAVINCI_GPIO_BASE)

#define GPIO(X) (X) /* 0 <= X <= (DAVINCI_N_GPIO - 1) */


void LPSCTransition(uint8_t module, uint8_t domain, uint8_t state);
int davinci_platform_init(void);
void PSCInit(void);
int UART0Init(void);
int PLL1Init(void);
int PLL2Init(void);
int DDR2Init(void);

uint32_t TIMER0Init(void);
void TIMER0Start(void);
uint32_t TIMER0Status(void);

struct gpio_controller * gpio_to_controller(unsigned gpio);
inline uint32_t gpio_mask(unsigned gpio);
int gpio_direction_out(unsigned gpio, int value);

void sleep_ms(int ms);

#ifdef DM6446
#include "dm644x.h"
#endif
#ifdef DM355
#include "dm355.h"
#endif

#endif /* _DAVINCI_H_ */
