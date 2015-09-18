
//8x8x8 rgb led cube
//now with 3d line algorithm
//now without animation classes

//pin2 dataPin(psx)
//pin3 clear
//pin4 blank
//pin5 cmndPin(psx)
//pin6 indica                                                                                                                                                                                                                                                                                                                                            tor
//pin7 attPin(psx)
//pin8 LATCH
//pin9 clockPin(psx)
//pin10 ackPin(psx)
//pin11 DATA
//pin13 CLOCK

//disable output when I start processing next animation frame
//enable output when I finish processing next animation frame
//or
//shift out an off light state right before proceessing the next animation frame (must be independent of background())
//otherwise
//the last subframe will be stuck on the cube while processing the next animation frame

//setup led light to turn on if the cube cant keep up with the framerate

/*
rain
clock
paper folding
sine
orbs
snake
RandomWalk
layer
random
lines
spin a plane
waves
sphere
fireworks
side to side points
points move random
dragon thing flying
cube inside cube
plane fall pauserandom fall
letters
cyclone
helix spin
explosion
corner fade
faces shifting
minecraft sand fill
minicube move around
points glide to new points
perimeter and blow up
atomic orbitals
fluctuating bar graphs
changing gravity for particles
*/

/*
Thanks to 
http://www.gamesx.com/controldata/psxcont/psxcont.htm
http://arduino.cc/en/Reference/SPI
http://playground.arduino.cc/Main/PSXLibrary
http://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/
http://www.kevindarrah.com/?cat=99
*/

#include <SPI.h>
#include <Psx.h>

int dataPin=2;
int cmndPin=5;
int attPin=7;
int clockPin=9;
int ackPin=10;

Psx Psx;
unsigned int data = 0;

boolean upPressed=false;
boolean downPressed=false;
boolean leftPressed=false;
boolean rightPressed=false;
boolean startPressed=false;
boolean selectPressed=false;
boolean xPressed=false;
boolean circlePressed=false;
boolean trianglePressed=false;
boolean squarePressed=false;
boolean r1Pressed=false;
boolean r2Pressed=false;
boolean l1Pressed=false;
boolean l2Pressed=false;

float hueCounter=0;
int frameCounter=0;

boolean timeForNextAnimation=false;
boolean oldSelectPressed=false;

int latchPin=8;
int ledPin=6;
int clearPin=3;
int blankPin=4;
int const bulbsPerSide=8;
byte maxBrightness=8; //max brightness; brightness resolution; also the number of subframes

unsigned long tweenTime=0;
unsigned long millisPassed=0;
unsigned long oldMillis=0;
unsigned long frameRate=60; //framerate of animation

byte HSLr;
byte HSLg;
byte HSLb;


int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


void setup(){
  Psx.setupPins(dataPin, cmndPin, attPin, clockPin, 10);
  Serial.begin(9600);
  SPI.begin();
  SPI.setBitOrder(LSBFIRST); 
  SPI.setClockDivider(SPI_CLOCK_DIV2); // run fastest
  SPI.setDataMode(SPI_MODE0);
  pinMode(latchPin, OUTPUT);
  pinMode(ledPin, OUTPUT); 
  pinMode(blankPin, OUTPUT); 
  pinMode(clearPin, OUTPUT); 
  digitalWrite(clearPin, HIGH); 
  digitalWrite(blankPin, LOW);
  digitalWrite(ackPin, LOW);
  digitalWrite(ledPin, HIGH); 
  //Serial.println("test");
  //delay(100);
}


