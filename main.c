/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.6
        Device            :  PIC18F24K42
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include "nrf24mod.h"
#include "ADXL345.h"
#include <string.h>
#define ONESHOT_PULSE (uint16_t) 2000
#define PWM_PULSE (uint16_t) 740
/*
                         Main application
 */
enum STATE {CONTROL, SEND};
enum DRIVE {HOVER,REMOTE, COMERR};
enum DRIVE drive = REMOTE;
enum STATE state = CONTROL;
char conf = 0;
typedef struct motor{
    void (*motor_controller)(uint16_t);
    uint16_t max;
}motor;
struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    uint32_t OnTimeMillis;
    uint32_t FailedTxCount;
};

motor motors[] = {{&PWM5_LoadDutyValue, PWM_PULSE}, 
{&PWM6_LoadDutyValue, PWM_PULSE}, {&PWM7_LoadDutyValue, PWM_PULSE}, 
{&PWM8_LoadDutyValue, PWM_PULSE}};
void set_motor(motor m, double value){
    (*m.motor_controller)((m.max * (value)));
    
}

void zero_motors(void){
    
    for(char i =0; i < 4; i++) {
        set_motor(motors[i], 0);
    }
}
void set_motors(double value){
    for(char i =0; i < 4; i++) {
        set_motor(motors[i], value);
    }
}

void hover(int16_t ref[],int16_t axises[]){
    int16_t change = 0;
    change = axises[2] - ref[2];
    if (change < 0){
        set_motors(60);
        return;
    } else {
        for (char i = 0; i < 4; i+=2) {
           double bal = ( (ref[i / 2] -  axises[i/2])) / 300;
           set_motor(motors[i], 20.0 + bal);
           set_motor(motors[i + 1], 20.0 - bal);
        }
        __delay_ms(3);
    }
}

void change_module(spi1_modes_t module){
    SPI1_Close();
    SPI1_Open(module);
}
void check_comm(){
        
        NRF24_readReg(0, &conf, 1);
        if((conf & (EN_CRC | PWRUP | PRIM_RX)) < (EN_CRC | PWRUP | PRIM_RX)){
             setuptrans(3,AW_5, 60, DR2MBS|RF_PWR_4|LNA_HCURR, 10, false);
        }
       
    
}
void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();
    TMR3_SetInterruptHandler(check_comm);
    //change_module(NRF24)
    zero_motors();
    while(!IRQ_GetValue());
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();
    
    setuptrans(3,AW_5, 60, DR2MBS|RF_PWR_4|LNA_HCURR, 10, false);
    INTCON0bits.GIE = 1;
    INTCON0bits.GIEL = 1;
    unsigned char c[5];
    struct RadioPacket _radioData;
    int16_t axises[3], hold[3];
    /*change_module(ACC);
    read(0);
    set_up_acc();
    read_axises(hold);
    change_module(NRF24);*/
    while (1)
    {
       if(!IRQ_GetValue()){
           PIE6bits.TMR3IE =0;
            NRF24_readPLD(c, 5);
            if(c[0]==0 || c[0]!='s'){
                setuptrans(3,AW_5, 60, DR2MBS|RF_PWR_4|LNA_HCURR, 10, false);
                zero_motors();
            } else{
                
                for (char i = 0; i < 4; i++) {
                    set_motor(motors[i], (c[i + 1] /100.0));
                }
            }
            PIE6bits.TMR3IE =1;
            continue;
            /*const char *m = &c;
            break;
            drive = REMOTE;
            for (char i = 0; i < 3; i++) {
                if (c[i] != 0xff || c[i] != 0) {
                    drive = COMERR;
                    break;
                } else if (strcmp("HOV", m)){
                    drive = HOVER;
                }
            }
            if (drive == HOVER){
                change_module(ACC);
                if(((read(0)!=0xE5)||!read(POWER_CTL))){
                    set_up_acc();
                    continue;
                }
            }  */
           //NRF24_readPLD(&_radioData, sizeof(_radioData));
       }
       if(drive == REMOTE){
            zero_motors();
           /*_radioData.OnTimeMillis++;
           const char mes[7] = "LAMB425";
           NRF24_Send(mes, true, 7);
           
           NRF24_reusePLD();*/
           
           
        }else if (read(INT_SOURCE) & 128 && drive == HOVER) {
           read_axises(axises);
           hover(hold, axises);
        } else if (drive == COMERR) {
            setuptrans(3,AW_5, 60, DR2MBS|RF_PWR_4, 10, false );
        }
       
        //zero_motors();
    }
}
/**
 End of File
*/