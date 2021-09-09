

#include <p24fxxxx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Configuration Bits
#ifdef __PIC24FJ64GA004__ //Defined by MPLAB when using 24FJ64GA004 device
_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1 & IOL1WAY_ON) 
_CONFIG2( FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRI & I2C1SEL_SEC)
#else
_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2) 
_CONFIG2( FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRI)
#endif

int static standby;
int static alarm;
char static motion;
int static monitoring;


void delay(int ms){
	//int current_time = clock();
	//while(clock() < (current_time + ms));
	int i,j;
	for( i=0; i<ms;i++){
		for( j=0; j< ms;j++);
	}	
}


void configureADC(){
	AD1PCFG = 0xFFC0;      
	AD1CON1 = 0x2202;     
	AD1CON2 = 0;          
	AD1CON3 = 0x0100;                
	AD1CSSL = 0; 
	AD1CON1bits.ADON = 1;
}

int convertADC(int channel){
	int adc_value;
	/*AD1CHS = channel;
	AD1CON1bits.SAMP = 1;
	delay(1000);
	AD1CON1bits.SAMP = 0;
	while (!AD1CON1bits.DONE);
	*/
	AD1PCFG = 0xFFC0;
	AD1CON1 = 0x0000;
	AD1CHS = channel;
	AD1CSSL = 0;
	AD1CON3 = 0x0002;
	AD1CON2 = 0;
	AD1CON1bits.ADON = 1;
	AD1CON1bits.SAMP = 1;
	delay(250);
	AD1CON1bits.SAMP = 0;
	while (!AD1CON1bits.DONE){};
	
	adc_value = ADC1BUF0;
	return adc_value;
}




void setBaudRate(){
	U2BRG=25;
}

void SetupTransmitter(){
	U2STA = 0; //UxSTA: UARTx Status and Control Register
	U2MODE = 0x8000; //Enable Uart for 8-bit data -> UTXISEL<1:0> = 00
	//no parity, 1 STOP bit
	
	U2STAbits.UTXEN = 1; //Enable Transmit
	//IEC0bits.U2TXIE = 1; //Enable Transmit Interrupt
}	

char receiveChar() {

	char ch = '\n';
	if (U2STAbits.URXDA) {
		ch = U2RXREG;
		IFS1bits.U2RXIF = 0; //igualar a zero para limpar
	}	 						// espera que um caracter seja inserido



	return ch;
}

void readCmd(){
	char buffer[65];
	int i=0;
	
	do{
		buffer[i] = receiveChar();
	}	while(buffer[i++]);
}	

void sendUART(data){
	//The UxTX pin is at logic ?1? when no transmission is taking place.
	//while(U2STAbits.TRMT){ //Advances when Register is empty
//			continue;
//	}
	
//	while(U2STAbits.UTXISEL1 != 0 || U2STAbits.UTXISEL0 != 0){ //Advances when ISEL = 00
//			continue;
//	}
	
	if (U2STAbits.UTXBF == 0) { // isso verifica se o buffer nao esta cheio
			U2TXREG = data;
	
	}
	//U2TXREG = data;

}	
	


void print(int ldr1, int ldr2) {
	int i = 0;
	char frase[65];
	sprintf(frase,"\n<ldr_value>\n\t<ldr_1>%d</ldr_1>\n\t<ldr_2>%d</ldr_2>\n</ldr_value>",ldr1,ldr2);
	//sprintf(frase,"%d",value);
	while (frase[i]) {
		sendUART(frase[i++]);
		
		
	//	if (U2STAbits.UTXBF == 0) { // isso verifica se o buffer nao esta cheio
		//	U2TXREG = frase[i];		// atribuir ao registo U2TXREG um caracter da frase
			//i++;
			delay(100);
		//}
	}
}

int heatAlarm(int max_temp){
    if(convertADC(4)>2*max_temp)
        return 1;
    return 0;
}