class Color{
  public:
    byte red;
    byte green;
    byte blue;
    Color(){
      red = 0;
      green = 0;
      blue = 0;
    }
    void setColorRGB(byte r, byte g, byte b){
      red = r;
      green = g;
      blue = b;
    }
    void setColorHSL(float h, float s, float l){
      float redf, greenf, bluef;
      if (s == 0 ){redf = greenf = bluef = l;}
      float temp2;
      if ( l < 0.5) temp2 = l * ( 1 + s);
      else temp2 = l + s - l * s;
      float temp1 = 2.0 * l - temp2;
      float rtemp = h + 0.33333;
      float gtemp = h ;
      float btemp = h - 0.33333;
      if (rtemp > 1 ) rtemp -= 1;
      else if (rtemp < 0 ) rtemp += 1;
      if (gtemp > 1 ) gtemp -= 1;
      else if (gtemp < 0 ) gtemp += 1;
      if (btemp > 1 ) btemp -= 1;
      else if (btemp < 0 ) btemp += 1;
      //red:
      if ( 6.0 * rtemp < 1 ) redf = temp1+ (temp2-temp1) *6.0*rtemp;
      else if ( 2.0 * rtemp < 1 ) redf = temp2;
      else if ( 3.0 * rtemp < 2 ) redf = temp1+ (temp2-temp1) *(0.6666-rtemp)*6.0;
      else redf = temp1;
      //green:
      if ( 6.0 * gtemp < 1 ) greenf = temp1+ (temp2-temp1) *6.0*gtemp;
      else if ( 2.0 * gtemp < 1 ) greenf = temp2;
      else if ( 3.0 * gtemp < 2 ) greenf = temp1+ (temp2-temp1) *(0.6666-gtemp)*6.0;
      else greenf = temp1;
      //blue:
      if ( 6.0 * btemp < 1 ) bluef = temp1+ (temp2-temp1) *6.0*btemp;
      else if ( 2.0 * btemp < 1 ) bluef = temp2;
      else if ( 3.0 * btemp < 2 ) bluef = temp1+ (temp2-temp1) *(0.6666-btemp)*6.0;
      else bluef = temp1;
      //finally:
      red = byte(maxBrightness * redf);
      green = byte(maxBrightness * greenf);
      blue = byte(maxBrightness * bluef);
    }
};
Color MyColor;

class Pixel{ //pixel class; there is one of these for each real world led bulb
  private:
    byte redBrightness; //the brightness of the red component of this pixel, range from 0 to maxBrightness
    byte greenBrightness;
    byte blueBrightness; 
  public:
    byte getRedBrightness(){return redBrightness;} //gets the value of the led's red brightness
    void setRedBrightness(byte tRedBrightness){redBrightness=constrain(tRedBrightness, 0, maxBrightness);} //sets the red brightness of the led to whatever value specified, constrained to 0-maxBrightness
    byte getGreenBrightness(){return greenBrightness;}
    void setGreenBrightness(byte tGreenBrightness){greenBrightness=constrain(tGreenBrightness, 0, maxBrightness);} 
    byte getBlueBrightness(){return blueBrightness;}
    void setBlueBrightness(byte tBlueBrightness){blueBrightness=constrain(tBlueBrightness, 0, maxBrightness);} 
    Pixel(){ //constructor
      redBrightness=0; //default at 0, off
      greenBrightness=0;
      blueBrightness=0;
    }
};
Pixel Pixels[bulbsPerSide][bulbsPerSide][bulbsPerSide]; //makes an array of pixels, one for each real world bulb of leds


void updateCube(int rounds){
  for(int u=0; u<rounds; u++){
    updatePsxController();
    updateTimeForNextAnimation();
    waitTillItsTimeForNextFrame();
    displayPixels();
    frameCounter++;
  }
}


void updateTimeForNextAnimation(){
  if(oldSelectPressed==false && selectPressed==true){timeForNextAnimation=true;}
  else{timeForNextAnimation=false;}
  oldSelectPressed=selectPressed;
}


void updatePsxController(){
  data = Psx.read();
  //Serial.println(data);
  if (data & psxLeft){leftPressed=true;}else{leftPressed=false;}
  if (data & psxDown){downPressed=true;}else{downPressed=false;}
  if (data & psxRight){rightPressed=true;}else{rightPressed=false;}
  if (data & psxUp){upPressed=true;}else{upPressed=false;}
  if (data & psxStrt){startPressed=true;}else{startPressed=false;}
  if (data & psxSlct){selectPressed=true;}else{selectPressed=false;}
  if (data & psxSqu){squarePressed=true;}else{squarePressed=false;}
  if (data & psxX){xPressed=true;}else{xPressed=false;}
  if (data & psxO){circlePressed=true;}else{circlePressed=false;}
  if (data & psxTri){trianglePressed=true;}else{trianglePressed=false;}
  if (data & psxR1){r1Pressed=true;}else{r1Pressed=false;}
  if (data & psxL1){l1Pressed=true;}else{l1Pressed=false;}
  if (data & psxR2){r2Pressed=true;}else{r2Pressed=false;}
  if (data & psxL2){l2Pressed=true;}else{l2Pressed=false;}
}


