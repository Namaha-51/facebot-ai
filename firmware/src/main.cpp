#include <MD_MAX72xx.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <ESP32Servo.h>

#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 1
#define DATA_PIN 23
#define CS_PIN 5
#define CLK_PIN 18
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

#define BUZZER_PIN 33
Servo headServo;
#define SERVO_PIN 32

String currentEmotion = "neutral";
unsigned long lastInteractionTime = 0;
unsigned long lastBlinkTime = 0;
int blinkInterval = 3000; 

int currentRobotState = 0; // 0 = Active, 1 = Restless, 2 = Bored, 3 = Sleeping

const unsigned long RESTLESS_TIMEOUT = 15000; // 15 sec test
const unsigned long BORED_TIMEOUT = 45000;    // 45 sec test
const unsigned long SLEEP_TIMEOUT = 90000;    // 90 sec test

void drawPixel(int x, int y) { mx.setPoint(x, 7 - y, true); }
void servoCenter() { headServo.write(90); }
void happyMove() { headServo.write(70); delay(200); headServo.write(110); delay(200); servoCenter(); }
void angryMove() { headServo.write(50); delay(120); headServo.write(130); delay(120); servoCenter(); }
void sleepyMove() { headServo.write(140); delay(600); }
void talkingMove() { headServo.write(80); delay(80); headServo.write(100); delay(80); }

void startupSound() { tone(BUZZER_PIN, 900, 150); delay(200); tone(BUZZER_PIN, 1200, 150); delay(200); tone(BUZZER_PIN, 1500, 200); }
void happySound() { tone(BUZZER_PIN, 1200, 120); delay(120); tone(BUZZER_PIN, 1500, 120); }
void angrySound() { tone(BUZZER_PIN, 400, 200); delay(150); tone(BUZZER_PIN, 300, 200); }
void sleepySound() { tone(BUZZER_PIN, 400, 400); }
void thinkingSound() { tone(BUZZER_PIN, 900, 100); delay(120); tone(BUZZER_PIN, 1000, 100); }
void wakeUpSound() { tone(BUZZER_PIN, 800, 100); delay(100); tone(BUZZER_PIN, 1200, 100); delay(100); tone(BUZZER_PIN, 1600, 200); }

void happyFace() { mx.clear(); drawPixel(1,2); drawPixel(1,3); drawPixel(6,2); drawPixel(6,3); drawPixel(2,6); drawPixel(3,7); drawPixel(4,7); drawPixel(5,6); mx.update(); }
void angryFace() { mx.clear(); drawPixel(1,1); drawPixel(2,2); drawPixel(5,2); drawPixel(6,1); drawPixel(2,6); drawPixel(3,6); drawPixel(4,6); drawPixel(5,6); mx.update(); }
void sleepyFace() { mx.clear(); drawPixel(1,2); drawPixel(2,2); drawPixel(5,2); drawPixel(6,2); drawPixel(3,6); drawPixel(4,6); mx.update(); }
void neutralFace() { mx.clear(); drawPixel(1,2); drawPixel(2,2); drawPixel(5,2); drawPixel(6,2); drawPixel(3,6); drawPixel(4,6); mx.update(); }
void sadFace() { mx.clear(); drawPixel(1,2); drawPixel(2,1); drawPixel(5,1); drawPixel(6,2); drawPixel(2,7); drawPixel(3,6); drawPixel(4,6); drawPixel(5,7); mx.update(); }
void surpriseFace() { mx.clear(); drawPixel(1,1); drawPixel(1,2); drawPixel(2,1); drawPixel(2,2); drawPixel(5,1); drawPixel(5,2); drawPixel(6,1); drawPixel(6,2); drawPixel(3,6); drawPixel(3,7); drawPixel(4,6); drawPixel(4,7); mx.update(); }
void blinkFace() { mx.clear(); drawPixel(1,2); drawPixel(2,2); drawPixel(5,2); drawPixel(6,2); mx.update(); }

void lookLeftFace() { mx.clear(); drawPixel(0,2); drawPixel(1,2); drawPixel(4,2); drawPixel(5,2); drawPixel(3,6); drawPixel(4,6); mx.update(); }
void lookRightFace() { mx.clear(); drawPixel(2,2); drawPixel(3,2); drawPixel(6,2); drawPixel(7,2); drawPixel(3,6); drawPixel(4,6); mx.update(); }

void talkingFace1() { mx.clear(); drawPixel(1,2); drawPixel(1,3); drawPixel(6,2); drawPixel(6,3); drawPixel(3,5); drawPixel(4,5); mx.update(); }
void talkingFace2() { mx.clear(); drawPixel(1,1); drawPixel(1,2); drawPixel(6,1); drawPixel(6,2); drawPixel(3,6); drawPixel(4,6); mx.update(); }

