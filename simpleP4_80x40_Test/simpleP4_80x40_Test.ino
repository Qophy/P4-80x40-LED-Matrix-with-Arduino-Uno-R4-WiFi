//simplep4_80x40_TEST
// This code is designed for the P4 80x40 1/10 scan LED Matrix panel.
// With minor modifications, it can also be adapted for other LED matrix panels.
//
// The project is structured to guide users step-by-step through:
//  - lighting coupled rows due to the scan
//  - to lighting individual pixels,
//  - drawing simple shapes and graphics,
//  - creating a digital clock,
//  - and building a basic timer.
//
// Author: Dafil Jonathan, 2026.
// Enjoy learning!!! The update on the way soon....
// 

#include "Hub75.h"

Hub75 panel;

int x = 0;
int y = 0;
int dx = 1;
int dy = 1;
int barX = 0;
int offset = 0;
int color_num = 2;

uint8_t testX = 0;
uint8_t testRow = 0;

int hour = 6;
int minute = 34;
int second = 0;
unsigned long lastSecond = 0;

//================ TIMER SETTINGS ================
// Set starting time here. Example below = 1 hour, 30 minutes.
unsigned long timerTotalSeconds = (0UL * 3600UL) + (1UL * 60UL);

unsigned long lastTimerTick = 0;
unsigned long lastBlinkTick = 0;
bool blinkVisible = true;
bool timerFinished = false;

void setup() {
  panel.begin();
  Serial.begin(115200);

//set pixels
  //set_pixels();

  //drawClock(hour, minute, second);
//CountDown Timer
  lastTimerTick = millis();
  lastBlinkTick = millis();
  drawTimer(timerTotalSeconds);
}

void loop() {
  panel.refresh();
// fillRect();
  if (millis() - lastTimerTick >= 1000) {
    lastTimerTick = millis();

    if (timerTotalSeconds > 0) {
      timerTotalSeconds--;
    }
  }
  drawTimer(timerTotalSeconds);
}


void digital_clock(){
    if (millis() - lastSecond >= 1000) {
    lastSecond += 1000;

    second++;

    if (second >= 60) {
      second = 0;
      minute++;

      if (minute >= 60) {
        minute = 0;
        hour++;

        if (hour >= 24) {
          hour = 0;
        }
      }
    }

    drawClock(hour, minute, second);
  }
}
//=================================SCAN ROWS============================================
//Scan the panel to know the row sequence. Some panels scan randomly. 
//This will help you to take note of the rows if randomly scan and reorganize them
void scan_rows(){
  for (uint8_t row = 0; row < 10; row++) {
    unsigned long startTime = millis();

    while (millis() - startTime < 1000) {
      panel.drawFullRow(row);
    }
    Serial.println(row);
    delay(200);
  }
}

void mapping_row(){
  for (uint8_t logicalRow = 0; logicalRow < SCAN_ROWS; logicalRow++) {
    unsigned long startTime = millis();

    while (millis() - startTime < 800) {
      panel.drawMappedRow(logicalRow, true, true, false, false, true, true);
    }

    delay(300);
  }
}

//=================================Check the colors ==================================================
void test_color(uint8_t row, uint8_t pin) {
  panel.testOneColorPin(row, pin);
}

//====================DRAW TWO PIXELS================================================================
//This is a massive progress since the scan has been reduced to two pixels and not the full rows
void draw_pixels(){
  panel.drawPixel(testRow, testX, MAGENTA, false);
  testRow = testRow + 1;
  if (testRow >= 10) testRow = 0;
  delay(5000);
}

  // //=========Pixel seting================
  void set_pixels(){
      panel.clear();
      panel.setPixel(0, 0, RED);
      panel.setPixel(0, 10, GREEN);
      panel.setPixel(0, 20, BLUE);
      panel.setPixel(0, 30, WHITE);
  }


//==============Bouncing Pixel================================================================
void bouncing_pixel(){
  //bouncing pixel
    panel.clear();

  panel.setPixel(x, y, static_cast<Color>(color_num));

  for (int i = 0; i < 20; i++) {
    panel.refresh();
  }

  x += dx;
  y += dy;

  if (x <= 0 || x >= 79) {dx = -dx; color_num++; color_num %= 9;}
  if (y <= 0 || y >= 39) {dy = -dy; color_num++; color_num %= 9;}
}

//=========Moving Pixel ===================
void moving_pixel() {
  static uint8_t x = 0;
  panel.clear();
  panel.setPixel(x, 0, RED);
  panel.refresh();
   x = (x + 1) % 80; 
}

//=============== Draw line================
void draw_line(Color color){
  panel.clear();
  for (int x = 0; x < 80; x++) {
  panel.refresh();
  panel.setPixel(x, 5, color);
  }  
}