void waitTillItsTimeForNextFrame(){//if it doesnt wait, its overloaded
//  tweenTime=1000/frameRate;
//  millisPassed=millis()-oldMillis;
//  if(millisPassed>tweenTime){
//    digitalWrite(ledPin, LOW);//low turns indicator on
//  }
//  else{
//    delay(tweenTime-millisPassed);
//    digitalWrite(ledPin, HIGH);//high turns indicator off
//  }
//  oldMillis=millis();



//  int state=LOW;//low turns indicator on
//  while(millis()<(oldMillis+((1000/frameRate)))){state=HIGH;}//high turns indicator off
//  oldMillis=millis();
//  digitalWrite(ledPin, state);
}


void HSL(float h, float s, float l){
  float redf, greenf, bluef;
  if (s == 0 ){redf = greenf = bluef = l;}
  float temp2;
  if ( l < 0.5) temp2 = l * ( 1 + s);
  else temp2 = l + s - l * s;
  float temp1 = 2.0 * l - temp2;
  float rtemp = h + 0.33333;
  float gtemp = h ;
  float btemp = h - 0.33333;
  if (rtemp > 1 ) rtemp -= 1;
  else if (rtemp < 0 ) rtemp += 1;
  if (gtemp > 1 ) gtemp -= 1;
  else if (gtemp < 0 ) gtemp += 1;
  if (btemp > 1 ) btemp -= 1;
  else if (btemp < 0 ) btemp += 1;
  //red:
  if ( 6.0 * rtemp < 1 ) redf = temp1+ (temp2-temp1) *6.0*rtemp;
  else if ( 2.0 * rtemp < 1 ) redf = temp2;
  else if ( 3.0 * rtemp < 2 ) redf = temp1+ (temp2-temp1) *(0.6666-rtemp)*6.0;
  else redf = temp1;
  //green:
  if ( 6.0 * gtemp < 1 ) greenf = temp1+ (temp2-temp1) *6.0*gtemp;
  else if ( 2.0 * gtemp < 1 ) greenf = temp2;
  else if ( 3.0 * gtemp < 2 ) greenf = temp1+ (temp2-temp1) *(0.6666-gtemp)*6.0;
  else greenf = temp1;
  //blue:
  if ( 6.0 * btemp < 1 ) bluef = temp1+ (temp2-temp1) *6.0*btemp;
  else if ( 2.0 * btemp < 1 ) bluef = temp2;
  else if ( 3.0 * btemp < 2 ) bluef = temp1+ (temp2-temp1) *(0.6666-btemp)*6.0;
  else bluef = temp1;
  //finally:
  HSLr = byte(maxBrightness * redf);
  HSLg = byte(maxBrightness * greenf);
  HSLb = byte(maxBrightness * bluef);
}

void incrementHue(float inc){
  hueCounter=hueCounter+inc;
  if (hueCounter>1){hueCounter=hueCounter-1;}
  HSL(hueCounter, 1, .5);
}

void fade(int amount){
  for(int x=0; x<=(bulbsPerSide); x++){
    for(int y=0; y<=(bulbsPerSide); y++){
      for(int z=0; z<=(bulbsPerSide); z++){
        if(Pixels[x][y][z].getRedBrightness()>0){Pixels[x][y][z].setRedBrightness(Pixels[x][y][z].getRedBrightness()-amount);}
        if(Pixels[x][y][z].getGreenBrightness()>0){Pixels[x][y][z].setGreenBrightness(Pixels[x][y][z].getGreenBrightness()-amount);}
        if(Pixels[x][y][z].getBlueBrightness()>0){Pixels[x][y][z].setBlueBrightness(Pixels[x][y][z].getBlueBrightness()-amount);}
      }
    }
  }
}


