#include <ArduinoJson.h>

 int lux_value_cfg = 15;
int lux_value = 0;


void setup(){
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(9,OUTPUT);
  
  pinMode(6,OUTPUT);

  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  
  Serial.begin(9600);
  
  //Variables
  
 

  
  //Enable Pin
  digitalWrite(12, HIGH);
}

void loop(){
  
  //-------------------Comunication----------------------------
  //Reveive Data from terminal  
/*
  if (Serial.available() > 0){
  
  }
  
  //Send Data to terminal
void sendLdrValues(int ldr1, int ldr2){
    Serial.print("{\"C\":8,\"S1\":");
  Serial.print(ldr1);
    Serial.print(",\"S2\":");
    Serial.print(ldr2);
    Serial.print("}");
} */ 
  //--------------------Control--------------------------------
  
  /*
  digitalWrite(12, HIGH);
  digitalWrite(6,HIGH); //direita
  digitalWrite(9,LOW); //esquerda
  
  
  
  delay(1000);
 
  digitalWrite(6,LOW); //direita
  digitalWrite(9,HIGH); //esquerda
  
  delay(1000);
  
  */
  
  //TODO mudar a resistencia do fotovoltaico   
  //para fazer variar os valores lidos em A0 e A1 
 
  
  
  Serial.print(analogRead(A0)); //1-310
  Serial.print(":"); //1-310
  Serial.print(analogRead(A1)); //1-310
  Serial.println(" "); //1-310
  
 lux_value = abs( analogRead(A0) - analogRead(A1));
  if( lux_value < lux_value_cfg){ //change this value accordingly
    digitalWrite(6,LOW); //direita
    digitalWrite(9,LOW); //esquerda           
  } 
  
  else if(analogRead(A0) > analogRead(A1)){
    digitalWrite(6,LOW);  //direita
    digitalWrite(9,HIGH); //esquerda
  }
     
  else if (analogRead(A0)< analogRead(A1)){ //andar para a dir
       digitalWrite(6,HIGH); //direita
       digitalWrite(9,LOW); //esquerda
  }
           
}
  
