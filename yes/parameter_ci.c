/***************************************************************************//**
 * @file parameter_ci.c
 * @brief This file implements the parameter commands for RAIL test applications.
 * @copyright Copyright 2015 Silicon Laboratories, Inc. www.silabs.com
 ******************************************************************************/
#include <string.h>
#include <stdio.h>

#include "command_interpreter.h"
#include "response_print.h"

#include "rail.h"

#include "rail_config.h"
//#include "yes.h"
//#include "yes_config.h"
#include "message.h"
#include "yes_radio.h"
#include "app_common.h"


extern RAIL_Handle_t railHandle2p4;
extern RAIL_Handle_t railHandleSubG;

extern uint8_t gChannel_2p4;
extern uint16_t gNodeId;


uint8_t set_gpio = 2;

void resetChip(int argc, char **argv)
{
  // Use the NVIC to reset the chip
  NVIC_SystemReset();
}

void getNodeId(int argc, char **argv)
{
    responsePrint(argv[0], "NodeId:%d", gNodeId);
}

void setNodeId(int argc, char **argv)
{
  int proposedNodeId = ciGetUnsigned(argv[1]);
  bool success = false;

  // Make sure this is a valid channel
  if (proposedNodeId >= 0) {
    changeNodeId(proposedNodeId);
    success = true;
  }

  if (!success) {
    responsePrintError(argv[0], 0x11, "Invalid node id '%d'", proposedNodeId);
    return;
  }

  getNodeId(1, argv);
}



void getChannel(int argc, char **argv)
{
  if (RAIL_GetDebugMode(railHandle2p4) & RAIL_DEBUG_MODE_FREQ_OVERRIDE) {
    responsePrintError(argv[0], 0x12, "Channels are not valid in Debug Mode");
  } else {
    responsePrint(argv[0], "channel:%d", gChannel_2p4);
  }
}

void setChannel(int argc, char **argv)
{
  int proposedChannel = ciGetUnsigned(argv[1]);
  bool success = false;

  // Make sure this is a valid channel
  if (RAIL_IsValidChannel(railHandle2p4, proposedChannel)
      == RAIL_STATUS_NO_ERROR) {
    changeChannel(proposedChannel);
    success = true;
  }

  if (!success) {
    responsePrintError(argv[0], 0x11, "Invalid channel '%d'", proposedChannel);
    return;
  }

  getChannel(1, argv);
}

void getChannelSubG(int argc, char **argv)
{
  if (RAIL_GetDebugMode(railHandleSubG) & RAIL_DEBUG_MODE_FREQ_OVERRIDE) {
    responsePrintError(argv[0], 0x12, "SubG Channels are not valid in Debug Mode");
  } else {
    responsePrint(argv[0], "SubG channel:%d", gChannel_SubG);
  }
}

void setChannelSubG(int argc, char **argv)
{
  int proposedChannel = ciGetUnsigned(argv[1]);
  bool success = false;

  // Make sure this is a valid channel
  if (RAIL_IsValidChannel(railHandleSubG, proposedChannel)
      == RAIL_STATUS_NO_ERROR) {
	  changeChannelSubG(proposedChannel);
    success = true;
  }

  if (!success) {
    responsePrintError(argv[0], 0x11, "Invalid SubG channel '%d'", proposedChannel);
    return;
  }

  getChannelSubG(1, argv);
}

void getPower(int argc, char **argv)
{
  responsePrint(argv[0],
                "powerLevel:%d,power:%d",
                RAIL_GetTxPower(railHandle2p4),
                RAIL_GetTxPowerDbm(railHandle2p4));
}

