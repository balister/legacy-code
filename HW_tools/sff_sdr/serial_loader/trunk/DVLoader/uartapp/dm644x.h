/* -------------------------------------------------------------------------- *
 * dm644x.h - System module registers of DM644x and other general defines     *
 * -------------------------------------------------------------------------- */

#ifndef _DM644X_H_
#define _DM644X_H_ 

//Return type defines 
#define E_PASS    0x00000000u
#define E_FAIL    0x00000001u
#define E_TIMEOUT 0x00000002u 

/* -------------------------------------------------------------------------- *
 *    System Control Module register structure - See sprue14.pdf, Chapter 10  *
 *       for more details.                                                    *
 * -------------------------------------------------------------------------- */ 

typedef struct _sys_module_regs_
{
	volatile unsigned int PINMUX[2]; 		//0x00
	volatile unsigned int DSPBOOTADDR;		//0x08
	volatile unsigned int SUSPSRC;			//0x0C
	volatile unsigned int INTGEN;				//0x10
	volatile unsigned int BOOTCFG;			//0x14
	volatile unsigned char RSVD0[16];		//0x18
	volatile unsigned int DEVICE_ID;			//0x28
	volatile unsigned char RSVD1[8];			//0x2C
	volatile unsigned int USBPHY_CTL;		//0x34
	volatile unsigned int CHP_SHRTSW;		//0x38
	volatile unsigned int MSTPRI[2];			//0x3C
	volatile unsigned int VPSS_CLKCTL;		//0x44
	volatile unsigned int VDD3P3V_PWDN;		//0x48
	volatile unsigned int DDRVTPER;			//0x4C
	volatile unsigned char RSVD2[32];		//0x50 
} sysModuleRegs;

#define SYSTEM ((sysModuleRegs*) 0x01C40000)

/* -------------------------------------------------------------------------- *
 *    ARM Interrupt Controller register structure - See sprue26.pdf for more      *
 *       details.                                                             *
 * -------------------------------------------------------------------------- */

typedef struct _aintc_regs_
{
    volatile unsigned int FIQ0;
    volatile unsigned int FIQ1;
    volatile unsigned int IRQ0;
    volatile unsigned int IRQ1;
    volatile unsigned int FIQENTRY;
    volatile unsigned int IRQENTRY;
    volatile unsigned int EINT0;
    volatile unsigned int EINT1;
    volatile unsigned int INTCTL;
    volatile unsigned int EABASE;
    volatile unsigned char RSVD0[8];
    volatile unsigned int INTPRI0;
    volatile unsigned int INTPRI1;
    volatile unsigned int INTPRI2;
    volatile unsigned int INTPRI3;
    volatile unsigned int INTPRI4;
    volatile unsigned int INTPRI5;
    volatile unsigned int INTPRI6;
    volatile unsigned int INTPRI7;
} aintcRegs;

#define AINTC ((aintcRegs*) 0x01C48000)

/* -------------------------------------------------------------------------- *
 *    PLL Register structure - See sprue14.pdf, Chapter 6 for more details.   *
 * -------------------------------------------------------------------------- */
typedef struct _PLL_regs_
{
	volatile unsigned int PID;
	volatile unsigned char RSVD0[224];
	volatile unsigned int RSTYPE;
	volatile unsigned char RSVD1[24];	
	volatile unsigned int PLLCTL;
	volatile unsigned char RSVD2[12];	
	volatile unsigned int PLLM;
	volatile unsigned char RSVD3[4];
	volatile unsigned int PLLDIV1;
	volatile unsigned int PLLDIV2;
	volatile unsigned int PLLDIV3;	
	volatile unsigned char RSVD4[4]; 
	volatile unsigned int POSTDIV;
	volatile unsigned int BPDIV;
	volatile unsigned char RSVD5[2];	
	volatile unsigned int PLLCMD;
	volatile unsigned int PLLSTAT;
	volatile unsigned int ALNCTL;
	volatile unsigned int DCHANGE;
	volatile unsigned int CKEN;
	volatile unsigned int CKSTAT;
	volatile unsigned int SYSTAT;
	volatile unsigned char RSVD6[12];
	volatile unsigned int PLLDIV4;
	volatile unsigned int PLLDIV5;
} PLLRegs;

