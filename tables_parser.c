#include "tables.h"

ParseErrorCode parsePatHeader(const uint8_t* patHeaderBuffer, PatHeader* patHeader)
{    
    if(patHeaderBuffer==NULL || patHeader==NULL)
    {
        printf("\n%s : ERROR received parameters are not ok\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }

    patHeader->tableId = (uint8_t)* patHeaderBuffer; 
    if (patHeader->tableId != 0x00)
    {
        printf("\n%s : ERROR it is not a PAT Table\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }
    
    uint8_t lower8Bits = 0;
    uint8_t higher8Bits = 0;
    uint16_t all16Bits = 0;
    
    lower8Bits = (uint8_t)(*(patHeaderBuffer + 1));
    lower8Bits = lower8Bits >> 7;
    patHeader->sectionSyntaxIndicator = lower8Bits & 0x01;

    higher8Bits = (uint8_t) (*(patHeaderBuffer + 1));
    lower8Bits = (uint8_t) (*(patHeaderBuffer + 2));
    all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
    patHeader->sectionLength = all16Bits & 0x0FFF;
    
    higher8Bits = (uint8_t) (*(patHeaderBuffer + 3));
    lower8Bits = (uint8_t) (*(patHeaderBuffer + 4));
    all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
    patHeader->transportStreamId = all16Bits & 0xFFFF;
    
    lower8Bits = (uint8_t) (*(patHeaderBuffer + 5));
    lower8Bits = lower8Bits >> 1;
    patHeader->versionNumber = lower8Bits & 0x1F;

    lower8Bits = (uint8_t) (*(patHeaderBuffer + 5));
    patHeader->currentNextIndicator = lower8Bits & 0x01;

    lower8Bits = (uint8_t) (*(patHeaderBuffer + 6));
    patHeader->sectionNumber = lower8Bits & 0xFF;

    lower8Bits = (uint8_t) (*(patHeaderBuffer + 7));
    patHeader->lastSectionNumber = lower8Bits & 0xFF;

    return TABLES_PARSE_OK;
}

ParseErrorCode parsePatServiceInfo(const uint8_t* patServiceInfoBuffer, PatServiceInfo* patServiceInfo)
{
    if(patServiceInfoBuffer==NULL || patServiceInfo==NULL)
    {
        printf("\n%s : ERROR received parameters are not ok\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }
    
    uint8_t lower8Bits = 0;
    uint8_t higher8Bits = 0;
    uint16_t all16Bits = 0;

    higher8Bits = (uint8_t) (*(patServiceInfoBuffer));
    lower8Bits = (uint8_t) (*(patServiceInfoBuffer + 1));
    all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
    patServiceInfo->programNumber = all16Bits & 0xFFFF; 

    higher8Bits = (uint8_t) (*(patServiceInfoBuffer + 2));
    lower8Bits = (uint8_t) (*(patServiceInfoBuffer + 3));
    all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
    patServiceInfo->pid = all16Bits & 0x1FFF;
    
    return TABLES_PARSE_OK;
}

ParseErrorCode parsePatTable(const uint8_t* patSectionBuffer, PatTable* patTable)
{
    uint8_t * currentBufferPosition = NULL;
    uint32_t parsedLength = 0;
    
    if(patSectionBuffer==NULL || patTable==NULL)
    {
        printf("\n%s : ERROR received parameters are not ok\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }
    
    if(parsePatHeader(patSectionBuffer,&(patTable->patHeader))!=TABLES_PARSE_OK)
    {
        printf("\n%s : ERROR parsing PAT header\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }
    
    parsedLength = 12 /*PAT header size*/ - 3 /*Not in section length*/;
    currentBufferPosition = (uint8_t *)(patSectionBuffer + 8); /* Position after last_section_number */
    patTable->serviceInfoCount = 0; /* Number of services info presented in PAT table */
    
    while(parsedLength < patTable->patHeader.sectionLength)
    {
        if(patTable->serviceInfoCount > TABLES_MAX_NUMBER_OF_PIDS_IN_PAT - 1)
        {
            printf("\n%s : ERROR there is not enough space in PAT structure for Service info\n", __FUNCTION__);
            return TABLES_PARSE_ERROR;
        }
        
        if(parsePatServiceInfo(currentBufferPosition, &(patTable->patServiceInfoArray[patTable->serviceInfoCount])) == TABLES_PARSE_OK)
        {
            currentBufferPosition += 4; /* Size from program_number to pid */
            parsedLength += 4; /* Size from program_number to pid */
            patTable->serviceInfoCount ++;
        }    
    }
    
    return TABLES_PARSE_OK;
}

ParseErrorCode printPatTable(PatTable* patTable)
{
    uint8_t i=0;
    
    if(patTable==NULL)
    {
        printf("\n%s : ERROR received parameter is not ok\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }
    
    printf("\n********************PAT TABLE SECTION********************\n");
    printf("table_id                 |      %d\n",patTable->patHeader.tableId);
    printf("section_length           |      %d\n",patTable->patHeader.sectionLength);
    printf("transport_stream_id      |      %d\n",patTable->patHeader.transportStreamId);
    printf("section_number           |      %d\n",patTable->patHeader.sectionNumber);
    printf("last_section_number      |      %d\n",patTable->patHeader.lastSectionNumber);
    
    for (i=0; i<patTable->serviceInfoCount;i++)
    {
        printf("-----------------------------------------\n");
        printf("program_number           |      %d\n",patTable->patServiceInfoArray[i].programNumber);
        printf("pid                      |      %d\n",patTable->patServiceInfoArray[i].pid); 
    }
    printf("\n********************PAT TABLE SECTION********************\n");
    
    return TABLES_PARSE_OK;
}


ParseErrorCode parsePmtHeader(const uint8_t* pmtHeaderBuffer, PmtTableHeader* pmtHeader)
{

    if(pmtHeaderBuffer==NULL || pmtHeader==NULL)
    {
        printf("\n%s : ERROR received parameters are not ok\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }

    pmtHeader->tableId = (uint8_t)* pmtHeaderBuffer; 
    if (pmtHeader->tableId != 0x02)
    {
        printf("\n%s : ERROR it is not a PMT Table\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }
    
    uint8_t lower8Bits = 0;
    uint8_t higher8Bits = 0;
    uint16_t all16Bits = 0;

    lower8Bits = (uint8_t) (*(pmtHeaderBuffer + 1));
    lower8Bits = lower8Bits >> 7;
    pmtHeader->sectionSyntaxIndicator = lower8Bits & 0x01;
    
    higher8Bits = (uint8_t) (*(pmtHeaderBuffer + 1));
    lower8Bits = (uint8_t) (*(pmtHeaderBuffer + 2));
    all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
    pmtHeader->sectionLength = all16Bits & 0x0FFF;

    higher8Bits = (uint8_t) (*(pmtHeaderBuffer + 3));
    lower8Bits = (uint8_t) (*(pmtHeaderBuffer + 4));
    all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
    pmtHeader->programNumber = all16Bits & 0xFFFF;
    
    lower8Bits = (uint8_t) (*(pmtHeaderBuffer + 5));
    lower8Bits = lower8Bits >> 1;
    pmtHeader->versionNumber = lower8Bits & 0x1F;

    lower8Bits = (uint8_t) (*(pmtHeaderBuffer + 5));
    pmtHeader->currentNextIndicator = lower8Bits & 0x01;

    lower8Bits = (uint8_t) (*(pmtHeaderBuffer + 6));
    pmtHeader->sectionNumber = lower8Bits & 0xFF;

    lower8Bits = (uint8_t) (*(pmtHeaderBuffer + 7));
    pmtHeader->lastSectionNumber = lower8Bits & 0xFF;

    higher8Bits = (uint8_t) (*(pmtHeaderBuffer + 8));
    lower8Bits = (uint8_t) (*(pmtHeaderBuffer + 9));
    all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
    pmtHeader->pcrPid = all16Bits & 0xFFFF;

    higher8Bits = (uint8_t) (*(pmtHeaderBuffer + 10));
    lower8Bits = (uint8_t) (*(pmtHeaderBuffer + 11));
    all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
    pmtHeader->programInfoLength = all16Bits & 0x0FFF;

    return TABLES_PARSE_OK;
}

ParseErrorCode parsePmtElementaryInfo(const uint8_t* pmtElementaryInfoBuffer, PmtElementaryInfo* pmtElementaryInfo)
{
    if(pmtElementaryInfoBuffer==NULL || pmtElementaryInfo==NULL)
    {
        printf("\n%s : ERROR received parameters are not ok\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }
    
    uint8_t lower8Bits = 0;
    uint8_t higher8Bits = 0;
    uint16_t all16Bits = 0;
    
    pmtElementaryInfo->streamType = (uint8_t) (*(pmtElementaryInfoBuffer));

    higher8Bits = (uint8_t) (*(pmtElementaryInfoBuffer + 1));
    lower8Bits = (uint8_t) (*(pmtElementaryInfoBuffer + 2));
    all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
    pmtElementaryInfo->elementaryPid = all16Bits & 0x1FFF; 

    higher8Bits = (uint8_t) (*(pmtElementaryInfoBuffer + 3));
    lower8Bits = (uint8_t) (*(pmtElementaryInfoBuffer + 4));
    all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
    pmtElementaryInfo->esInfoLength = all16Bits & 0x0FFF;

    return TABLES_PARSE_OK;
}

ParseErrorCode parsePmtTable(const uint8_t* pmtSectionBuffer, PmtTable* pmtTable)
{
    uint8_t * currentBufferPosition = NULL;
    uint32_t parsedLength = 0;
    
    if(pmtSectionBuffer==NULL || pmtTable==NULL)
    {
        printf("\n%s : ERROR received parameters are not ok\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }
    
    if(parsePmtHeader(pmtSectionBuffer,&(pmtTable->pmtHeader))!=TABLES_PARSE_OK)
    {
        printf("\n%s : ERROR parsing PMT header\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }
    
    parsedLength = 12 + pmtTable->pmtHeader.programInfoLength /*PMT header size*/ + 4 /*CRC size*/ - 3 /*Not in section length*/;
    currentBufferPosition = (uint8_t *)(pmtSectionBuffer + 12 + pmtTable->pmtHeader.programInfoLength); /* Position after last descriptor */
    pmtTable->elementaryInfoCount = 0; /* Number of elementary info presented in PMT table */
    
    while(parsedLength < pmtTable->pmtHeader.sectionLength)
    {
        if(pmtTable->elementaryInfoCount > TABLES_MAX_NUMBER_OF_ELEMENTARY_PID - 1)
        {
            printf("\n%s : ERROR there is not enough space in PMT structure for elementary info\n", __FUNCTION__);
            return TABLES_PARSE_ERROR;
        }
        
        if(parsePmtElementaryInfo(currentBufferPosition, &(pmtTable->pmtElementaryInfoArray[pmtTable->elementaryInfoCount])) == TABLES_PARSE_OK)
        {
            currentBufferPosition += 5 + pmtTable->pmtElementaryInfoArray[pmtTable->elementaryInfoCount].esInfoLength; /* Size from stream type to elemntary info descriptor*/
            parsedLength += 5 + pmtTable->pmtElementaryInfoArray[pmtTable->elementaryInfoCount].esInfoLength; /* Size from stream type to elementary info descriptor */
            pmtTable->elementaryInfoCount++;
        }    
    }

    return TABLES_PARSE_OK;
}

ParseErrorCode printPmtTable(PmtTable* pmtTable)
{
    uint8_t i=0;
    
    if(pmtTable==NULL)
    {
        printf("\n%s : ERROR received parameter is not ok\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }
    
    printf("\n********************PMT TABLE SECTION********************\n");
    printf("table_id                 |      %d\n",pmtTable->pmtHeader.tableId);
    printf("section_length           |      %d\n",pmtTable->pmtHeader.sectionLength);
    printf("program_number           |      %d\n",pmtTable->pmtHeader.programNumber);
    printf("section_number           |      %d\n",pmtTable->pmtHeader.sectionNumber);
    printf("last_section_number      |      %d\n",pmtTable->pmtHeader.lastSectionNumber);
    printf("program_info_legth       |      %d\n",pmtTable->pmtHeader.programInfoLength);
    
    for (i=0; i<pmtTable->elementaryInfoCount;i++)
    {
        printf("-----------------------------------------\n");
        printf("stream_type              |      %d\n",pmtTable->pmtElementaryInfoArray[i].streamType);
        printf("elementary_pid           |      %d\n",pmtTable->pmtElementaryInfoArray[i].elementaryPid);
    }
    printf("\n********************PMT TABLE SECTION********************\n");
    
    return TABLES_PARSE_OK;
}

ParseErrorCode parseEitHeader(const uint8_t* eitHeaderBuffer, EitHeader* eitHeader)
{
	if(eitHeaderBuffer == NULL || eitHeader == NULL){
		printf("\n%s : ERROR received parameters are not ok\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
	}
	
	eitHeader->tableId = (uint8_t)* eitHeaderBuffer;
	if(eitHeader->tableId != 0x4E){
		printf("\n%s : ERROR it is not a EIT Table\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
	}
	
    uint8_t lower8Bits = 0;
    uint8_t higher8Bits = 0;
    uint16_t all16Bits = 0;

    lower8Bits = (uint8_t) (*(eitHeaderBuffer + 1));
    lower8Bits = lower8Bits >> 7;
    eitHeader->sectionSyntaxIndicator = lower8Bits & 0x01;
	
	/* parsing section_length*/
	// eitHeader->sectionLength = (uint8_t) (((*(eitHeaderBuffer + 1) << 8) + (*(eitHeaderBuffer + 2))) & 0x0FFF;
    higher8Bits = (uint8_t) (*(eitHeaderBuffer + 1));
    lower8Bits = (uint8_t) (*(eitHeaderBuffer + 2));
    all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
    eitHeader->sectionLength = all16Bits & 0x0FFF;	
	
	/* Parsing service_id */
	higher8Bits = (uint8_t) (*(eitHeaderBuffer + 3));
	lower8Bits = (uint8_t) (*(eitHeaderBuffer + 4));
	all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
	eitHeader->serviceId = all16Bits & 0xFFFF;
	
	/* Parsiranje version_number */
	lower8Bits = (uint8_t) (*(eitHeaderBuffer + 5));
	lower8Bits = lower8Bits >> 1;
	eitHeader->versionNumber = lower8Bits & 0x1F;
	
	/* Parsiranje current_next_indicator */
	lower8Bits = (uint8_t) (*(eitHeaderBuffer + 5));
	eitHeader->currentNextIndicator = lower8Bits & 0x01;
	
	/* Parsiranje section_number */
	lower8Bits = (uint8_t) (*(eitHeaderBuffer + 6));
	eitHeader->sectionNumber = lower8Bits & 0xFF;
	
	/* Parsiranje last_section_number */
	lower8Bits = (uint8_t) (*(eitHeaderBuffer + 7));
	eitHeader->lastSectionNumber = lower8Bits & 0xFF;
	
	/* Parsing transport_stream_id */
	higher8Bits = (uint8_t) (*(eitHeaderBuffer + 8));
	lower8Bits = (uint8_t) (*(eitHeaderBuffer + 9));
	all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
	eitHeader->transportStreamId = all16Bits & 0xFFFF;
	
	/* Parsing original_network_id */
	higher8Bits = (uint8_t) (*(eitHeaderBuffer + 10));
	lower8Bits = (uint8_t) (*(eitHeaderBuffer + 11));
	all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
	eitHeader->originalNetworkId = all16Bits & 0xFFFF;
	
	/* Parsiranje segmen_last_section */
	lower8Bits = (uint8_t) (*(eitHeaderBuffer + 12));
	eitHeader->segmentLastSectionNumber = lower8Bits & 0xFF;
	
	/* Parsiranje last_table_id */
	lower8Bits = (uint8_t) (*(eitHeaderBuffer + 13));
	eitHeader->lastTabeId = lower8Bits & 0xFF;	
	
    return TABLES_PARSE_OK;
}

ParseErrorCode parseEitEventInfo(const uint8_t* eitEventInfoBuffer, EitEventInfo* eitEventInfo)
{
	if(eitEventInfoBuffer == NULL || eitEventInfo == NULL){
		printf("\n%s : ERROR received parameters are not ok\n", __FUNCTION__);
        	return TABLES_PARSE_ERROR;
	}
	
	uint8_t lower8Bits = 0;
    uint8_t higher8Bits = 0;
    uint16_t all16Bits = 0;
	uint32_t all32Bits = 0;
	uint64_t all64Bits = 0;
	
	/* Parsing event_id */
	lower8Bits = (uint8_t) (*(eitEventInfoBuffer));
	higher8Bits = (uint8_t) (*(eitEventInfoBuffer + 1));
	all16Bits = (uint16_t) ((higher8Bits << 8) + lower8Bits);
	eitEventInfo->eventId = all16Bits & 0xFF;
	
	/* Parsing start_time */
	all64Bits = (uint64_t) ((*(eitEventInfoBuffer + 2)) << 8) + (*(eitEventInfoBuffer + 3));
	all64Bits = (uint64_t) (all64Bits << 8) + (*(eitEventInfoBuffer + 4));
	all64Bits = (uint64_t) (all64Bits << 8) + (*(eitEventInfoBuffer + 5));
	all64Bits = (uint64_t) (all64Bits << 8) + (*(eitEventInfoBuffer + 6));
	eitEventInfo->startTime = all64Bits & 0x000FFFFF;
	
	/* Parse duration */
	all32Bits = (uint32_t) ((*(eitEventInfoBuffer + 7)) << 8) + (*(eitEventInfoBuffer + 8));
	all32Bits = (uint32_t) (all32Bits << 8) + (*(eitEventInfoBuffer + 9));
	eitEventInfo->duration = all32Bits & 0x0FFF;

	/* Parse running_status */
	lower8Bits = (uint8_t) (*(eitEventInfoBuffer + 10));
	lower8Bits = lower8Bits >> 5;
	eitEventInfo->runningStatus & 0x03;
	
	/* Parse free_CA_mode */
	lower8Bits = (uint8_t) (*(eitEventInfoBuffer + 10));
	lower8Bits = lower8Bits >> 4;
	eitEventInfo->freeCaMode = lower8Bits & 0x01;
	
	/* Parse descriptors_loop_ lengts */
	lower8Bits = (uint8_t) (*(eitEventInfoBuffer + 10));
	higher8Bits = (uint8_t) (*(eitEventInfoBuffer + 11));
	all16Bits = (uint16_t) (lower8Bits << 8) + higher8Bits;
	eitEventInfo->descriptorsLoopLength = all16Bits & 0x0FFF;
	
    return TABLES_PARSE_OK;
}

ParseErrorCode parseShortEventDescriptor(const uint8_t* shortEventDescriptorBuffer, EitEventInfo* eitEventInfo)
{
	uint8_t descriptorTag = 0;
	uint8_t descriptorLength = 0;
	uint8_t eventNameLength = 0;
	uint8_t parsedCount = 0;
	uint8_t i;

	while(parsedCount > eitEventInfo->descriptorsLoopLength){
		descriptorTag = (uint8_t)(*shortEventDescriptorBuffer + parsedCount);
		descriptorLength = (uint8_t)(*shortEventDescriptorBuffer + parsedCount + 1);
		if(descriptorTag == 0x4D){
			eventNameLength = (uint8_t)(*(shortEventDescriptorBuffer + parsedCount + 5));	
			for(i = 0; i < eventNameLength; i++){
				eitEventInfo->eventTitle[i] = (*(shortEventDescriptorBuffer + parsedCount + 5 + i));
			}
		}
		parsedCount += (descriptorLength + 2 /* Bites for descriptor_tag and descriptor_length */);
	}
	return TABLES_PARSE_OK;
}

ParseErrorCode parseEitTabe(const uint8_t* eitSectionBuffer, EitTable* eitTable)
{
    uint8_t * currentBufferPosition = NULL;
    uint32_t parsedLength = 0;
    
    if(eitSectionBuffer == NULL || eitTable == NULL){
        printf("\n%s : ERROR received parameters are not ok\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }
	
    if(parseEitHeader(eitSectionBuffer,&(eitTable->eitHeader))!= TABLES_PARSE_OK){
        printf("\n%s : ERROR parsing EIT header\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }

    parsedLength = 14 /*EIT header size*/ + 4 /*CRC size*/;
    currentBufferPosition = (uint8_t*)(eitSectionBuffer + 14); /* After Header */
    eitTable->eventInfoCount = 0; 
	
    while(parsedLength < eitTable->eitHeader.sectionLength)
    {
        if(eitTable->eventInfoCount > TABLES_MAX_NUMBER_OF_EVENTS_IN_EIT - 1)
        {
            printf("\n%s : ERROR there is not enough space in EIT structure for event info\n", __FUNCTION__);
            return TABLES_PARSE_ERROR;
        }
        
        if(parseEitEventInfo(currentBufferPosition, &(eitTable->eitEventInfoArray[eitTable->eventInfoCount])) == TABLES_PARSE_OK)
        {
		currentBufferPosition += 12; /* Positioning on first bite in description loop */
            //currentBufferPosition += 12 + eitTable->eitEventInfoArray[eitTable->eventInfoCount].descriptorsLoopLength;
            //parsedLength += 12 + eitTable->eitEventInfoArray[eitTable->eventInfoCount].descriptorsLoopLength;
            //eitTable->eventInfoCount++;
        }
	
	if(parseShortEventDescriptor(currentBufferPosition, &(eitTable->eitEventInfoArray[eitTable->eventInfoCount])) == TABLES_PARSE_OK){
		currentBufferPosition += eitTable->eitEventInfoArray[eitTable->eventInfoCount].descriptorsLoopLength; /* Positioning on next event info, after descriptors */
		parsedLength += 12/* Data without descriptors*/ + eitTable->eitEventInfoArray[eitTable->eventInfoCount].descriptorsLoopLength;
		eitTable->eventInfoCount++;
	}    
    }

    return TABLES_PARSE_OK;	
}
