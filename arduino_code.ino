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
#define NUM_COLORS 4
byte data[WIDTH];
byte keyPressed = 0;
void setup() { 
  matrix.begin();
  matrix.setBrightness(32);
  matrix.clear();
  show_noglitch();
  Serial.begin(230400);
}




//Takes in an int representing which band to light up, 
//an int which is the height sent over from processing, 
//and a color to light the pixels up with
void colorBand(int band, double height, unsigned int color){
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


int colors[NUM_COLORS][2][3] = {{{0,255,0},{255,0,0}},{{0,255,0},{0,0,255}},{{255,0,0},{0,0,255}},{{140,255,0},{0,0,255}}};
int curColor = 0;


byte frame_header[4];

//given 2 colors (lists of 3 ints), gets quadratic gradient between them in form of list of 16 band colors
//finds interpolating parabola
unsigned int* getGradient(int* color1, int* color2){
  unsigned int* retVal = (unsigned int*)malloc(WIDTH*sizeof(unsigned int));
  for(double i = 0; i < WIDTH; i++){
    int red = (int)((double)(color2[0]-color1[0])*pow((i/(double)WIDTH),(double)2)) + (double)color1[0]; //quadratic gradient
    int green = (int)((double)(color2[1]-color1[1])*pow((i/(double)WIDTH),(double)2)) + (double)color1[1];
    int blue = (int)((double)(color2[2]-color1[2])*pow((i/(double)WIDTH),(double)2)) + (float)color1[2];
    retVal[(int)i] = matrix.Color(red, green, blue);
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
      Serial.readBytes(&keyPressed, 1);
      Serial.readBytes(data, WIDTH);
      if(keyPressed){//A key was pressed, change color
          curColor = (curColor + 1)%NUM_COLORS;
      }
      matrix.clear();
      unsigned int* gradient = getGradient(colors[curColor][0], colors[curColor][1]);
      for(int j = 0; j < WIDTH; j++){
        colorBand(j, (double)data[j], gradient[j]);
      }
      show_noglitch();
      free(gradient);
      }
    } 
}