void setPower(int argc, char **argv)
{
  /*if (!inRadioState(RAIL_RF_STATE_IDLE, argv[0])) {
    return;
  }*/

	if (railHandle2p4 == NULL) {
		responsePrintError(argv[0], 0x11, "Invalid 2p4 channel '%d'. Please change 2p4 mode.", ciGetUnsigned(argv[1]));
		return;
	}

  RAIL_TxPowerLevel_t powerLevel;
  RAIL_TxPower_t power;

  if (argc >= 3 && strcmp(argv[2], "raw") == 0) {
    RAIL_SetTxPower(railHandle2p4, ciGetUnsigned(argv[1]));
  } else {
    RAIL_SetTxPowerDbm(railHandle2p4, ciGetSigned(argv[1]));
  }

  powerLevel = RAIL_GetTxPower(railHandle2p4);
  //lastSetTxPowerLevel = powerLevel;
  power = RAIL_GetTxPowerDbm(railHandle2p4);

  responsePrint(argv[0], "powerLevel:%d,power:%d", powerLevel, power);
}

void getPowerSubG(int argc, char **argv)
{
  responsePrint(argv[0],
                "powerLevel:%d,power:%d",
                RAIL_GetTxPower(railHandleSubG),
                RAIL_GetTxPowerDbm(railHandleSubG));
}

void setPowerSubG(int argc, char **argv)
{
  /*if (!inRadioState(RAIL_RF_STATE_IDLE, argv[0])) {
    return;
  }*/

	if (railHandleSubG == NULL) {
		responsePrintError(argv[0], 0x11, "Invalid SubG channel '%d'. Please change SubG mode.", ciGetUnsigned(argv[1]));
		return;
	}

  RAIL_TxPowerLevel_t powerLevel;
  RAIL_TxPower_t power;

  if (argc >= 3 && strcmp(argv[2], "raw") == 0) {
    RAIL_SetTxPower(railHandleSubG, ciGetUnsigned(argv[1]));
  } else {
    RAIL_SetTxPowerDbm(railHandleSubG, ciGetSigned(argv[1]));
  }

  powerLevel = RAIL_GetTxPower(railHandleSubG);
  //lastSetTxPowerLevel = powerLevel;
  power = RAIL_GetTxPowerDbm(railHandleSubG);
  changePowerSubG(power);
  responsePrint(argv[0], "powerLevel:%d,power:%d", powerLevel, power);
}

/*void getRetxCountSubG(int argc, char **argv) {
	responsePrint(argv[0], "retransmit(Sub-G) count :%d", gReTxCountSubG);
}

void setRetxCountSubG(int argc, char **argv)
{
  int reTxCnt = ciGetUnsigned(argv[1]);
  bool success = false;

  if (reTxCnt < 10) {
	  changeRetxCountSubG(reTxCnt);
	  success = true;
  }

  responsePrint(argv[0], "set retransmit(Sub-G) count :%d", reTxCnt);
}*/

void getRetxCount802154(int argc, char **argv) {
	responsePrint(argv[0], "retransmit(2.4Ghz) count :%d", gReTxCount802154);
}

void setRetxCount802154(int argc, char **argv)
{
  int reTxCnt = ciGetUnsigned(argv[1]);
  bool success = false;

  if (reTxCnt < 10) {
	  changeRetxCount802154(reTxCnt);
	  success = true;
  }

  responsePrint(argv[0], "set retransmit(2.4Ghz) count :%d", reTxCnt);
}

void getRetxCountSubG(int argc, char **argv) {
	responsePrint(argv[0], "retransmit(Sub-Ghz) count :%d", gReTxCountSubG);
}

void setRetxCountSubG(int argc, char **argv)
{
  int reTxCnt = ciGetUnsigned(argv[1]);
  bool success = false;

  if (reTxCnt < 10) {
	  changeRetxCountSubG(reTxCnt);
	  success = true;
  }

  responsePrint(argv[0], "set retransmit(Sub-Ghz) count :%d", reTxCnt);
}

void lightOff(int argc, char **argv)
{
	//BSP_LedClear(0);
	//BSP_LedClear(1);
	//BSP_LedClear(2);
  set_gpio = 0;
#if 0
	uint8_t data[] = {
		//4D 00 01 00 00 00 00 0B 00 0A 00 65 0E 01 00 00 00 00 00 00 00 00 00 00 00 64 F9
	  0x4D, 0x00, 0x00, 0x01, 0x0A, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, \
    0x38, 0x02, 0x18, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x41, 0x03, 0x00
	};
	//initRadioSubGiga();
	gSysMode = SYS_MODE_LIGHT_OFF;
	MakeIEEE802154Packet();
	SendMessageIEEE802154(YES_BCAST_ADDR, data, 27);
	responsePrint(argv[0], "light off");
#endif
}

