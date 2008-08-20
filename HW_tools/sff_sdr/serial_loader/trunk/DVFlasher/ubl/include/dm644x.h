/* --------------------------------------------------------------------------
    FILE        : dm644x.h
    PURPOSE     : Platform init header file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Daniel Allred
    DATE	    : Jan-22-2007  
 
    HISTORY
 	     v1.00 completion 							 						      
 	          Daniel Allred - Jan-22-2007
 	     v1.11 DJA - 7-Mar-2007 
 	        Added additional required defines for the LPSCs                                          
 ----------------------------------------------------------------------------- */


#ifndef _DM644X_H_
#define _DM644X_H_

#include <stdint.h> 

#include "tistdtypes.h" 

//Return type defines 
#define E_PASS    0x00000000u
#define E_FAIL    0x00000001u
#define E_TIMEOUT 0x00000002u 

// BIT mask defines
//  BIT masks
#define BIT0    0x00000001
#define BIT1    0x00000002
#define BIT2    0x00000004
#define BIT3    0x00000008
#define BIT4    0x00000010
#define BIT5    0x00000020
#define BIT6    0x00000040
#define BIT7    0x00000080
#define BIT8    0x00000100
#define BIT9    0x00000200
#define BIT10   0x00000400
#define BIT11   0x00000800
#define BIT12   0x00001000
#define BIT13   0x00002000
#define BIT14   0x00004000
#define BIT15   0x00008000
#define BIT16   0x00010000
#define BIT17   0x00020000
#define BIT18   0x00040000
#define BIT19   0x00080000
#define BIT20   0x00100000
#define BIT21   0x00200000
#define BIT22   0x00400000
#define BIT23   0x00800000
#define BIT24   0x01000000
#define BIT25   0x02000000
#define BIT26   0x04000000
#define BIT27   0x08000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000

/* -------------------------------------------------------------------------- *
 *    System Control Module register structure - See sprue14.pdf, Chapter 10  *
 *       for more details.                                                    *
 * -------------------------------------------------------------------------- */ 
