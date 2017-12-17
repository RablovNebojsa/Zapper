#include "tables.h"

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

ParseErrorCode parseEitEventInfo(const uint8_t* eitEventInfoBuffer, EitEventInfo eitEventInfo)
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
	all64Bits = (uint64_t) ((*(eitEventInfoBuffer + 2)) << 8) + (*(eitEventInfoBuffer + 3);
	all64Bits = (uint64_t) (all64Bits << 8) + (*(eitEventInfoBuffer + 4);
	all64Bits = (uint64_t) (all64Bits << 8) + (*(eitEventInfoBuffer + 5);
	all64Bits = (uint64_t) (all64Bits << 8) + (*(eitEventInfoBuffer + 6);
	eitEventInfo->startTime = all64Bits & 0x000FFFFF;
	
	/* Parse duration */
	all32Bits = (uint32_t) ((*(eitEventInfoBuffer + 7)) << 8) + (*(eitEventInfoBuffer + 8);
	all32Bits = (uint32_t) (all32Bits << 8) + (*(eitEventInfoBuffer + 9);
	eitEventInfo->duration = all32Bits & 0x0FFF;

	/* Parse running_status */
	lower8Bits = (uint8_t) (*(eitEventInfoBuffer + 10);
	lower8Bits = lower8Bits >> 5;
	eitEventInfo->runningStatus & 0x03;
	
	/* Parse free_CA_mode */
	lower8Bits = (uint8_t) (*(eitEventInfoBuffer + 10);
	lower8Bits = lower8Bits >> 4;
	eitEventInfo->freeCaMode = lower8Bits & 0x01;
	
	/* Parse descriptors_loop_ lengts */
	lower8Bits = (uint8_t) (*(eitEventInfoBuffer + 10);
	higher8Bits = (uint8_t) (*(eitEventInfoBuffer + 11);
	all16Bits = (uint16_t) (lower8Bits << 8) + higher8Bits;
	eitEventInfo->descriptorsLoopLength = all16Bits & 0x0FFF;
	
    return TABLES_PARSE_OK;
}

ParseErrorCode parseEitTabe(const uint8_t* eitSectionBuffer, EitTable* eitTable)
{
    uint8_t * currentBufferPosition = NULL;
    uint32_t parsedLength = 0;
    
    if(eitSectionBuffer==NULL || eitTable==NULL){
        printf("\n%s : ERROR received parameters are not ok\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }
	
    if(parseEitHeader(eitSectionBuffer,&(eitTable->eitHeader))!= TABLES_PARSE_OK){
        printf("\n%s : ERROR parsing EIT header\n", __FUNCTION__);
        return TABLES_PARSE_ERROR;
    }

    parsedLength = 14 /*EIT header size*/ + 4 /*CRC size*/;
    currentBufferPosition = *(eitTable->sectionLength) + 11; /* Position after last descriptor */
    pmtTable->elementaryInfoCount = 0; /* Number of elementary info presented in PMT table */
	
    while(parsedLength < eitTable->eitHeader.sectionLength)
    {
        if(eitTable->eventInfoCount > TABLES_MAX_NUMBER_OF_EVENTS_IN_EIT - 1)
        {
            printf("\n%s : ERROR there is not enough space in EIT structure for event info\n", __FUNCTION__);
            return TABLES_PARSE_ERROR;
        }
        
        if(parseEitEventInfo(currentBufferPosition, &(eitTable->eitEventInfoArray[eitTable->eventInfoCount])) == TABLES_PARSE_OK)
        {
            currentBufferPosition += 12 + eitTable->eitEventInfoArray[eitTable->eventInfoCount].descriptorsLoopLength; /* Size from stream type to elemntary info descriptor*/
            parsedLength += 12 + eitTable->eitEventInfoArray[eitTable->eventInfoCount].descriptorsLoopLength; /* Size from stream type to elemntary info descriptor*/
            eitTable->eventInfoCount++;
        }    
    }
    return TABLES_PARSE_OK;	
}