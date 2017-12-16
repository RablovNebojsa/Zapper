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