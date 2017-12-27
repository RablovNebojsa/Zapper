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
* \file stream_controller.h
* \brief
*  Header file contains type definitions and functio prototyps for stream controller
* 
* Created on Dec 2017.
*
* @Author Nebojsa Rablov
* \notes
*
*****************************************************************************/

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
 * @brief 	Structure that defines stream controller error
 */
typedef enum _StreamControllerError
{
	SC_NO_ERROR = 0,
	SC_ERROR,
	SC_THREAD_ERROR
}StreamControllerError;

/**
 * @brief 	Structure that defines channel info used for graphic elements
 */
typedef struct _ChannelInfo
{
	uint16_t	serviceId;
	uint32_t 	programNumber;
	int16_t		audioPid;
	int16_t 	videoPid;
	uint8_t		eventNameLength;
	char		eventName[256];
	uint8_t 	startTime[5];
}ChannelInfo;

/**
 * @brief 	Structure that defines starting parameters
 */
typedef struct _config {
	uint32_t 	freq;
	uint32_t 	bandwidth;
	t_Module 	modul;
	uint32_t 	programNumber;
} Config;

/**
 * @brief 	Initializes stream controller module
 *
 * @return 	stream controller error code
 */
StreamControllerError streamControllerInit(const Config initData);

/**
 * @brief 	Deinitializes stream controller module
 *
 * @return	stream controller error code
 */
StreamControllerError streamControllerDeinit();

/**
 * @brief 	Set actual volume on player
 *
 * @param 	[in] volume - value for volume
 *
 * @return 	stream controller error code
 */
StreamControllerError setPlayerVolume(const uint32_t volume);

/**
 * @brief 	Get volume from player
 *
 * @param 	[out] volume - locatin to save volume value
 *
 * @return 	stream controller error code
 */
StreamControllerError getPlayerVolume(uint32_t *volume);

/**
 * @brief 	Channel up, when channel up button is pressed on remote controller
 *
 * @return 	stream controller error
 */
StreamControllerError channelUp();

/**
 * @brief 	Channel down, when channel down button is pressed on remote controller
 *
 * @return 	stream controller error
 */
StreamControllerError channelDown();

/**
 * @brief 	Change on channel given by user over remote controller
 *
 * @param 	[in] program - program number from remote controller (change to program - 1)
 *
 * @return 	stream controller error code
 */
StreamControllerError channelChange(uint32_t program);

/**
 * @brief 	Returns current channel info
 *
 * @param 	[out] channelInfo - channel info structure with current channel information
 *
 * @return 	stream controller error code
 */
StreamControllerError getChannelInfo(ChannelInfo* channelInfo);

#endif /* __STREAM_CONTROLLER_H__ */
