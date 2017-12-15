#include "config_parser.h"

static ConfigError checkConfigParametar(char* param, Config* configStruct);

ConfigError parseConfigFile(char* fileName, Config* configStruct){
    FILE* fp;
	char buf[bufSize];

	if ((fp = fopen(fileName, "r")) == NULL)
	{ /* Open source file. */
		printf("Config file cannot open!\n");
		return CONFIG_ERROR;
	}

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
	    if(strcmp(buf,"\n")){
            buf[strlen(buf) - 1] = '\0';
            if(checkConfigParametar(buf, configStruct)){
                printf("Extracting parametar error!\n");
                return CONFIG_ERROR;
            }
	    }
	}
	fclose(fp);
	return CONFIG_OK;
}

// Trebalo bi ispitati za svaki parametar da li je ispravno zadat( (granice frekvencija, bandwidth,
// moduli koji su podrzani i slicno
static ConfigError checkConfigParametar(char* param, Config* configStruct){
    char* token;
	token = strtok(param,"=");
printf("\n%s",token);
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
