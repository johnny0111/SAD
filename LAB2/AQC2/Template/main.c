#include <p24fxxxx.h>
#include <stdio.h>
#include <string.h>

// Configuration Bits
#ifdef __PIC24FJ64GA004__ //Defined by MPLAB when using 24FJ64GA004 device
_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1 & IOL1WAY_ON) 
_CONFIG2( FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRI & I2C1SEL_SEC)
#else
_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2) 
_CONFIG2( FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRI)
#endif

void configureADC(int channel){
	AD1PCFG = 0xFFFE;      
	AD1CON1 = 0x2202;     
	AD1CON2 = 0;          
	AD1CON3 = 0x0100;      
	AD1CHS  = channel;           
	AD1CSSL = 0;                    
	IFS0bits.AD1IF = 0;
	IEC0bits.AD1IE  = 1;
	AD1CON1bits.ADON = 1;
	AD1CON1bits.SAMP = 1;
}

int convertADC(){
	int ADCValue;
	while (!IFS0bits.AD1IF){};
	ADCValue = ADC1BUF0;
	IFS0bits.AD1IF = 0;
	return ADCValue;
}

void __attribute__ ((__interrupt__)) _ADC1Interrupt(void)
{
	IFS0bits.AD1IF = 0;
}

int main(void)
{
	TRISDbits.TRISD6 = 1;
	TRISAbits.TRISA0 = 0;

	int i = 0;

	while ( 1 ){
		if ( !PORTDbits.RD6 ){
			PORTAbits.RA0 = 1;
			for( i = 0 ; i < 20000 ; i++){};
			PORTAbits.RA0 = 0;
			for( i = 0 ; i < 20000 ; i++){};
		}
	}
}
