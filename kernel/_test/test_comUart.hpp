#include "com_uart.h"
#include "utility/cobs_u8.h"
#include "utility/softCRC.h"
#include "bootloader_test.h"

uint8_t rxRegister = 0;

uint8_t txMockBuffer[100];
uint8_t txMockBufferIndex = 0;

void com_txMockCallback(uint8_t byte)
{
    if(byte == 0x00) byte = 0x55;
    else if(byte == 0x55) byte = 0x00;

    txMockBuffer[txMockBufferIndex] = byte;
    txMockBufferIndex++;
    USART0_TX_interruptHandler();
}

void sendData(uint8_t *data, uint8_t size)
{
    for(uint8_t i = 0; i< sizeof(txMockBuffer); i++){
        txMockBuffer[i] = 0;
    }
    txMockBufferIndex = 0;

    uint8_t temp[100];
    for(uint8_t i = 0; i< size; i++){
        temp[i] = data[i];
    }

    uint16_t crc = crc16(&data[0], size);
    temp[size]= ((crc>>8)&0xFF);
    temp[size+1]= (crc&0xFF);

    uint8_t txData[100];
    uint8_t txDataSize = cobs_encode(&txData[0], &temp[0], size+2);

    com_setUartIdle();

    rxRegister = 0x55;
    USART0_RX_interruptHandler();
    for(uint8_t i = 0; i<txDataSize-1; i++){
        rxRegister = txData[i];
        USART0_RX_interruptHandler();
    }
    rxRegister = 0x55;
    USART0_RX_interruptHandler();

    com_handler();
}

void checkReply(uint8_t *data, uint8_t size)
{
    uint8_t rxData[100];
    uint8_t rxDataSize = 0;
    if(size){
        rxDataSize = cobs_decode(&rxData[0], &txMockBuffer[1], txMockBufferIndex-1);
    }

    uint16_t crc = crc16(&rxData[0], rxDataSize);
    if(rxDataSize) {
        rxDataSize -= 2;
    }

    REQUIRE(((size != 0 && crc==0) || (size == 0 && crc==0xFFFF)));
    REQUIRE(match_array(&rxData[0], &data[0], rxDataSize));
}

TEST_CASE( "Test comUart", "[comUart]" ) {

    SECTION("Test buffer overflow") {
        uint8_t data[] = {0x0F, 0x01, 0x6d, 0x89, 0x46, 0xe3, 0x75, 0x80,
                          0xb0, 0xa0, 0x56, 0xe5, 0x1b, 0x4b, 0x7a, 0x2d,
                          0x09, 0x50, 0x83, 0xe0, 0x3e, 0x36, 0x7c, 0x18,
                          0x65, 0xc4, 0x03, 0x51, 0x96, 0x6e, 0x4e, 0x0b,
                          0x0f, 0xc1, 0x17, 0xA8, 0x66, 0xAB, 0x2A, 0xE4
        };
        sendData(&data[0], sizeof(data));

        checkReply(nullptr, 0);
    }
}

TEST_CASE( "Test protocol", "[protocol]" ) {
    SECTION("Test \"Get Device State\" command") {

        uint8_t data[] = {0x0F, 0x00};
        sendData(&data[0], sizeof(data));

        uint8_t txExpected[] = {0x0F, 0x80, 0x00};
        checkReply(&txExpected[0], sizeof(txExpected));
    }

    SECTION("Test \"Get Hardware Information\" command") {
        uint8_t data[] = {0x0F, 0x01};
        sendData(&data[0], sizeof(data));

        uint8_t txExpected[] = {
            0x0F,
            0x81,
            (CONTROLLER_ID>>8)&0xFF,
            CONTROLLER_ID&0xFF,
            (HARDWARE_ID>>8)&0xFF,
            HARDWARE_ID&0xFF,
            HARDWARE_VERSION_MAJOR,
            HARDWARE_VERSION_MINOR,
            KERNEL_VERSION_MAJOR,
            KERNEL_VERSION_MINOR
        };
        checkReply(&txExpected[0], sizeof(txExpected));
    }

    SECTION("Test \"Get Memory Information\" command") {
        uint8_t data[] = {0x0F, 0x02};
        sendData(&data[0], sizeof(data));

        uint8_t txExpected[] = {
                0x0F,
                0x82,
                (FlashByteSize>>8)&0xFF,
                FlashByteSize&0xFF,
                (AppBaseByteAddress>>8)&0xFF,
                AppBaseByteAddress&0xFF,
                FlashPageByteSize,
                (RamSize>>8)&0xFF,
                RamSize&0xFF,
                (AppRamStart>>8)&0xFF,
                AppRamStart&0xFF,
                (EepromSize>>8)&0xFF,
                EepromSize&0xFF,
                (AppEepromStart>>8)&0xFF,
                AppEepromStart&0xFF,
        };
        checkReply(&txExpected[0], sizeof(txExpected));
    }
}