#define PLL1 ((PLLRegs*) 0x01C40800)
#define PLL2 ((PLLRegs*) 0x01C40C00)

/* -------------------------------------------------------------------------- *
 *    Power/Sleep Ctrl Register structure - See sprue14.pdf, Chapter 7        * 
 *       for more details.                                                    *
 * -------------------------------------------------------------------------- */
typedef struct _PSC_regs_
{
	volatile unsigned int PID;
	volatile unsigned char RSVD0[12];
	volatile unsigned int GBLCTL;
	volatile unsigned char RSVD1[4];
	volatile unsigned int INTEVAL;
	volatile unsigned char RSVD2[36];	
	volatile unsigned int MERRPR0;
	volatile unsigned int MERRPR1;
	volatile unsigned char RSVD3[8];
	volatile unsigned int MERRCR0;
	volatile unsigned int MERRCR1;		
	volatile unsigned char RSVD4[8];
	volatile unsigned int PERRPR;
	volatile unsigned char RSVD5[4];
	volatile unsigned int PERRCR;
	volatile unsigned char RSVD6[4];
	volatile unsigned int EPCPR;
	volatile unsigned char RSVD7[4];
	volatile unsigned int EPCCR;
	volatile unsigned char RSVD8[132];
	volatile unsigned int RAILSTAT;
	volatile unsigned int RAILCTL;
	volatile unsigned int RAILSEL;
	volatile unsigned char RSVD9[20];
	volatile unsigned int PTCMD;
	volatile unsigned char RSVD10[4];
	volatile unsigned int PTSTAT;
	volatile unsigned char RSVD11[212];
	volatile unsigned int PDSTAT0;
	volatile unsigned int PDSTAT1;
	volatile unsigned char RSVD12[248];
	volatile unsigned int PDCTL0;
	volatile unsigned int PDCTL1;
	volatile unsigned char RSVD13[536];
	volatile unsigned int MCKOUT0;
	volatile unsigned int MCKOUT1;
	volatile unsigned char RSVD14[728];
	volatile unsigned int MDSTAT[41];
	volatile unsigned char RSVD15[348];
	volatile unsigned int MDCTL[41];
} PSCRegs;

#define PSC ((PSCRegs*) 0x01C41000)	
 
/* -------------------------------------------------------------------------- *
 *    DDR2 Memory Ctrl Register structure - See sprue22b.pdf for more details.*
 * -------------------------------------------------------------------------- */
typedef struct _DDR2_MEM_CTL_REGS_
{
	volatile unsigned char RSVD0[4];		//0x00
	volatile unsigned int SDRSTAT;		//0x04
	volatile unsigned int SDBCR;			//0x08
	volatile unsigned int SDRCR;			//0x0C
	volatile unsigned int SDTIMR;			//0x10
	volatile unsigned int SDTIMR2;		//0x14
	volatile unsigned char RSVD1[8];		//0x18 
	volatile unsigned int PBBPR;			//0x20
	volatile unsigned char RSVD2[156];	//0x24 
	volatile unsigned int IRR;				//0xC0
	volatile unsigned int IMR;				//0xC4
	volatile unsigned int IMSR;			//0xC8
	volatile unsigned int IMCR;			//0xCC
	volatile unsigned char RSVD3[20];	//0xD0
	volatile unsigned int DDRPHYCR;		//0xE4
	volatile unsigned char RSVD4[8];		//0xE8
	volatile unsigned int VTPIOCR;		//0xF0
} DDR2Regs;

#define DDRVTPR (*((volatile unsigned int*) 0x01C42030))
#define DDR ((DDR2Regs*) 0x20000000)
#define DDR_TEST_PATTERN 0xA55AA55Au
#define DDR_RAM_SIZE 0x8000000u


