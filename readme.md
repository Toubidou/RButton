# RButton

## 简介
一个异步按键事件框架，支持短按，长按，组合按键，兼容不同数量按键，面向对象风格编程。通过创建按键对象，将用户按键逻辑与按键处理事件分离，用户无需处理复杂麻烦的逻辑事件与修改内部源码
## 依赖 
RT-Thread v4.1.1
## 使用
### 创建按键
    //传入按键按下检测函数与对应短按回调函数
    BUTTON *button_create(int (*detect_func)(void), void (*short_evt)(void))
### 设置长按回调函数
    //传入按键对象函数与长按对应回调函数
    void button_set_long_pressed_cb(BUTTON *btn, void (*long_event)(void))
### 注册按键组合
    //传入按键id与，组合按键的回调函数
    rt_err_t button_cob_Reg(uint8_t btn_id0, uint8_t btn_id1, void (*cob_event)(void))

## License
遵循 Apache License v2.0 开源许可协议