////============Fill Panel==================
void fill_panel(){
  panel.clear();
  color_num %= 8;
  if (color_num == 0) color_num = 1;
  for (int y = 0; y < 40; y++) {
  for (int x = 0; x < 80; x++) {
    panel.setPixel(x, y, static_cast<Color>(color_num));
  }
  }  
  color_num++;
  for (int i = 0; i < 50; i++) {
    panel.refresh();
  }
}

////============Fill Panel==================
void fill_panel_colors_vertical(){
  panel.clear();
  if (color_num == 0) color_num = 1;
  for (int y = 0; y < 40; y++) {
  for (int x = 0; x < 80; x++) {
    color_num %= 8;
    panel.setPixel(x, y, static_cast<Color>(color_num));
    color_num++;
  }
  }  
  for (int i = 0; i < 50; i++) {
    panel.refresh();
  }
}

////============Fill Panel==================
void fill_panel_colors_diagonal(){
  panel.clear();
  //if (color_num == 0) color_num = 1;
  for (int y = 0; y < 40; y++) {
  for (int x = 0; x < 80; x++) {
    panel.setPixel(x, y, static_cast<Color>(color_num));
    color_num %= 9;
    color_num++;
  }
  }  
  
  for (int i = 0; i < 50; i++) {
    panel.refresh();
  }
}

//=========Moving BAR ===================
void fillRect(){
  panel.clear();

  panel.fillRect(barX, 10, 10, 8,static_cast<Color>(color_num));

  for (int i = 0; i < 20; i++) {
    panel.refresh();
  }

  barX++;

  if (barX > 79) {
    barX = -10;
    color_num %= 8;
    color_num++;
  }  
}

//=================DIGITAL CLOCK HH:MM==============================================================================
void drawDigit(int x, int y, int digit, Color color) {
  for (int col = 0; col < 5; col++) {
    uint8_t line = digitFont[digit][col];

    for (int row = 0; row < 7; row++) {
      if (line & (1 << row)) {
        panel.setPixel(x + col, y + row, color);
      }
    }
  }
}
void drawBlock(int x, int y, int size, Color color) {
  panel.fillRect(x, y, size, size, color);
}
void drawDigitBig(int x, int y, int digit, int scale, Color color) {
  for (int col = 0; col < 5; col++) {
    uint8_t line = digitFont[digit][col];

    for (int row = 0; row < 7; row++) {
      if (line & (1 << row)) {
        drawBlock(x + col * scale, y + row * scale, scale, color);
      }
    }
  }
}
void drawColonBig(int x, int y, int scale, bool on, Color color) {
  if (!on) return;

  drawBlock(x, y + 2 * scale, scale, color);
  drawBlock(x, y + 4 * scale, scale, color);
}
void drawClock(int h, int m, int s) {
  panel.clear();

  int scale = 3;

  int digitW = 5 * scale;   // 15
  int digitH = 7 * scale;   // 21
  int spacing = 3;
  int colonW = scale;       // 3

  int totalW = (digitW * 4) + (spacing * 5) + colonW;
  int x = (80 - totalW) / 2 + 2;
  int y = (40 - digitH) / 2 + 6;

  drawDigitBig(x, y, h / 10, scale, static_cast<Color>(color_num));
  x += digitW + spacing;

  drawDigitBig(x, y, h % 10, scale, static_cast<Color>(color_num));
  x += digitW + spacing;

  drawColonBig(x, y, scale, (s % 2 == 0), static_cast<Color>(color_num));
  x += colonW + spacing;

  drawDigitBig(x, y, m / 10, scale, static_cast<Color>(color_num));
  x += digitW + spacing;

  drawDigitBig(x, y, m % 10, scale, static_cast<Color>(color_num));
}

//================= COUNTDOWN TIMER HH:MM / FINAL 00:SS ==============================
void drawTimer(unsigned long totalSeconds) {

  bool finalMinuteBlink = (totalSeconds > 0 && totalSeconds < 60);

  bool screenVisible = true;
  bool colonVisible = true;

  // Colon blinks every 500ms while timer is running
  if (totalSeconds > 0) {
    colonVisible = ((millis() / 500) % 2 == 0);
  }

  // Whole screen blinks only from 00:59 to 00:01
  if (finalMinuteBlink) {
    screenVisible = ((millis() / 500) % 2 == 0);
  }

  // At 00:00, stop blinking and keep display visible
  if (totalSeconds == 0) {
    screenVisible = true;
    colonVisible = true;
  }

  if (!screenVisible) {
    panel.clear();
    return;
  }

  int leftValue;
  int rightValue;

  if (totalSeconds >= 60) {
    // HH:MM mode
    leftValue = totalSeconds / 3600;
    rightValue = (totalSeconds % 3600) / 60;
  } else if (totalSeconds < 60){
    // 00:SS mode
    leftValue = 0;
    rightValue = totalSeconds;
    color_num=1;
    Color color = static_cast<Color>(color_num);
  }else{
    color_num=3;
    Color color = static_cast<Color>(color_num);
  }

   drawClock(leftValue, rightValue, 0);
}