void showEmotion() {
  if(currentEmotion == "happy") { happyFace(); }
  else if(currentEmotion == "angry") { angryFace(); }
  else if(currentEmotion == "sleep") { sleepyFace(); }
  else if(currentEmotion == "sad") { sadFace(); }
  else if(currentEmotion == "surprise") { surpriseFace(); }
  else { neutralFace(); }
}

void wakeUpRobot() {
  if (currentRobotState == 3) {
    lcd.display();
    headServo.attach(SERVO_PIN);
    servoCenter();
    wakeUpSound();
  }
  currentRobotState = 0;
  lastInteractionTime = millis();
}

void robotSpeak(String text) {
  lcd.clear();
  String line1 = text.substring(0, min(16, (int)text.length()));
  String line2 = "";
  if(text.length() > 16) { line2 = text.substring(16, min(32, (int)text.length())); }

  lcd.setCursor(0,0);
  for(int i = 0; i < line1.length(); i++) {
    lcd.print(line1[i]);
    if(i % 2 == 0) { talkingFace1(); } else { talkingFace2(); }
    talkingMove();
    tone(BUZZER_PIN, 1200, 120);
    delay(140);
  }

  lcd.setCursor(0,1);
  for(int i = 0; i < line2.length(); i++) {
    lcd.print(line2[i]);
    if(i % 2 == 0) { talkingFace2(); } else { talkingFace1(); }
    talkingMove();
    tone(BUZZER_PIN, 1000, 120);
    delay(140);
  }

  delay(1000);
  showEmotion();
  servoCenter();
}

void setup() {
  Serial.begin(115200);
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 3);
  lcd.begin(16,2);
  pinMode(BUZZER_PIN, OUTPUT);
  headServo.attach(SERVO_PIN);
  
  servoCenter();
  currentEmotion = "happy";
  showEmotion();
  startupSound();
  robotSpeak("SYSTEM ONLINE");
  lastInteractionTime = millis();
}

void loop() {
  unsigned long timeSinceLastMsg = millis() - lastInteractionTime;

  if (currentRobotState != 3) {
    if (timeSinceLastMsg > SLEEP_TIMEOUT && currentRobotState != 3) {
      currentRobotState = 3;
      currentEmotion = "sleep";
      sleepyFace();
      sleepySound();
      lcd.noDisplay();
      headServo.detach();
    } 
    else if (timeSinceLastMsg > BORED_TIMEOUT && currentRobotState < 2) {
      currentRobotState = 2;
      currentEmotion = "sad";
      sadFace();
      sleepySound();
      headServo.write(130);
    } 
    else if (timeSinceLastMsg > RESTLESS_TIMEOUT && currentRobotState < 1) {
      currentRobotState = 1;
    }
  }

  if (currentRobotState == 0) {
    if (millis() - lastBlinkTime > blinkInterval) {
      blinkFace(); delay(150); showEmotion();
      lastBlinkTime = millis();
      blinkInterval = random(2000, 5000);
    }
  } 
  else if (currentRobotState == 1) {
    if (millis() - lastBlinkTime > blinkInterval) {
      int choice = random(0, 3);
      if (choice == 0) lookLeftFace();
      else if (choice == 1) lookRightFace();
      else blinkFace();
      
      delay(300);
      neutralFace();
      
      lastBlinkTime = millis();
      blinkInterval = random(1500, 4000);
    }
  }
  else if (currentRobotState == 2) {
    if (millis() - lastBlinkTime > 6000) {
      blinkFace(); delay(200); sadFace();
      lastBlinkTime = millis();
    }
  }

  if(Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    
    wakeUpRobot();
    
    int colonIndex = msg.indexOf(':');

    if (colonIndex > 0) {
      String emotionCommand = msg.substring(0, colonIndex);
      String spokenText = msg.substring(colonIndex + 1);
      
      emotionCommand.trim();
      emotionCommand.toUpperCase();
      spokenText.trim();

      if (emotionCommand == "HAPPY") { currentEmotion = "happy"; happySound(); happyMove(); } 
      else if (emotionCommand == "ANGRY") { currentEmotion = "angry"; angrySound(); angryMove(); } 
      else if (emotionCommand == "SLEEP") { currentEmotion = "sleep"; sleepySound(); sleepyMove(); } 
      else if (emotionCommand == "SAD") { currentEmotion = "sad"; sleepySound(); } 
      else if (emotionCommand == "SURPRISE") { currentEmotion = "surprise"; thinkingSound(); } 
      else { currentEmotion = "neutral"; }

      robotSpeak(spokenText);
      
    } else {
      currentEmotion = "neutral";
      thinkingSound();
      robotSpeak(msg);
    }
    
    lastBlinkTime = millis();
  }
}
