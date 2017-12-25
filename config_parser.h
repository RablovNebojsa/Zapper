#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "tdp_api.h"
#include "stream_controller.h"

#define bufSize 1024

/**
/* @brief Enumeration that defines configuration file parsing error
*/
typedef enum _ConfigError{
    CONFIG_OK = 0,
    CONFIG_ERROR
}ConfigError;

/**
 * @brief Reads parameters from config.txt and store them in struct
 *
 * @param [fileName] Contains name of configuration file
 * @param [configStruct] Configuration structure to save config parameters
 *
 * @return configuration file parsing error
 */
ConfigError parseConfigFile(char* fileName, Config* configStruct);
