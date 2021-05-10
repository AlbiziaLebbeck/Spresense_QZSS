#include "AIS_BC95_API.h"

String address = "122.155.170.120";    // Your Server IP
String desport = "4533";    // Your Server Port
String payload = "Hello World";
String data_return;
unsigned long previousMillis = 0;
const long interval = 20000;  //millisecond
long cnt=0;

AIS_BC95_API nb;

void setup() {
    Serial.begin(115200);
    nb.begin(address,desport);
    Serial.print("Device IP : ");
    Serial.println(nb.getDeviceIP());
    Serial.print(F(">>IMSI   : "));
    Serial.println(nb.getIMSI());
    Serial.print(F(">>ICCID  : "));
    Serial.println(nb.getICCID());
    Serial.print(F(">>IMEI   : "));
    Serial.println(nb.getIMEI());
    Serial.print(F(">>Signal : "));
    Serial.print(nb.getSignal()); 
    Serial.println(F(" dBm")); 
    nb.pingIP(address);  
    previousMillis = millis();
}

void loop() {
    data_return="";
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
        {
        cnt++;     
            
        // Send data in String 
        nb.sendMsgSTR(address,desport,payload+String(cnt));
        
        //Send data in HexString     
        //nb.sendMsgHEX(address,desport,payload);
        previousMillis = currentMillis;
    
        }
        
    nb.waitResponse(data_return,address);
    
    if(data_return!=""){
        Serial.println("# Receive : "+data_return);   
    }
}