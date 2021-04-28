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

void setADC(){

    ADCON1bits.PCFG3 = 0;
    ADCON1bits.PCFG2 = 0;
    ADCON1bits.PCFG1 = 0;
    ADCON1bits.PCFG0 = 0;
    ADCON1bits.ADFM = 1;
    ADCON0bits.ADCS0 = 0;
    ADCON0bits.ADCS1 = 0;
    ADCON0bits.ADON = 1;
}


int convertADC_0(){
    ADCON0bits.GO_nDONE = 1;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS0 = 0;
    while(ADCON0bits.GO_nDONE == 1)
        ;
    return (ADRESH << 8)+ADRESL;
}

int convertADC_1(){
    ADCON0bits.GO_nDONE = 1;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS0 = 1;
    while(ADCON0bits.GO_nDONE == 1)
        ;
    return (ADRESH << 8)+ADRESL;
}

int main (){
   
    //UART
    setTransmitter();
    setReceiver();
    setADC();
    while(1){
        sendUART(convertADC_0());
        sendUART(convertADC_1());
    }

    return 0;
}