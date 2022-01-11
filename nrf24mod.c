#include <xc.h>
#include "nrf24mod.h"
#include "mcc_generated_files/spi1.h"
#include <pic18f24k42.h>
#include "mcc_generated_files/device_config.h"
#include "mcc_generated_files/pin_manager.h"
void NRF24_flushTX();
bool streaming = false;
void setuptrans(uint8_t plw,uint8_t address_w,uint8_t channel, 
        uint8_t rfSettings, uint8_t radioid, bool transmiter){
    WCSN_SetHigh();
    NRF24_writeReg(ACTIVATE, &ACTIVATE_CODE, 1);
    /*flushRX();
    flushTX();*/
    if (transmiter){
        transmitMode();
    } else {
        receiveMode();
    }
    char one= 0b11111;
    char zero = 0;
    NRF24_writeReg(EN_AA, &one, 1);
    //set address width
    NRF24_writeReg(SETUP_AW, &address_w, 1);
    //set channel
    NRF24_writeReg(RF_CH, &channel, 1);
    //enable pipe
    //NRF24_writeReg(EN_RXADDR, &one, 1);
    //rx payload width
    /*NRF24_writeReg(RX_PW_P0, &plw, 1);
    NRF24_writeReg(RX_PW_P1, &zero, 1);
    NRF24_writeReg(RX_PW_P2, &zero, 1);
    NRF24_writeReg(RX_PW_P3, &zero, 1);
    NRF24_writeReg(RX_PW_P4, &zero, 1);
    NRF24_writeReg(RX_PW_P5, &zero, 1);*/
    //Radio transfer rate
    NRF24_writeReg(RF_SETUP, &rfSettings, 1);
    //
    char setup_value = 0b00011111;
    NRF24_writeReg(SETUP_RETR, &setup_value , 1);
    uint8_t address[5] = { 1, 2, 3, 4, radioid};
    NRF24_writeReg(RX_ADDR_P1, address, 5);
    setup_value = DPL_P1|DPL_P0;
    NRF24_writeReg(DYNPD, &setup_value , 1);
    setup_value = EN_DYN_ACK|EN_ACK_PAY|EN_DPL;
    NRF24_writeReg(FEATURE, &setup_value , 1);
    NRF24_flushRX();
    NRF24_clear_RX_DR();
    NRF24_getStat();
   // }
}
void NRF24_writeReg(uint8_t reg, uint8_t *value, char width){
    
    WCSN_SetLow();
    SPI1_ExchangeByte(reg|W_REGISTER);
    for(char i = 0; i < width; i++) {
        SPI1_ExchangeByte(*(value + i));
    }
    WCSN_SetHigh();
    //}
}
void transmitMode(){
    NRF24_flushTX();
    uint8_t address[5] = { 1, 2, 3, 4, 4 };
    target(address, 5);
    char setings = PWRUP|EN_CRC;
    NRF24_writeReg(CONFIG, &setings, 1);
}
void target(uint8_t address[], uint8_t addressWidth){
    NRF24_writeReg(TX_ADDR, address, addressWidth);
    NRF24_writeReg(RX_ADDR_P0, address, addressWidth);
}
void receiveMode(){
    char setings = (PWRUP)|PRIM_RX|EN_CRC;
    NRF24_writeReg(CONFIG, &setings, 1);
}
void NRF24_Send(uint8_t c[], bool ack, char width){
    for(char j = 0; j <= width / 32; j++) {
        uint8_t fifoReg, statusReg;
        uint8_t txBufferIsEmpty;
        uint8_t packetWasSent, packetCouldNotBeSent;
        uint8_t txAttemptCount = 0;
        uint8_t result = 0; // Default to indicating a failure.

        // TX buffer can store 3 packets, sends retry up to 15 times, and the retry wait time is about half
        // the time necessary to send a 32 byte packet and receive a 32 byte ACK response.  3 x 15 x 2 = 90
        const static uint8_t MAX_TX_ATTEMPT_COUNT = 90;

        while (txAttemptCount++ < MAX_TX_ATTEMPT_COUNT)
        {
            NRF24_readReg(NRF_FIFO_STATUS, &fifoReg, 1);
            txBufferIsEmpty = fifoReg & TX_EMPTY;

            if (txBufferIsEmpty)
            {
                result = 1; // Indicate success.
                break;
            }


            __delay_us(500);

            NRF24_readReg(STATUSR, &statusReg, 1);
            packetWasSent = statusReg & TX_DS;
            packetCouldNotBeSent = statusReg & MAX_RT;

            uint8_t s = 0;
            if (packetWasSent)
            {   
                s = TX_DS;
                NRF24_writeReg(STATUSR, &s, 1);   // Clear TX success flag.
            }
            else if (packetCouldNotBeSent)
            {   
                NRF24_flushTX();
                s = MAX_RT;
                NRF24_writeReg(STATUSR, &s, 1);          // Clear max retry flag.
                break;
            }
        }

    

    
        WCSN_SetLow();
        if (ack){
        SPI1_ExchangeByte(W_TX_PAYLOAD);
        }else{
            SPI1_ExchangeByte(W_TX_PAYLOAD_NO_ACK);
        }
        for (char i = 0; i < width % 32; i++)
        SPI1_ExchangeByte(c[32 * j + i]);
        WCSN_SetHigh();
    }
}
void NRF24_clear_TX_DS(){
    char c =TX_DS;
    NRF24_writeReg(STATUSR, &c, 1);
}
void NRF24_clear_MAX_RT(){
    char c =MAX_RT;
    NRF24_writeReg(STATUSR, &c, 1);
}
void NRF24_readReg(uint8_t reg, char *block, char width){
    WCSN_SetLow();
    SPI1_ExchangeByte(R_REGISTER|reg);
    uint8_t j = 0;
    while( j < width) {
        block[j++] =SPI1_ExchangeByte(0);
        
    }
    WCSN_SetHigh();
}
unsigned char NRF24_getStat(void){
    char stat = 0;
        NRF24_readReg(STATUSR, &stat, 1);
    return stat;
}
void NRF24_flushTX(){
    WCSN_SetLow();
    SPI1_ExchangeByte(FLUSH_TX);
    WCSN_SetHigh();
}
void NRF24_flushRX(){
    WCSN_SetLow();
    
    SPI1_ExchangeByte(FLUSH_RX);
    WCSN_SetHigh();
}

void NRF24_clear_RX_DR() {
    char c = RX_DR;
    NRF24_writeReg(STATUSR, &c, 1);
}
void NRF24_reusePLD(){
    WCSN_SetLow();
    SPI1_ExchangeByte(REUSE_TX_PL);
    WCSN_SetHigh();
}
void NRF24_readPLD(char *pld, char width){
    WCSN_SetLow();
    SPI1_ExchangeByte(READ_RECEIVER);
    for (char i = 0; i < width ; i++) {
        pld[i] = SPI1_ExchangeByte(0);
    }
    WCSN_SetHigh();
}