void lightON(int argc, char **argv)
{
	//BSP_LedClear(0);
	//BSP_LedClear(1);
	//BSP_LedClear(2);
  set_gpio = 1;
#if 0
	uint8_t data[] = {
		//4D 00 01 00 00 00 00 0B 00 0A 00 65 0E 01 00 00 00 00 00 00 00 00 00 00 00 64 F9
	  0x4D, 0x00, 0x00, 0x01, 0x0A, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, \
    0x38, 0x02, 0x18, 0x4F, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x41, 0x03, 0x00
	};
	//initRadioSubGiga();
	gSysMode = SYS_MODE_LIGHT_OFF;
	MakeIEEE802154Packet();
	//SendMessageIEEE802154(YES_BCAST_ADDR, data, 27);
	responsePrint(argv[0], "light on");

#endif
}


void Stop(int argc, char **argv)
{
	//BSP_LedClear(0);
	//BSP_LedClear(1);
	//BSP_LedClear(2);
  set_gpio = 2;
#if 0
	uint8_t data[] = {
		//4D 00 01 00 00 00 00 0B 00 0A 00 65 0E 01 00 00 00 00 00 00 00 00 00 00 00 64 F9
	  0x4D, 0x00, 0x00, 0x01, 0x0A, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, \
    0x38, 0x02, 0x18, 0x4F, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x41, 0x03, 0x00
	};
	//initRadioSubGiga();
	gSysMode = SYS_MODE_LIGHT_OFF;
	MakeIEEE802154Packet();
	//SendMessageIEEE802154(YES_BCAST_ADDR, data, 27);
	responsePrint(argv[0], "light on");

#endif
}

void rgbTest(int argc, char **argv)
{
	 int delay = ciGetUnsigned(argv[1]);
	gSysMode = SYS_MODE_RGB_TEST;//
	//initRadioSubGiga();
	setRgbTest(delay);

	responsePrint(argv[0], "rgbTest period : %d ms", delay);
}


void rx(int argc, char **argv)
{
  bool enable = ciGetUnsigned(argv[1]);

  // Don't allow Rx enable calls when scheduled Rx has been started
  if ((currentAppMode() == RX_SCHEDULED) && enable) {
    responsePrintError(argv[0], 29, "Can't turn on receive when in ScheduledRx");
    return;
  }

  // Only allow calls to Rx if we're in direct mode, no mode, or this is a
  // scheduled Rx disable call
  if ((currentAppMode() != DIRECT)
      && (currentAppMode() != RX_SCHEDULED)
      && (!inAppMode(NONE, argv[0]))) {
    return;
  }

  // Make sure this transition is allowed
  RAIL_RadioState_t currentState = RAIL_GetRadioState(railHandle2p4);
  if ((enable && (currentState & RAIL_RF_STATE_RX))
      || (!enable && (currentState <= RAIL_RF_STATE_IDLE))) {
    // Do nothing since we're already in the right state
  } else if (enable) {
    RAIL_StartRx(railHandle2p4, gChannel_2p4, NULL);
    receiveModeEnabled = enable;
  } else {
    RAIL_Idle(railHandle2p4, RAIL_IDLE_ABORT, false);
    receiveModeEnabled = enable;

    // Turn off ScheduledRx if we were in it
    if (currentAppMode() == RX_SCHEDULED) {
      enableAppModeSync(RX_SCHEDULED, false, NULL);
    }
  }

  // Print out the current status of receive mode
  responsePrint(argv[0],
                "Rx:%s,Idle:%s,Time:%u",
                (enable ? "Enabled" : "Disabled"),
                ((!enable) ? "Enabled" : "Disabled"),
                RAIL_GetTime());
}


void setTxStream(int argc, char **argv)
{
  uint8_t enable = ciGetUnsigned(argv[1]);
  enableAppMode(TX_STREAM, enable, argv[0]);
}
