#include "lxgpio.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

// 寄存器地址
#define LS1C_GPIO_CFG                      (0xbfe10530)
#define LS1C_GPIO_EN                       (0xbfe10500)
#define LS1C_GPIO_IN                       (0xbfe10510)
#define LS1C_GPIO_OUT                      (0xbfe10520)
 
 
 
/*
 * gpio初始化
 * @gpio gpio引脚，取值范围[0, 127]
 * @mode gpio的工作模式(输入、输出)
 *
 * 例: 将gpio50初始化为输出
 * gpio_init(50, gpio_mode_output);
 */
void gpio_init(unsigned int gpio, gpio_mode_t mode)
{
	int ret = 0;

	char gpio_creat[100] ;
	char cmd[100] ;

	printf( "set %d /sys/class/gpio/export\n",gpio);

        sprintf(gpio_creat, "echo %d > /sys/class/gpio/export",gpio);
//        popen(gpio_creat, "W");
	ret = system(gpio_creat);
	printf( "set gpio%d: \n",gpio);  

    if (gpio_mode_output == mode)       // 输出
    {
	    sprintf(cmd, "echo out > /sys/class/gpio/gpio%d/direction",gpio);  
   //         popen(cmd, "W");		
        ret = system(cmd);  
		printf( "set gpio%d mode = out \n",gpio);  
	}
    else                                // 输入
    {
	    sprintf(cmd, "echo in > /sys/class/gpio/gpio%d/direction",gpio);  
	  //  popen(cmd, "W");	
        ret = system(cmd);  
		printf( "set gpio%d mode = in \n",gpio);  
    }


	/*
    unsigned int *gpio_cfgx;       //volatile  GPIO_CFGx寄存器
    unsigned int *gpio_enx;        // GPIO_ENx寄存器
  
    unsigned int pin = gpio;
 
    unsigned int temp;
    unsigned int mask;
 	printf("1. gpio_init: gpio%d  mode = %d\n", gpio, mode);

	gpio_cfgx   = ( unsigned int *)LS1C_GPIO_CFG;
    gpio_enx    = ( unsigned int *)LS1C_GPIO_EN;

	printf("2. gpio_init: gpio%d  mode = %d\n", gpio, mode);
 
    // 将pin设为普通GPIO
    mask = ((unsigned int)1 << pin);

	printf("3. gpio_init: gpio%d  mode = %d\n", gpio, mode);

    temp = (unsigned int)(*gpio_cfgx);
 	printf("4. gpio_init: gpio%d  mode = %d\n", gpio, mode);
    temp |= mask;           // 将指定位置1
    *gpio_cfgx = temp;
 

    // 设置gpio工作模式(输入、输出)
    temp = *gpio_enx;
    if (gpio_mode_output == mode)       // 输出
    {
        temp &= ~mask;      // 将指定位清零
    }
    else                                // 输入
    {
        temp |= mask;       // 将指定位置1
    }
    *gpio_enx = temp;

	printf("5. gpio_init: gpio%d  mode = %d\n", gpio, mode);
 */
    return ;
}
 
 
/*
 * 在指定gpio输出高电平或低电平
 * @gpio gpio引脚，取值范围[0, 127]
 * @level 电平值
 *
 * 例: 在gpio50上输出低电平
 * gpio_set(50, gpio_level_low);
 */
void gpio_set(unsigned int gpio, gpio_level_t level)
{
	char cmd[100];
	int ret = 0;
    if (gpio_level_low == level)
    {
		// echo 1 > /sys/class/gpio/gpio60/value 
	    sprintf(cmd,"echo 0 > /sys/class/gpio/gpio%d/value", gpio);    // 输出低电平
		//popen(cmd, "W");	
        ret = system(cmd);   
		//printf("%s\n", cmd);
		printf("set gpio%d low %d\n", gpio, level);
    }
    else
    {
	    sprintf(cmd,"echo 1 > /sys/class/gpio/gpio%d/value", gpio);    // 输出高电平
 		//popen(cmd, "W");	
        ret = system(cmd);   
		printf("set gpio%d high %d\n", gpio, level);
	//	printf("%s\n", cmd);
    }
	


	/*
    volatile unsigned int *gpio_outx;       // GPIO_OUTx寄存器 
    unsigned int pin    = gpio; 
    unsigned int temp;
    unsigned int mask;

	printf("1. gpio_set: gpio%d  level = %d\n", gpio, level);

    gpio_outx = (volatile unsigned int *)LS1C_GPIO_OUT; 

	printf("2. gpio_set: gpio%d  level = %d\n", gpio, level);

    mask = 1 << pin;
    temp = *gpio_outx;


 	printf("3. gpio_set: gpio%d  level = %d\n", gpio, level);


    if (gpio_level_low == level)
    {
        temp &= ~mask;          // 输出低电平
    }
    else
    {
        temp |= mask;           // 输出高电平
    }
    *gpio_outx = temp;
 
	printf("4. gpio_set: gpio%d  level = %d\n", gpio, level);
*/

    return ;
}
 
 
/*
 * 读取指定gpio引脚的值
 * @gpio gpio引脚，取值范围[0,127]
 *
 * 例: 读取gpio50引脚上的值
 * gpio_level_t level;
 * level = gpio_get(50);
 */
gpio_level_t gpio_get(unsigned int gpio)
{
    gpio_level_t level;
	/*
    volatile unsigned int *gpio_inx;        // GPIO_INx寄存器
    
    unsigned int pin  = gpio;
 
    unsigned int temp;

    gpio_inx = (volatile unsigned int *)LS1C_GPIO_IN; 
 
    temp = *gpio_inx;
    temp = (temp >> pin) & 1;
    if (temp)
    {
        level = gpio_level_high;
    }
    else
    {
        level = gpio_level_low;
    }
	*/
 
    return level;
}
 
void InitGpio(void)
{ 
printf("InitGpio \n");

	gpio_init(GPIO_WLS, gpio_mode_output);
	gpio_init(GPIO_VLS, gpio_mode_output);
	gpio_init(GPIO_HLS, gpio_mode_output);
	gpio_init(GPIO_RESET, gpio_mode_output);


	gpio_set(GPIO_WLS, gpio_level_low);
	gpio_set(GPIO_VLS, gpio_level_low);
	gpio_set(GPIO_HLS, gpio_level_low); 
	gpio_set(GPIO_RESET, gpio_level_high);

}
