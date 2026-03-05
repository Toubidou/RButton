#ifndef _ASYNC_EVENT_H_
#define _ASYNC_EVENT_H_

#include "main.h"
#include "rtthread.h"

#define     SIG_NUM_MAX     256

typedef struct _asyncevent_attr
{
    void (*evt_cb)(void);
    uint32_t sig;
}ASY_EVT_ATTR;

typedef struct _ASY_EVT_HANDLE
{   
    rt_slist_t list_head;
    ASY_EVT_ATTR map[SIG_NUM_MAX];
}ASY_EVT_HANDLE;

typedef struct _ASY_EVT
{
    uint32_t sig;
    rt_slist_t next;
}ASY_EVT;

uint8_t asy_evt_register(uint32_t sig, void (*evt_cb)(void));
uint8_t asy_evt_emit(uint32_t sig);
void asy_evt_process_thread_entry(void *p);

#endif
