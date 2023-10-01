// ====================================================
//   tomato configuration file
// 
// ====================================================

char shell[] = "/home/terminal/usr/local/bin/oksh";

char* mode_change_cmd = "echo mode changed | dnote -loc 2 -exp 1.3;";
char* pause_cmd       = "echo mode paused  | dnote -loc 2 -exp 1.3;";
char* start_cmd       = "echo node started | dnote -loc 2 -exp 1.3;";

const char* timer_symbols[] = {
    [TimerFocusMode]    = "<focus>",
    [TimerRestMode]     = "<rest>",
    [TimerLongRestMode] = "<long-rest>",
};

const char* timer_state_symbols[] = {
    [TimerStateInactive] = "/inactive/",
    [TimerStateRunning]  = "/running/",
    [TimerStatePaused]   = "/paused/",
};

const int keybindings[] = {
    [KeybindContinue] = 'c',
    [KeybindReset]    = 'r',
    [KeybindQuit]     = 'q',
};

#define FOCUS_MODE    {TimerFocusMode,    {.seconds = 10, .minutes = 0, .hours = 0}}
#define REST_MODE     {TimerRestMode,     {.seconds = 10, .minutes = 0, .hours = 0}}
#define LONGREST_MODE {TimerLongRestMode, {.seconds = 10, .minutes = 0, .hours = 0}}

TimerIteration iterations[] = {
    FOCUS_MODE, REST_MODE,
    FOCUS_MODE, REST_MODE,
    FOCUS_MODE, REST_MODE,
    FOCUS_MODE, LONGREST_MODE,
};
