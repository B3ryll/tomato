// ================================================================
//  ./tomato.c
//  
//  I created this program as a personal tool to assist me in managing my
//  programming time more efficiently. Similar to the standard software
//  available on suckless.org, this program is configured by a header that must
//  be modified at compile time, which is the "config.h" file. Users can adapt
//  it to their preferences to personalize the experience.
//
// ================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ncurses.h>

#define NormalMode -1
#define cmd(c) {c, NormalMode, False}

// ----------------------------------------------
// base definitions
// ----------------------------------------------

extern char** environ;

// timer defs

enum TimerMode
{
    TimerFocusMode,
    TimerRestMode,
    TimerLongRestMode,
    
    TIMER_MODE_SIZE, 
};

enum TimerState
{
    TimerStateInactive,
    TimerStateRunning,
    TimerStatePaused,

    TIMER_STATE_SIZE,
};

enum Keybind
{
    KeybindContinue,
    KeybindReset,
    KeybindQuit,
    KeybindNext,
    KeybindPrevious,

    KEYBIND_SIZE,
};

struct TimeFormat
{
    unsigned short int seconds;
    unsigned short int minutes;
    unsigned short int hours;
};
typedef struct TimeFormat TimeFormat;

struct TimerIteration
{
    char        mode;
    TimeFormat  interval;
};
typedef struct TimerIteration TimerIteration;

struct Timer {
    char      state;
    short int iter_index;
    int       time_left_secs;
};
typedef struct Timer Timer;

// ----------------------------------------------

#include "config.h"

// ----------------------------------------------

const size_t iter_size = sizeof(iterations)/sizeof(iterations[0]);
    
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

inline void timeformat_print(TimeFormat time)
{
    printw("\e\r %d : %d : %d", time.hours, time.minutes, time.seconds);
}

// ----------------------------------------------
// program execution loop functions
// ----------------------------------------------

// ---- state management -----

void timer_update(int delta, Timer* state)
{
    if (state->state != TimerStateRunning) return;

    if (state->time_left_secs > 0)
    {
        state->time_left_secs--;
        return;
    }

    state->iter_index     = (state->iter_index + 1) % iter_size;
    state->time_left_secs = SECONDS(iterations[state->iter_index].interval);

    state->state = TimerStateInactive;
}

void timer_print(Timer timer)
{ 
    TimerIteration current_iter = iterations[timer.iter_index];
    
    const char* mode_str  = timer_symbols[current_iter.mode];
    const char* state_str = timer_state_symbols[timer.state];
    
    TimeFormat timeform = timeformat_from_seconds(timer.time_left_secs);
    
    printw(
        "%d : %d : %d [%s] - %s",
        timeform.hours, timeform.minutes, timeform.seconds,
        state_str,      mode_str 
    );
}

// ---------------------------

void handle_input(Timer* timer, char* is_running)
{
    int ch = getch();

    if (ch == keybindings[KeybindQuit])
    {
        *is_running = FALSE;
    }
    else if (ch == keybindings[KeybindReset])
    {
        timer->state          = TimerStateInactive;
        timer->time_left_secs = SECONDS(iterations[timer->iter_index].interval);
    }
    else if (ch == keybindings[KeybindNext])
    {
        timer->state          = TimerStateInactive;
        timer->iter_index     = (timer->iter_index + 1) % iter_size;
        timer->time_left_secs = SECONDS(iterations[timer->iter_index].interval);
    }
    else if (ch == keybindings[KeybindPrevious])
    {
        timer->state          = TimerStateInactive;
        timer->iter_index     = (timer->iter_index - 1) % iter_size;
        timer->time_left_secs = SECONDS(iterations[timer->iter_index].interval);
    }
    else if (ch == keybindings[KeybindContinue])
    {
        char is_active = timer->state == TimerStateRunning;
        timer->state   = is_active ? TimerStatePaused : TimerStateRunning;

        if (timer->state == TimerStatePaused)
        {
            char* cmd[] = {shell, "-c", pause_cmd, NULL};
            spawn(cmd);
        } else {
            char* cmd[] = {shell, "-c", start_cmd, NULL};
            spawn(cmd);
        }

    }
}

void init_curses()
{
    initscr();
    cbreak();
    noecho();

    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    curs_set(0);
}

typedef struct {
    const Timer* timer;
} RenderCtx;

// -----------------------
// render function
// -----------------------
void render(RenderCtx ctx)
{
    for (int counter = 0; counter < TIMER_MODE_SIZE; counter++)
    {
        const char is_current     = iterations[ctx.timer->iter_index].mode == counter;
        const char sign           = is_current ? '>' : '.';
        const TimeFormat interval = iterations[counter].interval;

        printw(" %c %-12s [%d:%d:%d]\n", sign, timer_symbols[counter],
                interval.hours, interval.minutes, interval.seconds);
    }

    move(LINES - 2, 1);

    timer_print(*ctx.timer);

    const char next_index   = (ctx.timer->iter_index + 1) % iter_size;
    const char* next_symbol = timer_symbols[iterations[next_index].mode];
    printw("  {next: %s}", next_symbol);
}

void run(short int *err)
{
    Timer counter = {
        .time_left_secs = SECONDS(iterations[0].interval),
    };

    RenderCtx render_ctx = {.timer = &counter};


    const int UPDATE_COUNTER_MAX = 4;
    int update_counter           = 0;

    init_curses();

    char is_running = TRUE;
    do
    {
        handle_input(&counter, &is_running);
        const char curr_iter = counter.iter_index;

        update_counter++;

        if (update_counter >= 4)
        {
            timer_update(1000, &counter);
            update_counter = 0;
        }

        render(render_ctx);

        if (curr_iter != counter.iter_index)
        {
            char* cmd[] = {
                shell, "-c", mode_change_cmd, NULL
            };
            spawn(cmd);
        }

        refresh();
        erase();

        napms(250);

    } while (is_running);

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
        printf("blob :3\n");
    }

    return 0;
}
