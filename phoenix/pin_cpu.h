
#define _GNU_SOURCE

#ifndef PIN_CPU
#define PIN_CPU


/**
 * Pin the current thread to the current CPU core.
 */
void pin_cpu(void);


#endif
