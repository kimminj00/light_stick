/***************************************************************************//**
 * @file app_modes.c
 * @brief This is the states machine for the base test application. It handles
 * transmit, receive, and various debug modes.
 * @copyright Copyright 2015 Silicon Laboratories, Inc. www.silabs.com
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rail.h"
#include "response_print.h"
#include "app_common.h"

// At any given time, at least one of (currAppMode, prevAppMode)
// should be NONE, due to the way that setAppModeInternal works
char nextCommandBuf[16];
char *nextCommand;
bool enableMode = true;
AppMode_t nextAppMode = NONE;
AppMode_t currAppMode = NONE;
AppMode_t prevAppMode = NONE;
bool transitionPend = false;

AppMode_t currentAppMode()
{
  return currAppMode;
}

AppMode_t previousAppMode()
{
  return prevAppMode;
}

const char *appModeNames(AppMode_t appMode)
{
  char *appModes[] = { "None", "Stream", "Tone", "ContinuousTx", "DirectMode",
                       "PacketTx", "ScheduledTx", "SchTxAfterRx", "RxOverflow",
                       "TxUnderflow", "TxCancel", "RfSense", "PER", "BER",
                       "ScheduledRx" };
  return appModes[appMode];
}

void enableAppMode(AppMode_t next, bool enable, char *command)
{
  // Should disable current mode instead of enabling NONE
  if (!transitionPend && (next != NONE)) {
    transitionPend = true;
    nextAppMode = next;
    enableMode = enable;
    if (command == NULL) {
      nextCommand = (logLevel & ASYNC_RESPONSE) ? "appMode" : NULL;
    } else {
      memcpy(&nextCommandBuf[0], command, sizeof(nextCommandBuf));
      nextCommand = &nextCommandBuf[0];
    }
  }
}

static void transitionAppMode(AppMode_t nextAppMode)
{
  if (currAppMode == NONE) {
    RAIL_CancelTimer(railHandle2p4);
  } else if (currAppMode == TX_STREAM) {
    RAIL_StopTxStream(railHandle2p4);
  } else if (currAppMode == TX_TONE) {
    RAIL_StopTxStream(railHandle2p4);
  } else if (currAppMode == DIRECT) {
    RAIL_EnableDirectMode(railHandle2p4, false);
  } else if (currAppMode == TX_CONTINUOUS   || currAppMode == TX_N_PACKETS
             || currAppMode == TX_SCHEDULED || currAppMode == TX_UNDERFLOW
             || currAppMode == TX_CANCEL) {
    // Disable timer just in case
    RAIL_CancelTimer(railHandle2p4);
    txCount = 0;
    pendFinishTxSequence();
  } else if (currAppMode == RF_SENSE) {
    (void) RAIL_StartRfSense(railHandle2p4, RAIL_RFSENSE_OFF, 0, NULL);
  } else if (currAppMode == PER) {
    RAIL_CancelTimer(railHandle2p4);
  } else if (currAppMode == BER) {
    RAIL_Idle(railHandle2p4, RAIL_IDLE_ABORT, false);
  }

  if (nextAppMode == TX_STREAM) {
    RAIL_StartTxStream(railHandle2p4, gChannel_2p4, RAIL_STREAM_PN9_STREAM);
  } else if (nextAppMode == TX_TONE) {
    RAIL_StartTxStream(railHandle2p4, gChannel_2p4, RAIL_STREAM_CARRIER_WAVE);
  } else if (nextAppMode == DIRECT) {
    RAIL_EnableDirectMode(railHandle2p4, true);
  } else if (nextAppMode == TX_CONTINUOUS || nextAppMode == TX_N_PACKETS) {
    pendPacketTx();
  } else if (nextAppMode == TX_SCHEDULED || nextAppMode == TX_CANCEL) {
    txCount = 1;
    pendPacketTx();
  } else if (nextAppMode == SCHTX_AFTER_RX || nextAppMode == RX_OVERFLOW) {
    RAIL_StartRx(railHandle2p4, gChannel_2p4, NULL);
  }
  prevAppMode = currAppMode;
  currAppMode = nextAppMode;
}


static void setAppModeInternal(void)
{
  AppMode_t next = enableMode ? nextAppMode : NONE;
  if (currAppMode == nextAppMode && enableMode) {
    if (nextCommand) {
      responsePrint(nextCommand, "%s:Enabled", appModeNames(next));
    }
  } else if ((currAppMode == nextAppMode && !enableMode)
             || (currAppMode == NONE && enableMode)) {
    if (nextCommand) {
      responsePrint(nextCommand,
                    "%s:Enabled,%s:Disabled,Time:%u",
                    appModeNames(next),
                    appModeNames(currAppMode),
                    RAIL_GetTime());
    }
    transitionAppMode(next);
  } else { // Ignore mode change request
    if (nextCommand) {
      responsePrintError(nextCommand, 1, "Can't %s %s during %s",
                         enableMode ? "enable" : "disable",
                         appModeNames(nextAppMode), appModeNames(currAppMode));
    }
  }
}

void changeAppModeIfPending()
{
  if (transitionPend) {
    transitionPend = false;
    setAppModeInternal();
  }
}

