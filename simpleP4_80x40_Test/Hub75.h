//Hub75.h
#ifndef HUB75_H
#define HUB75_H

#include <Arduino.h>
#include "ConfigPins.h"

#define PANEL_WIDTH 80
#define SCAN_ROWS   10

#define PANEL_HEIGHT 40
//set valid scan rows to true if you have manually identified valid scan rows
bool valid_scan_rows = true;

uint8_t buffer[PANEL_HEIGHT][PANEL_WIDTH];

if valid_scan_rows{
  //specify the right sequence of rows
  const uint8_t VALID_SCAN_ROWS[SCAN_ROWS] = {
    0, 1, 8, 9, 4, 5, 12, 13, 2, 3
  };
}


enum Color {OFF=0, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE};

const uint8_t digitFont[10][5] = {
  {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
  {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
  {0x62, 0x51, 0x49, 0x49, 0x46}, // 2
  {0x22, 0x49, 0x49, 0x49, 0x36}, // 3
  {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
  {0x2F, 0x49, 0x49, 0x49, 0x31}, // 5
  {0x3E, 0x49, 0x49, 0x49, 0x32}, // 6
  {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
  {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
  {0x26, 0x49, 0x49, 0x49, 0x3E}  // 9
};

//===========================================CLASS HUB75===============================
class Hub75{
public:
  void begin() {
    pinMode(PIN_A, OUTPUT);
    pinMode(PIN_B, OUTPUT);
    pinMode(PIN_C, OUTPUT);
    pinMode(PIN_D, OUTPUT);

    pinMode(PIN_CLK, OUTPUT);
    pinMode(PIN_LAT, OUTPUT);
    pinMode(PIN_OE, OUTPUT);

    pinMode(PIN_R1, OUTPUT);
    pinMode(PIN_G1, OUTPUT);
    pinMode(PIN_B1, OUTPUT);

    pinMode(PIN_R2, OUTPUT);
    pinMode(PIN_G2, OUTPUT);
    pinMode(PIN_B2, OUTPUT);

    allLow();

    // OE is usually active LOW.
    // HIGH = display OFF
    digitalWrite(PIN_OE, HIGH);
  }
//===========================Draw FULL ROWS===========================================
void drawFullRow(uint8_t scanRow) {
  disableOutput();
  setScanRow(scanRow);

  for (uint8_t col = 0; col < PANEL_WIDTH; col++) {
    digitalWrite(PIN_R1, LOW);
    digitalWrite(PIN_G1, HIGH);
    digitalWrite(PIN_B1, LOW);

    digitalWrite(PIN_R2, LOW);
    digitalWrite(PIN_G2, HIGH);
    digitalWrite(PIN_B2, LOW);

    pulseClock();
  }

  pulseLatch();

  enableOutput();
  delayMicroseconds(800);
  disableOutput();
}

//=========================TEST COLORS USING ROWS==========================================
void testOneColorPin(uint8_t scanRow, uint8_t colorPin) {
  if (scanRow >= SCAN_ROWS) return;

  disableOutput();
  //uncoment if you have manually identified valid scan rows
  setScanRow(VALID_SCAN_ROWS[scanRow]);

  //uncoment if valid scan rows without your intervention
    // setScanRow(scanRow);

  for (uint8_t col = 0; col < PANEL_WIDTH; col++) {
    digitalWrite(PIN_R1, LOW);
    digitalWrite(PIN_G1, LOW);
    digitalWrite(PIN_B1, LOW);
    digitalWrite(PIN_R2, LOW);
    digitalWrite(PIN_G2, LOW);
    digitalWrite(PIN_B2, LOW);

    digitalWrite(colorPin, HIGH);

    pulseClock();
  }

  pulseLatch();

  enableOutput();
  delayMicroseconds(800);
  disableOutput();
}

//========================================Generic Mapping Function==============================
// 
void drawMappedRow(uint8_t logicalRow, bool r1, bool g1, bool b1, bool r2, bool g2, bool b2) {
  if (logicalRow >= SCAN_ROWS) return;
  disableOutput();
  //uncoment if you have manually identified right order of rows
  setScanRow(VALID_SCAN_ROWS[logicalRow]);
  //uncoment if rows are in right order without your intervention
  //setScanRow(logicalRow);

  for (uint8_t col = 0; col < PANEL_WIDTH; col++) {

    digitalWrite(PIN_R1, r1 ? HIGH : LOW);
    digitalWrite(PIN_G1, g1 ? HIGH : LOW);
    digitalWrite(PIN_B1, b1 ? HIGH : LOW);

    digitalWrite(PIN_R2, r2 ? HIGH : LOW);
    digitalWrite(PIN_G2, g2 ? HIGH : LOW);
    digitalWrite(PIN_B2, b2 ? HIGH : LOW);

    pulseClock();
  }

  pulseLatch();

  enableOutput();
  delayMicroseconds(800);
  disableOutput();
}

//========================================Draw Pixel With Color=======================================================
void drawPixel(uint8_t logicalRow, uint8_t x, Color color, bool useBank2 = false) {
  if (logicalRow >= SCAN_ROWS) return;
  if (x >= PANEL_WIDTH) return;

  bool r = false;
  bool g = false;
  bool b = false;

  switch (color) {
    case RED:     r = true; break;
    case GREEN:   g = true; break;
    case BLUE:    b = true; break;
    case YELLOW:  r = true; g = true; break;
    case CYAN:    g = true; b = true; break;
    case MAGENTA: r = true; b = true; break;
    case WHITE:   r = true; g = true; b = true; break;
    default: break;
  }

  disableOutput();
  //uncoment if you have manually identified right order of rows
  setScanRow( VALID_SCAN_ROWS[logicalRow]);
  //uncoment if rows are in right order without your intervention
  //setScanRow(logicalRow);

  for (uint8_t col = 0; col < PANEL_WIDTH; col++) {
    bool on = (col == x);

    if (!useBank2) {
      digitalWrite(PIN_R1, on && r ? HIGH : LOW);
      digitalWrite(PIN_G1, on && g ? HIGH : LOW);
      digitalWrite(PIN_B1, on && b ? HIGH : LOW);

      digitalWrite(PIN_R2, LOW);
      digitalWrite(PIN_G2, LOW);
      digitalWrite(PIN_B2, LOW);
    } else {
      digitalWrite(PIN_R1, LOW);
      digitalWrite(PIN_G1, LOW);
      digitalWrite(PIN_B1, LOW);

      digitalWrite(PIN_R2, on && r ? HIGH : LOW);
      digitalWrite(PIN_G2, on && g ? HIGH : LOW);
      digitalWrite(PIN_B2, on && b ? HIGH : LOW);
    }

    pulseClock();
  }

  pulseLatch();

  enableOutput();
  delayMicroseconds(500);
  disableOutput();
}

//============================SET PIXELS============================================================
//This is the game changer
uint8_t mapY(uint8_t y) {
  if (y < 10) return y + 10;
  if (y < 20) return y - 10;
  if (y < 30) return y + 10;
  return y - 10;
}


void setPixel(uint8_t x, uint8_t y, Color color) {
  if (x >= PANEL_WIDTH || y >= PANEL_HEIGHT) return;

  uint8_t mappedY = mapY(y);
  buffer[mappedY][x] = color;
}

void clear() {
  for (uint8_t y = 0; y < PANEL_HEIGHT; y++) {
    for (uint8_t x = 0; x < PANEL_WIDTH; x++) {
      buffer[y][x] = OFF;
    }
  }
}

void refresh() {
  for (uint8_t logicalRow = 0; logicalRow < SCAN_ROWS; logicalRow++) {

    uint8_t y0 = logicalRow;
    uint8_t y1 = logicalRow + 10;
    uint8_t y2 = logicalRow + 20;
    uint8_t y3 = logicalRow + 30;

    disableOutput();
    //uncoment if you have manually identified right order of rows
    setScanRow( VALID_SCAN_ROWS[logicalRow]);
    //uncoment if rows are in right order without your intervention
    // setScanRow(logicalRow);

// Second 80 clocks: rows y0 and y2
  for (uint8_t col = 0; col < PANEL_WIDTH; col++) {
  bool r1 = false, g1 = false, b1 = false;
  bool r2 = false, g2 = false, b2 = false;

  applyColor(buffer[y0][col], r1, g1, b1);
  applyColor(buffer[y2][col], r2, g2, b2);

  digitalWrite(PIN_R1, r1);
  digitalWrite(PIN_G1, g1);
  digitalWrite(PIN_B1, b1);

  digitalWrite(PIN_R2, r2);
  digitalWrite(PIN_G2, g2);
  digitalWrite(PIN_B2, b2);

  pulseClock();
}

  // First 80 clocks: rows y1 and y3
  for (uint8_t col = 0; col < PANEL_WIDTH; col++) {
    bool r1 = false, g1 = false, b1 = false;
    bool r2 = false, g2 = false, b2 = false;

    applyColor(buffer[y1][col], r1, g1, b1);
    applyColor(buffer[y3][col], r2, g2, b2);

    digitalWrite(PIN_R1, r1);
    digitalWrite(PIN_G1, g1);
    digitalWrite(PIN_B1, b1);

    digitalWrite(PIN_R2, r2);
    digitalWrite(PIN_G2, g2);
    digitalWrite(PIN_B2, b2);

    pulseClock();
  }

      pulseLatch();

      enableOutput();
      delayMicroseconds(300);
      disableOutput();
    }
}

//===========================Apply Color================================================
void applyColor(uint8_t color, bool &r, bool &g, bool &b) {
  switch (color) {
    case RED:     r = true; break;
    case GREEN:   g = true; break;
    case BLUE:    b = true; break;
    case YELLOW:  r = true; g = true; break;
    case CYAN:    g = true; b = true; break;
    case MAGENTA: r = true; b = true; break;
    case WHITE:   r = true; g = true; b = true; break;
    default: break;
  }
}

//=====================================DRAW FUNCTIONS==================================================================

void drawLine(int x0, int y0, int x1, int y1, Color color) {
  int dx = abs(x1 - x0);
  int dy = -abs(y1 - y0);
  int sx = x0 < x1 ? 1 : -1;
  int sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;

  while (true) {
    setPixel(x0, y0, color);

    if (x0 == x1 && y0 == y1) break;

    int e2 = 2 * err;

    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }

    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void drawRect(int x, int y, int w, int h, Color color) {
  drawLine(x, y, x + w - 1, y, color);
  drawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
  drawLine(x, y, x, y + h - 1, color);
  drawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
}

void fillRect(int x, int y, int w, int h, Color color) {
  for (int yy = y; yy < y + h; yy++) {
    for (int xx = x; xx < x + w; xx++) {
      setPixel(xx, yy, color);
    }
  }
}

void drawHorizontalLine(int x, int y, int length, Color color) {
  for (int i = 0; i < length; i++) {
    setPixel(x + i, y, color);
  }
}

void drawVerticalLine(int x, int y, int length, Color color) {
  for (int i = 0; i < length; i++) {
    setPixel(x, y + i, color);
  }
}

//=================================================================================================
//=================================================================================================
private:
  void setScanRow(uint8_t row) {
    digitalWrite(PIN_A, (row & 0x01) ? HIGH : LOW);
    digitalWrite(PIN_B, (row & 0x02) ? HIGH : LOW);
    digitalWrite(PIN_C, (row & 0x04) ? HIGH : LOW);
    digitalWrite(PIN_D, (row & 0x08) ? HIGH : LOW);
  }

  void pulseClock() {
    digitalWrite(PIN_CLK, HIGH);
    delayMicroseconds(1);
    digitalWrite(PIN_CLK, LOW);
    delayMicroseconds(1);
  }

  void pulseLatch() {
    digitalWrite(PIN_LAT, HIGH);
    delayMicroseconds(1);
    digitalWrite(PIN_LAT, LOW);
    delayMicroseconds(1);
  }

  void enableOutput() {
    digitalWrite(PIN_OE, LOW);
  }

  void disableOutput() {
    digitalWrite(PIN_OE, HIGH);
  }

  void allLow() {
    digitalWrite(PIN_A, LOW);
    digitalWrite(PIN_B, LOW);
    digitalWrite(PIN_C, LOW);
    digitalWrite(PIN_D, LOW);

    digitalWrite(PIN_CLK, LOW);
    digitalWrite(PIN_LAT, LOW);

    digitalWrite(PIN_R1, LOW);
    digitalWrite(PIN_G1, LOW);
    digitalWrite(PIN_B1, LOW);

    digitalWrite(PIN_R2, LOW);
    digitalWrite(PIN_G2, LOW);
    digitalWrite(PIN_B2, LOW);
  }
};

#endif