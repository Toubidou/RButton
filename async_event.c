#include "async_event.h"

static ASY_EVT_HANDLE AsyEvtHandle;

uint8_t asy_evt_register(uint32_t sig, void (*evt_cb)(void))
{
    if((sig < 0) || (sig > SIG_NUM_MAX))
        return 1;
    if(AsyEvtHandle.map[sig].evt_cb != NULL)
        return 2;
    AsyEvtHandle.map[sig].evt_cb = evt_cb;
    AsyEvtHandle.map[sig].sig = sig;
    return 0;
}
uint8_t asy_evt_emit(uint32_t sig)
{
    if((sig < 0) || (sig > SIG_NUM_MAX))
        return 1;
    if(AsyEvtHandle.map[sig].evt_cb == NULL)
        return 2;
    ASY_EVT *node = (ASY_EVT *)rt_malloc(sizeof(ASY_EVT));
    if(node == NULL)
    {
        return 3;
    }
    
    node->sig = sig;
    rt_slist_append(&(AsyEvtHandle.list_head), &(node->next));
    return 0;
}

uint8_t asy_evt_process(void)
{
    if(rt_slist_len(&(AsyEvtHandle.list_head)) == 0)
    {
        return 1;
    }
    // rt_kprintf("evt num %d", rt_slist_len(&(AsyEvtHandle.list_head)));
    rt_slist_t *node = RT_NULL;
    ASY_EVT *temp_evt_p;
    rt_slist_for_each(node, &(AsyEvtHandle.list_head))
    {
        temp_evt_p = rt_slist_entry(node, ASY_EVT, next);
        AsyEvtHandle.map[temp_evt_p->sig].evt_cb();
        rt_slist_remove(&(AsyEvtHandle.list_head), &(temp_evt_p->next));
        rt_free(temp_evt_p);
    }
    return 0;
}

void asy_evt_process_thread_entry(void *p)
{
    while (1)
    {
        asy_evt_process();
    }
    rt_thread_delay(5);
}