void background(byte tRedBrightness, byte tGreenBrightness, byte tBlueBrightness){
  for(int x=0; x<=(bulbsPerSide); x++){
    for(int y=0; y<=(bulbsPerSide); y++){
      for(int z=0; z<=(bulbsPerSide); z++){
        Pixels[x][y][z].setRedBrightness(tRedBrightness);
        Pixels[x][y][z].setGreenBrightness(tGreenBrightness);
        Pixels[x][y][z].setBlueBrightness(tBlueBrightness);
      }
    }
  }
}


void displayPixels(){//displays what is in pixels array on the leds; display this frame by cycling through subframes until it is time to calculate the next animation frame, but make sure the cycle runs at least once. If it cant keep up, turn on an indicator light.  
//Run the subframe cycle once, then check if it is time for the next animation frame to be calculated. If it is time after only one cycle, turn on indicator light 
//only run one cycle. if i run more than one cycle, it will be inconsistantly brighter.  
//nonono this is how we do it: run the display cycle asfast as possible once complete, calculate the next animation as fast as possible, and then wait till it is time to display it, based on frameRate.    
//each frame is made of a cycle of subframes. Subframes allow for me to mix colors for a frame. There are maxBrightness subframes
//each subframe is made of a cycle of hypoframes. Hypoframes enable me to have a height dimention. There are 8 hypoframes per subframe
//take the pixels array, break it into three locationbytes- the subframe for one color; and do this for each subframe
  byte redLocationBytes[8] = {B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000};
  byte greenLocationBytes[8] = {B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000};
  byte blueLocationBytes[8] = {B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000};
  for(int b=0; b<=(maxBrightness); b++){//for each subframe, maxBrightness times:
    for(int z=0; z<(bulbsPerSide); z++){//for each level
      for(int x=0; x<(bulbsPerSide); x++){//for each byte
        for(int y=0; y<(bulbsPerSide); y++){//for each bit
          redLocationBytes[x]<<=1;
          greenLocationBytes[x]<<=1;
          blueLocationBytes[x]<<=1;
          if (Pixels[x][y][z].getRedBrightness()>b){redLocationBytes[x]++;}
          if (Pixels[x][y][z].getGreenBrightness()>b){greenLocationBytes[x]++;}
          if (Pixels[x][y][z].getBlueBrightness()>b){blueLocationBytes[x]++;}
        }
      }
      //while(millis()<(oldMillis+((1000/frameRate)/maxBrightness))){}//wait until its time for the next subframe //one frame=one cycle of subframes //instead of waiting for the next subframe, Id like to continue cycling through the subframes as fast as possible until the time is up for the next frame, be sure the subframe cycle is complete before running the next frame //maybe i only need one cycle of subframes per frame? idk. changing the framerate should change the framerate, not the time between flashing frames.
      //oldMillis=millis();
      renderSubframe(redLocationBytes, greenLocationBytes, blueLocationBytes, z);
    }
  }
  clearRegisters();
}


void renderSubframe(byte rByte[], byte gByte[], byte bByte[], int a){
  if(a==0){SPI.transfer(B10000000);}
  if(a==1){SPI.transfer(B01000000);}
  if(a==2){SPI.transfer(B00100000);}
  if(a==3){SPI.transfer(B00010000);}
  if(a==4){SPI.transfer(B00001000);}
  if(a==5){SPI.transfer(B00000100);}
  if(a==6){SPI.transfer(B00000010);}
  if(a==7){SPI.transfer(B00000001);}
  for(int b=0; b<(bulbsPerSide); b++){SPI.transfer(bByte[b]);}
  for(int g=0; g<(bulbsPerSide); g++){SPI.transfer(gByte[g]);}
  for(int r=0; r<(bulbsPerSide); r++){SPI.transfer(rByte[r]);}  
  latch();
}


void clearRegisters(){
  SPI.transfer(B00000000);
  //anode^
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  //blue^
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  //green^
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  SPI.transfer(B00000000);
  //red^
  latch();
}


void latch(){
 digitalWrite(latchPin, HIGH); 
 digitalWrite(latchPin, LOW); 
}
