#ifndef CPU_CONF_H_
#define CPU_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#define THREAD_EXTRA_STACKSIZE_PRINTF   (1024)
#endif
#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT        (4096)
#endif
#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE           (1024) + THREAD_EXTRA_STACKSIZE_PRINTF /* Needed when debug logging turned on on code which runs on this thread eg timer debug */
#endif

/* FIXME: hardcoded context size = 32 * 4*/
#define CONTEXT_SIZE_ON_STACK 128

#ifdef __cplusplus
}
#endif

#endif
