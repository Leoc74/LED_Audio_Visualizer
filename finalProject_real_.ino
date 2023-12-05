#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
Adafruit_SSD1306 lcd(128, 64); // create display object
#define BAUD_RATE 230400
#define PIN 2
#define show_noglitch() {delay(1);matrix.show();delay(1);matrix.show();}
#define MAX_HEIGHT 8.0
Adafruit_NeoPixel matrix = Adafruit_NeoPixel(256, PIN, NEO_RGB + NEO_KHZ800);
#define WIDTH 16
#define HEIGHT 8
#define NUM_COLORS 2
byte data[WIDTH];
void setup() { 
  matrix.begin();
  matrix.setBrightness(16);
  matrix.clear();
  matrix.setPixelColor(0,0);
  show_noglitch();
  Serial.begin(230400);
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


byte colors[2][NUM_COLORS][3] = {{{255,0,255},{255,255,0}},{{255,0,0},{0,255,0}}};
int curColor = 0;


byte frame_header[4];

//given 2 colors (lists of 3 bytes), gets linear gradient between them in form of list of 16 band colors
byte* getGradient(byte* color1, byte* color2){
  byte retVal[WIDTH];
  float redSlope = ((float)color2[0]-(float)color1[0])/(float)WIDTH;
  float greenSlope = (color2[1]-color1[1])/WIDTH;
  float blueSlope = (color2[2]-color1[2])/WIDTH;
  for(int i = 0; i < WIDTH; i++){
    int red = redSlope*i + color1[0];
    int green = greenSlope*i + color1[1];
    int blue = blueSlope*i + color1[2];
    retVal[i] = matrix.Color(red, green, blue);
  }
  return retVal;
}

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
      Serial.readBytes(data, WIDTH+1);
      if(data[0]==1){//A key was pressed, change color
          curColor = (curColor + 1)%NUM_COLORS;
      }
      matrix.clear();
      byte* gradient = getGradient(colors[curColor][0], colors[curColor][1]);
      lcd.clearDisplay();
      for(int j = 1; j <= WIDTH; j++){
        
        colorBand(j, (double)data[j], gradient[j-1]);
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
