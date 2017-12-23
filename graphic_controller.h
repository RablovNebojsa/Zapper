#ifndef __GRAPHIC_CONTROLLER_H__
#define __GRAPHIC_CONTROLLER_H__

#include "pthread.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <directfb.h>
#include <stdint.h>

/**
 * @brief Enumeration of possible graphic controller error codes
 */
typedef enum _GraphicControllerError{
	GRAPHIC_NO_ERROR = 0,
	GRAPHIC_ERROR,
	GRAPHIC_THREAD_ERROR
}GraphicControllerError;

typedef struct _GraphicStatus{
	bool	showVolume;
	uint8_t	volumeValue;
	bool	showChannelNumber;
	uint8_t	channelNumberValue;
	bool	showInfoBanner;
	char	infoBannerText[256];
}GraphicStatus;

GraphicControllerError graphicConntrollerInit();

GraphicControllerError graphicConntrollerDeinit();

GraphicControllerError showVolume(const uint8_t value);

GraphicControllerError showChannelNumber(const uint8_t value);

GraphicControllerError showInfoBanner(const char* text);

#endif
