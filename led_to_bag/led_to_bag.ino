#include "state.h"

#define L1 8 // Digital -- Position 1
#define L2 7 // Digital -- Position 2
#define L3 4  // Digital -- Position 3
#define LB 5 // PWM -- Bag
#define LR 12 // PWM -- GameOver
#define TS 2 // Digital -- Start Button
#define TD 3 // Digital -- Move Button 
#define POT A0 // Analogic - Difficulty Selector
#define WELC_DELAY 800
#define START_DELAY 200
#define LB_DELAY 7
#define LR_DELAY 2000
#define TIME_DECREASER 8
#define DEBOUNCE_DELAY 50
#define FADE_AMOUNT 5
#define MAX_DIFF_LEVEL 8
#define MIN_DIFF_LEVEL 2

// Handler initialization
void (*handlers[]) () = {welcome, start, ingame, win, lose};
state currentState;
// Variables
long randNum;
int leds[] = {L1, L2, L3};
int currentLedPos, currentIntensity, fadeAmount, score, buttonState;
// Debounce's attributes
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
// Timer's attributes
unsigned long timer, maxTime;

void setup() {
  Serial.begin(9600);
  // Pin setup
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(LR, OUTPUT);
  pinMode(TD, INPUT); 
  // Interrupt
  attachInterrupt(digitalPinToInterrupt(TS), startBtnPressed, RISING);
  // Variables initialization
  lastButtonState = LOW;
  currentState = WELCOME;
  currentIntensity = 0;
  currentLedPos = -1;
  fadeAmount = FADE_AMOUNT;
  // Random initialization
  randomSeed(analogRead(0));
}

void loop() {
  // Runs the function related to the current game state.
  (*handlers[currentState])();
}
// Interrupt handler
void startBtnPressed() {
  currentState = START;
}

void welcome() {
  digitalWrite(L3, LOW);
  digitalWrite(L1, HIGH);
  delay(WELC_DELAY);
  digitalWrite(L1, LOW);
  digitalWrite(L3, HIGH);
  delay(WELC_DELAY);
  Serial.println("Welcome to Led to Bag. Press Key TS to Start");
}

void start() {
  resetGame();
  currentState = INGAME;
  delay(START_DELAY);
  Serial.println("Go!");
}

void ingame() {
  if (currentLedPos < 0) {
    currentLedPos = random(0, 3);
    timer = millis();
  }
  digitalWrite(leds[currentLedPos], HIGH);
  if (isPressed(TD) == HIGH && currentLedPos >= 0) {
    digitalWrite(leds[currentLedPos--], LOW);
    timer = millis();
  } 
  if (currentLedPos < 0) {
    currentState = WIN;
    score++;
    Serial.println(String("Another object in the bag! Count ") + score + String(" objects!"));
  }
  isTimeOver();
}

void win() {
  delay(LB_DELAY);
  currentIntensity += fadeAmount;
  if (currentIntensity == 255 || currentIntensity == 0) {
    fadeAmount = -fadeAmount;
  }
  if (currentIntensity == 0) {
    currentState = INGAME;
    maxTime -= maxTime / TIME_DECREASER;
  }
  if (isPressed(TD) == HIGH) {
    currentIntensity = 0;
    currentState = LOSE;
  }
  analogWrite(LB, currentIntensity);
}

void lose() {
  digitalWrite(LR, HIGH);    
  Serial.println(String("Game Over - Score: ") + score);
  delay(LR_DELAY);
  digitalWrite(LR, LOW);
  currentState = START;
}

int isPressed(int buttonPin) {
  int result = LOW;
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        result = HIGH;
      }
    }
  }
  lastButtonState = reading;
  return result;
}

void isTimeOver() {
  if(millis() - timer >= maxTime) {
    digitalWrite(leds[currentLedPos], LOW);
    currentState = LOSE;
  }
}

void setDifficulty() {
  maxTime = map(analogRead(POT), 0, 1023, MIN_DIFF_LEVEL, MAX_DIFF_LEVEL) * 1000;
}

void resetGame() {
  setDifficulty();
  digitalWrite(L1, LOW);
  digitalWrite(L2, LOW);
  digitalWrite(L3, LOW);
  analogWrite(LR, LOW);
  analogWrite(LB, LOW);
  fadeAmount = FADE_AMOUNT;
  currentIntensity = 0;
  currentLedPos = -1;
  score = 0;
}
