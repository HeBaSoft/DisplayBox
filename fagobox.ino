#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "ESP8266.h"
#include <ctype.h>
#include "pitches.h"

#define SSID "ssid"
#define PASSWORD "password"

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
SoftwareSerial esp(9, 10);
ESP8266 wifi(esp, 9600);

String points = "";

void setup() {
  lcd.begin(16, 2);
  lcd.print("Initializing...");
  
  Serial.begin(9600);
  
  wifi.setOprToStationSoftAP();
  wifi.disableMUX();

  if (!wifi.joinAP(SSID, PASSWORD)) {
    lcd.clear();
    show("Network Error!", 0);
    while(true);
  }
}

void loop() {
  if (!wifi.createTCP("api.filipsi.net", 80)) {
    lcd.clear();
    show("Network Error!", 0);
    return;
  }
  
  char* request =  "GET /faggotpoints/jemmotar HTTP/1.1\r\nHost: api.filipsi.net\r\nConnection: close\r\n\r\n";
  
  if (!wifi.sendSingle(request)) {
    lcd.clear();
    show("Network Error!", 0);
    return;
  }

  uint8_t buf[300] = {0};
  wifi.recvSingle(buf, 300);

  String result = String((char*)buf);
  result.trim();

  int cstart = result.indexOf("\r\n\r\n") + 4;
  int cend = result.indexOf("\n", cstart);
  int contentLength = result.substring(cstart, cend).toInt();
  result = result.substring(cend + 1, result.indexOf("\n", cend + 1) - 1);

  if(result.length() > 16 || !isValidNumber(result))
    return;

  if(points != result) {
    lcd.clear();
    show("jemmotar", 0);
    show(result, 1);

    if(points != "")
      playSound();
      
    points = result;
  }

  delay(5000);
}

void show(String msg, byte line) {
  Serial.println(msg);
  lcd.setCursor(16 / 2 - msg.length() / 2, line);
  lcd.print(msg);
}

void playSound() {
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(6, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(8);
  }
}

boolean isValidNumber(String str){
   for(byte i=0;i<str.length();i++) {
      if(isDigit(str.charAt(i))) return true;
   }
   return false;
} 

