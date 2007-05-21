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
#include <arm926ejs.h>

#include "soc.h"
#include "timer.h"

static ulong timestamp;
static ulong lastdec;

/* Use Timer 3&4 (Timer 2) */
#define TIMER_BASE_ADDR	CSL_TMR_1_REGS

#define TIMER_LOAD_VAL 0xffffffff

davinci_timer_reg    *davinci_timer = (davinci_timer_reg *) TIMER_BASE_ADDR;

/* Timer Initialize */
void timer_init (void)
{
        /* disable Timer 1 & 2 timers */
        davinci_timer->tcr = 0;

        /* Set timers to unchained dual 32 bit timers, Unreset timer34 */
        davinci_timer->tgcr = 0x0;
        davinci_timer->tgcr = 0x6;

        /* Program the timer12 counter register - set the prd12 for right count */
        davinci_timer->tim34 = 0;

        /* The timer is programmed to expire after 0xFFFFFFFF ticks */
        davinci_timer->prd34 = 0xFFFFFFFF;

        /* Enable timer34 */
        davinci_timer->tcr = (0x80 << 16); /* Timer34 continously enabled, Timer12 disabled */
}

/* c+p from cpu/arm926ejs/omap/timer.c */ 

/*
 * timer without interrupts
 */

void reset_timer (void)
{
	reset_timer_masked ();
}

ulong get_timer (ulong base)
{
	return get_timer_masked () - base;
}

void set_timer (ulong t)
{
	timestamp = t;
}

/* delay x useconds AND perserve advance timstamp value */
void udelay (unsigned long usec)
{
	ulong tmo, tmp;

	if(usec >= 1000){		/* if "big" number, spread normalization to seconds */
		tmo = usec / 1000;	/* start to normalize for usec to ticks per sec */
		tmo *= CFG_HZ;		/* find number of "ticks" to wait to achieve target */
		tmo /= 1000;		/* finish normalize. */
	}else{				/* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CFG_HZ;
		tmo /= (1000*1000);
	}

	tmp = get_timer (0);		/* get current timestamp */
	if( (tmo + tmp + 1) < tmp )	/* if setting this fordward will roll time stamp */
		reset_timer_masked ();	/* reset "advancing" timestamp to 0, set lastdec value */
	else
		tmo += tmp;		/* else, set advancing stamp wake up time */

	while (get_timer_masked () < tmo)/* loop till event */
		/*NOP*/;
}

void reset_timer_masked (void)
{
	/* reset time */
	lastdec = READ_TIMER;  /* capure current decrementer value time */
	timestamp = 0;	       /* start "advancing" time stamp from 0 */
}

ulong get_timer_masked (void)
{
	ulong now = READ_TIMER;		/* current tick value */

	if (lastdec >= now) {		/* normal mode (non roll) */
		/* normal mode */
		timestamp += lastdec - now; /* move stamp fordward with absoulte diff ticks */
	} else {			/* we have overflow of the count down timer */
		/* nts = ts + ld + (TLV - now)
		 * ts=old stamp, ld=time that passed before passing through -1
		 * (TLV-now) amount of time after passing though -1
		 * nts = new "advancing time stamp"...it could also roll and cause problems.
		 */
		timestamp += lastdec + TIMER_LOAD_VAL - now;
	}
	lastdec = now;

	return timestamp;
}

/* waits specified delay value and resets timestamp */
void udelay_masked (unsigned long usec)
{
	ulong tmo;
	ulong endtime;
	signed long diff;

	if (usec >= 1000) {		/* if "big" number, spread normalization to seconds */
		tmo = usec / 1000;	/* start to normalize for usec to ticks per sec */
		tmo *= CFG_HZ;		/* find number of "ticks" to wait to achieve target */
		tmo /= 1000;		/* finish normalize. */
	} else {			/* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CFG_HZ;
		tmo /= (1000*1000);
	}

	endtime = get_timer_masked () + tmo;

	do {
		ulong now = get_timer_masked ();
		diff = endtime - now;
	} while (diff >= 0);
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
	ulong tbclk;

	tbclk = CFG_HZ;
	return tbclk;
}

/************************************************************
********************** Reset Processor **********************
************************************************************/
#define WDT_BASE_ADDR          CSL_WDT_1_REGS


void reset_processor(void)
{
        davinci_timer_reg *davinci_wdt = (davinci_timer_reg *) WDT_BASE_ADDR;
        davinci_wdt->tgcr   = 0x00000008;
        davinci_wdt->tgcr  |= 0x00000003;
        davinci_wdt->tim12  = 0x00000000;
        davinci_wdt->tim34  = 0x00000000;
        davinci_wdt->prd12  = 0x00000000;
        davinci_wdt->prd34  = 0x00000000;
        davinci_wdt->tcr   |= 0x00000040;
        davinci_wdt->wdtcr |= 0x00004000;
        davinci_wdt->wdtcr  = 0xA5C64000;
        davinci_wdt->wdtcr  = 0xDA7E4000;
}
