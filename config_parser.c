#include "config_parser.h"

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

// Trebalo bi ispitati za svaki parametar da li je ispravno zadat (granice frekvencija, bandwidth,
// moduli koji su podrzani i slicno)
static ConfigError checkConfigParametar(char* param, Config* configStruct){
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
		// Ovde bi se mogao iskoristiti RegEx
		if((!strcmp(token, "DVB_T")) || (!strcmp(token, "dvb_t"))){
		    configStruct->modul = 0;
		}
	}
	else if(!strcmp(token, "programNumber")){
        token = strtok(NULL,"=");
        configStruct->programNumber = atoi (token);
	}
    return CONFIG_OK;
}
