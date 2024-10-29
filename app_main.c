/***************************************************************************//**
 * @file main.c
 * @brief Application specific overrides of weak functions defined as part of
 * the test application.
 * @copyright Copyright 2015 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yes/yes_radio.h>
#include "rail.h"
#include "rail_types.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_emu.h"
#include "bsp.h"
#include "gpiointerrupt.h"
#include "rail_config.h"
#include "hal_common.h"
#include "app_hal.h"

#include "command_interpreter.h"

#include "app_common.h"
#include "yes/my_uart.h"
#include "yes/app_ci.h"
#include "user_data/user_data.h"

#include "rtcdriver.h"

#ifdef DISABLE_LOCAL_ECHO
#define localEcho 0
#else
#define localEcho 1
#endif

// Memory manager configuration
//#define MAX_BUFFER_SIZE  256

// Minimum allowed size of the TX FIFO
#define RAIL_TX_FIFO_SIZE 64

#define RX_ALIVE_TIME 10


static char *version = "20220719_2p4GHz";
// General application memory sizes
//#define APP_MAX_PACKET_LENGTH  (MAX_BUFFER_SIZE - 12) /* sizeof(RAIL_RxPacketInfo_t) == 12) */

// Prototypes
/*void RAILCb_Generic2p4(RAIL_Handle_t railHandle, RAIL_Events_t events);
 void radioInit();
 void changeChannel915(uint32_t i);
 void setRx(bool enable);*/

RAIL_Handle_t railHandleSubG = NULL;
RAIL_Handle_t railHandle2p4 = NULL;
uint16_t channel = 0;
static bool finishTxSequence = false;

static RTCDRV_TimerID_t one_sec_timer_handle;

static uint32_t one_sec_timer_tick;

uint8_t logLevel = PERIPHERAL_ENABLE | ASYNC_RESPONSE;

//Command line variables
static CommandEntry_t commands[] = {
APP_CI_COMMANDS,
COMMAND_ENTRY(NULL, NULL, NULL, NULL) };
static CommandState_t ciState;
static char ciBuffer[APP_COMMAND_INTERFACE_BUFFER_SIZE];

// External control and status variables
bool receiveModeEnabled = false;

uint8_t channel915 = 0;
//uint8_t channel2p4 = 16;
volatile bool packetTx = false; //go into transfer mode
volatile bool packetRx = true;  //go into receive mode

// extern volatile bool isTransmit;

// static uint8_t transmitData[RAIL_TX_FIFO_SIZE] = { 0x0F, 0x16, 0x11, 0x22, 0x33,
		// 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, };

//static void RAILCb_RfReady(RAIL_Handle_t railHandle);

#define LED_TX (0)
#define LED_RX (1)

void pendFinishTxSequence()
{
  finishTxSequence = true;
}

void pendPacketTx()
{
  packetTx = true;
}

// uint16_t dataLength = 37;  
// uint16_t initialCrc = 0x0000;
// uint16_t crcResult;

// uint16_t halCommonCrc16(uint8_t* data, uint16_t length, uint16_t crc) {
//     for (uint16_t i = 0; i < length; i++) {
//         uint8_t byte = data[i];
//         crc = (crc >> 8) | (crc << 8);
//         crc ^= byte;
//         crc ^= (crc & 0xff) >> 4;
//         crc ^= (crc << 8) << 4;
//         crc ^= ((uint8_t) ((uint8_t) ((uint8_t) (crc & 0xff)) << 5))
//                | ((uint16_t) ((uint8_t) ((uint8_t) (crc & 0xff)) >> 3) << 8);
//     }
//     return crc;
// }

void processInputCharacters(void);
#define test_length 48
static const uint8_t data[];

extern uint8_t set_gpio;

	uint8_t data_on[] = {
		0x4D, 0x00, 0x00, 0x01, 0x0A, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, \
    	0x38, 0x02, 0x18, 0x4F, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, \
    	0x00, 0x00, 0x00, 0x00, 0x41, 0x03, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64
	};
		uint8_t data_off[] = {
		0x4D, 0x00, 0x00, 0x01, 0x0A, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, \
    	0x38, 0x02, 0x18, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    	0x00, 0x00, 0x00, 0x00, 0x41, 0x03, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64
	};

void one_sec_Timer_Trigger_on(RTCDRV_TimerID_t id, void *user){
	( void)id;
	( void)user;
	switch(set_gpio){
		case 0:
			printf("off \r\n");
			// uint8_t data[] = {
			// 	0x4D, 0x00, 0x00, 0x01, 0x0A, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, \
			// 	0x38, 0x02, 0x18, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
			// 	0x00, 0x00, 0x00, 0x00, 0x41, 0x03, 0x00
			// };
			SendMessageIEEE802154(YES_BCAST_ADDR, data_off, 37);
			// dataLength = sizeof(data_off) / sizeof(data_off[0]);
			// crcResult = halCommonCrc16(data_off, dataLength, initialCrc);
    		// printf("CRC16 Result: 0x%04X\n", crcResult);
		break;
		case 1:
			printf("on \r\n");
			// uint8_t data[] = {
			// 	0x4D, 0x00, 0x00, 0x01, 0x0A, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, \
			// 	0x38, 0x02, 0x18, 0x4F, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, \
			// 	0x00, 0x00, 0x00, 0x00, 0x41, 0x03, 0x00
			// };
			SendMessageIEEE802154(YES_BCAST_ADDR, data_on, 37);
			// dataLength = sizeof(data_on) / sizeof(data_on[0]);
			// crcResult = halCommonCrc16(data_on, dataLength, initialCrc);
    		// printf("CRC16 Result: 0x%04X\n", crcResult);
		break;
		case 2:
			//RTCDRV_StopTimer(one_sec_timer_handle);
			
		break;
	}

}

