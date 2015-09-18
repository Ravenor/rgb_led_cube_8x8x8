
void loop(){

  background(0,0,0);
  while(true){//run random spatter animation
    MyBuggy.updateRandomSpatter();//updates my buggy by one step
    MyBuggy.render();
   // Serial.println("abcd");
    updateCube(1);
    if(timeForNextAnimation){break;}
  }
  
  //if (Serial.available()){Serial.write(Serial.read());}
  
  background(0,0,0);
  while(true){//run hue cycle animation
    incrementHue(.005);
    background(HSLr, HSLg, HSLb);
    updateCube(1);
    if(timeForNextAnimation){break;}
  } 
  
  background(0,0,0);
  while(true){//run random walk animation
    MyBuggy.updateRandomWalk();//updates my buggy by one step
    MyBuggy.render();
    updateCube(1);
    if(timeForNextAnimation){break;}
  } 
  
  background(0,0,0);
  while(true){
    background(0,0,0);
    incrementHue(.005);
    line(random(8),random(8),random(8),random(8),random(8),random(8), HSLr, HSLg, HSLb);
    updateCube(1);
    if(timeForNextAnimation){break;}
  } 
  
  background(0,0,0);
  while(true){
    MyBuggy.updateBouncingBall();//updates my buggy by one step
    MyBuggy.render();
    updateCube(1);
    if(timeForNextAnimation){break;}
  } 
  
  background(0,0,0);
  while(true){
    MyBuggy.updateDrive();//updates my buggy by one step
    MyBuggy.render();
    updateCube(1);
    if(timeForNextAnimation){break;}
  } 
  
}

