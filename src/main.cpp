#include <Arduino.h>


class finray {
  public:
    finray(int pinA_, int pinB_, int pwm_){
      pinA = pinA_;
      pinB = pinB_;
      pwm = pwm_;
      
      pinMode(pinA, OUTPUT);
      pinMode(pinB, OUTPUT);
      pinMode(pwm, OUTPUT);
    }

    void actuatorhigh(int amplitude){//max is 255, 8-bit
      digitalWrite(pinA, HIGH);
      digitalWrite(pinB, LOW);
      if (amplitude > 255){
        analogWrite(pwm, (amplitude - 255));
      }
      else{
        analogWrite(pwm, amplitude);
      }
    }

    void actuatorhigh(){
      digitalWrite(pinA, HIGH);
      digitalWrite(pinB, LOW);
      analogWrite(pwm, 255);
    }

    void actuatorlow(int amplitude){
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, HIGH);
      if (amplitude > 255){
        analogWrite(pwm, (amplitude - 255));
      }
      else{
        analogWrite(pwm, amplitude);
      }
    }

    void actuatorlow(){
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, HIGH);
      analogWrite(pwm, 255);
    }

    void actuatoroff(){
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, LOW);
    }

    void actuatorautomatic(int amplitude){//take in value from LUT 512 and shift down to -255 to 255
      amplitude = amplitude - 255;

      if(amplitude > 0){
        digitalWrite(pinA, LOW);
        digitalWrite(pinB, HIGH);
        if (amplitude > 255){
          analogWrite(pwm, (amplitude - 255));
        }
        else{
          analogWrite(pwm, amplitude);
        }
      }
      else {
        digitalWrite(pinA, HIGH);
        digitalWrite(pinB, LOW);
        amplitude = -amplitude;
        if (amplitude > 255){
          analogWrite(pwm, (amplitude - 255));
        }
        else{
          analogWrite(pwm, amplitude);
        }
      }
    }

    //need function with automatic flapping
  private:
    int pinA;
    int pinB;
    int pwm;
};

/*class fins : public finray{ //class for entire fin
  public:
    fins(finray _fin[]){
      fin = _fin;
    }

    void undulation(){
      for(auto &f: fin){
        f.actuatorautomatic(sineLUT512[amplitude]);
      }
    }


  private:
    finray fin[];
};*/


void setup() {
  // put your setup code here, to run once:


  Serial.begin(9600);
}

int sineLUT512[] = //values between 0-512
{
256,259,262,265,269,272,275,278,
281,284,287,290,294,297,300,303,
306,309,312,315,318,321,324,327,
330,333,336,339,342,345,348,351,
354,357,360,363,365,368,371,374,
377,379,382,385,388,390,393,396,
398,401,403,406,408,411,413,416,
418,421,423,426,428,430,433,435,
437,439,441,444,446,448,450,452,
454,456,458,460,462,463,465,467,
469,471,472,474,476,477,479,480,
482,483,485,486,487,489,490,491,
493,494,495,496,497,498,499,500,
501,502,503,504,504,505,506,506,
507,508,508,509,509,510,510,510,
511,511,511,512,512,512,512,512,
512,512,512,512,512,512,511,511,
511,510,510,510,509,509,508,508,
507,506,506,505,504,504,503,502,
501,500,499,498,497,496,495,494,
493,491,490,489,487,486,485,483,
482,480,479,477,476,474,472,471,
469,467,465,463,462,460,458,456,
454,452,450,448,446,444,441,439,
437,435,433,430,428,426,423,421,
418,416,413,411,408,406,403,401,
398,396,393,390,388,385,382,379,
377,374,371,368,365,363,360,357,
354,351,348,345,342,339,336,333,
330,327,324,321,318,315,312,309,
306,303,300,297,294,290,287,284,
281,278,275,272,269,265,262,259,
256,253,250,247,243,240,237,234,
231,228,225,222,218,215,212,209,
206,203,200,197,194,191,188,185,
182,179,176,173,170,167,164,161,
158,155,152,149,147,144,141,138,
135,133,130,127,124,122,119,116,
114,111,109,106,104,101,99,96,
94,91,89,86,84,82,79,77,
75,73,71,68,66,64,62,60,
58,56,54,52,50,49,47,45,
43,41,40,38,36,35,33,32,
30,29,27,26,25,23,22,21,
19,18,17,16,15,14,13,12,
11,10,9,8,8,7,6,6,
5,4,4,3,3,2,2,2,
1,1,1,0,0,0,0,0,
0,0,0,0,0,0,1,1,
1,2,2,2,3,3,4,4,
5,6,6,7,8,8,9,10,
11,12,13,14,15,16,17,18,
19,21,22,23,25,26,27,29,
30,32,33,35,36,38,40,41,
43,45,47,49,50,52,54,56,
58,60,62,64,66,68,71,73,
75,77,79,82,84,86,89,91,
94,96,99,101,104,106,109,111,
114,116,119,122,124,127,130,133,
135,138,141,144,147,149,152,155,
158,161,164,167,170,173,176,179,
182,185,188,191,194,197,200,203,
206,209,212,215,218,222,225,228,
231,234,237,240,243,247,250,253,
};

