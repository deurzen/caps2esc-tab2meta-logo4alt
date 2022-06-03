#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <linux/input.h>

const struct input_event
syn       = { .type = EV_SYN, .code = SYN_REPORT,   .value = 0 },
esc_up    = { .type = EV_KEY, .code = KEY_ESC,      .value = 0 },
ctrl_up   = { .type = EV_KEY, .code = KEY_LEFTCTRL, .value = 0 },
esc_down  = { .type = EV_KEY, .code = KEY_ESC,      .value = 1 },
ctrl_down = { .type = EV_KEY, .code = KEY_LEFTCTRL, .value = 1 };

static inline int
read_event(struct input_event *event)
{
    return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}

static inline void
write_event(const struct input_event *event)
{
    if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
        exit(EXIT_FAILURE);
}

static inline void
intercept_event(struct input_event *event)
{
    if (event->type == EV_KEY) {
        if (event->code == KEY_LEFTALT)
            event->code = KEY_LEFTMETA;
        else if (event->code == KEY_LEFTMETA)
            event->code = KEY_LEFTALT;
    }

    write_event(event);
}

int
main(void)
{
    const int delay = 20000;

    enum {
        START,
        CAPSLOCK_HELD,
        CAPSLOCK_IS_CTRL
    } state = START;
    struct input_event input;

    setbuf(stdin, NULL), setbuf(stdout, NULL);

    while (read_event(&input)) {
        if (input.type == EV_MSC && input.code == MSC_SCAN)
            continue;

        if (input.type != EV_KEY && input.type != EV_REL && input.type != EV_ABS) {
            write_event(&input);
            continue;
        }

        switch (state) {
        case START:
            if (input.type == EV_KEY && input.code == KEY_CAPSLOCK &&
                input.value)
                state = CAPSLOCK_HELD;
            else
                intercept_event(&input);
            break;
        case CAPSLOCK_HELD:
            if (input.type == EV_KEY && input.code == KEY_CAPSLOCK) {
                if (input.value == 0) {
                    write_event(&esc_down);
                    write_event(&syn);
                    usleep(delay);
                    write_event(&esc_up);
                    state = START;
                }
            } else if ((input.type == EV_KEY && input.value == 1) ||
                       input.type == EV_REL || input.type == EV_ABS) {
                write_event(&ctrl_down);
                write_event(&syn);
                usleep(delay);
                intercept_event(&input);
                state = CAPSLOCK_IS_CTRL;
            } else
                intercept_event(&input);
            break;
        case CAPSLOCK_IS_CTRL:
            if (input.type == EV_KEY && input.code == KEY_CAPSLOCK) {
                input.code = KEY_LEFTCTRL;
                write_event(&input);
                if (input.value == 0)
                    state = START;
            } else
                intercept_event(&input);
            break;
        }
    }
}
