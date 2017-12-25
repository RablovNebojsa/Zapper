#ifndef __GRAPHIC_CONTROLLER_H__
#define __GRAPHIC_CONTROLLER_H__

#include "pthread.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <directfb.h>
#include <stdint.h>

#define FRAME_WIDTH 20
#define FRAME_HEIGHT 10
#define CHANNEL_NUMBER_HEIGHT 50

#define INFO_BANNER_HEIGHT 200

/**
 * @brief Enumeration of possible graphic controller error codes
 */
typedef enum _GraphicControllerError{
	GRAPHIC_NO_ERROR = 0,
	GRAPHIC_ERROR,
	GRAPHIC_THREAD_ERROR
}GraphicControllerError;

/**
 * @brief Structure contains status information and values of graphic elements
 */
typedef struct _GraphicStatus{
	bool		showVolume;
	uint8_t		volumeValue;
	bool		showChannelNumber;
	uint32_t	channelNumberValue;
	bool		showInfoBanner;
	char		infoBannerName[50];
	char		infoBannerStart[30];
	char  		infoBannerPid[50];
	char		infoBannerChannel[30];
}GraphicStatus;

GraphicControllerError graphicConntrollerInit();

GraphicControllerError graphicConntrollerDeinit();

GraphicControllerError showVolume(const uint32_t value);

GraphicControllerError showChannelNumber(const uint32_t value);

GraphicControllerError showInfoBanner(const char* name, const char* start, const char* pid, const char* channel);

#endif
