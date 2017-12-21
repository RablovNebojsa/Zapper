#ifndef __STREAM_CONTROLLER_H__
#define __STREAM_CONTROLLER_H__

#include <stdio.h>
#include "tables.h"
#include "tdp_api.h"
#include "tables.h"
#include "pthread.h"
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include <linux/input.h>
#include <signal.h>
#include <string.h>

/**
 * @brief Structure that defines stream controller error
 */
typedef enum _StreamControllerError
{
	SC_NO_ERROR = 0,
	SC_ERROR,
	SC_THREAD_ERROR
}StreamControllerError;

/**
 * @brief Structure that defines channel info
 */
typedef struct _ChannelInfo
{
	uint16_t	serviceId;
	int16_t 	programNumber;
	int16_t		audioPid;
	int16_t 	videoPid;
	uint8_t		eventNameLength;
	char		eventName[256];
	uint8_t 	startTime[5];
}ChannelInfo;

/**
/* @brief Structure that holds starting parameters
*/
typedef struct _config {
	uint32_t 	freq;
	uint32_t 	bandwidth;
	uint32_t 	modul;
	uint32_t 	programNumber;
} Config;

/**
 * @brief Initializes stream controller module
 *
 * @return stream controller error code
 */
StreamControllerError streamControllerInit(const Config initData);

/**
 * @brief Deinitializes stream controller module
 *
 * @return stream controller error code
 */
StreamControllerError streamControllerDeinit();

/**
 * @brief Channel up
 *
 * @return stream controller error
 */
StreamControllerError channelUp();

/**
 * @brief Channel down
 *
 * @return stream controller error
 */
StreamControllerError channelDown();

/**
 * @brief Registers numeric key press
 *
 * @param [in]  program - program number from remote controller
		(change to program - 1)
 * @return stream controller error code
 */
StreamControllerError channelChange(uint32_t program);

/**
 * @brief Returns current channel info
 *
 * @param [out] channelInfo - channel info structure with current channel info
 * @return stream controller error code
 */
StreamControllerError getChannelInfo(ChannelInfo* channelInfo);

#endif /* __STREAM_CONTROLLER_H__ */
