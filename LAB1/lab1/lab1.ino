#include<string.h>

//variables
int lux_value = 0;
int C = 0, B = 0, L = 0, n_aux = 0;
char motion='P'; //P:stop; E:left; D:right;
char c_aux, M;
String st;
bool monitoring = true, standby = false;

//variables for control
int low_light_value = 200;
int lux_value_cfg = 50;


int resetValue;

//FUNCTION {send LDR JSON to terminal}
void sendLdrValues(int ldr1, int ldr2){
  Serial.print("{\"C\":8,\"S1\":");
  Serial.print(ldr1);
  Serial.print(",\"S2\":");
  Serial.print(ldr2);
  Serial.print("}");
  Serial.print("\n");
} 

//FUNCTION {send motor state JSON to terminal}
void sendMotorState(char M){
  Serial.print("{\"C\":8,\"M\":");
  Serial.print(M);
  Serial.print("}");
  Serial.print("\n");
} 

//FUNCTION {send ACK JSON to terminal}
void SendACK(){
  Serial.println("{\"C\":0}");
}



//FUNCTION {toggle StandBy}
void standby_toggle(bool standby){

  if(standby){
      digitalWrite(12, LOW);      
      Serial.println("Standy Mode Activated");
    }
    else{
      digitalWrite(12, HIGH);
      Serial.println("Working Mode Activated");
   }
}
//FUNCTION {read JSON from terminal}
void readJSON(int c, char c_char, int c_char_value, int ldr1, int ldr2){
      switch(C){
      case 1: SendACK(); sendLdrValues(ldr1,ldr2); break;
      case 2: SendACK(); monitoring = true;  break;
      case 3: SendACK(); monitoring = false;  break;
      case 4: SendACK(); standby = false; standby_toggle(standby); break;
      case 5: SendACK(); standby = true; standby_toggle(standby); break;
      case 6: SendACK(); low_light_value = n_aux; break;
      case 7: SendACK(); lux_value_cfg = n_aux; break;
    }
}
void setup(){
  
    pinMode(12,OUTPUT);
    pinMode(11,OUTPUT);
    pinMode(10,OUTPUT);
    pinMode(9,OUTPUT);

    pinMode(6,OUTPUT);

    pinMode(A0,INPUT);
    pinMode(A1,INPUT);

    Serial.begin(9600);

    //Enable Pin
    digitalWrite(12, HIGH);
    standby = false;
  
    pinMode(2, INPUT_PULLUP);

  }

void loop(){
  //-------------------Comunication----------------------------
  //Reveive Data from terminal  
  
  if(Serial.available() > 0){
     st = Serial.readString();
     sscanf(st.c_str(), "{\"C\":%d,\"%c\":%d}", &C, &c_aux, &n_aux);
   readJSON(C,c_aux,n_aux,analogRead(A0),analogRead(A1)); 
  }
  //--------------------Control--------------------------------
  
  
  if(digitalRead(2) == 0){
    //Standby here, refers to the previous state,
    // and function argument asks 
    //for the requested state
    if(standby){ 
      standby = false;
    }
    else{
      standby = true;
    }  
    standby_toggle(standby);
    
    while(digitalRead(2) == 0){ //we want to toggle it once only
      delay(300);
        //Serial.println(digitalRead(2),DEC);
    }      
  }
  

  /*
  Serial.print(analogRead(A0)); //4-580
  Serial.print(":"); //1-310
  Serial.print(analogRead(A1)); //1-310
  Serial.println(" "); //1-310
  */
  
 lux_value = abs( analogRead(A0) - analogRead(A1));
  
 if(!standby){ 
   
    if(analogRead(A0) > low_light_value || analogRead(A1) > low_light_value )   
    {
        if( lux_value < lux_value_cfg){ //change lux_value_accordingly value accordingly
          digitalWrite(6,LOW); //direita
          digitalWrite(9,LOW); //esquerda
          digitalWrite(10,LOW);
          digitalWrite(11,LOW);
          if(monitoring && (motion == 'E' || motion == 'D')){
            motion = 'P';
            sendMotorState(motion);          
          }
        } 

        else if(analogRead(A0) > analogRead(A1)){
          digitalWrite(6,LOW);  //direita
          digitalWrite(9,HIGH); //esquerda
          digitalWrite(11,LOW);
          digitalWrite(10,HIGH);
          if(monitoring && (motion == 'P' || motion == 'D')){
            motion = 'E';
            sendMotorState(motion);         

          }
        }

        else if (analogRead(A0) < analogRead(A1)){ //andar para a dir
          digitalWrite(6,HIGH); //direita
          digitalWrite(9,LOW); //esquerda
          digitalWrite(11,HIGH);
          digitalWrite(10,LOW);
          if(monitoring && (motion == 'E' || motion == 'P')){
            motion = 'D';
            sendMotorState(motion);         

          }
        }
    }
    else{ // é de noite
      digitalWrite(6,LOW); //direita
        digitalWrite(9,LOW); //esquerda
        digitalWrite(10,LOW);
        digitalWrite(11,LOW);     
     }


   } 
   else{ // StandBy
        digitalWrite(6,LOW); //direita
        digitalWrite(9,LOW); //esquerda

         if(standby){
           digitalWrite(10,LOW);
           digitalWrite(11,LOW);
           delay(150); 
           digitalWrite(10,HIGH);
           digitalWrite(11,HIGH);
           delay(150);
         }
     }
}
   

  /*
  With a pull-up resistor, the input pin will read a high state 
  when the button is not pressed. In other words, a small amount
  of current is flowing between VCC and the input pin (not to ground),
  thus the input pin reads close to VCC. When the button is pressed, 
  it connects the input pin directly to ground. The current flows through 
  the resistor to ground, thus the input pin reads a low state. Keep in mind, 
  if the resistor wasn’t there, your button would connect VCC to ground,
  which is very bad and is also known as a short.
  
  https://www.tinkercad.com/things/4mpOvfR4N8Y-tremendous-densor/editel?tenant=circuits
  
  
  */
  
  
 

           
