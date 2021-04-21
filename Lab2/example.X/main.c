/*
 * File:   main.c
 * Author: Pedro Amaral
 *
 * Created on 7 de Abril de 2021, 14:08
 */

#include <xc.h>
#pragma config FOSC = HS // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF // Watchdog Timer Enable bit
#pragma config PWRTE = OFF // Power-up Timer Enable bit
#pragma config BOREN = OFF // Brown-out Reset Enable bit
#pragma config LVP = OFF // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit
#pragma config CPD = OFF // Data EEPROM Memory Code Protection bit
#pragma config WRT = OFF // Flash Program Memory Write Enable bits
#pragma config CP = OFF // Flash Program Memory Code Protection bit

void setBaudRate(void){
    SPBRG = 25; // BaudRate = 9600 -> This was on Syncronous USART

//
//    // TXSTA<4> = SYNC -> Secalhar é este
//
//    TXSTAbits.
//
//    TXSTAbits.SYNC = 0; //Asynchronous Mode
//    TXSTAbits.BRGH = 0;
//
//
//    #pragma config BRGH = 0;
//
}

void setTransmitter(){
    setBaudRate();
    
    // Enable the asynchronous serial port
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;
    
    //if interrupts are desired then
    //PIE1bits.TXIE = 1;
    
    //9-bit Transmission (parity)
    //TXSTAbits.TX9 = 1;
    
    //Enable the transmission 
    TXSTAbits.TXEN =1; //also sets TXIF
    
    //Start the Transmission
    //data = 25;
    //TXREG = data;    
}

void setReceiver(void){
    setBaudRate();
    
    // Enable the asynchronous serial port
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;
    
    //if interrupts are desired then
    //PIE1bits.RCIE = 1;
    
    //9-bit Reception (parity)
    //RCSTAbits.RX9 = 1;
    
    //Enable the reception
    RCSTAbits.CREN =1;
    
    //Flag bit RCIF will be set when reception is complete
    //and an interrupt will be generated if enable
    //bit RCIE is set.
    
    /*Read the RCSTA register to get the ninth bit (if
    enabled) and determine if any error occurred
    during reception.*/
    
    //Read the 8-bit received data by reading the
    //RCREG;
}

void sendUART(char data){
    TXREG = data;
}

char receiveUART(){
    return RCREG;
}


int main(void)
{
    TRISBbits.TRISB3 = 1;
    TRISDbits.TRISD0 = 0;
    PORTDbits.RD0 = 0;
    int i = 0;
//    while (1){
//        if (!PORTBbits.RB3){
//            PORTDbits.RD0 = 1;
//            for(i = 0 ; i < 20000 ; i++){};
//                PORTDbits.RD0 = 0;
//            for(i = 0 ; i < 20000 ; i++){};
//            
//        }
//    }
    
    setTransmitter();
    setReceiver();
    
    sendUART(25);
    receiveUART();
 
    
    
}