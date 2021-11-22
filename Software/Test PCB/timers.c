/*
 * timers.c
 * Code to handle PIC timers.
 */


#include "Test PCB.h"
#include "timers.h"

/* System Clock and Timer values */
volatile unsigned int clockMilliseconds;
volatile unsigned int clockSeconds;
unsigned int timerMilliseconds;
unsigned int timerSeconds;
volatile unsigned int MeasurementTick;
unsigned int MeasurementTicksPerMillisecond;
Bool TimerSet;
Bool TickTimerOn;

/* ******************************
 * Timer Routines
 * Some generic routines for handling timers.
 * Also there is a single timer, the tick timer,
 * that is used by various other routines.
 * This timer can be set and tested for 
 * expiration by the routines:
 * SetTimer()
 * TimerExpired()
 * The routine Wait() uses these two routines.
 * This timer is also used as the measurement timer, and
 * the interval is changed when that happens.
 */

/*
 * Once at power up.
 */
void InitializeTimers(void) {
	/*
	 * Timer1 is the tick timer
	 */
	T1CONbits.TSIDL = TRUE;
	T1CONbits.TCKPS = 0x01;     /* Timer pre scale of 8 */
	T1CONbits.TCS = 0;          /* use peripheral clock */
    T1CONbits.TGATE = 0;
    PR1 = TICK_TIMER_VALUE;
	TMR1 = 0;
    IFS0bits.T1IF = 0;
    IEC0bits.INT1IE = TRUE;
	T1CONbits.TON = TRUE;
}

/*
 * Get time from system clock.
 */
void GetTime(unsigned int * seconds, unsigned int * milliseconds) {
	/* Timer Interrupt might break in, so be careful */
	*milliseconds = clockMilliseconds;
	*seconds = clockSeconds;
	if (*milliseconds != clockMilliseconds) {
		*milliseconds = clockMilliseconds;
		*seconds = clockSeconds;
	}
}

/*
 * Add some milliseconds to a second and millisecond pair.
 */
void AddTime(unsigned int * seconds, unsigned int * milliseconds, unsigned int millisecondsAdd) {
	*milliseconds = *milliseconds + millisecondsAdd + 1;
	while (*milliseconds > 999) {
		*milliseconds -= 1000;
		*seconds = *seconds + 1;
	}
}

/*
 * Set timer to go true after the requested delay.
 * Used with Wait() and TimerExpired() routines.
 */
void SetTimer(unsigned int milliseconds) {
	GetTime(&timerSeconds, &timerMilliseconds);
	AddTime(&timerSeconds, &timerMilliseconds, milliseconds);
	TimerSet = TRUE;
}

/*
 * Test passed values against system clock.
 */
Bool TimeExpired(unsigned int seconds, unsigned int milliseconds) {
	Bool theReturn=FALSE;

	if (clockSeconds > seconds) {
		theReturn = TRUE;
	} else if ((clockSeconds == seconds) && (clockMilliseconds > milliseconds)) {
		theReturn = TRUE;
	}
	return theReturn;
}

/*
 * Timer routine, used with SetTimer.
 * Returns true if timer has expired, false if not.
 * The variable TimerSet is only valid on the first
 * call of this routine after expiration.
 */
Bool TimerExpired(void) {
	Bool theReturn;

	theReturn = TimeExpired(timerSeconds, timerMilliseconds);

	if (theReturn) {	
		TimerSet = FALSE;
	}
	return theReturn;
}

/*
 * Wait routine 
 * waits for requested milliseconds, but adds a fraction of
 * a millisecond to sync with the timer. The fraction may be
 * almost a full millisecond.
 */
void Wait(unsigned int milliseconds) {
	SetTimer(milliseconds);
	
	/* wait here */
	while (!TimerExpired() && TickTimerOn) ;

	return;
}
