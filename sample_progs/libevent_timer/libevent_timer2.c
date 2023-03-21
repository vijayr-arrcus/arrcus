#include <event2/event.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static struct timeval LAST;

static void refresh(int fd, short flags, void *userdata)
{
    struct timeval now, diff;
    gettimeofday(&now, NULL);
    timersub(&now, &LAST, &diff);
    printf("%ld.%06ld\n", diff.tv_sec, diff.tv_usec);
    LAST = now;
    sleep(15);
    printf("done with sleep \n");
}

int main(int argc, const char *argv[])
{
    struct event_base *base = event_base_new();
    struct event *event = event_new(base, -1, EV_PERSIST, refresh, NULL);
    struct timeval timeout = { .tv_sec = 10, .tv_usec = 0 };
    event_add(event, &timeout);
//    event_active(event, EV_TIMEOUT, 0);
    gettimeofday(&LAST, NULL);
    event_base_dispatch(base);
    return 0;
}
