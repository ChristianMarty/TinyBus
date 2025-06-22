//**********************************************************************************************************************
// FileName : ledFader.c
// FilePath : /utility
// Author   : Christian Marty
// Date		: 15.06.2025
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include "ledFader.h"

#ifdef __cplusplus
extern "C" {
#endif

void ledFader_calculateStepValue(ledFader_t *data);

void ledFader_initialize(ledFader_t *data)
{
    data->currentValue = 0;
    data->destinationValue = 0;
    data->fadeTime = 0;
    data->waitForExecution = false;
    data->stepSize = 0;
}

void ledFader_fade(ledFader_t *data, uint16_t destinationValue, uint8_t fadeTime, bool waitForExecution)
{
    data->destinationValue = destinationValue;
    data->fadeTime = fadeTime;
    data->waitForExecution = waitForExecution;
    data->stepSize = 0;

    if(waitForExecution != true){
        ledFader_execute(data);
    }
}

void ledFader_execute(ledFader_t *data)
{
    if(data->fadeTime != 0){
         ledFader_calculateStepValue(data);
    }else{
        data->stepSize = 0;
        data->currentValue = data->destinationValue;
    }
}

void ledFader_calculateStepValue(ledFader_t *data)
{
    uint16_t difference = 0;

    if(data->destinationValue > data->currentValue){
        difference = data->destinationValue - data->currentValue;
    }else if(data->destinationValue < data->currentValue){
        difference = data->currentValue - data->destinationValue;
    }

    uint16_t fadeTimeTicks = data->fadeTime;
    uint16_t fadeStep = (difference / fadeTimeTicks);
    if(fadeStep <1){
        fadeStep = 1;
    }

    data->stepSize = fadeStep;
}

void ledFader_handler(ledFader_t *data)
{
    if(data->stepSize != 0) {
        if(data->currentValue < data->destinationValue) // dimming up
        {
            // Prevent overflow
            if((PWM_TOP - data->currentValue) > data->stepSize){
                data->currentValue += data->stepSize;
                if(data->currentValue > data->destinationValue){
                    data->currentValue = data->destinationValue;
                    data->stepSize = 0;
                }
            }else{
                data->currentValue = data->destinationValue;
                data->stepSize = 0;
            }
        }
        else if (data->currentValue > data->destinationValue)  // dimming down
        {
            // Prevent underflow
            if(data->currentValue > data->stepSize){
                data->currentValue -= data->stepSize;
                if(data->currentValue < data->destinationValue){
                    data->currentValue = data->destinationValue;
                    data->stepSize = 0;
                }
            }else{
                data->currentValue = data->destinationValue;
                data->stepSize = 0;
            }
        }
    }
}

#ifdef __cplusplus
}
#endif