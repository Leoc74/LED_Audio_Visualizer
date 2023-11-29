#include <Adafruit_SSD1306.h>
#include <Wire.h>
Adafruit_SSD1306 lcd(128, 64); // create display object
#define BAUD_RATE 230400

void setup() {
  lcd.begin(SSD1306_SWITCHCAPVCC, 0x3C); // init
  lcd.clearDisplay();
  lcd.setTextColor(WHITE);
  lcd.setCursor(0, 28);
  lcd.print("Waiting for data...");
  lcd.display();
  Serial.begin(BAUD_RATE);
}

#define WIDTH 128
#define HEIGHT 64
byte data[WIDTH];

byte frame_header[4];
void loop() {
  /* TODO: complete loop function to receive data from serial
   * and plot the spectrum graphs to OLED display
   */
   if(Serial.available()){
    Serial.readBytes(frame_header, 4);
    byte i;
    for (i = 0; i<4; i++){
      if(frame_header[i] != i+61) break;
      }
    if (i == 4){ //valid header
      Serial.readBytes(data, WIDTH);
      lcd.clearDisplay();
      for(int j = 0; j < WIDTH; j++){
        lcd.drawLine(j, HEIGHT-1-data[j],  j, HEIGHT-1, 0xFFFF);
      }
      lcd.display();
      }
    }
}