void checkLeds(){
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////CONTROL//////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int lux_value = abs(convertADC(2) - convertADC(3));
	//int standby = 1;
    
    int lux_value_cfg = 150; //tolerance range
    int low_light_value = 250;
    if(!heatAlarm(50)){
        PORTAbits.RA3 = 0; 
    }	
    if(!standby && !heatAlarm(50)){ //standby variable 
        
        if(convertADC(2) > low_light_value || convertADC(3) > low_light_value )   //DayLight
        
        {
            if( lux_value < lux_value_cfg){ //change lux_value_accordingly value accordingly
                //Lux value cfg acts as a Range tolerance for flunctuations between the sensors
                PORTAbits.RA6 = 0;
                PORTAbits.RA5 = 0;
              if(monitoring && (motion == 'E' || motion == 'D')){
                motion = 'P';
                sendUART(motion);
                sendUART('\n');
              }
            } 

            else if(convertADC(2) > convertADC(3)){ //Motion to the left
                PORTAbits.RA6 = 1;  //Left
                PORTAbits.RA5 = 0;  //Right
                
              if(monitoring && (motion == 'P' || motion == 'D')){
                motion = 'E';
                sendUART(motion);  
                sendUART('\n');
              }
            }

            else if (convertADC(2) < convertADC(3)){ //motion to the right

                PORTAbits.RA6 = 0;  //Left
                PORTAbits.RA5 = 1;  //Right
                  if(monitoring && (motion == 'E' || motion == 'P')){
                    motion = 'D';
                    sendUART(motion);         
                    sendUART('\n');         

                  }
            }
        }
        else{ // NightLight
            PORTAbits.RA6 = 0;  //Left
            PORTAbits.RA5 = 0;  //Right
            
            if(monitoring && (motion == 'E' || motion == 'D')){
                motion = 'P';
                sendUART(motion);         
                sendUART('\n');
              }
         }
    
    
    } 
    else{ // StandBy = True
        PORTAbits.RA6 = 0;  //Left
        PORTAbits.RA5 = 0;  //Right
        
        
        if(standby){
            int c;
            PORTAbits.RA4 = 1;
            for(c = 0 ; c < 4000 ; c++){};
            PORTAbits.RA4 = 0;
            for(c = 0 ; c < 4000 ; c++){};
//
//            delay(100);
//            PORTBbits.RB4 = 0;  //Standby LED

        }
           if(monitoring && (motion == 'E' || motion == 'D')){
                motion = 'P';
                sendUART(motion);         
                sendUART('\n');
              }
//        if(heatAlarm(50)){
//            
//            PORTAbits.RA3 = 1; //Cooling down
//            
//        }
    }
}





int main(void)
{
	setBaudRate();
	SetupTransmitter();
	
	TRISDbits.TRISD6 = 1; // input
	TRISBbits.TRISB2 = 1;
	TRISBbits.TRISB3 = 1;
	
	TRISAbits.TRISA0 = 0; // OUTPUT
	TRISAbits.TRISA6 = 0; // Direita
	TRISAbits.TRISA5 = 0; // Esquerda
	TRISAbits.TRISA4 = 0; //StandBy
	TRISAbits.TRISA3 = 0; //Heat Alarm

	checkLeds();

	return 0;


}


























/*
	while ( 1 ){
		if ( !PORTDbits.RD6 ){
			PORTAbits.RA0 = 1;
			for( i = 0 ; i < 20000 ; i++){};
			PORTAbits.RA0 = 0;
			for( i = 0 ; i < 20000 ; i++){};
		}
	
	}*/
	
/*	

	while ( 1 ){
	
		if ( !PORTDbits.RD6 ){
			PORTAbits.RA5 = 1;
			for( i = 0 ; i < 20000 ; i++){};
			PORTAbits.RA5 = 0;
			for( i = 0 ; i < 20000 ; i++){};
		}
		if ( !PORTDbits.RD6 ){
			PORTAbits.RA6 = 1;
			for( i = 0 ; i < 20000 ; i++){};
			PORTAbits.RA6 = 0;
			for( i = 0 ; i < 20000 ; i++){};
		}
		
		
			
	//	print(PORTBbits.RB2);
	//	print(PORTBbits.RB3);
	
	}
	*/


	


