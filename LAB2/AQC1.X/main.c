#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma config FOSC = HS // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF // Watchdog Timer Enable bit
#pragma config PWRTE = OFF // Power-up Timer Enable bit
#pragma config BOREN = OFF // Brown-out Reset Enable bit
#pragma config LVP = OFF // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit
#pragma config CPD = OFF // Data EEPROM Memory Code Protection bit
#pragma config WRT = OFF // Flash Program Memory Write Enable bits
#pragma config CP = OFF // Flash Program Memory Code Protection bit

#define TRUE 1
#define FALSE 0

int static standby;
int static standbyINT;
int static alarm;
int static timer;
int static count;
char static motion;
int static monitoring;
unsigned int lux_value_cfg ; //tolerance range
unsigned int low_light_value ;






//================================================================================================
//======================================= S E T U P ==============================================
//================================================================================================

void delay(long int n){
    int i=0;
    for (i=0;i<n;i++)
 
        continue;
}



void setBaudRate(void){
    
    TXSTA = 0; 
    RCSTA = 0;
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
    
   
    TXSTAbits.BRGH = 1;
    setBaudRate();
    
    // Enable the asynchronous serial port
   
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
 
    
    // Enable the asynchronous serial port
   
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

void setADC(){
    
    ADCON1bits.PCFG3 = 0;
    ADCON1bits.PCFG2 = 1; // in order to make button RA5 work
    ADCON1bits.PCFG1 = 0;
    ADCON1bits.PCFG0 = 0;
    ADCON1bits.ADFM = 1;
    ADCON0bits.ADCS0 = 0;
    ADCON0bits.ADCS1 = 0;
    ADCON0bits.ADON = 1;
}

void setADCRes(){
    ADCON1bits.PCFG3 = 0;
    ADCON1bits.PCFG2 = 0; // in order to make button RA5 not work
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
        continue;
    return (ADRESH << 8)+ADRESL;
}

int convertADC_1(){
    ADCON0bits.GO_nDONE = 1;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS0 = 1;
    while(ADCON0bits.GO_nDONE == 1)
        continue;
    return (ADRESH << 8)+ADRESL;
}

int convertADC_heat(){
    ADCON0bits.GO_nDONE = 1;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS1 = 1;
    ADCON0bits.CHS0 = 0;
    while(ADCON0bits.GO_nDONE == 1)
        continue;
    return (ADRESH << 8)+ADRESL;    
    
}



//================================================================================================
//===================================== M E S S A G E S ==========================================
//================================================================================================


void sendUART(char data){
    
    while(!PIR1bits.TXIF)
        continue;

    TXREG = data;
}


char receiveUART(){
    
    while(!PIR1bits.RCIF); /* set when register is not empty */
      
    char c = RCREG;
    
    return c;
}



void sendMessage(int ldr1, int ldr2){
    char c[70];
    int i = 0;
    sprintf(c,"\n<ldr_value>\n\t<ldr_1>%d</ldr_1>\n\t<ldr_2>%d</ldr_2>\n</ldr_value>\n",ldr1,ldr2);
    while(c[i]){
  
        sendUART(c[i++]);
        delay(200);
    } 
    
}

void sendParameters_message(unsigned int low_light, unsigned int lux_cfg ){
    char c[90];
    int i = 0;
    sprintf(c,"\n<Parameters>\n\t<low_light>%d</low_light>\n\t<lux_cfg>%d</lux_cfg>\n</Parameters>\n",low_light,lux_cfg);
    while(c[i]){
  
        sendUART(c[i++]);
        delay(200);
    } 
   
}
void sendInfo(char info){
    char c[60];
    int i = 0;
    
    switch (info){
        case 'e':
            sprintf(c,"\n<info>\n\t<state><error><\\state>\n<\\info>");break;
                
        case 'P':
            sprintf(c,"\n<info>\n\t<state><Motor has stopped><\\state>\n<\\info>");break;
            
        case 'E':
            sprintf(c,"\n<info>\n\t<state><Motion to the left><\\state>\n<\\info>");break;
                    
        case 'D':
            sprintf(c,"\n<info>\n\t<state><Motion to the right><\\state>\n<\\info>");break;
            
        case 'S':
            sprintf(c,"\n<info>\n\t<state><StandBy><\\state>\n<\\info>");break;
            
        case 'A':
            sprintf(c,"\n<info>\n\t<state><Motor over-heated><\\state>\n<\\info>");break;
            
        case 'N':
            sprintf(c,"\n<info>\n\t<state><Light is too low><\\state>\n<\\info>");break;
    
    }
    while(c[i]){
  
        sendUART(c[i++]);
        delay(20);
    } 
    sendUART('\n');
    
}





//================================================================================================
//===================================== C O N T R O L O ==========================================
//================================================================================================



void standby_toggle(){
    if(standby == 1){
        standby=0;
    }
    else {
        standby=1;
    }
    
}

int heatAlarm(int max_temp){
    if(convertADC_heat() > 2*max_temp)
        return TRUE;
    return FALSE;

}

void turnRes(int s){
    TRISCbits.TRISC5 = 0;
    if(s)
        PORTCbits.RC5 = 1;
    else
        PORTCbits.RC5 = 0;
    
//    if(alarm == 1){
//       PORTCbits.RC5 = 1;
//    }
//    else {
//       PORTCbits.RC5 = 0;
//    }
}

int isResOn(){
    if(PORTCbits.RC5)
        return TRUE;
    return FALSE;
}

void setupTimer(){
    OPTION_REG=7;
}

__interrupt() void interruptServiceRoutine(void){
    if(INTCONbits.TMR0IF){
        timer = 1;
        INTCONbits.TMR0IF = 0;   
    }
    if(INTCONbits.INTF){
        standbyINT = 1;
        INTCONbits.INTF = 0;
    }
}      

void checkLeds(){

    setADCRes();
    alarm = heatAlarm(50);
    setADC();
    //sendUART(alarm+'0');
    
    if(!alarm){
        PORTBbits.RB1 = 0; 
    }
    if(!standby){
        PORTBbits.RB2 = 0;
    }
    
    
    int lux_value = abs(convertADC_0() - convertADC_1());
    
    if(!standby && !alarm){ //standby variable 
        
        if(convertADC_0() > low_light_value || convertADC_1() > low_light_value )   //DayLight        
        {
            if( lux_value < lux_value_cfg){ //change lux_value_accordingly value accordingly
                //Lux value cfg acts as a Range tolerance for flunctuations between the sensors
                PORTBbits.RB6 = 0;
                PORTBbits.RB7 = 0;
              if(monitoring && (motion == 'E' || motion == 'D')){
                motion = 'P';
                sendInfo(motion);
                sendUART('\n');
              }
            } 

            else if(convertADC_0() > convertADC_1() ){ //Motion to the left
                PORTBbits.RB6 = 1;  //Left
                PORTBbits.RB7 = 0;  //Right
                
              if(monitoring && (motion == 'P' || motion == 'D')){
                motion = 'E';
                sendInfo(motion);  
                sendUART('\n');
              }
            }            

            else if (convertADC_0() < convertADC_1()){ //motion to the right

                PORTBbits.RB6 = 0;  //Left
                PORTBbits.RB7 = 1;  //Right
                  if(monitoring && (motion == 'E' || motion == 'P')){
                    motion = 'D';
                    sendInfo(motion);         
                    sendUART('\n');         

                  }
            }
        }
        else{ // NightLight
            PORTBbits.RB6 = 0;  //Left
            PORTBbits.RB7 = 0;  //Right
            
            if(monitoring && (motion == 'E' || motion == 'D')){
                motion = 'P';
                sendInfo('N');         
                sendUART('\n');
              }
        }
    
    
    } 
    else{ // StandBy = True || Alarm = True
        PORTBbits.RB6 = 0;  //Left
        PORTBbits.RB7 = 0;  //Right
        
        if(standby){
            int c;
            
            PORTBbits.RB2 = 1;

           if(monitoring && (motion == 'E' || motion == 'D')){
                motion = 'P';
                sendInfo('S');         
                sendUART('\n');
              }
        }
        if(alarm){
            
            PORTBbits.RB1 = 1; //Cooling down
            standby = 1;
            if(monitoring && (motion == 'E' || motion == 'D')){
                motion = 'P';
                sendInfo('A');         
                sendUART('\n');
              }
        }
    }
}

void printACK( int value) {
	int i = 0;
    value = value ;
	char frase[65];
	sprintf(frase,"\n<ACK>\n\t<C>%c</C>\n</ACK>", value);
	//sprintf(frase,"%d",value);

	while (frase[i]) {
		sendUART(frase[i++]);
		
	//	if (U2STAbits.UTXBF == 0) { // isso verifica se o buffer nao esta cheio
		//	U2TXREG = frase[i];		// atribuir ao registo U2TXREG um caracter da frase
			//i++;
		//}
	}
	//	clearBuffer();
}

int main(void)
{
	count = 0;
    timer=0;
    alarm=0;
    standby = 0;
    standbyINT = 0;
    monitoring = 1;
    motion = 'P'; //Reset
    lux_value_cfg = 100; //tolerance range
    low_light_value = 100; //Since when is considered NightLite
    
    RCSTA = 0;
    TXSTA = 0;
    TXSTAbits.BRGH = 1;
    SPBRG = 25;//9600 para BRGH = 1
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;
    RCSTAbits.SPEN = 1;
    
    INTCONbits.GIE = 1;
    INTCONbits.TMR0IE = 1;
    INTCONbits.INTE = 1;
    OPTION_REGbits.INTEDG = 1;
    
//    setTransmitter();
//    setReceiver();
    
    setADC();
    setupTimer();
    

    TRISB = 0b00111001; //RB3:5=1 (Button)
    TRISAbits.TRISA5 = 1; //RA5 is an input button
    
    char buffer[70];
   
    int i = 0;    
    int command = 0;
    unsigned int  value[2] ;
    turnRes(FALSE); //the initial state started with the Res ON
    while(1){
        if(PIR1bits.RCIF){ //if we have something to read from
            i = -1;          
            do{

                i++;
                buffer[i] = receiveUART();
                delay(5);
            }while(buffer[i] != '\n' && i<= 68 );
            buffer[++i] = '\0';
            delay(10);

//            for(i=0;i<69;i++){
//                sendUART(buffer[i]);
//            }
            
            //buffer[70] = "<command><C>6<\C><V>20<\V><\command>"
          
            //buffer[12] == Value on C
            command = buffer[12];

            //buffer[20:21] == Value on V
            value[0] = buffer[20];
            value[1] = buffer[21];
         
           
            switch (command){
                
                        case '1': printACK(command); sendMessage(convertADC_0(),convertADC_1());break;
                        case '2': printACK(command); monitoring = 0;break;
                        case '3': printACK(command); monitoring = 1;break; 
                        case '4': printACK(command); standby = 0; break;
                        case '5': printACK(command); standby = 1; break;
                        case '6': 
                            printACK(command);  if((value[0] <'0' || value[0] > '9') || (value[1] < '0' || value[1] > '9')  ){
                                                    sendInfo('e');}        
                                                else{low_light_value = ((value[0] - '0')*10 )+ (value[1] - '0');} break;
                        case '7': 
                            printACK(command);  if((value[0] <'0' || value[0] > '9') || (value[1] < '0' || value[1] > '9')  ){
                                                    sendInfo('e');}        
                                                else{ lux_value_cfg = ((value[0] - '0')*10 )+ (value[1] - '0');} break;
                        case '8': printACK(command); sendParameters_message( low_light_value ,  lux_value_cfg);break;
                default: sendInfo('e');break;
             
           } 
            PIR1bits.RCIF = 0;
            buffer[12] = buffer[20] = buffer[21] = 'e'; //empty the buffer values
        }
        if(timer){
            count++;
            timer = 0;
            if(count == 50){ //900 corresponde a 1 min
                count = 0;
                sendMessage(convertADC_0(),convertADC_1());                
            }
        }        

        if(!PORTBbits.RB3){ //button pressed

           while(!PORTBbits.RB3){
               continue; //makes sure the button is realeased
           }   

           standby_toggle();
        }
        if(standbyINT){
            standby_toggle();
            standbyINT = 0;
        }
        
        if(!PORTBbits.RB4 ||  !PORTBbits.RB5 ||  !PORTAbits.RA5){
             
            while(!PORTBbits.RB4  ||  !PORTBbits.RB5 ||  !PORTAbits.RA5);
            
            if(!isResOn())
                turnRes(TRUE);
            else
                turnRes(FALSE);             
            
             
        }
        
        
        checkLeds();
        //sendUART(alarm);
      
    }

    return 0;
}

