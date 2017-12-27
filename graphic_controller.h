/****************************************************************************
*
* Univerzitet u Novom Sadu, Fakultet tehnickih nauka
* Katedra za Računarsku Tehniku i Računarske Komunikacije
*
* -----------------------------------------------------
* Ispitni zadatak iz predmeta:
* PROGRAMSKA PODRSKA U TELEVIZIJI I OBRADI SLIKE
* -----------------------------------------------------
* TV Application
* -----------------------------------------------------
*
* \file graphic_controller.h
* \brief
* Module that manages the display of graphic elements
* Created on Dec 2017.
*
* @Author Nebojsa Rablov
*
*****************************************************************************/

#ifndef __GRAPHIC_CONTROLLER_H__
#define __GRAPHIC_CONTROLLER_H__

#include "pthread.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <directfb.h>
#include <stdint.h>

#define FRAME_WIDTH 20	/** Width of channel graphic element frame */
#define FRAME_HEIGHT 10	/** height of channel graphic element frame */
#define CHANNEL_NUMBER_HEIGHT 50	/** Height of number representing channel number */
#define INFO_BANNER_HEIGHT 200	/** Height of info banner frame */

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

/**
* @brief	Start initialization of timers and DFB, set initial 
* 			values and create new thread for graphic controller
*
* @return	Graphic controller error code
*/
GraphicControllerError graphicConntrollerInit();

/**
* @brief	Deinitialize DFB and timers and stop graphic task 
* 			
* @return	Graphic controller error code
*/
GraphicControllerError graphicConntrollerDeinit();

/**
* @brief	Set flag in GraphicStatus struct to display volume graphic element
* 	
* @param	[in] value - Value of volume that indicate which image to be displayed
*		
* @return	Graphic controller error code
*/
GraphicControllerError showVolume(const uint32_t value);

/**
* @brief	Set flag in GraphicStatus to display channel number graphic element
* 	
* @param	[in] value - Which channel number to display
*		
* @return	Graphic controller error code
*/
GraphicControllerError showChannelNumber(const uint32_t value);

/**
* @brief	Set flag in GraphicStatus to display info banner graphic element
* 	
* @param	[in] name - string that contains name of current event to be displayed
* @param	[in] start - string that contains start time of current event to be displayed
* @param	[in] pid - string that contains video and audio PIDs of service to be displayed
* @param	[in] channel - string that contains value of current channel number to be displayed
*		
* @return	Graphic controller error code
*/
GraphicControllerError showInfoBanner(const char* name, const char* start, const char* pid, const char* channel);

#endif
