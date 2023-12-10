#include <sys/time.h>
#include <time.h>

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