//------------------------------FIN DECLARATIONS------------------------------
//right fin
finray R1(22,23, 6);
finray R2(24,25, 7);
finray R3(26,27, 8);
finray R4(28,29, 9);

finray R[]={R1,R2,R3, R4};

//left fin
finray L1(36,37, 2);
finray L2(34,35, 3);
finray L3(32,33, 4);
finray L4(30,31, 5);

finray L[]={L1,L2,L3,L4};

//------------------------------VARIABLES DECLARATIONS------------------------------
int phase = 60;
int delaytime = 700;
float finMultiplierR[4] = {1,1,1,1}; //value between 0 and 1
float finMultiplierL[4] = {1,1,1,1};


//------------------------------SERVICE Functions------------------------------
void initialsetup(){
  for(int n = 0; n<=3; n++){
    R[n].actuatorhigh();
    delay(1000);
    R[n].actuatorlow();
    delay(2000);

    L[n].actuatorhigh();
    delay(1000);
    L[n].actuatorlow();
    delay(2000);
  }
}

void curvemeasurement(){
  int value[] = {255,204,153,102,51,0};
  for(int k = 0; k<=5;k++){
    delay(3000);
    
    //R3.actuatorhigh(255);

    for(int n = 0; n<=3; n++){
      R[n].actuatorhigh(value[k]);
    }

    delay(3000);
    //R3.actuatorlow(255);
    for(int n = 0; n<=3; n++){
      R[n].actuatorlow(value[k]);
    }
  }
}


//------------------------------LOCOMOTION Functions------------------------------

void undulation(){ //redundant. problem with phase offset.

    for (int amplitude = 0; amplitude <= 255; amplitude++){

    delayMicroseconds(delaytime);
  }

  for (int amplitude = 255; amplitude >= 0; amplitude--){
    R1.actuatorhigh(amplitude);
    R2.actuatorhigh(amplitude + phase);
    R3.actuatorhigh(amplitude + (phase*2));
    R4.actuatorhigh(amplitude + (phase*3));
 
    L1.actuatorhigh(amplitude);
    L2.actuatorhigh(amplitude + phase);
    L3.actuatorhigh(amplitude + (phase * 2));
    L4.actuatorhigh(amplitude + (phase * 3));
    delayMicroseconds(delaytime);
  }

  for (int amplitude = 0; amplitude <= 255; amplitude++){
    R1.actuatorlow(amplitude);
    R2.actuatorlow(amplitude + phase);
    R3.actuatorlow(amplitude + (phase*2));
    R4.actuatorlow(amplitude + (phase*3));
 
    L1.actuatorlow(amplitude);
    L2.actuatorlow(amplitude + phase);
    L3.actuatorlow(amplitude + (phase * 2));
    L4.actuatorlow(amplitude + (phase * 3));
    delayMicroseconds(delaytime);
  }

  for (int amplitude = 255; amplitude >= 0; amplitude--){
    R1.actuatorlow(amplitude);
    R2.actuatorlow(amplitude + phase);
    R3.actuatorlow(amplitude + (phase*2));
    R4.actuatorlow(amplitude + (phase*3));
 
    L1.actuatorlow(amplitude);
    L2.actuatorlow(amplitude + phase);
    L3.actuatorlow(amplitude + (phase * 2));
    L4.actuatorlow(amplitude + (phase * 3));
    delayMicroseconds(delaytime);
  }
}

