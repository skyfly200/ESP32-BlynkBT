#include <WS2812FX.h>
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#define BLYNK_PRINT Serial

#define LED_COUNT 8
#define LED_PIN 2
#define DIAL_PIN 15

#define BLYNK_USE_DIRECT_CONNECT

// Blynk Creds
char auth[] = "5da4f202cbda48bbb3c19b22529c28b4"; // PixeLED App
//char auth[] = "d604988eda63446f8a155f13e174c9c1"; // PixeLED Audio App

WidgetLCD lcd(V0);

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

bool alerted = false;
int lastAlert = 0;
int alertLen = 3000;

extern const uint8_t gamma8[];


const byte fxs[] = {11, 12, 0, 2, 15, 1, 19, 9, 22, 23, 24, 25, 29, 46, 49, 48, 50};
const int fxSpeeds[][2] = {{1, 1000}, {1, 1000}, {1, 1000}, {1, 1000}, {1, 1000}, {1, 1000}, {1, 1000}, {1, 1000}, 
                        {1, 1000}, {1, 1000}, {1, 1000}, {1, 1000}, {1, 1000}, {1, 1000}, {1, 1000}, {1, 1000}, {1, 1000}};

// Control Vars
bool power = true;
byte fxSpeed = 500;

// set from eeprom or non voltile source
volatile byte fxI = 0; // default animation mode
byte favs[] = {8,0,1,2,3,4,5,6}; // animation favs
byte fav = 0;

void setup() {
  Serial.begin(115200);

  // LED Library Init
  ws2812fx.init();
  ws2812fx.setBrightness(100);
  ws2812fx.setSpeed(500);
  ws2812fx.setColor(ORANGE);
  ws2812fx.setMode(fxs[fxI]);
  ws2812fx.start();

  // Blynk Stuff

  Blynk.begin(auth);

  dispMode();
  // Please use timed events when LCD printintg in void loop to avoid sending too many commands
  // It will cause a FLOOD Error, and connection will be dropped
}

void loop() {
  Blynk.run();
  ws2812fx.service();

  if (alerted && lastAlert-millis() > alertLen) {
    dispMode();
  }
}

// Blynk app - Virtual Pin V1
BLYNK_WRITE(V1)
{
  power = !power;
}

// Blynk app - Virtual Pin V2
BLYNK_WRITE(V2)
{
  ws2812fx.setBrightness(gamma8[param.asInt()]);
}

// Blynk app - Virtual Pin V5
BLYNK_WRITE(V5)
{
  fxSpeed = param.asInt();
  ws2812fx.setSpeed(map(fxSpeed,1,1000,fxSpeeds[fxI][0],fxSpeeds[fxI][1]));
}

// Blynk app - Virtual Pin V4
BLYNK_WRITE(V4)
{
  fxI = param.asInt();
  ws2812fx.setMode(fxs[fxI]);
  dispMode();
}

// Blynk app - Virtual Pin V3
BLYNK_WRITE(V3)
{
  ws2812fx.setColor(
    gamma8[param[0].asInt()],
    gamma8[param[1].asInt()],
    gamma8[param[2].asInt()]);
}

// Blynk app - Virtual Pin V6
BLYNK_WRITE(V6)
{
  fav = param.asInt();
  ws2812fx.setMode(fxs[favs[fav]]);
  dispMode();
}

// Blynk app - Virtual Pin V7
BLYNK_WRITE(V7)
{
  alert("Favorite Saved");
  favs[fav] = ws2812fx.getMode();
}

// Blynk app - Virtual Pin V10
BLYNK_WRITE(V10)
{
  alert("Default Saved");
}

void dispMode() {
  lcd.clear();
  lcd.print(0, 0, ws2812fx.getModeName(ws2812fx.getMode()));
}

void alert(char* m) {
  lcd.clear();
  lcd.print(0, 0, m);
  alerted = true;
}

const uint8_t gamma8[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};
