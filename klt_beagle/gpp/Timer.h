#ifndef _TIMER_H
#define _TIMER_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>
#include<sys/time.h>

typedef struct
{
    struct timeval startTime;
    struct timeval stopTime;
    double elapsedTime;
    char nameTime[128];
}Timer;

void initTimer(Timer *t, char *s)
{
    t->elapsedTime = 0.0;
    strncpy(t->nameTime,s,128);
}

void clearTimer(Timer *t)
{
    t->elapsedTime = 0.0;
}

void startTimer(Timer *t)
{
    gettimeofday(&(t->startTime), NULL);
}

void restartTimer(Timer *t)
{
    t->elapsedTime = 0.0;
    gettimeofday(&(t->startTime), NULL);
}

void stopTimer(Timer *t)
{
    gettimeofday(&(t->stopTime), NULL);

    t->elapsedTime =  ( (t->stopTime).tv_sec  - (t->startTime).tv_sec) * 1000.0;      // sec to ms
    t->elapsedTime += ( (t->stopTime).tv_usec - (t->startTime).tv_usec) / 1000.0;   // us to ms
}

double getTime(Timer t)
{
    return t.elapsedTime;
}

void printTime(Timer t)
{
    printf("%s = %g msec\n",t.nameTime, t.elapsedTime);
}

#endif
