#include <Arduino.h>
#include <M5Stack.h>

#define HEATER_PIN 5 
#define VIN_PIN 2
#define SENSOR_PIN 36

#define WIDTH 320
#define HEIGHT 240
#define GRAPH_X 25
#define GRAPH_Y 50
#define GRAPH_SPACE 2
#define GRAPH_W WIDTH - GRAPH_X - GRAPH_SPACE
#define GRAPH_H HEIGHT - GRAPH_Y - GRAPH_SPACE
#define GRAPH_MAX 4096
#define GRAPH_MIN 2000

uint16_t GraphBuff[int(GRAPH_W)] = {0};
uint16_t graphStartPos[2] = {
  GRAPH_X + 1,
  GRAPH_Y - 1 + int(GRAPH_H)
};

void slideBuff(uint16_t buff[], uint16_t size){
  for(int i = size - 1; i >= 0; i--) buff[i] = buff[i - 1];
}

void drawText(uint32_t x, uint32_t y, String text, uint32_t color = -1, uint8_t size = 1){
  M5.Lcd.setTextColor(color);
  M5.Lcd.setTextSize(size);
  M5.Lcd.setCursor(x, y);
  M5.Lcd.print(text);
}

void updateGraph(){
  M5.Lcd.fillRect(GRAPH_X + 1, GRAPH_Y + 1, GRAPH_W - GRAPH_SPACE, GRAPH_H - GRAPH_SPACE, 0);
  for(int i = 0; i < sizeof(GraphBuff); i++){
    M5.Lcd.drawPixel(graphStartPos[0] + i, graphStartPos[1] - GraphBuff[i], -1);
  }
}

void setupGraph(){
  M5.Lcd.drawRect(GRAPH_X, GRAPH_Y, GRAPH_W, GRAPH_H, -1);
  drawText(0, HEIGHT - 10, String(GRAPH_MIN), -1, 1);
  drawText(0, HEIGHT - GRAPH_SPACE - (int(GRAPH_H) / 2), String(((GRAPH_MAX - GRAPH_MIN) / 2) + GRAPH_MIN), -1, 1);
  drawText(0, HEIGHT - GRAPH_SPACE - int(GRAPH_H), String(GRAPH_MAX), -1, 1);
}

void setup(){
  delay(500);
  M5.begin();
  dacWrite(25, 0);

  pinMode(HEATER_PIN, OUTPUT);
  pinMode(VIN_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  setupGraph();
}

void loop(){
  int val = 0;
  double vol = 0;

  delay(237);

  digitalWrite(VIN_PIN,HIGH); 
  delay(3);
  val = analogRead(SENSOR_PIN); 
  delay(2);
  digitalWrite(VIN_PIN,LOW); 

  digitalWrite(HEATER_PIN,LOW); 
  delay(8);
  digitalWrite(HEATER_PIN,HIGH);  

  vol = val * double(3.3 / 4095);
  M5.Lcd.fillRect(0, 0, 100, 20, 0);
  drawText(0, 0, "Raw: " + String(val) + "\nVoltage: " + String(vol), -1, 1);

  slideBuff(GraphBuff, sizeof(GraphBuff) / 2); 
  GraphBuff[0] = map(val, GRAPH_MIN, GRAPH_MAX, 0, GRAPH_H - 2);
  updateGraph();
}