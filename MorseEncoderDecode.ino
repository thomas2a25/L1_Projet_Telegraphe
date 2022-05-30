/*
 * Morse Code Encoder Decoder  Version finale
 */

#include <FastLED.h>
#include <SPI.h>
#include "LoRa.h"

#define PIN_BTN0        2
#define PIN_BTN1        3

#define LED_ON          HIGH
#define LED_OFF         LOW

#define BUZZER_ON       HIGH
#define BUZZER_OFF      LOW

#define BUTTON_PRESSED  LOW
#define BUTTON_RELEASED HIGH

#define INTER_CHARACTER_SPACE 3
#define INTRA_CHARACTER_SPACE 1
#define DOT_TIME              1
#define DASH_TIME             3
#define WORD_SPACE            7
#define ASCII_TO_MORSE_SPEEDUP  3

#define LEDS_PIN        4
#define NUM_LEDS        21

#define SS 10
#define RST 8
#define DI0 6
#define BAND 865E6

unsigned long morse_code_timing_1_unit = 300; // ms 
unsigned long inter_character_space_millis;

const int button_morseDot   = PIN_BTN0;
const int button_morseDash  = PIN_BTN1;
const int led_morseDot      = 12;
const int led_morseDash     = 13;
const int buzzer            = 5;

int txPower = 14; // from 0 to 20, default is 14
int spreadingFactor = 12; // from 7 to 12, default is 12
long signalBandwidth = 125E3; // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3,41.7E3,62.5E3,125E3,250E3,500e3, default is 125E3
int codingRateDenominator=5; // Numerator is 4, and denominator from 5 to 8, default is 5
int preambleLength=8; // from 2 to 20, default is 8

String morseCode[26] = {
  ".-",   // a
  "-...", // b
  "-.-.", // c
  "-..",  // d
  ".",    // e
  "..-.", // f
  "--.",  // g
  "....", // h
  "..",   // i
  ".---", // j
  "-.-",  // k
  ".-..", // l
  "--",   // m
  "-.",   // n
  "---",  // o
  ".--.", // p
  "--.-", // q
  ".-.",  // r
  "...",  // s
  "-",    // t
  "..-",  // u
  "...-", // v
  ".--",  // w
  "-..-", // x
  "-.--", // y
  "--..",  // z
  };

int ledState_morseDot;          // the current state of the output pin
int buttonState_morseDot;       // the current reading from the input pin
int lastButtonState_morseDot;   // the previous reading
int debounced_lastButtonState_morseDot;
int triggered_morseDot; 

int ledState_morseDash;         // the current state of the output pin
int buttonState_morseDash;      // the current reading from the input pin
int lastButtonState_morseDash;  // the previous reading
int debounced_lastButtonState_morseDash;
int triggered_morseDash; 

unsigned long lastDebounceTime_morseDot = 0;  // the last time the output pin was toggled
unsigned long lastDebounceTime_morseDash = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50;             // the debounce time; increase if the output flickers

String morse;
bool spaceCheck;
bool dataFromLoRa;
char charTriggeredByButton[20];
int charIndex;

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Encodeur/Decodeur de Morse utilisant LoRa Phy comme Emetteur/Recepteur");
  Serial.print("SetFrequency : ");
  Serial.print(BAND);
  Serial.println("Hz");
  Serial.print("SetSpreadingFactor : SF");
  Serial.println(spreadingFactor);
  
  SPI.begin();
  LoRa.setPins(SS,RST,DI0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Erreur de démarrage LoRa!, la carte va cesser de fonctionner");
    while (1);
  }

  LoRa.setTxPower(txPower,1);
  LoRa.setSpreadingFactor(spreadingFactor);
  LoRa.setSignalBandwidth(signalBandwidth);
  LoRa.setCodingRate4(codingRateDenominator);
  LoRa.setPreambleLength(preambleLength);
  
  pinMode(button_morseDot, INPUT_PULLUP);
  pinMode(button_morseDash, INPUT_PULLUP);
  pinMode(led_morseDot, OUTPUT);
  pinMode(led_morseDash, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lastButtonState_morseDot = digitalRead(button_morseDot);
  lastButtonState_morseDash = digitalRead(button_morseDash);

  FastLED.addLeds<WS2812, LEDS_PIN, RGB>(leds, NUM_LEDS);
}

