#include <Arduino.h>
#include "idle_eye_emotion.h"  // Frames en PROGMEM
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

const int totalFrames = 14;
const int idleTime = 2500;
const int frameDelay = 4;
uint32_t previousMillis = 0;
int currentFrame = 0;
bool isIdle = true;

const int frameWidth = 160;
const int frameHeight = 85;
const int screenWidth = 320;
const int screenHeight = 170;
uint16_t lineBuffer[screenWidth]; // Buffer para una línea de la imagen

static const uint16_t* idleFrames[] = {
  idle_001, idle_002, idle_003, idle_004, idle_005, idle_006, idle_007,
  idle_008, idle_009, idle_010, idle_011, idle_012, idle_013, idle_014
};

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

void setup() {
  Serial.begin(9600);
  Serial.print("ST7789 TFT Animación de Pestañeo");

  tft.begin();
  tft.setSwapBytes(true);
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  drawScaledFrame(idleFrames[0]);  
  previousMillis = millis();
}

void loop() {
  uint32_t currentMillis = millis();

  if (isIdle) {
    if (currentMillis - previousMillis >= idleTime) {
      isIdle = false;
      previousMillis = currentMillis;
    }
  } else {
    if (currentMillis - previousMillis >= frameDelay) {
      previousMillis = currentMillis;
      currentFrame++;

      if (currentFrame >= totalFrames) {
        currentFrame = 0;
        isIdle = true;
      }

      drawScaledFrame(idleFrames[currentFrame]);
    }
  }
}
