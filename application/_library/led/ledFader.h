//**********************************************************************************************************************
// FileName : ledFader.h
// FilePath : /utility
// Author   : Christian Marty
// Date		: 15.06.2025
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PWM_TOP 0xFFFF

typedef struct{
    volatile uint16_t currentValue;
    volatile uint16_t destinationValue;
    volatile uint8_t fadeTime;
    volatile bool waitForExecution;
    volatile int16_t stepSize;
}ledFader_t;

void ledFader_initialize(ledFader_t *data);
void ledFader_fade(ledFader_t *data, uint16_t value, uint8_t fadetime, bool waitForExecution);
void ledFader_execute(ledFader_t *data);

void ledFader_handler(ledFader_t *data);

#ifdef __cplusplus
}
#endif