/* -------------------------------------------------------------------------- *
 *    AEMIF Register structure - See sprue20a.pdf for more details.           *
 * -------------------------------------------------------------------------- */
typedef struct _emif_regs_
{
    volatile unsigned int ERCSR;
    volatile unsigned int AWCCR;
    volatile unsigned int SDBCR;
    volatile unsigned int SDRCR;
    volatile unsigned int AB1CR;
    volatile unsigned int AB2CR;
    volatile unsigned int AB3CR;
    volatile unsigned int AB4CR;
    volatile unsigned int SDTIMR;
    volatile unsigned int DDRSR;
    volatile unsigned int DDRPHYCR;
    volatile unsigned int DDRPHYSR;
    volatile unsigned int TOTAR;
    volatile unsigned int TOTACTR;
    volatile unsigned int DDRPHYID_REV;
    volatile unsigned int SDSRETR;
    volatile unsigned int EIRR;
    volatile unsigned int EIMR;
    volatile unsigned int EIMSR;
    volatile unsigned int EIMCR;
    volatile unsigned int IOCTRLR;
    volatile unsigned int IOSTATR;
    volatile unsigned char RSVD0[8];
    volatile unsigned int NANDFCR;
    volatile unsigned int NANDFSR;
    volatile unsigned char RSVD1[8];
    volatile unsigned int NANDF1ECC;
    volatile unsigned int NANDF2ECC;
    volatile unsigned int NANDF3ECC;
    volatile unsigned int NANDF4ECC;
    volatile unsigned char RSVD2[4];
    volatile unsigned int IODFTECR;
    volatile unsigned int IODFTGCR;
    volatile unsigned char RSVD3[4];
    volatile unsigned int IODFTMRLR;
    volatile unsigned int IODFTMRMR;
    volatile unsigned int IODFTMRMSBR;
    volatile unsigned char RSVD4[20];
    volatile unsigned int MODRNR;
} emifRegs;

#define AEMIF ((emifRegs*) 0x01E00000)

/* -------------------------------------------------------------------------- *
 *    UART Register structure - See sprue33.pdf for more details.             *
 * -------------------------------------------------------------------------- */
typedef struct _uart_regs_
{
	volatile unsigned int RBR;
	volatile unsigned int IER;
	volatile unsigned int IIR;
	volatile unsigned int LCR;
	volatile unsigned int MCR;
	volatile unsigned int LSR;
	volatile unsigned int MSR;
	volatile unsigned int SCR;
	volatile unsigned char DLL;
	volatile unsigned char RSVDO[3];
	volatile unsigned char DLH;
	volatile unsigned char RSVD1[3];	
	volatile unsigned int PID1;
	volatile unsigned int PID2;
	volatile unsigned int PWREMU_MGNT;
} uartRegs;

#define THR RBR
#define FCR IIR

#define UART0 ((uartRegs*) 0x01C20000)

/* -------------------------------------------------------------------------- *
 *    Timer Register structure - See sprue26.pdf for more details.             *
 * -------------------------------------------------------------------------- */
typedef struct _timer_regs_
{
    volatile unsigned int PID12;
    volatile unsigned int EMUMGT_CLKSPD;
    volatile unsigned int GPINT_GPEN;
    volatile unsigned int GPTDAT_GPDIR;
    volatile unsigned int TIM12;
    volatile unsigned int TIM34;
    volatile unsigned int PRD12;
    volatile unsigned int PRD34;
    volatile unsigned int TCR;
    volatile unsigned int TGCR;
    volatile unsigned int WDTCR;
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

/*
 * boot() has naked attribute (doesn't save registers since it is the entry point
 * out of boot and it doesn't have an exit point). This setup requires
 * that the gnu compiler uses the -nostdlib option. 
 */
void boot( void ) __attribute__((naked,section (".boot")));

void PlatformInit(void);
void UARTInit(void);
void PLL1Init(void);
void PLL2Init(void);
void DDR2Init(void);
void AEMIFInit(void);


// NOP wait loop 
void waitloop(unsigned int loopcnt);
  
#endif // End _DM644X_H_
