#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"

#include "pwm.h"
#include "gpio.h"
#include "driver/dHBridge.h"

dHBridge_params bridge = {0,0,0};

void ICACHE_FLASH_ATTR dHBridge_init(){
	uint32 io_info[PWM_CHANNEL][3] = {{PWM_0_OUT_IO_MUX,PWM_0_OUT_IO_FUNC,PWM_0_OUT_IO_NUM},{PWM_1_OUT_IO_MUX,PWM_1_OUT_IO_FUNC,PWM_1_OUT_IO_NUM}};

	uint32 callSpeed[PWM_CHANNEL] = {0};
	
	//New api is awesome. Apparently pwm_init sets up the gpio states.
	pwm_init(PWM_PERIOD_US,callSpeed,PWM_CHANNEL,io_info);

	//Init the 4 gpio for the bridge settings.
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);

	//Set them all to low, if 2 are same motor=stall.
	gpio_output_set(0,BIT2|BIT4|BIT5|BIT14,BIT2|BIT4|BIT5|BIT14,0);
}

void ICACHE_FLASH_ATTR dHBridge_set_mode(uint8 mode){
	//Real quick check that could save a lot of time.
	if(mode==bridge.mode)
		return;
	bridge.mode=mode;
	
	//These notes are dependent on wiring here is mine.
	//Left motor connected to left side of H-Bridge with red wire connected to top half of bridge.
	//When facing the motor (opposite of wheel) the red wire is on the right side.
	//The wheels are outside the frame the motor is inside.
	switch(mode){
		case 1:	//RIGHT
			gpio_output_set(BIT2|BIT14,BIT4|BIT5,BIT2|BIT4|BIT5|BIT14,0);
			break;
		case 2: //LEFT
			gpio_output_set(BIT4|BIT5,BIT2|BIT14,BIT2|BIT4|BIT5|BIT14,0);
			break;
		case 3: //BACKWARD
			gpio_output_set(BIT2|BIT5,BIT4|BIT14,BIT2|BIT4|BIT5|BIT14,0);
			break;
		case 4: //FORWARD
			gpio_output_set(BIT4|BIT14,BIT2|BIT5,BIT2|BIT4|BIT5|BIT14,0);
			break;
		default: //STOP
			gpio_output_set(0,BIT2|BIT4|BIT5|BIT14,BIT2|BIT4|BIT5|BIT14,0);
			break;
	}	
}

void ICACHE_FLASH_ATTR dHBridge_set_speed(uint8 left, uint8 right){
	//Everything doc wise says you need to call pwm_start after each config change. So I do it, even if IoT_demo doesn't... not sure who to listen to.
	if(left!=bridge.leftSpeed){
		bridge.leftSpeed=left;
		pwm_set_duty(dHBridge_conv_duty(left),0);	//0 and later 1 are the channels for the pwm.
		pwm_start();
	}
	if(right!=bridge.rightSpeed){
		bridge.rightSpeed=right;
		pwm_set_duty(dHBridge_conv_duty(right),1);
		pwm_start();
	}
}

dHBridge_params * ICACHE_FLASH_ATTR dHBridge_get_info(){
	return &bridge;
}

static uint32 ICACHE_FLASH_ATTR dHBridge_conv_duty(uint8 duty){
	uint32 max = PWM_PERIOD_US*1000/45;
	return (duty*max/255);	//AHHHH the evil magic number... yeah no it's 2^8-1
}
