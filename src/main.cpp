void pngDraw(struct png_draw_tag);

#include <Arduino.h>
#include "dog.h"
#include <TFT_eSPI.h>
#include <PNGdec.h>

PNG png; 
#define MAX_IMAGE_WIDTH 320 // Adjust for your images

int16_t xpos = 0;
int16_t ypos = 0;

TFT_eSPI tft = TFT_eSPI();           // TFT object

TFT_eSprite spr = TFT_eSprite(&tft); // Sprite object

void setup() {
  Serial.begin(115200); // Debug only
  Serial.println("\n\n Started");
  tft.begin();  // initialize
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(0);
}
void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WIDTH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}
void loop() {
  int16_t rc = png.openFLASH((uint8_t *)dog, sizeof(dog), pngDraw);
  if (rc == PNG_SUCCESS) {
    Serial.println("Successfully opened png file");
    Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
    tft.startWrite();
    uint32_t dt = millis();
    rc = png.decode(NULL, 0);
    Serial.print(millis() - dt); Serial.println("ms");
    tft.endWrite();
    // png.close(); // not needed for memory->memory decode
  }
  delay(3000);
  tft.fillScreen(random(0x10000));
}