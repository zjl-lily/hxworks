
#ifndef __LOONGSON_GPIO_H
#define __LOONGSON_GPIO_H


// gpio�Ĺ���ģʽ--���롢���
typedef enum{
    gpio_mode_output = 0,       // ���
    gpio_mode_input = 1         // ����
}gpio_mode_t;
 
 
//  gpio�ߵ͵�ƽֵ
typedef enum{
    gpio_level_low = 0,         // �͵�ƽ
    gpio_level_high = 1         // �ߵ�ƽ
}gpio_level_t;
 
 

#define GPIO_WLS  56
#define GPIO_RESET  59
#define GPIO_HLS  60
#define GPIO_VLS  61


 
/*
 * gpio��ʼ��
 * @gpio gpio���ţ�ȡֵ��Χ[0, 63]
 * @mode gpio�Ĺ���ģʽ(���롢���)
 *
 * ��: ��gpio50��ʼ��Ϊ���
 * gpio_init(50, gpio_mode_output);
 */
void gpio_init(unsigned int gpio, gpio_mode_t mode);
 
 
/*
 * ��ָ��gpio����ߵ�ƽ��͵�ƽ
 * @gpio gpio���ţ�ȡֵ��Χ[0, 63]
 * @level ��ƽֵ
 *
 * ��: ��gpio50������͵�ƽ
 * gpio_set(50, gpio_level_low);
 */
void gpio_set(unsigned int gpio, gpio_level_t level); 
 
/*
 * ��ȡָ��gpio���ŵ�ֵ
 * @gpio gpio���ţ�ȡֵ��Χ[0,127]
 *
 * ��: ��ȡgpio50�����ϵ�ֵ
 * gpio_level_t level;
 * level = gpio_get(50);
 */
gpio_level_t gpio_get(unsigned int gpio);
 

void InitGpio(void);
 

#endif