void one_sec_Timer_Trigger_off(RTCDRV_TimerID_t id, void *user){
	( void)id;
	( void)user;
	printf("off");
	uint8_t data[] = {
		0x4D, 0x00, 0x00, 0x01, 0x0A, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, \
    	0x38, 0x02, 0x18, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    	0x00, 0x00, 0x00, 0x00, 0x41, 0x03, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64
	};
	SendMessageIEEE802154(YES_BCAST_ADDR, data, 37);
}


void one_sec_Timer_Start(void){
	// printf("%s\r\n", __func__);
	RTCDRV_Init();
  	RTCDRV_AllocateTimer(&one_sec_timer_handle);
	// if(set_gpio==1)
	// {
		RTCDRV_StartTimer(one_sec_timer_handle, rtcdrvTimerTypePeriodic, 1000, one_sec_Timer_Trigger_on, NULL);
	// }
	// else
	// {
	// 	RTCDRV_StartTimer(one_sec_timer_handle, rtcdrvTimerTypePeriodic, 1000, one_sec_Timer_Trigger_off, NULL);
	// }

}


void Alive_TimerExpired(RAIL_Handle_t railHandle) {
	/*RAIL_RadioState_t currentState = RAIL_GetRadioState(railHandle);
	if (currentState == RAIL_RF_STATE_ACTIVE) {
		printf(" RAIL_RadioState_t : %d  \r\n", currentState);
		packetRx = true;
	}*/
	RAIL_RadioState_t currentState = RAIL_GetRadioState(railHandle);
	printf(" Alive_TimerExpired : %d  \r\n", currentState);

	packetRx = true;
	RAIL_SetTimer(railHandle2p4, 1000 * 1000 * RX_ALIVE_TIME, RAIL_TIME_DELAY,
				&Alive_TimerExpired);
}
/*----- 20200507_gyuplus_revised----start---*/
//void send_sync_data(RAIL_Handle_t railHandle)
//{
//	LedToggle(2);
//	RAIL_SetTimer(railHandle2p4, 1000 * 100, RAIL_TIME_DELAY,
//			&send_sync_data);
//}
/*----- 20200507_gyuplus_revised----Finish---*/


void Init_TimerExpired(RAIL_Handle_t railHandle) {
	// initUart();
	// initB2BUart();
	//BSP_LedSet(0); //BSP_LedClear(0);/*20200507_gyuplus_revised*/
	//BSP_LedSet(1);//BSP_LedClear(1);/*20200507_gyuplus_revised*/
	// BSP_LedClear(2);  /*20200507_gyuplus_revised*/

/*
#if !RADIO_2P4
	RAIL_SetTimer(railHandle2p4, 1000 * 1000 * RX_ALIVE_TIME, RAIL_TIME_DELAY,
				&Alive_TimerExpired);
#endif
*/
/*----- 20200507_gyuplus_revised----start---*/
GPIO_PinOutSet(gpioPortF, 7);
//#if RADIO_2P4
//	RAIL_SetTimer(railHandleSubG, 1000 * 100 , RAIL_TIME_DELAY,
//				&send_sync_data);
//#else
//	RAIL_SetTimer(railHandle2p4, 1000 * 100 , RAIL_TIME_DELAY,
//				&send_sync_data);
//#endif
/*----- 20200507_gyuplus_revised----Finish---*/

	printf(" OK, Let's Start!! \r\n");
}

/*----- 20200507_gyuplus_revised----start---*/
#if TEST_LAONZ
	static int g_data_sync_stop_cnt =0;
