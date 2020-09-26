#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 

#define gatePin 10
#define highPin A0
#define lowPin A1
#define batpluspin A2
#define ledPin 9

#define buttonUp 8
#define buttonDown 7
#define buttonOk 6

int interval = 3000; //Interval (ms) between measurements

boolean set = false;
float mAh = 0.0;
float voltRef = 5.05; // Reference voltage (probe your 5V pin)
float current = 0.0;
float loadInputVolt = 0.0;
float battVolt = 0.0;
float loadOutputvolt = 0.0;        
float battLow = 3;

unsigned long previousMillis = 0;
unsigned long millisPassed = 0;

void setup() {
    
  u8g2.begin();
  Serial.begin(9600);
  Serial.println("Battery Capacity Checker v1.1");
  
  pinMode(gatePin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonUp, INPUT);
  pinMode(buttonDown, INPUT);
  pinMode(buttonOk, INPUT); 
  
  digitalWrite(gatePin, LOW);
  digitalWrite(ledPin, LOW);

  u8g2.clearBuffer();
  u8g2.setFont(u8g_font_unifont); 
  u8g2.drawStr(26,15,"WELCOME");
  u8g2.drawStr(0,28,"SET CUTOFF VOLT");   
  u8g2.sendBuffer();
}

void loop() {

  while(!(set))
  {
  while(! ((digitalRead(buttonOk)) || (digitalRead(buttonDown)) ||(digitalRead(buttonUp))) );
  
  if(digitalRead(buttonDown))
  {
    while(digitalRead(buttonDown));
    battLow -= 0.5;
    u8g2.clearBuffer();
    u8g2.setFont(u8g_font_unifont); 
    u8g2.setCursor(0,28);
    u8g2.print(battLow);
    u8g2.sendBuffer();
  }
  else if(digitalRead(buttonUp))
  {
    while(digitalRead(buttonUp));
    battLow += 0.5;
    u8g2.clearBuffer();
    u8g2.setFont(u8g_font_unifont); 
    u8g2.setCursor(0,28);
    u8g2.print(battLow);
    u8g2.sendBuffer();
  }

  else if(digitalRead(buttonOk))
  {
    u8g2.clearBuffer();
    u8g2.setFont(u8g_font_unifont); 
    u8g2.setCursor(0,15);
    u8g2.print(battLow);
    u8g2.drawStr(0,25,"ACCEPTED");
    u8g2.sendBuffer();
    set = true;
  }
  if(battLow < 2.5)
  {
    battLow = 2.5;
    u8g2.clearBuffer();
    u8g2.setFont(u8g_font_unifont); 
    u8g2.drawStr(0,28,"NOT VALID V");
    u8g2.sendBuffer();
    
  }
 }
  
  loadInputVolt = analogRead(highPin) * voltRef / 1024.0;
  loadOutputvolt = analogRead(lowPin) * voltRef / 1024.0;
  battVolt = analogRead(batpluspin) * voltRef / 1024.0;
  
  if(battVolt <= 2.5)
    {
      low_bat();
    }
  
  else if(battVolt >= battLow)
    {
      discharge_bat();
    }
  
  else if(battVolt < battLow) //below 2.5-3.0 v cuz if battVolt lower than 2.5 prog. go in first if it'll say and say NO BAT
    {
      discharge_end();
    }

}

void low_bat(void)
{
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g_font_unifont); 
  u8g2.drawStr(8,25,"NO BATTERY");  
  u8g2.sendBuffer();         
  
  while(!(battVolt >= battLow)){
    battVolt = analogRead(batpluspin) * voltRef / 1024.0;
    delay(200);
  }
  u8g2.clearBuffer();
  u8g2.setFont(u8g_font_unifont); 
  u8g2.drawStr(8,29,"STARTING IN 3 SECOND");  
  u8g2.sendBuffer();
  delay(1000);
  u8g2.clearBuffer();
  u8g2.setFont(u8g_font_unifont); 
  u8g2.drawStr(8,29,"STARTING IN 2 SECOND");  
  u8g2.sendBuffer();
  delay(1000);
  u8g2.clearBuffer();
  u8g2.setFont(u8g_font_unifont); 
  u8g2.drawStr(8,29,"STARTING IN 1 SECOND");  
  u8g2.sendBuffer();
  delay(1000);
}

void discharge_bat(void){
      
      digitalWrite(gatePin, HIGH);
      millisPassed = millis() - previousMillis;
      current = (loadInputVolt - loadOutputvolt) / 3.75;
      mAh = mAh + (current * 1000.0) * (millisPassed / 3600000.0);
      previousMillis = millis();
      
      u8g2.clearBuffer();          
      u8g2.setFont(u8g_font_unifont);  
      u8g2.setCursor (0, 15);
      u8g2.print(mAh);
      u8g2.setCursor (0, 30);
      u8g2.print(battVolt);
      u8g2.drawStr(90,15, "mAH");
      u8g2.drawStr(90,30, "V");
      u8g2.sendBuffer(); 
      
      delay(interval);
}

void discharge_end(void){
      digitalWrite(gatePin, LOW);
    
      u8g2.clearBuffer();          
      u8g2.setFont(u8g_font_unifont);  
      u8g2.setCursor (0, 15);
      u8g2.print(mAh);
      u8g2.drawStr(90,30, "done");
      u8g2.sendBuffer(); 
      digitalWrite(ledPin, HIGH);
      while(1);
}
