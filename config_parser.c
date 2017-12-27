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
* \file config_parser.c
* \brief
*  Module for parsing config file
* Created on Dec 2017.
*
* @Author Nebojsa Rablov
* \notes
*
*****************************************************************************/

#include "config_parser.h"

/**
 * @brief	Validates parametars and store them in configStruct
 *
 * @param	[in] param - Line read from config.txt
 * @param	[out] configStruct - Configuration structure to save config parameters
 *
 * @return	configuration file parsing error
 */
static ConfigError checkConfigParametar(char* param, Config* configStruct);

ConfigError parseConfigFile(char* fileName, Config* configStruct){
    FILE* fp;
    size_t bufferSize = 128;
	char* buffer;
	int characters = 0;

	if ((fp = fopen(fileName, "r")) == NULL)
	{ /* Open source file. */
		printf("Config file cannot open!\n");
		return CONFIG_ERROR;
	}

    buffer = (char*)malloc(bufferSize * sizeof(char));
    if( buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }
	/* Read each line form config.txt and extracts parametaer value */
	while ((characters = getline(&buffer, &bufferSize, fp)) != -1)
	{
	    printf("%d\n", characters);
	    if(strcmp(buffer,"\n")){
            buffer[characters - 1] = '\0';
            if(checkConfigParametar(buffer, configStruct)){
                printf("Extracting parametar error!\n");
                return CONFIG_ERROR;
            }
	    }
	}
	fclose(fp);
	return CONFIG_OK;
}

ConfigError checkConfigParametar(char* param, Config* configStruct){
    char* token;
	token = strtok(param,"=");
	if(token == NULL){
		printf("Line parsing error!\n");
		return CONFIG_ERROR;
	}
	if(!strcmp(token, "freq")){
		token = strtok(NULL,"=");
		configStruct->freq = atoi (token);
	}else if(!strcmp(token, "bandwidth")){
		token = strtok(NULL,"=");
		configStruct->bandwidth = atoi (token);
	}else if(!strcmp(token, "modul")){
		token = strtok(NULL,"=");
		if((!strcmp(token, "DVB_T")) || (!strcmp(token, "dvb_t"))){
		    configStruct->modul = DVB_T;
		}else{
			printf("\nModul can be DVB_T or DVBT_T2. Default value is DVB_T\n");
			configStruct->modul = DVB_T;	
		}
	}
	else if(!strcmp(token, "programNumber")){
        token = strtok(NULL,"=");
        configStruct->programNumber = atoi (token);
	}
    return CONFIG_OK;
}
