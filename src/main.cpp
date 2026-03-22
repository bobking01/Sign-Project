#include <Arduino.h>
#include "Adafruit_GFX.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "LittleFS.h"

/*
Default Setup (modified output):
Using GPIO 25 for R1_PIN
Using GPIO 26 for G1_PIN
Using GPIO 27 for B1_PIN
Using GPIO 14 for R2_PIN
Using GPIO 12 for G2_PIN
Using GPIO 13 for B2_PIN
Using GPIO 23 for A_PIN
Using GPIO 19 for B_PIN
Using GPIO 5 for C_PIN
Using GPIO 17 for D_PIN
Using GPIO -1 for E_PIN
Using GPIO 4 for LAT_PIN
Using GPIO 15 for OE_PIN
Using GPIO 16 for CLK_PIN
*/

#define PANEL_RES_X 64      // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 32     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1      // Total number of panels chained one to another

void draw(int r, int g, int b);

MatrixPanel_I2S_DMA * dma_display = nullptr;

const int width = 64;
const int height = 32;

void setup() 
{
  // Module configuration
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN    // Chain length
  );

  if (!LittleFS.begin()) 
  {
    ESP_LOGI("ERR:", "LittleFS failed to begin!");
    return;
  }
  else ESP_LOGI("INFO:", "LittleFS started successfully!");

  //mxconfig.gpio.e = 18;
  //mxconfig.clkphase = false;
  //mxconfig.driver = HUB75_I2S_CFG::FM6126A;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90); //0-255
  dma_display->clearScreen();

  for(int i = 0; i < 32; i++)
  {
    for(int j = 0; j < 64; j++)
    {
      dma_display->drawPixelRGB888(j,i, 255, 0, 0);
      delay(1);
    }
  }
  dma_display->clearScreen();
}

const int frameSize = width * height * 2;
uint8_t buffer[frameSize];
double x = 0;
File file;
void loop() {
  // Start file read
  file = LittleFS.open("/output.raw", "r");
  if (!file) 
  {
      ESP_LOGI("ERR:", "Could not find output.raw!");
      return;
  }
  else ESP_LOGI("INFO:", "Found output.raw!");

  while(file.available())
  {
    file.read(buffer, frameSize);
  
    dma_display->drawRGBBitmap(0,0, (uint16_t *)buffer, 64, 32);
    
    delay(50);
  }
  /*
  while(1)
  {
    double r = floor(-127.5*cos(PI*x/180)+127.5);
    double g = floor(-127.5*cos(PI*x/270)+127.5);
    double b = floor(-127.5*cos(PI*x/90)+127.5);
    x+=1;
    draw(r, g, b);
  } 
  */
  file.close();
}

void draw(int r, int g, int b)
{
  
  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves
  
  dma_display->setCursor(5, 5);    // start at top left, with 8 pixel of spacing
  uint8_t w = 0;
  const char *str = "SOFTWARE";
  for (w=0; w<strlen(str); w++) {
    dma_display->setTextColor(dma_display->color565(r,g,b));
    dma_display->print(str[w]);
  }

  //dma_display->drawRGBBitmap();
}