/* --------------------------------------------------------------------------
    FILE        : ubl.c 				                             	 	        
    PURPOSE     : Main User Boot Loader file
    PROJECT     : DaVinci User Boot-Loader and Flasher
    AUTHOR      : Daniel Allred
    DATE	    : Dec-18-2006  
 
    HISTORY
        v1.00 completion 							 						      
 	        Daniel Allred - Jan-22-2006
 	    v1.10 - DJA - Feb-1-2007
 	        Added dummy entry point make NAND UBL work
 	        with the CCS flashing utility from SDI. This
 	        fakeentry is located at 0x20 at runtime and
 	        simply redirects to the true entry point, boot().
 	    v1.11 - DJA - 7-Mar-2007
 	        Changed power domain initializations.  Added call to 
 	        PSCInit() as first thing, otherwise WDT resets would fail.
 	                                                                      
 ----------------------------------------------------------------------------- */

#include "ubl.h"
#include "dm644x.h"
#include "uart.h"
#include "util.h"

#ifdef UBL_NOR
#include "nor.h"
#endif

#ifdef UBL_NAND
#include "nand.h"
#endif

#define C1_IC           (1<<12)         /* icache off/on */

uint32_t gEntryPoint;
BootMode gBootMode;

void selfcopy()
{
	// Self copy setup 
	extern uint32_t __selfcopysrc, __selfcopydest, __selfcopydestend;
		
	//Enable ITCM
	asm(" MRC p15, 0, r0, c9, c1, 1");
	asm(" MOV r0, #0x1");
	asm(" MCR p15, 0, r0, c9, c1, 1");
		
	//Enable DTCM
	asm(" MRC p15, 0, r0, c9, c1, 0");
	asm(" MOV r0, #0x8000");
	asm(" ORR r0, r0, #0x1");
	asm(" MCR p15, 0, r0, c9, c1, 0");
	
	volatile uint32_t *src = &(__selfcopysrc);
	volatile uint32_t *dest = &(__selfcopydest);
	volatile uint32_t *destend = &(__selfcopydestend);

	// Copy the words
	while (dest < destend)
	{
		*dest = *src;
		dest++;
		src++;
	}
	
	//Jump to the normal entry point 
	boot();	
}

void fake_entry()
{
    boot();
}

/* read co-processor 15, register #1 (control register) */
static unsigned long read_p15_c1 (void)
{
        unsigned long value;

        __asm__ __volatile__(
                "mrc    p15, 0, %0, c1, c0, 0   @ read control reg\n"
                : "=r" (value)
                :
                : "memory");

        return value;
}

/* write to co-processor 15, register #1 (control register) */
static void write_p15_c1 (unsigned long value)
{
        __asm__ __volatile__(
                "mcr    p15, 0, %0, c1, c0, 0   @ write it back\n"
                :
                : "r" (value)
                : "memory");

        read_p15_c1 ();
}

static void cp_delay (void)
{
        volatile int i;

        /* copro seems to need some delay between reading and writing */
        for (i = 0; i < 100; i++);
}

void icache_enable (void)
{
	uint32_t reg;

        reg = read_p15_c1 ();           /* get control reg. */
        cp_delay ();
        write_p15_c1 (reg | C1_IC);
}

void boot()
{   
	asm(" MRS	r0, cpsr");
	asm(" BIC	r0, r0, #0x1F");    // CLEAR MODES
	asm(" ORR	r0, r0, #0x13");    // SET SUPERVISOR mode
	asm(" ORR   r0, r0, #0xC0");    // Disable FIQ and IRQ
	asm(" MSR	cpsr, r0");       
	
	// Set the IVT to low memory, leave MMU & caches disabled 
	asm(" MRC	p15, 0, r1, c1, c0, 0");
 	asm(" BIC	r0,r0,#0x00002000");
	asm(" MCR	p15, 0, r1, c1, c0, 0");
	
	// Stack setup (__topstack symbol defined in linker script)
	extern uint32_t __topstack;
	register uint32_t* stackpointer asm ("sp");	
	stackpointer = &(__topstack);
	
    // Call to main code
    main();
    
    icache_enable();
    
    // Jump to entry point
	APPEntry = (void*) gEntryPoint;
    UARTSendData((uint8_t *) "About to jump to: ",FALSE);
    UARTSendInt((uint32_t)APPEntry);
    UARTSendData((uint8_t *) "\n\r",FALSE);

    (*APPEntry)();	
}

int32_t main(void)
{
	// Read boot mode 
	gBootMode = (BootMode) ( ( (SYSTEM->BOOTCFG) & 0xC0) >> 6);
	
	PSCInit();
	
	if (gBootMode == NON_SECURE_UART)
    {
        // Wait until the RBL is done using the UART. 
        while((UART0->LSR & 0x40) == 0 );
    }

	// Platform Initialization
	DM644xInit();

	// Set RAM pointer to beginning of RAM space
	set_current_mem_loc(0);

	// Send some information to host
    UARTSendData((uint8_t *) "TI UBL Version: ",FALSE);
    UARTSendData((uint8_t *) UBL_VERSION_STRING,FALSE);
    UARTSendData((uint8_t *) ", Flash type: ", FALSE);
    UARTSendData((uint8_t *) UBL_FLASH_TYPE, FALSE);
	UARTSendData((uint8_t *) "\r\nBooting PSP Boot Loader\r\nPSPBootMode = ",FALSE);
	
	/* Select Boot Mode */
	switch(gBootMode)
	{
#ifdef UBL_NAND
		case NON_SECURE_NAND:
		{
			//Report Bootmode to host
			UARTSendData((uint8_t *) "NAND\r\n",FALSE);

			// copy binary or S-record of application from NAND to DDRAM, and decode if needed
			if (NAND_Copy() != E_PASS)
			{
				UARTSendData((uint8_t *) "NAND Boot failed.\r\n", FALSE);
				goto UARTBOOT;
			}
			else
			{
				UARTSendData((uint8_t *) "NAND Boot success.\r\n", FALSE);
			}
			break;
		}
#endif		
#ifdef UBL_NOR
		case NON_SECURE_NOR:
		{
			//Report Bootmode to host
			UARTSendData((uint8_t *) "NOR \r\n", FALSE);

			// Copy binary or S-record of application from NOR to DDRAM, then decode
			if (NOR_Copy() != E_PASS)
			{
				UARTSendData((uint8_t *) "NOR Boot failed.\r\n", FALSE);
				goto UARTBOOT;
			}
			else
			{
				UARTSendData((uint8_t *) "NOR Boot success.\r\n", FALSE);
			}
			break;
		}
#endif		
		case NON_SECURE_UART:
		{
			//Report Bootmode to host
			UARTSendData((uint8_t *) "UART\r\n", FALSE);
            goto UARTBOOT;
			break;
		}
		default:
		{
UARTBOOT:	UART_Boot();
			break;
		}
	}
		
	UARTSendData((uint8_t*)"   DONE", TRUE);
        UARTSendData((uint8_t *) "\r\n", FALSE);

	waitloop(10000);

	// Disabling UART timeout timer
    while((UART0->LSR & 0x40) == 0 );
	TIMER0->TCR = 0x00000000;

	return E_PASS;    
}


