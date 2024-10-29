/***************************************************************************//**
 * @file mode_helpers.c
 * @brief This file contains helpers for transitioning into the various
 *   AppModes
 * @copyright Copyright 2015 Silicon Laboratories, Inc. www.silabs.com
 ******************************************************************************/

#include "rail.h"
#include "rail_types.h"
#include "response_print.h"
#include "app_common.h"

// Guard for CI functions to ensure a certain AppMode before running
bool inAppMode(AppMode_t appMode, char *command)
{
  bool ret = (currentAppMode() == appMode);
  if (!ret && command) {
    responsePrintError(command, 0x16,
                       "Need to be in %s mode for this command",
                       appModeNames(appMode));
  }
  return ret;
}

// Setting semantics for enabling AppMode
void setNextAppMode(AppMode_t next, char *command)
{
  if (next == NONE) {
    enableAppMode(currentAppMode(), false, command);
  } else {
    enableAppMode(next, true, command);
  }
}

// Used when the AppMode is needed to be enabled to continue the function
bool enableAppModeSync(AppMode_t mode, bool enable, char *command)
{
  changeAppModeIfPending();
  enableAppMode(mode, enable, command);
  changeAppModeIfPending();
  // inAppMode shouldn't print, since enableAppMode will cause a print
  return inAppMode(enable ? mode : NONE, NULL);
}