void uniformflapping(int delaytime, int amplitude){ //delay time in microseconds
  R1.actuatorhigh(amplitude);
  R2.actuatorhigh(amplitude);
  R3.actuatorhigh(amplitude);
  R4.actuatorhigh(amplitude);

  L1.actuatorhigh(amplitude);
  L2.actuatorhigh(amplitude);
  L3.actuatorhigh(amplitude);
  L4.actuatorhigh(amplitude);

  delay(delaytime);

  R1.actuatorlow(amplitude);
  R2.actuatorlow(amplitude);
  R3.actuatorlow(amplitude);
  R4.actuatorlow(amplitude);
  
  L1.actuatorlow(amplitude);
  L2.actuatorlow(amplitude);
  L3.actuatorlow(amplitude);
  L4.actuatorlow(amplitude);


  delay(delaytime);
}

void undulationLUT(int phase, int delaytime){//cycle through the LUT to set the amplitude
  for (int amplitude = 0; amplitude < 512; amplitude ++){
    R1.actuatorautomatic(sineLUT512[amplitude]);
    R2.actuatorautomatic(sineLUT512[amplitude + phase]);
    R3.actuatorautomatic(sineLUT512[amplitude + (phase*2)]);
    R4.actuatorautomatic(sineLUT512[amplitude + (phase*3)]);

    L1.actuatorautomatic(sineLUT512[amplitude]);
    L2.actuatorautomatic(sineLUT512[amplitude + phase]);
    L3.actuatorautomatic(sineLUT512[amplitude + (phase*2)]);
    L4.actuatorautomatic(sineLUT512[amplitude + (phase*3)]);
    delayMicroseconds(delaytime);
  }
}

void undulationLUTalternate(int phase, int delaytime, float multiplierR[4], float multiplierL[4]){//cycle through the LUT to set the amplitude
  for (int amplitude = 0; amplitude < 512; amplitude ++){
    R1.actuatorautomatic(sineLUT512[amplitude] *multiplierR[0]);
    R2.actuatorautomatic((sineLUT512[amplitude + phase]) *multiplierR[1]);
    R3.actuatorautomatic((sineLUT512[amplitude + (phase*2)]) *multiplierR[2]);
    R4.actuatorautomatic((sineLUT512[amplitude + (phase*3)]) *multiplierR[3]);

    L1.actuatorautomatic(sineLUT512[amplitude] *multiplierL[0]);
    L2.actuatorautomatic(sineLUT512[amplitude + phase] *multiplierL[1]);
    L3.actuatorautomatic(sineLUT512[amplitude + (phase*2)] *multiplierL[2]);
    L4.actuatorautomatic(sineLUT512[amplitude + (phase*3)] *multiplierL[3]);
    delayMicroseconds(delaytime);
  }
}

void ReverseUndulationLUT(int phase, int delaytime){//cycle through the LUT to set the amplitude
  for (int amplitude = 0, amplitudeReverse = 512; amplitude < 512; amplitude ++, amplitudeReverse--){
    R1.actuatorautomatic(sineLUT512[amplitudeReverse]);
    R2.actuatorautomatic(sineLUT512[amplitudeReverse + phase]);
    R3.actuatorautomatic(sineLUT512[amplitudeReverse + (phase*2)]);
    R4.actuatorautomatic(sineLUT512[amplitudeReverse + (phase*3)]);

    L1.actuatorautomatic(sineLUT512[amplitudeReverse]);
    L2.actuatorautomatic(sineLUT512[amplitudeReverse + phase]);
    L3.actuatorautomatic(sineLUT512[amplitudeReverse + (phase*2)]);
    L4.actuatorautomatic(sineLUT512[amplitudeReverse + (phase*3)]);
    delayMicroseconds(delaytime);
  }
}

void turnright(int phase, int delaytime){
  for (int amplitude = 0, amplitudeReverse = 512; amplitude < 512; amplitude ++, amplitudeReverse--){
    R1.actuatorautomatic(sineLUT512[amplitude]);
    R2.actuatorautomatic(sineLUT512[amplitude + phase]);
    R3.actuatorautomatic(sineLUT512[amplitude + (phase*2)]);
    R4.actuatorautomatic(sineLUT512[amplitude + (phase*3)]);

    L1.actuatorautomatic(sineLUT512[amplitudeReverse]);
    L2.actuatorautomatic(sineLUT512[amplitudeReverse + phase]);
    L3.actuatorautomatic(sineLUT512[amplitudeReverse + (phase*2)]);
    L4.actuatorautomatic(sineLUT512[amplitudeReverse + (phase*3)]);
    delayMicroseconds(delaytime);
  }
}

