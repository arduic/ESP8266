#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "gpio.h"
#include "user_config.h"
#include "c_types.h"
#include "user_interface.h"
#include "ets_sys.h"
#include "espconn.h"

#include "pwm.h"
#include "driver/dHBridge.h"

#define	PORT	7777	
#define SERVER_TIMEOUT	1000
#define	MAX_CONNS	5	
#define MAX_FRAME	2000

#define	procTaskPrio	0
#define procTaskQueueLen	1

static volatile os_timer_t	some_timer;
static struct espconn *pUdpServer;
os_event_t procTaskQueue[procTaskQueueLen];

static void user_procTask(os_event_t *events);
//static void myTimer(void *arg);
//static void udpserver_recv(void *arg, char *pdata, unsigned short len);

void user_rf_pre_init(void){
}

static void ICACHE_FLASH_ATTR user_procTask(os_event_t *events){
	//os_delay_us(10);
}

//static void ICACHE_FLASH_ATTR myTimer(void *arg){
//}

static void ICACHE_FLASH_ATTR udpserver_recv(void *arg, char *pdata, unsigned short len){
	//parse input (lazyly because it's a strict format)	
	char *token;
	token = strtok(pdata,"-");
	int mode = atoi(token);
	token = strtok(NULL,"-");
	int leftMotor = atoi(token);
	token = strtok(NULL,"-");
	int rightMotor = atoi(token);
	
	dHBridge_set_mode((uint8)mode);
	dHBridge_set_speed((uint8)leftMotor,(uint8)rightMotor);
}

void ICACHE_FLASH_ATTR user_init(void){

	int wifiMode = wifi_get_opmode();

	wifi_set_opmode(2);

	pUdpServer = (struct espconn *)os_zalloc(sizeof(struct espconn));
	os_memset(pUdpServer, 0, sizeof(struct espconn));
	espconn_create(pUdpServer);
	pUdpServer->type=ESPCONN_UDP;
	pUdpServer->proto.udp=(esp_udp *)os_zalloc(sizeof(esp_udp));
	pUdpServer->proto.udp->local_port = 7777;
	espconn_regist_recvcb(pUdpServer,udpserver_recv);

	if(espconn_create(pUdpServer)){
		while(1){
			//uart0_tx_buffer("FAULT\n",6);
		}
	}

	ets_wdt_disable();

	/*os_timer_disarm(&some_timer);
	os_timer_setfn(&some_timer,(os_timer_func_t *)myTimer, NULL);
	os_timer_arm(&some_timer, 500, 1);*/
	/*This note is specific to my motor so I left it out of the driver notes.
	* Facing the motor wheel facing away) the red wire is on the right.
	* For the left motor the red wire is in 1Y
	* For the right motor the red wire is in 4Y*/
	dHBridge_init();

	system_os_task(user_procTask, procTaskPrio, procTaskQueue, procTaskQueueLen);
}
