
class Buggy{  //buggy class; this is a movable object
  public:
  
    byte red; //the brightness of the red light component, range 0-maxBrightness
    byte blue;
    byte green;
    
    float xPos; //the position of the buggy
    float yPos;
    float zPos;
    
    int xVel;//the speed of the buggy
    int yVel;
    int zVel;
    
    float gravity;
    float airResistance;
    float bounceResistance;
    float speedLimit;
    float acceleration;
    
    Buggy(){ //constructor
      red=maxBrightness; //default at brightness of maxBrightness, full on
      green=maxBrightness;
      blue=maxBrightness;
      xPos=0; //the position of the buggy
      yPos-0;
      zPos=0;
      
      xVel=0;//the speed of the buggy
      yVel=0;
      zVel=0;
      
      gravity=.5;//pixels per frame per frame
      airResistance=.998;//between 0 (infinite resistance) and 1 (no resistance)
      bounceResistance=.99;//between 0 (infinite resistance) and 1 (no resistance)
      speedLimit=3;//pixels per frame
      acceleration=1;//pixels per frame per frame
    }
    
    void render(){
      Pixels[int(xPos)][int(yPos)][int(zPos)].setRedBrightness(red); //transfer the information from my buggy to the correct pixel
      Pixels[int(xPos)][int(yPos)][int(zPos)].setGreenBrightness(green); //transfer the information from my buggy to the correct pixel
      Pixels[int(xPos)][int(yPos)][int(zPos)].setBlueBrightness(blue); //transfer the information from my buggy to the correct pixel
    }
    
    void updateHue(){
      red=HSLr;
      green=HSLg;
      blue=HSLb;
    }
    
    void updateBouncingBall(){
      yVel=yVel+gravity;//gravity
      if (upPressed==true){yVel=yVel-(acceleration+gravity);}//increase speed up
      if (downPressed==true){yVel=yVel+acceleration;}//increase speed down
      if (leftPressed==true){xVel=xVel-acceleration;}//increase speed left
      if (rightPressed==true){xVel=xVel+acceleration;}//increase speed right
      xVel=xVel*airResistance;//airResistance on x axis
      yVel=yVel*airResistance;//airResistance on y axis
      if (xVel>speedLimit){xVel=speedLimit;}//speed limit on x axis
      if (yVel>speedLimit){yVel=speedLimit;}//speed limit on y axis
      xPos=xVel+xPos;//move on x axis
      yPos=yVel+yPos;//move on y axis
      if (xPos<0){xVel=xVel*-bounceResistance; xPos=0;}//bounce when hit left wall
      if (xPos>bulbsPerSide){xVel=xVel*-bounceResistance; xPos=bulbsPerSide;}//bounce when hit right wall
      if (yPos>bulbsPerSide){yVel=yVel*-bounceResistance; yPos=bulbsPerSide;}//bounce when hit bottom wall
      if (yPos<0){yVel=yVel*-bounceResistance; yPos=0;}//bounce when hit top wall
      background(0,0,0);
      incrementHue(.005);
      updateHue();
      if (xPressed==true){xPos=4;yPos=4;zPos=4;}
    }
    
    void updateDrive(){
      xPos=int(xPos);
      yPos=int(yPos);
      zPos=int(zPos);
      if (upPressed){xPos++;}
      if (downPressed){xPos--;}
      if (rightPressed){yPos++;}
      if (leftPressed){yPos--;}
      if (xPressed){zPos++;}
      if (squarePressed){zPos--;}
      if (startPressed){background(0,0,0);}
      constrain(xPos,1,6);
      constrain(yPos,1,6);
      constrain(zPos,1,6);
//      Serial.println("this");
//      Serial.print(xPos,3);
//      delay(200);
//      Serial.print(", ");
//      delay(200);
//      Serial.print(yPos,3);
//      delay(200);
//      Serial.print(", ");
//     delay(200);
//      Serial.print(zPos,3);
//      delay(200);
//      Serial.print("\n");
//      delay(200);
      incrementHue(.005);
      updateHue();
    }
    
    void updateRandomSpatter(){ //called to update the buggy for the next frame; put what the buggy should do per frame in here
      if(frameCounter%3==0){fade(1);}//fade every 3th frame
      incrementHue(.003);
      updateHue();
      xPos=random(bulbsPerSide);
      yPos=random(bulbsPerSide);
      zPos=random(bulbsPerSide);

    }
    
    void updateRandomWalk(){
      //if(random(10)<2){fade(1);}//i want this but it sets the buggy position to the corner. why?
      //if(random(10)<2){background(0,0,0);}
      //background(0,0,0);
      incrementHue(.005);
      updateHue();
      float r=random(1,7);
      if(r==1){xPos++;}
      if(r==2){xPos--;}
      if(r==3){yPos++;}
      if(r==4){yPos--;}
      if(r==5){zPos++;}
      if(r==6){zPos--;}
      //xpos=xpos+random(-1,2);
      //ypos=ypos+random(-1,2);
      //zpos=zpos+random(-1,2);
      //OR TRY THIS MAYBE?
      //if(random(1,7)==1){xpos++;}
      //if(random(1,7)==1){xpos--;}
      //if(random(1,7)==1){ypos++;}
      //if(random(1,7)==1){ypos--;}
      //if(random(1,7)==1){zpos++;}
      //if(random(1,7)==1){zpos--;}
      xPos=constrain(xPos,0,bulbsPerSide-1);
      yPos=constrain(yPos,0,bulbsPerSide-1);
      zPos=constrain(zPos,0,bulbsPerSide-1);
    }
    
    
};
Buggy MyBuggy;