void loop() {
  
  dotButtonHandler();
  dashButtonHandler();
  loraPacketCheck();
  
  if(triggered_morseDot == true){
    triggered_morseDot = false;
    morse.concat('.');
    Serial.print('.');
    turnOnLed(led_morseDot);
    turnOnBuzzer();
    if(dataFromLoRa){
      delay(morse_code_timing_1_unit * DOT_TIME / ASCII_TO_MORSE_SPEEDUP); 
    } else {
      delay(morse_code_timing_1_unit * DOT_TIME); 
    }
    turnOffLed(led_morseDot);
    turnOffBuzzer();
    inter_character_space_millis = millis();
    
  }
    
  if(triggered_morseDash == true){
    triggered_morseDash = false;
    morse.concat('-');
    Serial.print('-');
    turnOnLed(led_morseDash);
    turnOnBuzzer();
    if(dataFromLoRa){
      delay(morse_code_timing_1_unit * DASH_TIME / ASCII_TO_MORSE_SPEEDUP);
    } else {
      delay(morse_code_timing_1_unit * DASH_TIME);
    }
    turnOffLed(led_morseDash);
    turnOffBuzzer();
    inter_character_space_millis = millis();
  }

  unsigned long interCharacterSpace;
  if(dataFromLoRa){
    interCharacterSpace = morse_code_timing_1_unit * INTER_CHARACTER_SPACE;// / ASCII_TO_MORSE_SPEEDUP;
  } else {
    interCharacterSpace = morse_code_timing_1_unit * INTER_CHARACTER_SPACE;
  }
    
  if(millis() - inter_character_space_millis >= interCharacterSpace){
    if(morse != ""){
      bool exist = false;
      Serial.print(" ");
      for(int index=0; index < 26; index++){
        if(morse.equals(morseCode[index])){
          Serial.write(index+65);
          exist = true;
          spaceCheck = true;

          if(!dataFromLoRa){
            charTriggeredByButton[charIndex++] = index+65;
          } 
        }
      }
      if(exist)Serial.println();
      else Serial.println("code faux!");
    }
    morse = "";
  }

  unsigned long wordSpace;
  if(dataFromLoRa){
    wordSpace = morse_code_timing_1_unit * WORD_SPACE / (ASCII_TO_MORSE_SPEEDUP*0.65);
  } else {
    wordSpace = morse_code_timing_1_unit * WORD_SPACE;
  }

  if(millis() - inter_character_space_millis >= wordSpace){
    if(spaceCheck == true){
      spaceCheck = false;
      dataFromLoRa = false;
      
      if(charIndex > 0){
        for(int index=0; index < charIndex; index++){
          Serial.print(" envoi:");
          Serial.println(charTriggeredByButton[index]);
          sendCharUsingLora(charTriggeredByButton[index]);
        }
        memset(charTriggeredByButton, 0, charIndex);
        charIndex = 0;
      }
      Serial.println();  
    }
  }
  serialAscii2morse();
}

void dotButtonHandler(void){
  int reading_morseDot = digitalRead(button_morseDot);
  if (reading_morseDot != lastButtonState_morseDot) lastDebounceTime_morseDot = millis();

  if ((millis() - lastDebounceTime_morseDot) > debounceDelay) {
    if (reading_morseDot != buttonState_morseDot) {
      buttonState_morseDot = reading_morseDot;

      if (buttonState_morseDot == BUTTON_PRESSED && buttonState_morseDot != debounced_lastButtonState_morseDot){
        triggered_morseDot = true;
      }
      debounced_lastButtonState_morseDot = buttonState_morseDot;
    }
  }
  lastButtonState_morseDot = reading_morseDot;
}

void dashButtonHandler(void){
  int reading_morseDash = digitalRead(button_morseDash);
  if (reading_morseDash != lastButtonState_morseDash) lastDebounceTime_morseDash = millis();

  if ((millis() - lastDebounceTime_morseDash) > debounceDelay) {
    if (reading_morseDash != buttonState_morseDash) {
      buttonState_morseDash = reading_morseDash;

      if (buttonState_morseDash == BUTTON_PRESSED && buttonState_morseDash != debounced_lastButtonState_morseDot){
        triggered_morseDash = true;
      }
      debounced_lastButtonState_morseDot = buttonState_morseDash;
    }
  }
  lastButtonState_morseDash = reading_morseDash;
}

