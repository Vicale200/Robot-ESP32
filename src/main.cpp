#include <Arduino.h>
#include "idle_eye_emotion.h"  // Frames en PROGMEM
#include "idle_to_angry_emotion.h"  // Frames en PROGMEM
#include "angry_eye_emotion.h"  // Frames en PROGMEM
#include "idle_to_happy_emotion.h"  // Frames en PROGMEM
#include "happy_eye_emotion.h"  // Frames en PROGMEM
//#include "idle_to_sad.h"  // Frames en PROGMEM
//#include "sad_eye_emotion.h"  // Frames en PROGMEM
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

int totalFrames = 14;
const int idleTime = 2500;
const int frameDelay = 120;
uint32_t previousMillis = 0;
int currentFrame = 0;
bool isIdle = true;
int emotionID = 0;
int prevEmotionID;
int countDelay;

const int frameWidth = 160;
const int frameHeight = 85;
const int screenWidth = 320;
const int screenHeight = 170;
uint16_t lineBuffer[screenWidth];

enum STATE {
  TRANSITION,
  ANIMATION
};

STATE currentState;

static const uint16_t* idleFrames[] = {
  idle_001, idle_002, idle_003, idle_004, idle_005, idle_006, idle_007,
  idle_008, idle_009, idle_010, idle_011, idle_012, idle_013, idle_014
};

static const uint16_t* idleToAngryFrames[] = {
  idle_to_angry_001, idle_to_angry_002, idle_to_angry_003, idle_to_angry_004, idle_to_angry_005,
  idle_to_angry_006, idle_to_angry_007, idle_to_angry_008, idle_to_angry_009, idle_to_angry_010
};

static const uint16_t* angryFrames[] = {
  angry_001, angry_002, angry_003, angry_004,
  angry_005, angry_006, angry_007, angry_008
};

static const uint16_t* idleToHappyFrames[] = {
  idle_to_happy_001, idle_to_happy_002, idle_to_happy_003, idle_to_happy_004,
  idle_to_happy_005, idle_to_happy_006, idle_to_happy_007
};

static const uint16_t* happyFrames[] = {
  happy_001, happy_002, happy_003, happy_004, happy_005, 
  happy_006, happy_007, happy_008, happy_009, happy_010
};

//static const uint16_t* idleToSadFrames[] = {
  //idle_to_sad_001, idle_to_sad_002, idle_to_sad_003, idle_to_sad_004, idle_to_sad_005, idle_to_sad_006,
  //idle_to_sad_007, idle_to_sad_008, idle_to_sad_009, idle_to_sad_010, idle_to_sad_011, idle_to_sad_012
//};

//static const uint16_t* sadFrames[] = {
  //sad_001, sad_002, sad_003, sad_004, sad_005,
  //sad_006, sad_007, sad_008, sad_009, sad_010
//};

void drawScaledFrame(const uint16_t* frame) {
  for (int y = 0; y < frameHeight; y++) {
    for (int x = 0; x < frameWidth; x++) {
      uint16_t color = pgm_read_word(&frame[y * frameWidth + x]);
      
      int sx = x * 2;
      lineBuffer[sx] = color;
      lineBuffer[sx + 1] = color;
    }
    tft.pushImage(0, y * 2, screenWidth, 1, lineBuffer);
    tft.pushImage(0, y * 2 + 1, screenWidth, 1, lineBuffer);
  }
}

enum EmotionStage {
  EM_IDLE,
  TRANSITION_TO_ANGRY,
  EM_ANGRY,
  TRANSITION_TO_HAPPY,
  EM_HAPPY,
  TRANSITION_TO_IDLE, 
};

EmotionStage emotionStage = EM_IDLE;
int blinkRepeatCount = 3;    
int blinkCounter = 0;     

void setup() {
  Serial.begin(9600);
  Serial.print("ST7789 TFT Animación de Pestañeo");
  prevEmotionID = -1;
  tft.begin();
  tft.setSwapBytes(true);
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  currentState = ANIMATION;
  drawScaledFrame(happyFrames[0]); 
   
  previousMillis = millis();
}

void loop() {
  uint32_t currentMillis = millis();

  switch (emotionStage) {

    case EM_IDLE:
      totalFrames = 14;
      if (!isIdle && currentMillis - previousMillis >= frameDelay) {
        drawScaledFrame(idleFrames[currentFrame++]);
        previousMillis = currentMillis;
        if (currentFrame >= totalFrames) {
          currentFrame = 0;
          blinkCounter++;
          isIdle = true;
          if (blinkCounter >= blinkRepeatCount) {
            blinkCounter = 0;
            emotionStage = TRANSITION_TO_ANGRY;
          }
        }
      } else if (isIdle && currentMillis - previousMillis >= idleTime) {
        isIdle = false;
        previousMillis = currentMillis;
      }
      break;

    case TRANSITION_TO_ANGRY:
      totalFrames = 10;
      if (currentMillis - previousMillis >= frameDelay) {
        drawScaledFrame(idleToAngryFrames[currentFrame++]);
        previousMillis = currentMillis;
        if (currentFrame >= totalFrames) {
          currentFrame = 0;
          emotionStage = EM_ANGRY;
          isIdle = true;
          previousMillis = currentMillis;
        }
      }
      break;

    case EM_ANGRY:
      totalFrames = 8;
      if (!isIdle && currentMillis - previousMillis >= frameDelay) {
        drawScaledFrame(angryFrames[currentFrame++]);
        previousMillis = currentMillis;
        if (currentFrame >= totalFrames) {
          currentFrame = 0;
          blinkCounter++;
          isIdle = true;
          if (blinkCounter >= blinkRepeatCount) {
            blinkCounter = 0;
            emotionStage = TRANSITION_TO_HAPPY;
          }
        }
      } else if (isIdle && currentMillis - previousMillis >= idleTime) {
        isIdle = false;
        previousMillis = currentMillis;
      }
      break;

    case TRANSITION_TO_HAPPY:
      totalFrames = 7;
      if (currentMillis - previousMillis >= frameDelay) {
        drawScaledFrame(idleToHappyFrames[currentFrame++]);
        previousMillis = currentMillis;
        if (currentFrame >= totalFrames) {
          currentFrame = 0;
          emotionStage = EM_HAPPY;
          isIdle = true;
          previousMillis = currentMillis;
        }
      }
      break;

    case EM_HAPPY:
      totalFrames = 10;
      if (!isIdle && currentMillis - previousMillis >= frameDelay) {
        drawScaledFrame(happyFrames[currentFrame++]);
        previousMillis = currentMillis;
        if (currentFrame >= totalFrames) {
          currentFrame = 0;
          blinkCounter++;
          isIdle = true;
          if (blinkCounter >= blinkRepeatCount) {
            blinkCounter = 0;
            emotionStage = TRANSITION_TO_IDLE;
          }
        }
      } else if (isIdle && currentMillis - previousMillis >= idleTime) {
        isIdle = false;
        previousMillis = currentMillis;
      }
      break;

    case TRANSITION_TO_IDLE:
      emotionStage = EM_IDLE;
      currentFrame = 0;
      isIdle = true;
      previousMillis = currentMillis;
      break;
  }
}