typedef struct _sys_module_regs_
{
	volatile uint32_t PINMUX[2];         //0x00
	volatile uint32_t DSPBOOTADDR;       //0x08
	volatile uint32_t SUSPSRC;           //0x0C
	volatile uint32_t INTGEN;            //0x10
	volatile uint32_t BOOTCFG;           //0x14
	volatile uint8_t RSVD0[16];          //0x18
	volatile uint32_t DEVICE_ID;         //0x28
	volatile uint8_t RSVD1[8];           //0x2C
	volatile uint32_t USBPHY_CTL;        //0x34
	volatile uint32_t CHP_SHRTSW;        //0x38
	volatile uint32_t MSTPRI[2];         //0x3C
	volatile uint32_t VPSS_CLKCTL;       //0x44
	volatile uint32_t VDD3P3V_PWDN;      //0x48
	volatile uint32_t DDRVTPER;          //0x4C
	volatile uint8_t RSVD2[32];          //0x50 
} sysModuleRegs;

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
    volatile uint8_t RSVD0[8];
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
	volatile uint8_t RSVD0[224];
	volatile uint32_t RSTYPE;
	volatile uint8_t RSVD1[24];	
	volatile uint32_t PLLCTL;
	volatile uint8_t RSVD2[12];	
	volatile uint32_t PLLM;
	volatile uint8_t RSVD3[4];
	volatile uint32_t PLLDIV1;
	volatile uint32_t PLLDIV2;
	volatile uint32_t PLLDIV3;	
	volatile uint8_t RSVD4[4]; 
	volatile uint32_t POSTDIV;
	volatile uint32_t BPDIV;
	volatile uint8_t RSVD5[8];	
	volatile uint32_t PLLCMD;
	volatile uint32_t PLLSTAT;
	volatile uint32_t ALNCTL;
	volatile uint32_t DCHANGE;
	volatile uint32_t CKEN;
	volatile uint32_t CKSTAT;
	volatile uint32_t SYSTAT;
	volatile uint8_t RSVD6[12];
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
	volatile uint8_t RSVD0[12];   // 0x004
	volatile uint32_t GBLCTL;     // 0x010
	volatile uint8_t RSVD1[4];    // 0x014
	volatile uint32_t INTEVAL;    // 0x018
	volatile uint8_t RSVD2[36];	// 0x01C
	volatile uint32_t MERRPR0;    // 0x040
	volatile uint32_t MERRPR1;    // 0x044
	volatile uint8_t RSVD3[8];    // 0x048
	volatile uint32_t MERRCR0;    // 0x050
	volatile uint32_t MERRCR1;    // 0x054
	volatile uint8_t RSVD4[8];    // 0x058
	volatile uint32_t PERRPR;     // 0x060
	volatile uint8_t RSVD5[4];    // 0x064
	volatile uint32_t PERRCR;     // 0x068
	volatile uint8_t RSVD6[4];    // 0x06C
	volatile uint32_t EPCPR;      // 0x070
	volatile uint8_t RSVD7[4];    // 0x074
	volatile uint32_t EPCCR;      // 0x078
	volatile uint8_t RSVD8[132];  // 0x07C
	volatile uint32_t RAILSTAT;   // 0x100
	volatile uint32_t RAILCTL;    // 0x104
	volatile uint32_t RAILSEL;    // 0x108
	volatile uint8_t RSVD9[20];   // 0x10C
	volatile uint32_t PTCMD;      // 0x120
	volatile uint8_t RSVD10[4];   // 0x124
	volatile uint32_t PTSTAT;     // 0x128
	volatile uint8_t RSVD11[212]; // 0x12C
	volatile uint32_t PDSTAT0;    // 0x200
	volatile uint32_t PDSTAT1;    // 0x204
	volatile uint8_t RSVD12[248]; // 0x208
	volatile uint32_t PDCTL0;     // 0x300
	volatile uint32_t PDCTL1;     // 0x304
	volatile uint8_t RSVD13[536]; // 0x308
	volatile uint32_t MCKOUT0;    // 0x520
	volatile uint32_t MCKOUT1;    // 0x524
	volatile uint8_t RSVD14[728]; // 0x528
	volatile uint32_t MDSTAT[41]; // 0x800
	volatile uint8_t RSVD15[348]; // 0x8A4
	volatile uint32_t MDCTL[41];  // 0xA00
} PSCRegs;

#define PSC ((PSCRegs*) 0x01C41000)

