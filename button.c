#include "button.h"
#include "gpio.h"
#include "async_event.h"

static uint8_t button_num = 0;
static rt_slist_t btn_list_head = {NULL};
static ButtonCobInfo bci = {0};

BUTTON *button_create(int (*detect_func)(void), void (*short_evt)(void))
{
    if(button_num > BUTTON_NUM_MAX)
    {
        return NULL;
    }
    BUTTON *btn_new = (BUTTON *)rt_malloc(sizeof(BUTTON));
    btn_new->id = button_num;
    btn_new->next.next = NULL;
    btn_new->get_fun = detect_func;
    btn_new->short_event = short_evt;
    btn_new->long_event = NULL;
    btn_new->press_time = 0;
    btn_new->state = SCAN;
    btn_new->flag_long_pressed = 0;
    btn_new->cob_reg = 0;
    btn_new->cob_ready_flag = 0;
    rt_slist_append(&btn_list_head, &(btn_new->next));
    button_num++;

    asy_evt_register(BUTTON_0_SIG_NUM + btn_new->id * 2, btn_new->short_event);
    return btn_new;
}
void button_set_long_pressed_cb(BUTTON *btn, void (*long_event)(void))
{
    btn->long_event = long_event;
    asy_evt_register(BUTTON_0_SIG_NUM + 1 + btn->id * 2, btn->long_event);
}

rt_err_t button_cob_Reg(uint8_t btn_id0, uint8_t btn_id1, void (*cob_event)(void))
{
    rt_slist_t *node = RT_NULL;
    BUTTON *temp_btn_p = NULL;
    if (bci.cob_num < BUTTON_COB_NUM_MAX)
    {
        bci.mask_array[bci.cob_num] = (0x01 << btn_id0) | (0x01 << btn_id1);
        asy_evt_register(bci.cob_num, cob_event);
        bci.cob_num++;

        rt_slist_for_each(node, &btn_list_head)
        {
            temp_btn_p = rt_slist_entry(node, BUTTON, next);
            if ((temp_btn_p->id == btn_id0) || (temp_btn_p->id == btn_id1))
            {
                temp_btn_p->cob_reg = 1;
            }
        }
    }
    else
    {
        return RT_ERROR;
    }
    return RT_EOK;
}
rt_err_t button_cob_set_trig_flag(uint16_t button_cob_mask)
{
    rt_slist_t *node = RT_NULL;
    BUTTON *temp_btn_p = NULL;
    uint8_t i = 0;
    rt_slist_for_each(node, &btn_list_head)
    {
        temp_btn_p = rt_slist_entry(node, BUTTON, next);
        if (button_cob_mask & (0x01 << i))
        {
            temp_btn_p->cob_trigger_flag = 1;
        }
        i++;
        if (i > 15)
        {
            break;
        }
    }
}
int button_get_id(BUTTON *btn)
{
    if(btn == NULL)
    {
        return -1;
    }
    return btn->id;
}

//检测每个按键的状态并发送对应事件
void button_scan(void *p)
{
    BUTTON *temp_btn_p = NULL;
    rt_slist_t *node = RT_NULL;
    static uint16_t button_cob_mask = 0;
    while(1)
    {
        rt_slist_for_each(node, &btn_list_head)
        {
            temp_btn_p = rt_slist_entry(node, BUTTON, next);
            if(temp_btn_p->state == SCAN)
            {
                if(temp_btn_p->get_fun())
                {
                    temp_btn_p->state = PRESSED;
                }
            }
            else if(temp_btn_p->state == PRESSED)
            {
                if(temp_btn_p->get_fun())
                {
                    temp_btn_p->state = CONFIRM_PRESSED;
                }
                else{
                    temp_btn_p->state = SCAN;
                }
            }
            else if(temp_btn_p->state == CONFIRM_PRESSED)
            {
                if(temp_btn_p->get_fun())
                {
                    temp_btn_p->press_time++;
                    if((temp_btn_p->cob_reg != 0) && (temp_btn_p->press_time < 4))
                    {
                        if (temp_btn_p->cob_ready_flag == 0)
                        {
                            button_cob_mask |= 0x01 << temp_btn_p->id;
                            temp_btn_p->cob_ready_flag = 1;
                        }
                    }
                    else if((temp_btn_p->press_time > (LONG_PRESSED_TIME / 30)) && (temp_btn_p->flag_long_pressed != 1))
                    {
                        temp_btn_p->flag_long_pressed = 1;  //防止长按重复触发
                        asy_evt_emit(BUTTON_0_SIG_NUM + 1 + temp_btn_p->id * 2);
                    }            
                }
                else{
                    if((temp_btn_p->press_time < (LONG_PRESSED_TIME / 30)) && (temp_btn_p->cob_trigger_flag == 0))
                    {
                        asy_evt_emit(BUTTON_0_SIG_NUM + temp_btn_p->id * 2);
                    }
                    temp_btn_p->flag_long_pressed = 0;
                    temp_btn_p->state = SCAN;
                    temp_btn_p->press_time = 0;
                    button_cob_mask = 0;
                    temp_btn_p->cob_ready_flag = 0;
                    temp_btn_p->cob_trigger_flag = 0;
                }
            }
        }

        if (button_cob_mask != 0)
        {
            for (uint8_t i = 0; i < 4; i++)
            {
                if (button_cob_mask == bci.mask_array[i])
                {
                    asy_evt_emit(i);
                    button_cob_set_trig_flag(button_cob_mask);
                    button_cob_mask = 0;
                }
            }
        }
        rt_thread_delay(10);
    }
}