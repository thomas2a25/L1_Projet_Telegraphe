/*
 * Morse Code Encoder Decoder  V1.0
 * - Button to serial monitor (buzzer beep and led on when button pressed)
 * - Serial monitor to buzzer (led on when dot or dash)
 */

/* MORSE CODE TIMING, here is the information about timing for you.
 * Dot: 1 unit
 * Dash: 3 units
 * Intra-character space (the gap between dits and dahs within a character): 1 unit
 * Inter-character space (the gap between the characters of a word): 3 units
 * Word space (the gap between two words): 7 units
 * 
 * reference: https://morsecode.world/international/timing.html
 */
#include <FastLED.h>

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

unsigned long morse_code_timing_1_unit = 300; // ms 
unsigned long inter_character_space_millis;

const int button_morseDot   = PIN_BTN0;
const int button_morseDash  = PIN_BTN1;
const int led_morseDot      = 12;
const int led_morseDash     = 13;
const int buzzer            = 11;

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

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  
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

  if(triggered_morseDot == true){
    triggered_morseDot = false;
    morse.concat('.');
    Serial.print('.');
    turnOnLed(led_morseDot);
    turnOnBuzzer();
    delay(morse_code_timing_1_unit * DOT_TIME); 
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
    delay(morse_code_timing_1_unit * DASH_TIME);
    turnOffLed(led_morseDash);
    turnOffBuzzer();
    inter_character_space_millis = millis();
  }
    
  if(millis() - inter_character_space_millis >= morse_code_timing_1_unit * INTER_CHARACTER_SPACE){
    if(morse != ""){
      bool exist = false;
      Serial.print(" ");
      for(int index=0; index < 26; index++){
        if(morse.equals(morseCode[index])){
          Serial.write(index+65);
          exist = true;
          spaceCheck = true;
        }
      }
      if(exist)Serial.println();
      else Serial.println("wrong code!");
    }
    morse = "";
  }

  if(millis() - inter_character_space_millis >= morse_code_timing_1_unit * WORD_SPACE){
    if(spaceCheck == true){
      spaceCheck = false;
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
  digitalWrite(buzzer, BUZZER_ON);
}

void turnOffBuzzer(void){
  digitalWrite(buzzer, BUZZER_OFF);
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
      turnOnLed(led_morseDot);
      turnOnBuzzer();
      delay(morse_code_timing_1_unit * DOT_TIME / ASCII_TO_MORSE_SPEEDUP); 
      turnOffLed(led_morseDot);
      turnOffBuzzer();
    } else {
      turnOnLed(led_morseDash);
      turnOnBuzzer();
      delay(morse_code_timing_1_unit * DASH_TIME / ASCII_TO_MORSE_SPEEDUP);
      turnOffLed(led_morseDash);
      turnOffBuzzer();      
    }
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
