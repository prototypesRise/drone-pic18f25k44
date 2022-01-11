/*
 * File:   ov7670.c
 * Author: victor
 *
 * Created on 23 July 2021, 7:00 PM
 */
#include "mcc_generated_files/examples/i2c1_master_example.h"
#include "ov7670.h"

void init_cam() {
    I2C1_Write1ByteRegister(0x42, REG_DBLV, 0);
    I2C1_Write1ByteRegister(0x42, REG_CLKRC, 0b111111);
    I2C1_Write1ByteRegister(0x42, REG_COM3, COM3_DCWEN|COM3_SCALEEN);
    I2C1_Write1ByteRegister(0x42, REG_COM7, COM7_FMT_QCIF|COM7_RGB);
    I2C1_Write1ByteRegister(0x42, REG_COM6, 0);
    I2C1_Write1ByteRegister(0x42, REG_COM10, COM10_HSYNC|COM10_PCLK_HB);
    //I2C1_Write1ByteRegister(0x42, 
}

