import processing.sound.*;
import processing.serial.*;

FFT fft;  // Fast Fourier Transform object
AudioIn in;
final int W=16, H=9;  // Matrix Size: 8x32.
//Displaying in bands of 2 so there are 16 columns
//There are 9 possible heights, off and 1-8
final int fps = 30;
final int BAUD_RATE = 230400;
float[] spectrum = new float[512];  // create a spectrum with 512 frequency bands
byte[] data = new byte[W];  // spectrum data frame buffer: one byte for each spectrum
int numSamples = 4;
byte[][] q = new byte[numSamples][W];
int qidx=0;

byte[] getMean(){
  byte[] retVal = new byte[16];
  for(int i = 0; i < W; i++){
    int sum = 0;
    for(int j = 0; j < numSamples; j++){
      sum += q[j][i];
    }
    retVal[i] = (byte)((byte)sum/((byte)numSamples));
  }
  return retVal;
}

// STEP 1: declare serial port object and serial name
Serial port;
final String serialName = "/dev/cu.usbserial-110";

void setup() {
  size(128, 64);
  background(255);  
  frameRate(fps);  
  fft = new FFT(this, spectrum.length);  // create FFT object
  in = new AudioIn(this, 0);  // create microphone/audio-in object
  in.start();  // start the Audio Input
  fft.input(in);  // wire input signal to FFT
  
  // STEP 2: open serial port at BAUD_RATE
  port = new Serial(this, serialName, BAUD_RATE);

}      
boolean wasOff = true;
byte[] frame_header = new byte[] {61,62,63,64};  // 4-byte frame header
byte wasKey = 0;
void draw() { 
  background(0);  // clear background: 0
  stroke(255);    // set line color: 255
  fft.analyze(spectrum);  // generate spectrum data
  for(int i=0;i<W;i++)
  {
    float v = spectrum[i]*13*H; // amplify 8x and scale by H
    q[qidx][i]=(byte)((v>H-1)?H-1:v);  // clamp to H-1 max 
    line(i, H-1-q[qidx][i], i, H-1 );  // draw spectrum line
  }
  qidx = (qidx+1)%numSamples;
  data = getMean();
  //println(data);
  
  // STEP 3: send frame header array as well as data array to serial
  wasKey = (byte)((keyPressed && wasOff)?1:0);
  if(keyPressed && wasOff){
    wasOff = false;
  }
  port.write(frame_header);  // send frame header / signature
  port.write(wasKey);
  port.write(data);  // send frame buffer
}


void keyReleased(){
  wasOff = true;
}