void turnleft(int phase, int delaytime){
  for (int amplitude = 0, amplitudeReverse = 512; amplitude < 512; amplitude ++, amplitudeReverse--){
    L1.actuatorautomatic(sineLUT512[amplitude]);
    L2.actuatorautomatic(sineLUT512[amplitude + phase]);
    L3.actuatorautomatic(sineLUT512[amplitude + (phase*2)]);
    L4.actuatorautomatic(sineLUT512[amplitude + (phase*3)]);

    R1.actuatorautomatic(sineLUT512[amplitudeReverse]);
    R2.actuatorautomatic(sineLUT512[amplitudeReverse + phase]);
    R3.actuatorautomatic(sineLUT512[amplitudeReverse + (phase*2)]);
    R4.actuatorautomatic(sineLUT512[amplitudeReverse + (phase*3)]);
    delayMicroseconds(delaytime);
  }
}



//------------------------------DEMONSTRATION Functions------------------------------

void DemoForwardBackward(int dForward, int dBackward){
  for(int i = 0; i<dForward; i++){
    undulationLUT(phase, delaytime);
  }
  delay(1000);

  for(int i = 0; i<dBackward; i++){
    ReverseUndulationLUT(phase, delaytime);
  }
  delay(1000);
}

void DemoTurn(int turnamount){
  for(int i = 0; i<turnamount; i++){
    turnleft(phase,delaytime);
  }
  delay(1000);

  for(int i = 0; i<turnamount; i++){
    turnright(phase, delaytime);
  }
  delay(1000);
}

void DemoFlapping(int flapamount, int flaptime){
  for(int i = 0; i<15; i++){
    for(int n = 0; n<=3; n++){
      R[n].actuatorhigh();
    }
    delay(flaptime);
    for(int n = 0; n<=3; n++){
      R[n].actuatorlow();    
    }
    delay(flaptime);
  }
  delay(1000);

  for(int i = 0; i<20; i++){
    for(int n = 0; n<=3; n++){
      L[n].actuatorhigh();
    }
    delay(flaptime);
    for(int n = 0; n<=3; n++){
      L[n].actuatorlow();    
    }
    delay(flaptime);
  }
  delay(1000);
}

void pathDemoSquare(){
  for(int i = 0; i<20; i++){
    undulationLUT(phase, delaytime);
  }
  delay(1000);

  for(int i = 0; i<15; i++){
    for(int n = 0; n<=3; n++){
      R[n].actuatorhigh();
    }
    delay(100);
    for(int n = 0; n<=3; n++){
      R[n].actuatorlow();    
    }
    delay(100);
  }
  delay(1000);

  for(int i = 0; i<25; i++){
    ReverseUndulationLUT(phase, delaytime);
  }
  delay(1000);

  for(int i = 0; i<25; i++){
    for(int n = 0; n<=3; n++){
      L[n].actuatorhigh();
    }
    delay(100);
    for(int n = 0; n<=3; n++){
      L[n].actuatorlow();    
    }
    delay(100);
  }
  delay(1000);


}

void pathDemoZigZag(){
  //ZigZag Forward
  for(int i = 0; i<20; i++){
    undulationLUT(phase, delaytime);

  }
  delay(1000);

  for(int i = 0; i<20; i++){
    turnleft(phase,delaytime);
  }
  delay(1000);

  for(int i = 0; i<20; i++){
    undulationLUT(phase, delaytime);
  }
  delay(1000);

  for(int i = 0; i<20; i++){
    turnright(phase, delaytime);
  }
  delay(1000);

  for(int i = 0; i<20; i++){
    undulationLUT(phase, delaytime);
  }
  delay(1000);

  //ZigzagBackwards

  for(int i = 0; i<20; i++){
    ReverseUndulationLUT(phase, delaytime);
  }
  delay(1000);

  for(int i = 0; i<20; i++){
    turnleft(phase,delaytime);
  }
  delay(1000);

  for(int i = 0; i<20; i++){
    ReverseUndulationLUT(phase, delaytime);
    //turnleft(phase,delaytime);
  }
  delay(1000);

  for(int i = 0; i<20; i++){
    turnright(phase, delaytime);
  }
  delay(1000);

  for(int i = 0; i<20; i++){
    ReverseUndulationLUT(phase, delaytime);
    //turnleft(phase,delaytime);
  }
  delay(1000);
}

// void pitchNsurge(int multiplierR, int multiplierL){
//  undulationLUTalternate(phase, delaytime, finMultiplierR, finMultiplierL);
// }

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  //pathDemoZigZag();
  //initialsetup();

  //DemoForwardBackward(40, 30);

  //DemoTurn(10);

  //DemoFlapping(20,100);

  //pathDemoSquare();

  //pathDemoZigZag();
}