void turnOnLed(int _pin){
  //digitalWrite(_pin, LED_ON);
  if(_pin == led_morseDash) turnOnAllLeds_dash();
  else turnOnAllLeds_dot();
}

void turnOffLed(int _pin){
  //digitalWrite(_pin, LED_OFF);
  turnOffAllLeds();
}

void turnOnBuzzer(void){
  //digitalWrite(buzzer, BUZZER_ON);
  tone (buzzer, 900);
}

void turnOffBuzzer(void){
  //digitalWrite(buzzer, BUZZER_OFF);
  noTone (buzzer);
}

void serialAscii2morse(void){
  String receivedAscii = "";
  while(Serial.available() > 0){
    delay(100);
    char incomingChar = Serial.read();
    if(incomingChar == 32 || (incomingChar > 64 && incomingChar < 91) || (incomingChar > 96 && incomingChar < 123)){
      receivedAscii.concat(incomingChar);
    }
  }
  if(receivedAscii != ""){
    receivedAscii.toUpperCase();
    Serial.println(receivedAscii);
    for(int index = 0; index < receivedAscii.length(); index++){
      Serial.print(receivedAscii.charAt(index));
      Serial.print(" ");
      if(receivedAscii.charAt(index) == 32){
        Serial.println();
        delay(morse_code_timing_1_unit * WORD_SPACE / ASCII_TO_MORSE_SPEEDUP);
      } else {
        Serial.println(morseCode[receivedAscii.charAt(index)-65]);
        morseToBeep(morseCode[receivedAscii.charAt(index)-65]);
        delay(morse_code_timing_1_unit * INTER_CHARACTER_SPACE / ASCII_TO_MORSE_SPEEDUP);
      } 
    }
  }
}

void morseToBeep(String _morseCode){
  for(int index = 0; index < _morseCode.length(); index++){
    if(_morseCode.charAt(index) == '.'){
      loraSendPacket(".");
      turnOnLed(led_morseDot);
      turnOnBuzzer();
      delay(morse_code_timing_1_unit * DOT_TIME / ASCII_TO_MORSE_SPEEDUP); 
      turnOffLed(led_morseDot);
      turnOffBuzzer();
    } else {
      loraSendPacket("-");
      turnOnLed(led_morseDash);
      turnOnBuzzer();
      delay(morse_code_timing_1_unit * DASH_TIME / ASCII_TO_MORSE_SPEEDUP);
      turnOffLed(led_morseDash);
      turnOffBuzzer();      
    }
    // last dot or dash in a word still get intra character space
    delay(morse_code_timing_1_unit * INTRA_CHARACTER_SPACE / ASCII_TO_MORSE_SPEEDUP);
  }
}

void turnOnAllLeds_dash(void){
  leds[0] = CRGB(0, 0, 0);
  for(int index=1; index < NUM_LEDS; index++){
    leds[index] = CRGB(0, 50, 0); // Green, Red, Blue
  }
  FastLED.show();
}

void turnOnAllLeds_dot(void){
  leds[0] = CRGB(0, 0, 0);
  for(int index=1; index < NUM_LEDS; index++){
    leds[index] = CRGB(50, 0, 0); // Green, Red, Blue
  }
  FastLED.show();
}

void turnOffAllLeds(void){
  leds[0] = CRGB(0, 0, 0);
  for(int index=1; index < NUM_LEDS; index++){
    leds[index] = CRGB(0, 0, 0); // Green, Red, Blue
  }
  FastLED.show();  
}

void loraPacketCheck(void){
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      char dataReceived = LoRa.read();
      if(dataReceived == '.'){
        triggered_morseDot = true;
        dataFromLoRa = true;
      } else if (dataReceived == '-'){
        triggered_morseDash = true;
        dataFromLoRa = true;
      } else{
        Serial.print(dataReceived);
      }
    }
  }
}

void loraSendPacket(String _data){
  LoRa.beginPacket();
  LoRa.print(_data);  
  LoRa.endPacket();
}

void sendCharUsingLora(char _decimalValue){
  LoRa.beginPacket();
  LoRa.write(_decimalValue);  
  LoRa.endPacket();
}
