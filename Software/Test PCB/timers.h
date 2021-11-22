/*
 * timers.h
 * Public declarations for timers.c
 */

/*
 * $Log: timers.h,v $
 * Revision 1.1.1.1  2012/09/13 15:20:33  Dexter
 * Initial check in
 *
 * Revision 1.1.1.1  2012/05/21 21:56:53  Dexter
 * Initial check in
 *
 * Revision 1.2  2011/06/21 16:20:30  Dexter
 * Initial check in
 *
 */
#ifndef TIMERS_H

#include "types.h"

#define TICK_TIMER_PRESET PR1
#define TICK_TIMER_VALUE (1000/TICK_TIMER_PRESCALE) * FMcycle

#define TICK_TIMER_PRESCALE 8


/* Global variables */
extern volatile unsigned int clockMilliseconds;
extern volatile unsigned int clockSeconds;
extern unsigned int timerMilliseconds;
extern unsigned int timerSeconds;
extern volatile unsigned int MeasurementTick;
extern unsigned int MeasurementTicksPerMillisecond;

/* Public functions */
extern void InitializeTimers(void);
extern void GetTime(unsigned int * seconds, unsigned int * milliseconds);
extern void AddTime(unsigned int * seconds, unsigned int * milliseconds, unsigned int millisecondsAdd);
extern void SetTimer(unsigned int milliseconds);
extern void Wait(unsigned int milliseconds);
extern Bool TimeExpired(unsigned int seconds, unsigned int milliseconds);
extern Bool TimerExpired(void);
extern Bool TickTimerOn;

#define TIMERS_H
#endif
