
#include "mcc_generated_files/mcc.h"
#include "ADXL345.h"

uint8_t read(uint8_t address){
    GEN_SetLow();
    __delay_us(4);
    SPI1_ExchangeByte(0xff^(address|0x80));
    uint8_t value = SPI1_ExchangeByte(0);
    GEN_SetHigh();
    __delay_us(4);
    return value;
}

void write(uint8_t address, uint8_t data){
    GEN_SetLow();
    __delay_us(4);
    SPI1_ExchangeByte(0xff^address);
    uint8_t value = SPI1_ExchangeByte(0xff^data);
    GEN_SetHigh();
    __delay_us(4);
}

void set_up_acc(){
    write(POWER_CTL, 8);
    //write(DATA_FORMAT, 0b1000111);
    write(THRESH_TAP, 0xff);
    write(Window, 4);
    write(THRESH_ACT, 3);
    write(TIME_INACT, 1);
    write(ACT_INACT_CTL, 0xff);
    write(THRESH_FF, 10);
    write(TIME_FF, 2);
    write(BW_RATE,0b1111);
    write(INT_ENABLE, 0x80);
    write(FIFO_CTL, 0x80);
}

void read_axises(int16_t axises[3]){
    
    for(char i = DATAX0; i < DATAZ1; i += 2){
        axises[(i - DATAX0) / 2] = (read(i + 1) << 8) | read(i); 
    }
    
}