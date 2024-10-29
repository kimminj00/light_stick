/*
 * app_common.h
 *
 *  Created on: 2018. 7. 19.
 *      Author: sprit
 */

#ifndef APP_COMMON_H_
#define APP_COMMON_H_

#include "rail.h"
#include "hal_common.h"
#include "rail_config.h"
#include "rail_types.h"

#include "rail_ieee802154.h"

#include "yes/message.h"

#define RADIO_2P4			(true)
// #define RADIO_2P4			(false)
#define TEST_LAONZ	false







#define FRAME_CONTROL			0x8841
#define PAN_ID					0x2475

#define SUB_GHZ_915M		0x01
#define SUB_GHZ_433M		0x02
#define IEEE802154_2P4		0x04

#define SYS_MODE_UART			0
#define SYS_MODE_CONSOLE		1
#define SYS_MODE_RGB_TEST		2
#define SYS_MODE_LIGHT_OFF		3

#define APP_MAX_PACKET_LENGTH 504

IEEE802154_MESSAGE_t *pMessage;
SUB_GIGA_MESSAGE_t *pSubGigaMessage;

uint8_t gSysMode;
uint8_t gRfMode;

// :: CSMA Config
static const  RAIL_CsmaConfig_t CsmaConfig = RAIL_CSMA_CONFIG_802_15_4_2003_2p4_GHz_OQPSK_CSMA;

extern RAIL_Handle_t railHandle2p4;
extern RAIL_Handle_t railHandleSubG;
extern RAIL_Handle_t railHandle;

extern uint8_t gChannel_2p4;
extern uint8_t lastSetTxPowerLevel;
extern bool receiveModeEnabled;
extern volatile uint32_t txCount;
extern uint16_t gNodeID;

typedef enum AppMode{
  NONE = 0,           /**< RAILtest is not doing anything special */
  TX_STREAM = 1,      /**< Send a stream of pseudo-random bits */
  TX_TONE = 2,        /**< Send a tone at the carrier frequency */
  TX_CONTINUOUS = 3,  /**< Send an unending stream of packets*/
  DIRECT = 4,         /**< Send data to and from a GPIO, without any packet handling */
  TX_N_PACKETS = 5,   /**< Send a specific number of packets */
  TX_SCHEDULED = 6,   /**< Send one packet scheduled in the future */
  SCHTX_AFTER_RX = 7, /**< Schedule a TX for a fixed delay after receiving a packet */
  RX_OVERFLOW = 8,    /**< Cause overflow on receive */
  TX_UNDERFLOW = 9,   /**< Cause underflows on the next TX sequence */
  TX_CANCEL = 10,     /**< Cancel a single packet transmit to force an error event */
  RF_SENSE = 11,      /**< Sense RF energy to wake the radio */
  PER = 12,           /**< Packet Error Rate test mode */
  BER = 13,           /**< Bit Error Rate test mode */
  RX_SCHEDULED = 14,  /**< Enable receive at a time scheduled in the future */
} AppMode_t;



#define PERIPHERAL_ENABLE (0x01)
#define ASYNC_RESPONSE (0x02)
extern uint8_t logLevel;
extern RAIL_StreamMode_t streamMode;
void enableAppMode(AppMode_t appMode, bool enable, char *command);
bool enableAppModeSync(AppMode_t appMode, bool enable, char *command);
extern volatile uint32_t txCount;



void changeChannel2p4(uint8_t i);
void radioInit(void);


#endif /* APP_COMMON_H_ */
