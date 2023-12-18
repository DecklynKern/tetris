#include <sys/time.h>
#include <time.h>
#include <stdio.h>

static double timer_seconds_before_last_pause;
static struct timeval last_unpause_time;

static double seconds_since_last_unpause() {

    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    return
        current_time.tv_sec - last_unpause_time.tv_sec + 
        (double) (current_time.tv_usec - last_unpause_time.tv_usec) / 1000000;
        
}

void timer_pause(void) {
    timer_seconds_before_last_pause += seconds_since_last_unpause();
}

void timer_unpause(void) {
    gettimeofday(&last_unpause_time, NULL);
}

double get_timer_seconds(void) {
    return timer_seconds_before_last_pause + seconds_since_last_unpause();
}

long get_timer_ms(void) {
    return get_timer_seconds() * 1000;
}

void get_time_formatted(char* buf) {
    long timer_ms = get_timer_ms();
    sprintf(buf, "%02ld:%02ld.%03ld", timer_ms / 60000, timer_ms / 1000 % 60, timer_ms % 1000);
}