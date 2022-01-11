/* 
 * File:   nrf24mod.h
 * Author: Victor k
 *
 * Created on 1<<20 April 1<<2020, 17:44
 */

#ifndef NRF24MOD_H
#define	NRF24MOD_H

#ifdef	__cplusplus
extern "C" {
#endif
//SPI commands
#define READ_RECEIVER 0b01100001
#define W_TX_PAYLOAD 0xA0
#define W_TX_PAYLOAD_NO_ACK 0b10110000
#define FLUSH_TX 0b11100001
#define FLUSH_RX 0b11100010
#define ACTIVATE 0b01010000
const unsigned char ACTIVATE_CODE = 0x73;
#define NOPT 0xFF
#define R_REGISTER 0
#define W_REGISTER 0b00100000
#define REUSE_TX_PL 0b11100011
#define R_RX_PL_WID 0b10100000
#define W_ACK_PAYLOAD 0b10101000 //or with pipe
//Registers
#define CONFIG 0
    #define EN_CRC 1<<3
    #define PWRUP 2
    #define PRIM_RX 1
#define EN_AA 1
    #define ENAA_P5 1<<5
    #define ENAA_P4 1<<4
    #define ENAA_P3 1<<3
    #define ENAA_P2 1<<2
    #define ENAA_P1 1
    #define ENAA_P0 0
#define EN_RXADDR 2
    #define ERX_P5 1<<5
    #define ERX_P4 1<<4
    #define ERX_P3 1<<3
    #define ERX_P2 1<<2
    #define ERX_P1 1
    #define ERX_P0 0
#define SETUP_AW 3
    #define AW_3 1
    #define AW_4 0b10
    #define AW_5 0b11
#define SETUP_RETR 4
#define RF_CH 5
#define RF_SETUP 6
    #define CONT_WAVE 1<<7
    #define PLL_LOCK 1<<4
    #define DR2MBS 1<<3
    #define DR1MBS 0
    #define RF_PWR_1 0
    #define RF_PWR_2 0b010
    #define RF_PWR_3 0b100
    #define RF_PWR_4 0b110
    #define LNA_HCURR 1
#define STATUSR 7
    #define RX_DR 1<<6
    #define TX_DS 1<<5
    #define MAX_RT 1<<4
    #define RX_P_NO 2
    #define TX_FULL 0
#define OBSERVE_TX 8
    #define ARC_CNT 0xf
    #define PLOS_CNT 0xf0
#define CD 9
#define RX_ADDR_P0		0x0A
#define RX_ADDR_P1		0x0B
#define RX_ADDR_P2		0x0C
#define RX_ADDR_P3		0x0D
#define RX_ADDR_P4		0x0E
#define RX_ADDR_P5		0x0F
#define TX_ADDR         0x10
#define RX_PW_P0		0x11
#define RX_PW_P1		0x12
#define RX_PW_P2		0x13
#define RX_PW_P3		0x14
#define RX_PW_P4		0x15
#define RX_PW_P5		0x16
#define NRF_FIFO_STATUS	0x17
    #define TX_REUSE 1<<6
    #define FTX_FULL 1<<5
    #define TX_EMPTY 1<<4
    #define RX_FULL 2
    #define RX_EMPTY 0
#define DYNPD 0x1C
    #define DPL_P5 1<<5
    #define DPL_P4 1<<4
    #define DPL_P3 1<<3
    #define DPL_P2 1<<2
    #define DPL_P1 2
    #define DPL_P0 0
#define FEATURE 0x1D
    #define EN_DPL 0b100
    #define EN_ACK_PAY 0b10
    #define EN_DYN_ACK 0b1

#define FILL SSP1BUF=NOPT
#include <stdint.h>
#include <stdbool.h>
void setuptrans(uint8_t plw,uint8_t address_w,uint8_t channel,
        uint8_t rfSettings, uint8_t radioid, bool transmiter);
unsigned char NRF24_getStat(void);
void NRF24_writeReg(uint8_t reg, uint8_t *value, char width);
void NRF24_readReg(uint8_t reg, char *block, char width);
void transmitMode();
void receiveMode();
void NRF24_Send(uint8_t c[], bool ack, char width);
void NRF24_readPLD(char *pld, char width);
void flushRX();
void flushTX();
void NRF24_reusePLD();
void NRF24_clear_MAX_RT();
void NRF24_clear_TX_DS();
void NRF24_flushRX();
void NRF24_clear_RX_DR();
void target(uint8_t address[], uint8_t addressWidth);
#ifdef	__cplusplus
}
#endif

#endif	/* NRF24MOD_H */

