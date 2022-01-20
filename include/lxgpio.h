
#ifndef __LOONGSON_GPIO_H
#define __LOONGSON_GPIO_H


// gpio的工作模式--输入、输出
typedef enum{
    gpio_mode_output = 0,       // 输出
    gpio_mode_input = 1         // 输入
}gpio_mode_t;
 
 
//  gpio高低电平值
typedef enum{
    gpio_level_low = 0,         // 低电平
    gpio_level_high = 1         // 高电平
}gpio_level_t;
 
 

#define GPIO_WLS  56
#define GPIO_RESET  59
#define GPIO_HLS  60
#define GPIO_VLS  61


 
/*
 * gpio初始化
 * @gpio gpio引脚，取值范围[0, 63]
 * @mode gpio的工作模式(输入、输出)
 *
 * 例: 将gpio50初始化为输出
 * gpio_init(50, gpio_mode_output);
 */
void gpio_init(unsigned int gpio, gpio_mode_t mode);
 
 
/*
 * 在指定gpio输出高电平或低电平
 * @gpio gpio引脚，取值范围[0, 63]
 * @level 电平值
 *
 * 例: 在gpio50上输出低电平
 * gpio_set(50, gpio_level_low);
 */
void gpio_set(unsigned int gpio, gpio_level_t level); 
 
/*
 * 读取指定gpio引脚的值
 * @gpio gpio引脚，取值范围[0,127]
 *
 * 例: 读取gpio50引脚上的值
 * gpio_level_t level;
 * level = gpio_get(50);
 */
gpio_level_t gpio_get(unsigned int gpio);
 

void InitGpio(void);
 

#endif