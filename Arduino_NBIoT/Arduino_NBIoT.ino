#include "AIS_BC95_API.h"

String address = "206.189.151.251";    // Your Server IP
String desport = "4333";    // Your Server Port
String payload = "";//"Hello World";
String data_return;
unsigned long previousMillis = 0;
const long interval = 20000;  //millisecond
long cnt=0;
bool isSend = false;

AIS_BC95_API nb;

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);
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

    while (Serial1.available()) {
        //delay(5);  //delay to allow buffer to fill 
        if (Serial1.available() >0) {
            char c = Serial1.read();  //gets one byte from serial buffer
            
            if (c == '{') {
                payload = ""; //makes the string readString
            }
            
            payload += c; //makes the string readString
            
            if (c == '}') {
                isSend = true;
                break;
            }
        } 
    }

    data_return="";
    //unsigned long currentMillis = millis();
    //if (currentMillis - previousMillis >= interval) {
    if (isSend) {
        isSend = false;
        cnt++;     
            
        // Send data in String 
        Serial.println(payload);
        nb.sendMsgSTR(address,desport,payload);
        
        //Send data in HexString     
        //nb.sendMsgHEX(address,desport,payload);
        //previousMillis = currentMillis;
        payload = "";
    
    }
        
    nb.waitResponse(data_return,address);
    
    if(data_return!=""){
        Serial.println("# Receive : "+data_return);   
    }
}
