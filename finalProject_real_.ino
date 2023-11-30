#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
Adafruit_SSD1306 lcd(128, 64); // create display object
#define BAUD_RATE 230400
#define PIN 2
#define show_noglitch() {delay(1);matrix.show();delay(1);matrix.show();}
#define MAX_HEIGHT 8.0

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(256, PIN, NEO_RGB + NEO_KHZ800);

void setup() {
  /* 
  lcd.begin(SSD1306_SWITCHCAPVCC, 0x3C); // init
  lcd.clearDisplay();
  lcd.setTextColor(WHITE);
  lcd.setCursor(0, 28);
  lcd.print("Waiting for data...");
  lcd.display();
  */
  matrix.begin();
  matrix.setBrightness(32);
  matrix.clear();
  matrix.setPixelColor(0,0);
  show_noglitch();
  Serial.begin(BAUD_RATE);
}

//Takes in an int representing which band to light up, 
//an int which is the height sent over from processing, 
//and a color to light the pixels up with
void colorBand(int band, double height, unsigned long color){
  //array which pairs up the pixels which are next to each other on the matrix
  //The first element in the array is the pair of pixels at the bottom of the matrix
  int pairArr[8][2] = {{7,8}, {6,9}, {5,10}, {4,11}, {3,12}, {2,13}, {1,14}, {0,15}};
  double percentagePixels = ((height) / MAX_HEIGHT) * 8; //8 is the height of the matrix
  int i;
  for(i = 0; i < percentagePixels; i++){
    matrix.setPixelColor(pairArr[i][0] + (band*16), color);
    matrix.setPixelColor(pairArr[i][1] + (band*16), color);
  }
}

#define WIDTH 16
#define HEIGHT 8
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
      matrix.clear();
      for(int j = 0; j < WIDTH; j++){
        colorBand(j, (double)data[j], 0x00FFFF);
        //lcd.print(j);
        //lcd.display();
        //lcd.drawLine(j, HEIGHT-1-data[j],  j, HEIGHT-1, 0xFFFF);
      }
      show_noglitch();
      }
    }
    
    //TODO TESTING
    /*int heightArr[] = {1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8};
    for(int j = 0; j < WIDTH; j++){
        colorBand(j, heightArr[j], 0x00FFFF);
      }
      */
      
}
