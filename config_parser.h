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
* \file config_parser.h
* \brief
*  Header file that contains type definitions and functio prototypes for configuration file parser
* Created on Dec 2017.
*
* @Author Nebojsa Rablov
* \notes
*
*****************************************************************************/

#ifndef __CONFIG_PARSER_H__
#define __CONFIG_PARSER_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "tdp_api.h"
#include "stream_controller.h"


/**
 * @brief Enumeration that defines configuration file parsing error
 */
typedef enum _ConfigError{
    CONFIG_OK = 0,
    CONFIG_ERROR
}ConfigError;

/**
 * @brief	Reads parameters from config.txt and store them in structure
 *
 * @param	[in] fileName - Contains name of configuration file
 * @param	[out] configStruct - Configuration structure to save config parameters
 *
 * @return	configuration file parsing error
 */
ConfigError parseConfigFile(char* fileName, Config* configStruct);

#endif