/* PSC constants */
#define LPSC_VPSS_MAST      0
#define LPSC_VPSS_SLV       1
#define LPSC_TPCC        	2
#define LPSC_TPTC0       	3
#define LPSC_TPTC1       	4
#define LPSC_EMAC0       	5
#define LPSC_EMAC1       	6
#define LPSC_MDIO           7
#define LPSC_1394           8
#define LPSC_USB            9
#define LPSC_ATA            10
#define LPSC_VLYNQ          11
#define LPSC_HPI            12
#define LPSC_DDR2           13
#define LPSC_AEMIF			14
#define LPSC_MMCSD          15
#define LPSC_MEMSTK         16
#define LPSC_ASP            17
#define LPSC_I2C            18
#define LPSC_UART0          19
#define LPSC_GPIO           26
#define LPSC_TIMER0         27
#define LPSC_ARM            31
#define LPSC_DSP            39
#define LPSC_IMCOP          40

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
	volatile uint8_t RSVD0[4];		//0x00
	volatile uint32_t SDRSTAT;		//0x04
	volatile uint32_t SDBCR;			//0x08
	volatile uint32_t SDRCR;			//0x0C
	volatile uint32_t SDTIMR;			//0x10
	volatile uint32_t SDTIMR2;		//0x14
	volatile uint8_t RSVD1[8];		//0x18 
	volatile uint32_t PBBPR;			//0x20
	volatile uint8_t RSVD2[156];	//0x24 
	volatile uint32_t IRR;				//0xC0
	volatile uint32_t IMR;				//0xC4
	volatile uint32_t IMSR;			//0xC8
	volatile uint32_t IMCR;			//0xCC
	volatile uint8_t RSVD3[20];	//0xD0
	volatile uint32_t DDRPHYCR;		//0xE4
	volatile uint8_t RSVD4[8];		//0xE8
	volatile uint32_t VTPIOCR;		//0xF0
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
    volatile uint32_t ERCSR;      // 0x00
    volatile uint32_t AWCCR;      // 0x04
    volatile uint32_t SDBCR;      // 0x08
    volatile uint32_t SDRCR;      // 0x0C
    volatile uint32_t AB1CR;      // 0x10
    volatile uint32_t AB2CR;      // 0x14
    volatile uint32_t AB3CR;
    volatile uint32_t AB4CR;
    volatile uint32_t SDTIMR;     // 0x20
    volatile uint32_t DDRSR;
    volatile uint32_t DDRPHYCR;
    volatile uint32_t DDRPHYSR;
    volatile uint32_t TOTAR;      // 0x30
    volatile uint32_t TOTACTR;
    volatile uint32_t DDRPHYID_REV;
    volatile uint32_t SDSRETR;
    volatile uint32_t EIRR;       // 0x40
    volatile uint32_t EIMR;
    volatile uint32_t EIMSR;
    volatile uint32_t EIMCR;
    volatile uint32_t IOCTRLR;    // 0x50
    volatile uint32_t IOSTATR;
    volatile uint8_t RSVD0[8];
    volatile uint32_t NANDFCR;    // 0x60
    volatile uint32_t NANDFSR;
    volatile uint8_t RSVD1[8];
    volatile uint32_t NANDF1ECC;  // 0x70
    volatile uint32_t NANDF2ECC;
    volatile uint32_t NANDF3ECC;
    volatile uint32_t NANDF4ECC;
    volatile uint8_t RSVD2[4];    // 0x80
    volatile uint32_t IODFTECR;
    volatile uint32_t IODFTGCR;
    volatile uint8_t RSVD3[4];
    volatile uint32_t IODFTMRLR;  // 0x90
    volatile uint32_t IODFTMRMR;
    volatile uint32_t IODFTMRMSBR;
    volatile uint8_t RSVD4[20];
    volatile uint32_t MODRNR;     // 0xB0
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
	volatile uint32_t MSR;
	volatile uint32_t SCR;
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
    volatile uint32_t GPINT_GPEN;
    volatile uint32_t GPTDAT_GPDIR;
    volatile uint32_t TIM12;
    volatile uint32_t TIM34;
    volatile uint32_t PRD12;
    volatile uint32_t PRD34;
    volatile uint32_t TCR;
    volatile uint32_t TGCR;
    volatile uint32_t WDTCR;
} timerRegs;

#define TIMER0 ((timerRegs*) 0x01C21400)

//Timer inline functions
static inline void TIMER0Start(void)
{
     AINTC->IRQ1 |= 0x00000001;
     TIMER0->TGCR  = 0x00000000;
     TIMER0->TIM12 = 0x0;
     TIMER0->TCR   = 0x00400040;
     TIMER0->TGCR  = 0x00000003;
}

static inline int TIMER0Status(void)
{
     return ((AINTC->IRQ1)&1);
}

// Function Prototypes

// Execute LPSC state transition
void LPSCTransition(uint8_t module, uint8_t state);

// Initialization prototypes
void DM644xInit(void);
void PSCInit(void);
void UARTInit(void);
void PLL1Init(void);
void PLL2Init(void);
void DDR2Init(void);
//void AEMIFInit(void);
void IVTInit(void);

#endif // End _DM644X_H_