#endif
static int g_data_sync_cnt;
#if 0
static int data_sync_irq( void )
{
	if(g_data_sync_cnt >= 10000)
	{
#if TEST_LAONZ
	if(g_data_sync_stop_cnt <= 10)
	{
		g_data_sync_stop_cnt++;
	}
	else
	{
		return 0;
	}
#endif

		LedToggle(2);
		g_data_sync_cnt = 0;
	}
	g_data_sync_cnt++;
	return 0;
}
#endif
/*----- 20200507_gyuplus_revised----Finish---*/
uint8_t Led_Status=0;
uint8_t Led_Status_1=0;
uint16_t Led_Cnt=0;
uint16_t Led_Cnt_delay=0;
int main(void) {
	//	20200507_gyuplus_revised
	// uint16_t i;

	// Initialize the chip
	CHIP_Init();

	// Initialize hardware for application
	appHalInit();

	// Initialize the BSP
	BSP_Init(BSP_INIT_BCC);

	//BSP_LedClear(0);//BSP_LedSet(0);/*20200507_gyuplus_revised*/
	//BSP_LedClear(1);//BSP_LedSet(1);/*20200507_gyuplus_revised*/
	// BSP_LedSet(2); /*20200507_gyuplus_revised*/

	InitUserDataInfo();
	// initUart();
	// initB2BUart();
#if RADIO_2P4
	// Initialize 2.4Ghz Radio
	initRadio802154();
	changeChannel2p4(26);

#else
	// Initialize 915Mhz Radio
	initRadioSubGiga();
	SetRfConfig();
#endif

	QueueInit();
	GPIO_PinOutClear(gpioPortB, 11);
	GPIO_PinOutClear(gpioPortB, 12);
	//GPIO_PinOutSet(gpioPortB, 12);
	//GPIO_PinOutSet(gpioPortB, 11);

	printf("**     Initialize Complete!!   **\r\n");
	printf("***************%s**************\r\n", version );
	gSysMode = SYS_MODE_UART;
	//setRx(true);

	printf(
			" If you want setup, you should press the ENTER key in 3 second! \r\n");
	RAIL_SetTimer(railHandle2p4, 1000 * 10, RAIL_TIME_DELAY,
			&Init_TimerExpired);

	//initUart();
	//initB2BUart();
	printf("> ");
	printf("**     Send Mode    **\r\n");

	ciInitState(&ciState, ciBuffer, sizeof(ciBuffer), commands);
/*----- 20200507_gyuplus_revised----start---*/
	g_data_sync_cnt = 0;
	GPIO_PinOutSet(gpioPortB, 12);
	GPIO_PinOutSet(gpioPortB, 11);
	one_sec_Timer_Start();
	while (1) {
		//printf("**     Initialize Complete!!   **\r\n");

		//GPIO_PinOutSet(gpioPortB, 12);
		//GPIO_PinOutSet(gpioPortB, 11);

		//data_sync_irq();
/*----- 20200507_gyuplus_revised----Finish---*/
		processInputCharacters();

		//GPIO_PinOutClear(gpioPortB, 11);


		//setRgbTest(100);

		//ReceiveForwardPacket();
		//SendIEEE802154Packet(railHandle2p4);

		//B2B_UART_process();

		//GPIO_PinOutClear(gpioPortB, 11);
#if 0
		if(Led_Status_1 == 1)
		{
			GPIO_PinOutClear(gpioPortB, 11);
		}
		else
		{
			GPIO_PinOutSet(gpioPortB, 11);
		}
#endif

		if(Led_Status == 1)
		{
			//GPIO_PinOutClear(gpioPortB, 12);
			GPIO_PinOutSet(gpioPortB, 12);
		}
		else
		{
			//GPIO_PinOutSet(gpioPortB, 12);
			GPIO_PinOutClear(gpioPortB, 12);
		}

		Led_Cnt++;
		//if(Led_Cnt > 1000)
		if(Led_Cnt > 500)
		{
			Led_Cnt=0;
			Led_Cnt_delay++;

			if(Led_Status != 0)
				Led_Status=0;


			if(Led_Cnt_delay > 400)
			{
				Led_Cnt_delay=0;

				//if(Led_Status != 0)
				//	Led_Status=0;

				//if(Led_Status_1 !=0 )
				//	Led_Status_1 = 0;
			}


		}

		// UART_process();
		//B2B_UART_process();
		Tx2p4GPending();
		changeAppModeIfPending();
		




#if RADIO_2P4
#else
		//UART_process();
		//SendIEEE802154Packet(railHandle2p4);
		//SubGigaRx();
		//ReceiveForwardPacket();
		//TxSubGPending();
#endif


		//GPIO_PinOutClear(gpioPortB, 11);
	}
}

/*static void RAILCb_RfReady(RAIL_Handle_t railHandle)
 {
 //LedSet(0);
 //LedSet(1);
 //LedSet(2);
 LedToggle(1);
 }*/

void processInputCharacters(void) {
	char input = getchar();

	while (input != '\0' && input != 0xFF) {
		test_check=1;
		if (localEcho) {
			if (input != '\n') {
				printf("%c", input);
			}/* else {
			 printf("RAIL_CancelTimer\n");
			 RAIL_CancelTimer(railHandle);
			 gSysMode = SYS_MODE_CONSOLE;
			 }*/
		}

		if (ciProcessInput(&ciState, &input, 1) > 0) {
			printf("\n\r>");
			if (gSysMode == SYS_MODE_UART) {
				RAIL_CancelTimer(railHandle2p4);
			}
		}
		input = getchar();
	}
}

void changeChannel2p4(uint8_t i) {
	gChannel_2p4 = i;
	// Automatically apply the new channel to future Tx/Rx
	if (receiveModeEnabled) {
		RAIL_StartRx(railHandle2p4, gChannel_2p4, NULL);
	}
}

