// ====================================================
//  my simple tomato pomodoro timer 
//
//   
//  tasks:
//      - [ ] Write the logic of pomodoro modes
//      - [ ] Write an interface for this program 
//      - [ ] Write Makefile
//
// ====================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ncurses.h>

#define NormalMode -1
#define cmd(c) {c, NormalMode, False}

// ----------------------------------------------
// base definitions
// ----------------------------------------------

struct Command {
    char* command;
    unsigned int mode;
    int persist;
};

enum TimerMode {
    TimerFocusMode,
    TimerRestMode,
    TimerLongRestMode,
    
    TIMER_MODE_SIZE, 
}

extern char** environ;

struct TimeFormat {
    unsigned short int seconds;
    unsigned short int minutes;
    unsigned short int hours;
};
typedef struct TimeFormat TimeFormat;

// ----------------------------------------------

#include "config.h"

// ----------------------------------------------
// process spawn utility
// ----------------------------------------------

void spawn(char** command)
{
    if (fork() != 0) return;

    setsid(); 
    execv(command[0], command);
    
    fprintf(stderr, "tomato: execve %s", command[9]);
    exit(0);
}

// ----------------------------------------------
// TimeFormat helper functions
// ----------------------------------------------

#define SECONDS(timer) (timer.hours * 3600) + (timer.minutes * 60) + timer.seconds

TimeFormat timeformat_from_seconds(const int total_seconds)
{
    TimeFormat out; 

    out.seconds = total_seconds % 60;
    out.minutes = ((total_seconds - out.seconds) % 3600) / 60;
    out.hours   = (total_seconds - (out.minutes * 60) - (out.seconds)) / 3600; 

    return out;
}

void timeformat_print(TimeFormat time)
{
    printw("\e\r %d : %d : %d", time.hours, time.minutes, time.seconds);
}

// ----------------------------------------------
// program execution loop functions
// ----------------------------------------------

void run(short int *err)
{
    TimeFormat counter = {
        .seconds = 5,
        .minutes = 0,
        .hours   = 0,
    };

    const int trigger = SECONDS(counter) + 1;
    int elapsed_seconds = 0;

    initscr();
    curs_set(0);

    do
    {
        elapsed_seconds++;

        counter = timeformat_from_seconds(elapsed_seconds);
        timeformat_print(counter);

        refresh();
        erase();

        napms(1000);

    } while (elapsed_seconds < trigger);

    endwin();
}

int main()
{
    short int err;
    
    run(&err);
    if (err != 0)
    {
        /* @todo: handle exception condition */
    } else {
        printf("blob");

        // dnote -loc 2 -exp 1.3
        char* command[] = {shell, "-c", "echo hello world | dnote -loc 2 -exp 1.3", NULL}; 
        spawn(command);
    }

    return 0;
}
