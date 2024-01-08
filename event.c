#include "event.h"
#include <ev.h>

struct ev_loop *loop = NULL; // 定义 loop

void initialize_loop() {
    if (loop == NULL) {
        loop = ev_loop_new(EVFLAG_AUTO);
    }
}