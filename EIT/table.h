#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#define TABLES_MAX_NUMBER_OF_EVENTS_IN_EIT  20

typedef struct _EitHeader{
    uint8_t     tableId;
    uint8_t     sectionSyntaxIndicator;
    uint16_t    sectionLength;
    uint16_t    serviceId;
    uint8_t     versionNumber;
    uint8_t     currentNextIndicator;
    uint8_t     sectionNumber;
    uint8_t     lastSectionNumber;
    uint16_t    transportStreamId;
    uint16_t    originalNetworkId;
    uint8_t     segmentLastSectionNumber;
    uint8_t     lastTabeId;
}EitHeader;

typedef struct _EitEventInfo{
    uint16_t    eventId;
    uint64_t    startTime;
    uint32_t    duration;
    uint8_t     runningStatus;
    uint8_t     freeCaMode;
    uint16_t    descriptorsLoopLength;
}EitEventInfo;

typedef struct  _EitTable{
    EitHeader       eitHeader;
    EitEventInfo    eitEventInfoArray[TABLES_MAX_NUMBER_OF_EVENTS_IN_EIT];
}EitTable;

#endif // TABLE_H_INCLUDED
