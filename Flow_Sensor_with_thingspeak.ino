#include <SoftwareSerial.h>
#define RX 10
#define TX 11

String AP = "SHOCKWAVE";   
String PASS = "12345679"; 
String API = "I0QBDI84TYD7J377";
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

int flowPin = 2;
double flowRate;
double volume;
volatile int count;
unsigned long timeelapsed = 0;

SoftwareSerial esp8266(RX,TX); 
 
  
void setup() {
    Serial.begin(9600);
    pinMode(flowPin, INPUT);         
    attachInterrupt(0, Flow, RISING);
    esp8266.begin(115200);
    sendCommand("AT",5,"OK");
    sendCommand("AT+CWMODE=0",5,"OK");
    sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
    
}

void loop() {
    Communication();
}


void sendCommand(String command, int maxTime, char readReplay[]) {
      Serial.print(countTrueCommand);
      Serial.print(". at command => ");
      Serial.print(command);
      Serial.print(" ");
      while(countTimeCommand < (maxTime*1))
      {
        esp8266.println(command);//at+cipsend
        if(esp8266.find(readReplay))//ok
        {
          found = true;
          break;
        }
      
        countTimeCommand++;
      }
      
      if(found == true)
      {
        Serial.println("OYI");
        countTrueCommand++;
        countTimeCommand = 0;
      }
      
      if(found == false)
      {
        Serial.println("Fail");
        countTrueCommand = 0;
        countTimeCommand = 0;
      }
      
      found = false;
 }

 void Communication(){
      count = 0;      
      interrupts();  
      timeelapsed = millis();
        
      
       String getData = "GET /update?api_key="+ API +"&field1="+String(flowRate)+"&field2="+String(volume);
       sendCommand("AT+CIPMUX=1",5,"OK");
       sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
       sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
       esp8266.println(getData);
       delay(1000);
       countTrueCommand++;
       sendCommand("AT+CIPCLOSE=0",5,"OK");
       delay (15000);
      noInterrupts(); 
      flowRate = (count / (7.75*15));
      volume = flowRate * (timeelapsed/60000);
      Serial.println(String(flowRate)+"L/min");
      Serial.println(String(volume)+"Liters");
     
}

void Flow()
{
   count++; 
}


   
