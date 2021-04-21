//var
#include<string.h>

int C, B, L, count, n_aux = 0;
char c_aux;
//char st_aux[20];
//char st[20] = "hello";
String st;
//---------------------------------
/*void sendLdrValues(int ldr1){
    Serial.print("{\"C\":8,\"S1\":");
  Serial.print(ldr1);
    Serial.print("}");
}
*/
void setup()
{
  Serial.begin(9600);
}

void loop(){
  if(Serial.available() > 0){
     st = Serial.readString();
     count = sscanf(st.c_str(), "{\"C\":%d,\"%c\"%d}", &C, &c_aux, &n_aux);
     if (c_aux == 'B'){
        B = n_aux;
     }else if(c_aux == 'L'){
        L = n_aux;
     }
  }


  Serial.println(C);
  Serial.println(B);
  Serial.println(L);
  
}
