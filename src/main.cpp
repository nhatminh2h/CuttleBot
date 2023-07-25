#include <Arduino.h>
#include <Adafruit_INA219.h>
#include <HX711.h>

#define LEDFORVIDEO 45
#define GREENLEDFORVIDEO 43
#define BLUELEDFORVIDEO 50
#define LUT_SIZE 512

/*#ifdef ARDUINO_ARCH_ESP32
#define RIGHT_1_A   22
#define RIGHT_1_B   23
#define RIGHT_1_PWM 6

#define RIGHT_2_A   24
#define RIGHT_2_B   25
#define RIGHT_2_PWM 7

#define RIGHT_3_A   26
#define RIGHT_3_B   27
#define RIGHT_3_PWM 8

#define RIGHT_4_A   28
#define RIGHT_4_B   29
#define RIGHT_4_PWM 9

#define LEFT_1_A   36
#define LEFT_1_B   37
#define LEFT_1_PWM 2

#define LEFT_2_A   34
#define LEFT_2_B   35
#define LEFT_2_PWM 3

#define LEFT_3_A   32
#define LEFT_3_B   33
#define LEFT_3_PWM 4

#define LEFT_4_A   30
#define LEFT_4_B   31
#define LEFT_4_PWM 5*/
//#else
#ifdef ARDUINO_AVR_MEGA2560
#define RIGHT_1_A   22
#define RIGHT_1_B   23
#define RIGHT_1_PWM 6

#define RIGHT_2_A   24
#define RIGHT_2_B   25
#define RIGHT_2_PWM 7

#define RIGHT_3_A   26
#define RIGHT_3_B   27
#define RIGHT_3_PWM 8

#define RIGHT_4_A   28
#define RIGHT_4_B   29
#define RIGHT_4_PWM 9

#define LEFT_1_A   36
#define LEFT_1_B   37
#define LEFT_1_PWM 2

#define LEFT_2_A   34
#define LEFT_2_B   35
#define LEFT_2_PWM 3

#define LEFT_3_A   32
#define LEFT_3_B   33
#define LEFT_3_PWM 4

#define LEFT_4_A   30
#define LEFT_4_B   31
#define LEFT_4_PWM 5
#endif

Adafruit_INA219 ina219;//current sensor
float datalog[LUT_SIZE][2];
uint16_t logPosition = 0;

HX711 scale;//load cell
const int LOADCELL_DOUT_PIN = 20;
const int LOADCELL_SCK_PIN = 21;

void flashGREEN(){
  digitalWrite(GREENLEDFORVIDEO, HIGH);
  delay(5);
  digitalWrite(GREENLEDFORVIDEO,LOW);
}


void flashGREEN(int time){
  digitalWrite(GREENLEDFORVIDEO, HIGH);
  delay(time);
  digitalWrite(GREENLEDFORVIDEO,LOW);
}

void flashRED(){
  digitalWrite(LEDFORVIDEO, HIGH);
  delay(5); //was 2 ms
  digitalWrite(LEDFORVIDEO,LOW);
}

void flashRED(int time){
  digitalWrite(LEDFORVIDEO, HIGH);
  delay(time);
  digitalWrite(LEDFORVIDEO,LOW);
}

void flashBLUE(){
  digitalWrite(BLUELEDFORVIDEO, HIGH);
  delay(5); //was 2 ms
  digitalWrite(BLUELEDFORVIDEO,LOW);
}

void flashBLUE(int time){
  digitalWrite(BLUELEDFORVIDEO, HIGH);
  delay(time); //was 2 ms
  digitalWrite(BLUELEDFORVIDEO,LOW);
}

void currentSense(int samples){
  float current = 0;
  for(int j = 0; j <samples;j++){
    current += ina219.getCurrent_mA();
  }
  current = current/samples;
  Serial.println(current);
  //Serial.print(" ;");
  //Serial.println(ina219.getPower_mW());
}

void currentSenseRecord(){
  datalog[logPosition][0]=ina219.getCurrent_mA();
  datalog[logPosition][1]=ina219.getPower_mW();
  logPosition++;
}

void sendcurrentdata(){
  Serial.print("Amplitude");Serial.print(" ;");
  Serial.print("Current (mA)");Serial.print(" ;");
  Serial.println("Power (mW)");
  for(int i= 0; i <LUT_SIZE;i++){
    Serial.print(i);Serial.print(" ;");
    Serial.print(datalog[i][0]);Serial.print(" ;");
    Serial.println(datalog[i][1]);
  }
}



class finray {
  private:
    int pinA;
    int pinB;
    int pwm;

  public:
    finray() : pinA(0), pinB(0), pwm(0) {
      // Default constructor that initializes member variables to default values
    }

    finray(int pinA_, int pinB_, int pwm_){
      pinA = pinA_;
      pinB = pinB_;
      pwm = pwm_;
      
      pinMode(pinA, OUTPUT);
      pinMode(pinB, OUTPUT);
      pinMode(pwm, OUTPUT);
    }

    void actuatorpush(int amplitude){//max is 255, 8-bit
      digitalWrite(pinA, HIGH);
      digitalWrite(pinB, LOW);
      if (amplitude > 255){//clamp output to 255
        analogWrite(pwm, (255));
        //currentSenseRecord();
      }
      else{
        analogWrite(pwm, amplitude);
        //currentSenseRecord();
      }
    }

    void actuatorpush(){//fast function
      digitalWrite(pinA, HIGH);
      digitalWrite(pinB, LOW);
      analogWrite(pwm, 255);
      //currentSenseRecord(255);
    }

    void actuatorpull(int amplitude){
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, HIGH);
      if (amplitude > 255){
        analogWrite(pwm, (255));
        //currentSenseRecord(-amplitude);
      }
      else{
        analogWrite(pwm, amplitude);
        //currentSenseRecord(-amplitude);
      }
    }

    void actuatorpull(){//fast function
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, HIGH);
      analogWrite(pwm, 255);
      //currentSenseRecord(-255);
    }

    void actuatoroff(){
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, LOW);
    }

    void actuatorautomatic(int amplitude){
      amplitude = amplitude - 256;//take in value from LUT 512(0 to 512) and shift down to -255 to 255

      if(amplitude > 0){
        actuatorpush(amplitude);
      }
      //else if (amplitude == 0){
      //  actuatoroff();
      //}
      else {
        amplitude = -amplitude;
        actuatorpull(amplitude);
      }
    }

    void actuatorvariedamplitude(int amplitude, float multiplier){
        amplitude = amplitude - 256;//take in value from LUT 512(0 to 512) and shift down to -255 to 255

      if(amplitude > 0){
        actuatorpush(amplitude*multiplier);
      }
      //else if (amplitude == 0){
      //  actuatoroff();
      //}
      else {
        amplitude = -amplitude;
        actuatorpull(amplitude*multiplier);
      }
    }

    //need function with automatic flazpping
};

class fin : public finray{ //class for entire fin
  private:
    finray finray_array[4];

  public:
    fin(finray fr1, finray fr2, finray fr3, finray fr4) : finray_array{fr1, fr2, fr3, fr4} {
      // Constructor that takes four finray objects as arguments
    }


};
    
inline int wavetophase(int wave_in_cm){
  return (3.3 *511/wave_in_cm);
}

inline int frequencytoMicroSec(float frequencyinHz){
  return (1000000/512/frequencyinHz);
}

void setup() {
  // put your setup code here, to run once:

  delay(2000);
  Serial.begin(500000);

  pinMode(LEDFORVIDEO,OUTPUT);
  pinMode(GREENLEDFORVIDEO,OUTPUT);
  pinMode(BLUELEDFORVIDEO,OUTPUT);

  //Initialise the load cell
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN,128);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());			// print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));  	// print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);	// print the average of 5 readings from the ADC minus tare weight (not set) divided
						// by the SCALE parameter (not set yet)

  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();				        // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
						// by the SCALE parameter set with set_scale


  // Initialize the INA219.
  /*  while (ina219.begin()== false) {
    Serial.println("Failed to find INA219 chip");
    delay(1000);
  }*/
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).

  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();

}

const int sineLUT512[] = //values between 0-512
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
/*finray R1(22,23, 6);
finray R2(24,25, 7);
finray R3(26,27, 8);
finray R4(28,29, 9);*/

finray R1(RIGHT_1_A, RIGHT_1_B, RIGHT_1_PWM);
finray R2(RIGHT_2_A, RIGHT_2_B, RIGHT_2_PWM);
finray R3(RIGHT_3_A, RIGHT_3_B, RIGHT_3_PWM);
finray R4(RIGHT_4_A, RIGHT_4_B, RIGHT_4_PWM);

finray R[]={R1,R2,R3, R4};

//left fin
/*finray L1(36,37, 2);
finray L2(34,35, 3);
finray L3(32,33, 4);
finray L4(30,31, 5);*/

finray L1(LEFT_1_A, LEFT_1_B, LEFT_1_PWM);
finray L2(LEFT_2_A, LEFT_2_B, LEFT_2_PWM);
finray L3(LEFT_3_A, LEFT_3_B, LEFT_3_PWM);
finray L4(LEFT_4_A, LEFT_4_B, LEFT_4_PWM);

finray L[]={L1,L2,L3,L4};

//------------------------------VARIABLES DECLARATIONS------------------------------
int phase = 60;
int delaytime = 900;
float finMultiplierR[4] = {1,1,1,1}; //value between 0 and 1
float finMultiplierL[4] = {1,1,1,1};

//------------------------------WAVEFORMS------------------------------


float uniform[4] = {1,1,1,1};
float ascendingtriangle[4]= {0.3,0.4,0.6,1};
float descendingtriangle[4] = {1,0.6,0.4,0.3};
float bowtie[4] = {1,0.5,0.5,1};
float diamond[4] = {0.5,1,1,0.5};
float* waveshape[] = {uniform, ascendingtriangle, descendingtriangle, bowtie, diamond};




//------------------------------SERVICE Functions------------------------------
void initialsetup(){
  for(int n = 0; n<=3; n++){
    R[n].actuatorpush();
    delay(800);
    R[n].actuatorpull();
    delay(1500);
    R[n].actuatoroff();

    delay(200);

    L[n].actuatorpush();
    delay(800);
    L[n].actuatorpull();
    delay(1500);
    L[n].actuatoroff();
  }
}
void alloff();
void curvemeasurement(){
  //Serial.println("Amplitude ; Current (mA)");
  int value[] = {255,221,187,153,119,85,51};
  float freq[] = {
  1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10,
  10.5,11,11.5,12,12.5,13,13.5,14,14.5,15,15.5,16,16.5,17,17.5,18,18.5,19,19.5,20,
  20.5,21,21.5,22,22.5,23,23.5,24,24.5,25,25.5,26,26.5,27,27.5,28,28.5,29,29.5,30};

  for(uint16_t k = 0; k<(sizeof(value)/sizeof(value[0]));k++){//change voltage/current
    //Serial.print("Voltage: "); Serial.print(value[k]*12/255); Serial.println(" V");
    flashBLUE();
    delay(5);
    for(uint16_t i = 0; i<(sizeof(freq)/sizeof(freq[0])); i++){//change frequency
      flashGREEN();
      delay(5);
      flashRED();// record initial position
      //Serial.print("Frequency: "); Serial.print(freq[i]); Serial.println(" Hz");
      for(uint8_t actuation = 0; actuation < 5; actuation++){//5 actuation cycles
        //delay(1000);
        L1.actuatorpush(value[k]);
        
        //Serial.print(value[k]); Serial.print(" ;");

        //delayMicroseconds(50);
        //currentSense(10);
        delay((1000/freq[i])-6);//taking away the LED delay
        flashRED();// record push position

        /*for(int n = 0; n<=3; n++){
          R[n].actuatorpush(value[k]);
        }*/
        //delay(1000);

        
        L1.actuatorpull(value[k]);
        
        //Serial.print(-value[k]); Serial.print(" ;");

        //delayMicroseconds(50);
        //currentSense(10);
        delay((1000/freq[i])-6);//taking away the LED delay
        flashRED();// record pull position

        /*for(int n = 0; n<=3; n++){
          R[n].actuatorpull(value[k]);
        }*/
      }
      L1.actuatoroff();
      delay(500);//wait half a second for the beam to restart
    }
  }
  alloff();
  while(1){delay(1000);}
}

void curvemeasurementSine(){
  //Serial.println("Amplitude ; Current (mA)");
  float freq[] = {
  1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10,
  10.5,11,11.5,12,12.5,13,13.5,14,14.5,15,15.5,16,16.5,17,17.5,18,18.5,19,19.5,20,
  20.5,21,21.5,22,22.5,23,23.5,24,24.5,25,25.5,26,26.5,27,27.5,28,28.5,29,29.5,30};
  flashBLUE();
  for(uint16_t i = 0; i<(sizeof(freq)/sizeof(freq[0])); i++){//change frequency
    flashGREEN();
    delay(5);
    flashRED();// record initial position
    //Serial.print("Frequency: "); Serial.print(freq[i]); Serial.println(" Hz");
    for(uint8_t actuation = 0; actuation < 5; actuation++){//5 actuation cycles
        //delay(1000);
        for (int amplitude = 0; amplitude < 128; amplitude ++){
          L1.actuatorautomatic(sineLUT512[amplitude%512]);
          delayMicroseconds(1000000/256/freq[i]);
        }

        flashRED();
        for (int amplitude = 128; amplitude < 384; amplitude ++){
          L1.actuatorautomatic(sineLUT512[amplitude%512]);
          delayMicroseconds(1000000/256/freq[i]);
        }
        //Serial.print(value[k]); Serial.print(" ;");

        //delayMicroseconds(50);
        //currentSense(10);
        flashRED();

        for (int amplitude = 384; amplitude < 512; amplitude ++){
          L1.actuatorautomatic(sineLUT512[amplitude%512]);
          delayMicroseconds(1000000/256/freq[i]);
        }
        /*for(int n = 0; n<=3; n++){
          R[n].actuatorpush(value[k]);
        }*/
        //delay(1000);
        //Serial.print(-value[k]); Serial.print(" ;");

        //delayMicroseconds(50);
        //currentSense(10);

        /*for(int n = 0; n<=3; n++){
          R[n].actuatorpull(value[k]);
        }*/
    }
    L1.actuatoroff();
    delay(500);//wait half a second for the beam to restart
  }

  alloff();
  while(1){delay(1000);}
}

void curvemeasurementSineALL(){
  //Serial.println("Amplitude ; Current (mA)");
  float freq[] = {
  1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10,
  10.5,11,11.5,12,12.5,13,13.5,14,14.5,15,15.5,16,16.5,17,17.5,18,18.5,19,19.5,20,
  20.5,21,21.5,22,22.5,23,23.5,24,24.5,25,25.5,26,26.5,27,27.5,28,28.5,29,29.5,30};
  flashBLUE();
  for(uint16_t i = 0; i<(sizeof(freq)/sizeof(freq[0])); i++){//change frequency
    flashGREEN();
    delay(5);
    flashRED();// record initial position
    //Serial.print("Frequency: "); Serial.print(freq[i]); Serial.println(" Hz");
    for(uint8_t actuation = 0; actuation < 5; actuation++){//5 actuation cycles
        //delay(1000);
        for (int amplitude = 0; amplitude < 128; amplitude ++){
          for (int n = 0; n < 4; n++){
            L[n].actuatorautomatic(sineLUT512[amplitude%512]);
            R[n].actuatorautomatic(sineLUT512[amplitude%512]);
          }
          
          delayMicroseconds(1000000/256/freq[i]);
        }

        flashRED();
        for (int amplitude = 128; amplitude < 384; amplitude ++){
          for (int n = 0; n < 4; n++){
            L[n].actuatorautomatic(sineLUT512[amplitude%512]);
            R[n].actuatorautomatic(sineLUT512[amplitude%512]);
          }
          delayMicroseconds(1000000/256/freq[i]);
        }
        //Serial.print(value[k]); Serial.print(" ;");

        //delayMicroseconds(50);
        //currentSense(10);
        flashRED();

        for (int amplitude = 384; amplitude < 512; amplitude ++){
          L1.actuatorautomatic(sineLUT512[amplitude%512]);
          delayMicroseconds(1000000/256/freq[i]);
        }
        /*for(int n = 0; n<=3; n++){
          R[n].actuatorpush(value[k]);
        }*/
        //delay(1000);
        //Serial.print(-value[k]); Serial.print(" ;");

        //delayMicroseconds(50);
        //currentSense(10);

        /*for(int n = 0; n<=3; n++){
          R[n].actuatorpull(value[k]);
        }*/
    }
    L1.actuatoroff();
    delay(500);//wait half a second for the beam to restart
  }

  alloff();
  while(1){delay(1000);}
}

void sinusoidalCurrent(int delaytime){
  for (int amplitude = 0; amplitude < 512; amplitude ++){
    if(amplitude == 512 || amplitude == 0){
      flashRED();
    }
    L1.actuatorautomatic(sineLUT512[amplitude%512]);
    currentSenseRecord();
    delayMicroseconds(delaytime);
  }
}


//------------------------------LOCOMOTION Functions------------------------------
void rightoff(){
  for(int n = 0; n<=3; n++){
      R[n].actuatoroff();
  }
}
void leftoff(){
    for(int n = 0; n<=3; n++){
      L[n].actuatoroff();
  }
}
void alloff(){
    for(int n = 0; n<=3; n++){
      R[n].actuatoroff();
      L[n].actuatoroff();
    }
}

void bothflapping(int pushtime, int pulltime, int amplitude){ //delay time in microseconds
  for(int n = 0; n<=3; n++){
    R[n].actuatorpush(amplitude);
    L[n].actuatorpush(amplitude);
  }

  //alloff();
  delay(pushtime);

  for(int n = 0; n<=3; n++){
    R[n].actuatorpull(amplitude);
    L[n].actuatorpull(amplitude);
  }

  //alloff();
  delay(pulltime);
}

void rightundulation(int phase, int delaytime){
for (int amplitude = 0; amplitude < 512; amplitude ++){
    R1.actuatorautomatic(sineLUT512[amplitude%512]);
    R2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
    R3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
    R4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);
    delayMicroseconds(delaytime);
  }
}

void leftundulation(int phase, int delaytime){
for (int amplitude = 0; amplitude < 512; amplitude ++){
    rightoff();
    L1.actuatorautomatic(sineLUT512[amplitude%512]);
    L2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
    L3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
    L4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);
    delayMicroseconds(delaytime);
  }
}

void undulationPhaseDiff(int phase, int phasediff,int delaytime, bool directionForward){
  if (directionForward){
    for (int amplitude = 0; amplitude < 511; amplitude += 1){
    R1.actuatorautomatic(sineLUT512[amplitude%512]);
    R2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
    R3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
    R4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);

    L1.actuatorautomatic(sineLUT512[amplitude - phasediff %512]);
    L2.actuatorautomatic(sineLUT512[(amplitude + phase - phasediff)%512]);
    L3.actuatorautomatic(sineLUT512[(amplitude + (phase*2) - phasediff)%512]);
    L4.actuatorautomatic(sineLUT512[(amplitude + (phase*3) - phasediff)%512]);
    delayMicroseconds(delaytime);
    }
  }
  else{
    for (int amplitude =511 ; amplitude >= 0; amplitude -= 1){
      R1.actuatorautomatic(sineLUT512[amplitude%512]);
      R2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
      R3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
      R4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);

      L1.actuatorautomatic(sineLUT512[amplitude - phasediff %512]);
      L2.actuatorautomatic(sineLUT512[(amplitude + phase - phasediff)%512]);
      L3.actuatorautomatic(sineLUT512[(amplitude + (phase*2) - phasediff)%512]);
      L4.actuatorautomatic(sineLUT512[(amplitude + (phase*3) - phasediff)%512]);
      delayMicroseconds(delaytime);
    }
  }


}

void undulationLUT(int phase, int delaytime){//function will execute one full sine wave
  //cycle through the LUT to set the amplitude
  for (int amplitude = 0; amplitude < 512; amplitude ++){
    R1.actuatorautomatic(sineLUT512[amplitude%512]);
    R2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
    R3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
    R4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);

    L1.actuatorautomatic(sineLUT512[amplitude%512]);
    L2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
    L3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
    L4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);
    delayMicroseconds(delaytime);
  }
}

/*void undulationLUT(int phase, int delaytime){//function will execute one full sine wave
  //cycle through the LUT to set the amplitude
  for (int amplitude = 0; amplitude < 512; amplitude ++){
    for (int i =0; i<4;i++){
      R[i].actuatorautomatic(sineLUT512[(amplitude + phase*i)%512]);
      L[i].actuatorautomatic(sineLUT512[(amplitude + phase*i)%512]);
    }
    delayMicroseconds(delaytime);
  }
}*/

void undulationLUTpartial(int phase, int delaytime, int startPoint, int stopPoint){//function will execute a partial sinewave
  //cycle through the LUT to set the amplitude
  for (int amplitude = startPoint; amplitude < stopPoint; amplitude ++){
    R1.actuatorautomatic(sineLUT512[amplitude%512]);
    R2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
    R3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
    R4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);

    L1.actuatorautomatic(sineLUT512[amplitude%512]);
    L2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
    L3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
    L4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);
    delayMicroseconds(delaytime);
  }
}

void undulationLUTmultiplier(int phase, int delaytime, float multiplierR[4], float multiplierL[4]){//cycle through the LUT to set the amplitude
  for (int amplitude = 0; amplitude < 512; amplitude ++){
    R1.actuatorvariedamplitude(sineLUT512[amplitude%512],multiplierR[0]);
    R2.actuatorvariedamplitude((sineLUT512[(amplitude + phase)%512]),multiplierR[1]);
    R3.actuatorvariedamplitude((sineLUT512[(amplitude + (phase*2))%512]),multiplierR[2]);
    R4.actuatorvariedamplitude((sineLUT512[(amplitude + (phase*3))%512]),multiplierR[3]);

    L1.actuatorvariedamplitude(sineLUT512[amplitude%512],multiplierL[0]);
    L2.actuatorvariedamplitude((sineLUT512[(amplitude + phase)%512]),multiplierL[1]);
    L3.actuatorvariedamplitude((sineLUT512[(amplitude + (phase*2))%512]),multiplierL[2]);
    L4.actuatorvariedamplitude((sineLUT512[(amplitude + (phase*3))%512]),multiplierL[3]);
    delayMicroseconds(delaytime);
  }
}

void ReverseUndulationLUT(int phase, int delaytime){//cycle through the LUT to set the amplitude
  for (int amplitude = 0; amplitude < 512; amplitude ++){
    R4.actuatorautomatic(sineLUT512[amplitude%512]);
    R3.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
    R2.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
    R1.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);

    L4.actuatorautomatic(sineLUT512[amplitude%512]);
    L3.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
    L2.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
    L1.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);
    delayMicroseconds(delaytime);
  }
}

void undulationLUTrotated(int phase, int delaytime, int angle) {
  // Convert angle to radians
  float angleRad = angle * PI / 180.0;

  // Calculate rotation factors for each actuator ray
  int rotationFactorR[4];
  int rotationFactorL[4];

  for (int i = 0; i < 4; i++) {
    float rotatedAngle = angleRad + i * PI / 2;
    rotationFactorR[i] = static_cast<int>((512.0 * rotatedAngle) / (2 * PI));
    rotationFactorL[i] = static_cast<int>((512.0 * (rotatedAngle + PI)) / (2 * PI));
  }

  for (int amplitude = 0; amplitude < 512; amplitude++) {
    for (int i = 0; i < 4; i++) {
      int rotatedPhaseR = (phase*i + rotationFactorR[i] * amplitude) % 512;
      int rotatedPhaseL = (phase*i + rotationFactorL[i] * amplitude) % 512;

      R[i].actuatorautomatic(sineLUT512[rotatedPhaseR]);
      L[i].actuatorautomatic(sineLUT512[rotatedPhaseL]);
    }
    delay(1000);
  }
  alloff();
}

void undulationLUTadjustedBaseline(int phase, int delaytime){
  //int baseline = [255,192,128,64];
  //int baseline_a = [255,170,85,0];
  for (int amplitude = 0; amplitude < 512; amplitude++) {

    R1.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
    R2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]/2+192);
    R3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]/2+128);
    R4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]/2+64);

    L1.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
    L2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]/2+192);
    L3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]/2+128);
    L4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]/2+64);
    delayMicroseconds(delaytime);
  }
}

void REverseundulationLUTadjustedBaseline(int phase, int delaytime){
  //int baseline = [255,192,128,64];
  //int baseline_a = [255,170,85,0];
  for (int amplitude = 512; amplitude >=0; amplitude--) {

    R1.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
    R2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]/2+192);
    R3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]/2+128);
    R4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]/2+64);

    L1.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
    L2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]/2+192);
    L3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]/2+128);
    L4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]/2+64);
    delayMicroseconds(delaytime);
  }
}

void undulationLUTadjustedBaseline_alt(int phase, int delaytime){
  //int baseline = [255,192,128,64];
  //int baseline_a = [255,170,85,0];
  for (int amplitude = 0; amplitude < 512; amplitude++) {

    R4.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
    R3.actuatorautomatic(sineLUT512[(amplitude + phase)%512]/2+192);
    R2.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]/2+128);
    R1.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]/2+64);

    L4.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
    L3.actuatorautomatic(sineLUT512[(amplitude + phase)%512]/2+192);
    L2.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]/2+128);
    L1.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]/2+64);
    delayMicroseconds(delaytime);
  }
}

void REverseundulationLUTadjustedBaseline_alt(int phase, int delaytime){
  //int baseline = [255,192,128,64];
  //int baseline_a = [255,170,85,0];
  for (int amplitude = 512; amplitude >=0; amplitude--) {

    R4.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
    R3.actuatorautomatic(sineLUT512[(amplitude + phase)%512]/2+192);
    R2.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]/2+128);
    R1.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]/2+64);

    L4.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
    L3.actuatorautomatic(sineLUT512[(amplitude + phase)%512]/2+192);
    L2.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]/2+128);
    L1.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]/2+64);
    delayMicroseconds(delaytime);
  }
}

void bankRight(int phase, int delaytime){
  float baselineR[] = {1,1,0,0};
  float baselineL[] = {0,0,1,1};
  undulationLUTmultiplier(phase,delaytime,baselineR,baselineL);
}

void bankLeft(int phase, int delaytime){
  float baselineR[] = {0,0,1,1};
  float baselineL[] = {1,1,0,0};
  undulationLUTmultiplier(phase,delaytime,baselineR,baselineL);
}

void undulationLUTright(int phase, int delaytime){
  //int baseline = [255,192,128,64];
  //int baseline_a = [255,170,85,0];
  R4.actuatorautomatic(0)  ;
  R3.actuatorautomatic(0);
  R2.actuatorautomatic(0);
  R1.actuatorautomatic(0);

  L4.actuatorautomatic(512);
  L3.actuatorautomatic(512);
  L2.actuatorautomatic(512);
  L1.actuatorautomatic(512)  ;
  for (int amplitude = 0; amplitude < 512; amplitude++) {
    /*
    R4.actuatorautomatic(sineLUT512[(amplitude)%512]/2);
    R3.actuatorautomatic(sineLUT512[(amplitude)%512]/2);
    R2.actuatorautomatic(sineLUT512[(amplitude)%512]/2);
    R1.actuatorautomatic(sineLUT512[(amplitude)%512]/2);*/
    /*
    L1.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);
    L2.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);
    L3.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);
    L4.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);*/
    
    //delayMicroseconds(delaytime);
  }
}

void undulationLUTleft(int phase, int delaytime){
  //int baseline = [255,192,128,64];
  //int baseline_a = [255,170,85,0];
  R4.actuatorautomatic(512);
  R3.actuatorautomatic(512);
  R2.actuatorautomatic(512);
  R1.actuatorautomatic(512);
  
  L4.actuatorautomatic(0);
  L3.actuatorautomatic(0);
  L2.actuatorautomatic(0);
  L1.actuatorautomatic(0);
  
  for (int amplitude = 0; amplitude < 512; amplitude++) {
    /*
    R4.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);
    R3.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);
    R2.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);
    R1.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);*/
    /*
    L1.actuatorautomatic(sineLUT512[(amplitude)%512]/2);
    L2.actuatorautomatic(sineLUT512[(amplitude)%512]/2);
    L3.actuatorautomatic(sineLUT512[(amplitude)%512]/2);
    L4.actuatorautomatic(sineLUT512[(amplitude)%512]/2);*/

    //delayMicroseconds(delaytime);
  }
}

void undulationLUTsink(int delaytime){
  //int baseline = [255,192,128,64];
  //int baseline_a = [255,170,85,0];
  for (int amplitude = 0; amplitude < 512; amplitude++) {

    R4.actuatorautomatic(sineLUT512[amplitude%512]/2+64);
    R3.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);
    R2.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);
    R1.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);

    L1.actuatorautomatic(sineLUT512[amplitude%512]/2+64);
    L2.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);
    L3.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);
    L4.actuatorautomatic(sineLUT512[(amplitude)%512]/2+64);
    delayMicroseconds(delaytime);
  }
}

void undulationLUTfloat(int delaytime, int offset){
  //int baseline = [255,192,128,64];
  //int baseline_a = [255,170,85,0];
  for (int amplitude = 0; amplitude < 512; amplitude++) {

    R4.actuatorautomatic(sineLUT512[amplitude%512]/2+offset);
    R3.actuatorautomatic(sineLUT512[(amplitude)%512]/2+offset);
    R2.actuatorautomatic(sineLUT512[(amplitude)%512]/2+offset);
    R1.actuatorautomatic(sineLUT512[(amplitude)%512]/2+offset);

    L1.actuatorautomatic(sineLUT512[amplitude%512]/2+offset);
    L2.actuatorautomatic(sineLUT512[(amplitude)%512]/2+offset);
    L3.actuatorautomatic(sineLUT512[(amplitude)%512]/2+offset);
    L4.actuatorautomatic(sineLUT512[(amplitude)%512]/2+offset);
    delayMicroseconds(delaytime);
  }
}



void turnright(int phase, int delaytime){
  for (int amplitude = 0; amplitude < 512; amplitude ++){
    R1.actuatorautomatic(sineLUT512[amplitude%512]);
    R2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
    R3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
    R4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);

    L4.actuatorautomatic(sineLUT512[amplitude%512]);
    L3.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
    L2.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
    L1.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);
    delayMicroseconds(delaytime);
  }
}

void turnleft(int phase, int delaytime){
  for (int amplitude = 0; amplitude < 512; amplitude ++){
    R4.actuatorautomatic(sineLUT512[amplitude%512]);
    R3.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
    R2.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
    R1.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);

    L1.actuatorautomatic(sineLUT512[amplitude%512]);
    L2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
    L3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
    L4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);
    delayMicroseconds(delaytime);
  }
}

void verticallift(int amplitude, int pushtime, int pulltime){
  for(int n = 0; n<=3; n++){
    R[n].actuatorpush(amplitude);
    L[n].actuatorpush(amplitude);
  }

  delay(pushtime);
  for(int i = 255; i >=0;i--){
      for(int n = 0; n<=3; n++){
        R[n].actuatorpush(i);
        L[n].actuatorpush(i);
      }
      delayMicroseconds(pulltime*1000/256);
  }
  
}

void diagonal(){
  for (int n = 42; n < 252; n+=42){
    flashGREEN(15);
    delay(5);
    flashRED(15);
    for (int k = 0; k < 4; k++){
      undulationPhaseDiff(wavetophase(31.5),n,frequencytoMicroSec(2),0);
      flashRED(15);
    }
    alloff();
    delay(1000);
    flashRED(15);
    for (int k = 0; k < 4; k++){
      undulationPhaseDiff(wavetophase(31.5),n,frequencytoMicroSec(2),1);
      flashRED(15);
    }
    alloff();
    delay(2000);
  }
  alloff();
  while(1){
  digitalWrite(GREENLEDFORVIDEO, HIGH);
  digitalWrite(LEDFORVIDEO, HIGH);
  digitalWrite(BLUELEDFORVIDEO, HIGH);
  delay(5000);}
}

void floatsink(){
  for (float n = 0.5; n < 4; n += 0.5){
    flashGREEN(15);
    delay(5);
    flashRED(15);
    for (int k = 0; k < 5; k++){
      undulationLUTsink(frequencytoMicroSec(n));
      flashRED(5);
    }
    alloff();
    delay(1000);
    flashRED(15);
    for (int k = 0; k < 15; k++){
      undulationLUTfloat(frequencytoMicroSec(n),230);
      flashRED(5);
    }
    alloff();
    delay(1500);
  }
alloff();
while(1){
digitalWrite(GREENLEDFORVIDEO, HIGH);
digitalWrite(LEDFORVIDEO, HIGH);
digitalWrite(BLUELEDFORVIDEO, HIGH);
delay(5000);}
}

//------------------------------DEMONSTRATION Functions------------------------------

void DemoForwardBackward(int dForward, int dBackward){//number of wavegoing forward
  for(int i = 0; i<dForward; i++){
    undulationLUT(wavetophase(31), 1500);
  }
  alloff();
  delay(400);

  for(int i = 0; i<dBackward; i++){
    ReverseUndulationLUT(wavetophase(31), 1500);
  }
  alloff();
  delay(400);
}

void DemoTurn(int turnamount){
  for(int i = 0; i<turnamount; i++){
    turnleft(phase,delaytime);
  }
  alloff();
  delay(1000);

  for(int i = 0; i<turnamount; i++){
    turnright(phase, delaytime);
  }
  alloff();
  delay(1000);
}

void DemoFlapping(int flapamount, int delaytime){
  for(int i = 0; i<flapamount; i++){
    for(int n = 0; n<=3; n++){
      R[n].actuatorpush();
    }
    alloff();
    delay(delaytime);
    for(int n = 0; n<=3; n++){
      R[n].actuatorpull();    
    }
    alloff();
    delay(delaytime);
  }
  alloff();
  delay(800);

  for(int i = 0; i<flapamount; i++){
    for(int n = 0; n<=3; n++){
      L[n].actuatorpush();
    }
    delay(delaytime);
    for(int n = 0; n<=3; n++){
      L[n].actuatorpull();    
    }
    delay(delaytime);
  }
  alloff();
  delay(1200);
}

void pathDemoSquare(){
  for(int i = 0; i<20; i++){
    undulationLUT(phase, delaytime);
  }
  alloff();
  delay(1000);

  for(int i = 0; i<15; i++){
    for(int n = 0; n<=3; n++){
      R[n].actuatorpush();
    }
    delay(100);
    for(int n = 0; n<=3; n++){
      R[n].actuatorpull();    
    }
    delay(100);
  }
  alloff();
  delay(1000);

  for(int i = 0; i<25; i++){
    ReverseUndulationLUT(phase, delaytime);
  }
  alloff();
  delay(1000);

  for(int i = 0; i<25; i++){
    for(int n = 0; n<=3; n++){
      L[n].actuatorpush();
    }
    delay(100);
    for(int n = 0; n<=3; n++){
      L[n].actuatorpull();    
    }
    delay(100);
  }
  alloff();
  delay(1000);


}

void pathDemoZigZag(){
  //ZigZag Forward
  for(int i = 0; i<3; i++){
    undulationLUT(phase, delaytime);

  }
  alloff();
  delay(1000);

  for(int i = 0; i<6; i++){
    turnleft(phase,delaytime);
  }
  delay(1000);

  for(int i = 0; i<3; i++){
    undulationLUT(phase, delaytime);
  }
  delay(1000);

  for(int i = 0; i<6; i++){
    turnright(phase, delaytime);
  }
  delay(1000);

  for(int i = 0; i<3; i++){
    undulationLUT(phase, delaytime);
  }
  delay(1000);

  //ZigzagBackwards

  for(int i = 0; i<3; i++){
    ReverseUndulationLUT(phase, delaytime);
  }
  delay(1000);

  for(int i = 0; i<6; i++){
    turnleft(phase,delaytime);
  }
  delay(1000);

  for(int i = 0; i<3; i++){
    ReverseUndulationLUT(phase, delaytime);
    //turnleft(phase,delaytime);
  }
  delay(1000);

  for(int i = 0; i<6; i++){
    turnright(phase, delaytime);
  }
  delay(1000);

  for(int i = 0; i<3; i++){
    ReverseUndulationLUT(phase, delaytime);
    //turnleft(phase,delaytime);
  }
  delay(1000);
}

void pathDemoBank(){

  undulationLUT(wavetophase(36),frequencytoMicroSec(2));
  for(int i = 0; i <3; i++){
    bankLeft(wavetophase(36),frequencytoMicroSec(2));
    bankLeft(wavetophase(36),frequencytoMicroSec(2));
    undulationLUT(wavetophase(31),frequencytoMicroSec(2));
  }
  undulationLUT(wavetophase(36),frequencytoMicroSec(2));
  for(int i = 0; i <3; i++){
    //bankRight(wavetophase(36),frequencytoMicroSec(2));
    bankRight(wavetophase(36),frequencytoMicroSec(2));
    bankRight(wavetophase(36),frequencytoMicroSec(2));
    undulationLUT(wavetophase(31),frequencytoMicroSec(2));
  }
  alloff();
  while(1);
}

void pathDemoDive(){

  undulationLUT(wavetophase(36),frequencytoMicroSec(2));
  for(int i = 0; i <6; i++){
    for (int amplitude = 0; amplitude < 512; amplitude ++){
      R4.actuatorautomatic(sineLUT512[amplitude%512]/2+64);
      L4.actuatorautomatic(sineLUT512[amplitude%512]/2+64);
      R3.actuatorautomatic(sineLUT512[amplitude%512]/2+64);
      L3.actuatorautomatic(sineLUT512[amplitude%512]/2+64);
      R2.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
      L2.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
      R1.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
      L1.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
    }
  }
  undulationLUT(wavetophase(31),frequencytoMicroSec(2));
  undulationLUT(wavetophase(31),frequencytoMicroSec(2));
  undulationLUT(wavetophase(31),frequencytoMicroSec(2));
  for(int i = 0; i <6; i++){
    for (int amplitude = 0; amplitude < 512; amplitude ++){
      R1.actuatorautomatic(sineLUT512[amplitude%512]/2+64);
      L1.actuatorautomatic(sineLUT512[amplitude%512]/2+64);
      R2.actuatorautomatic(sineLUT512[amplitude%512]/2+64);
      L2.actuatorautomatic(sineLUT512[amplitude%512]/2+64);
      R3.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
      L3.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
      R4.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
      L4.actuatorautomatic(sineLUT512[amplitude%512]/2+255);
    }
  }
  undulationLUT(wavetophase(31),frequencytoMicroSec(2));
  undulationLUT(wavetophase(31),frequencytoMicroSec(2));
  undulationLUT(wavetophase(31),frequencytoMicroSec(2));
  alloff();
  while(1);
}

void pathDemoALL(){
  //sink
  for (int k = 0; k < 3; k++){
    undulationLUTsink(frequencytoMicroSec(1));
  }
  alloff();
  //right translation
  /*for (int k = 0; k < 15; k++){
    for (int amplitude = 0; amplitude < 512; amplitude ++){
      for (int n = 0; n < 4; n++){
        L[n].actuatorautomatic(sineLUT512[amplitude%512]);
      }
      delayMicroseconds(1300);
    }
  }*/
  undulationLUT(wavetophase(36),frequencytoMicroSec(2));
  undulationLUT(wavetophase(36),frequencytoMicroSec(2));
  //undulationLUT(wavetophase(36),frequencytoMicroSec(2));
  //diagonal
  for (int k = 0; k < 3; k++){
    undulationPhaseDiff(wavetophase(31.5),50,frequencytoMicroSec(2),1);
  }
  //undulationLUT(wavetophase(36),frequencytoMicroSec(1.5));
  undulationLUT(wavetophase(36),frequencytoMicroSec(1.5));
  undulationPhaseDiff(wavetophase(31.5),60,frequencytoMicroSec(2),1);

  alloff();
  //forward
  //forward bank left

  //
}


void wavelength_frequency_sweep(float initialWavelength, float endWavelength, float incrementWavelength, 
float initialFrequency, float endFrequency, float incrementFrequency, int actuationcycle){
  float freq[] = {
  1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10,
  10.5,11,11.5,12,12.5,13,13.5,14};

  for (float wavelength = initialWavelength; wavelength <= endWavelength; wavelength += incrementWavelength){
    flashBLUE(15);
    delay(5);
    phase = wavetophase(wavelength);
    for(uint16_t i = 0; i<(sizeof(freq)/sizeof(freq[0])); i++){
      flashGREEN(15);
      delay(5);
      flashRED(15);// flash to indicate the initial position

      for(int cycle = 0; cycle<actuationcycle;cycle++){
        for (int amplitude = 0; amplitude < 512; amplitude ++){
          if (amplitude==128||amplitude == 384){//flash Red at peaks
            flashRED(15);
          }
          R1.actuatorautomatic(sineLUT512[amplitude%512]);
          R2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
          R3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
          R4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);

          L1.actuatorautomatic(sineLUT512[amplitude%512]*0.70);
          L2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]*0.70);
          L3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]*0.70);
          L4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]*0.70);
          delayMicroseconds(1000000/256/freq[i]);
        }
      }
      alloff();
      flashRED(15);
      delay(1000);

      for(int cycle = 0; cycle<actuationcycle;cycle++){
        for(int amplitude = 511; amplitude >=0; amplitude --){
          if (amplitude==128 || amplitude == 384){//flash Red at peaks
            flashRED(15);
          }
          R1.actuatorautomatic(sineLUT512[amplitude%512]);
          R2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
          R3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
          R4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);

          L1.actuatorautomatic(sineLUT512[amplitude%512]*0.70);
          L2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]*0.70);
          L3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]*0.70);
          L4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]*0.70);
          delayMicroseconds(1000000/256/freq[i]);
        }
      }

      alloff();
      flashRED(15);
      //delay(800);
      delay(4000);
    }
  }
  alloff();
  while(1){
  digitalWrite(GREENLEDFORVIDEO, HIGH);
  digitalWrite(LEDFORVIDEO, HIGH);
  digitalWrite(BLUELEDFORVIDEO, HIGH);
  delay(5000);}
}

void wavelength_frequency_sweep_turn(float initialWavelength, float endWavelength, float incrementWavelength, 
float initialFrequency, float endFrequency, float incrementFrequency, int actuationcycle){
  float freq[] = {
  1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10,
  10.5,11,11.5,12,12.5,13,13.5,14};

  for (float wavelength = initialWavelength; wavelength <= endWavelength; wavelength += incrementWavelength){
    flashBLUE(15);
    delay(5);
    phase = wavetophase(wavelength);
    for(uint16_t i = 0; i<(sizeof(freq)/sizeof(freq[0])); i++){
      flashGREEN(15);
      delay(5);
      flashRED(15);// flash to indicate the initial position

      for(int cycle = 0; cycle<actuationcycle;cycle++){
        for (int amplitude = 0; amplitude < 512; amplitude ++){
          if (amplitude==128||amplitude == 384){//flash Red at peaks
            flashRED(15);
          }
          R1.actuatorautomatic(sineLUT512[amplitude%512]);
          R2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
          R3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
          R4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);

          L4.actuatorautomatic(sineLUT512[amplitude%512]*0.70);
          L3.actuatorautomatic(sineLUT512[(amplitude + phase)%512]*0.70);
          L2.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]*0.70);
          L1.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]*0.70);
          delayMicroseconds(1000000/256/freq[i]);
        }
      }
      alloff();
      flashRED(15);
      delay(500);

      for(int cycle = 0; cycle<actuationcycle;cycle++){
        for(int amplitude = 511; amplitude >=0; amplitude --){
          if (amplitude==128 || amplitude == 384){//flash Red at peaks
            flashRED(15);
          }
          R1.actuatorautomatic(sineLUT512[amplitude%512]);
          R2.actuatorautomatic(sineLUT512[(amplitude + phase)%512]);
          R3.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]);
          R4.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]);

          L4.actuatorautomatic(sineLUT512[amplitude%512]*0.70);
          L3.actuatorautomatic(sineLUT512[(amplitude + phase)%512]*0.70);
          L2.actuatorautomatic(sineLUT512[(amplitude + (phase*2))%512]*0.70);
          L1.actuatorautomatic(sineLUT512[(amplitude + (phase*3))%512]*0.70);
          delayMicroseconds(1000000/256/freq[i]);
        }
      }

      alloff();
      flashRED(15);
      //delay(800);
      delay(4000);
    }
    delay(3000);
  }
  alloff();
  while(1){
  digitalWrite(GREENLEDFORVIDEO, HIGH);
  digitalWrite(LEDFORVIDEO, HIGH);
  digitalWrite(BLUELEDFORVIDEO, HIGH);
  delay(5000);}
}

void frequency_sweep_flapping(float initialFrequency, float endFrequency, float incrementFrequency, int actuationcycle){
  float freq[] = {
  1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10,
  10.5,11,11.5,12,12.5,13,13.5,14};

  for(uint16_t i = 0; i<(sizeof(freq)/sizeof(freq[0])); i++){
      flashGREEN(15);
      delay(5);
      flashRED(15);// flash to indicate the initial position

      for(int cycle = 0; cycle<actuationcycle;cycle++){
        for (int amplitude = 0; amplitude < 512; amplitude ++){
          if (amplitude == 128 || amplitude == 384){//flash Red at peaks
            flashRED(15);
          }
          for (int n = 0; n < 4; n++){
            R[n].actuatorautomatic(sineLUT512[amplitude%512]);
          }
          delayMicroseconds(1000000/256/freq[i]);
        }
      }
      alloff();
      flashRED(15);
      delay(500);

      for(int cycle = 0; cycle<actuationcycle;cycle++){
        for (int amplitude = 0; amplitude < 512; amplitude ++){
          if (amplitude == 128 || amplitude == 384){//flash Red at peaks
            flashRED(15);
          }
          for (int n = 0; n < 4; n++){
            L[n].actuatorautomatic(sineLUT512[amplitude%512]);
          }
          delayMicroseconds(1000000/256/freq[i]);
        }
      }
      alloff();
      flashRED(15);
      //delay(800);
      delay(4000);
  }
  alloff();
  while(1){
  digitalWrite(GREENLEDFORVIDEO, HIGH);
  digitalWrite(LEDFORVIDEO, HIGH);
  digitalWrite(BLUELEDFORVIDEO, HIGH);
  delay(5000);}
}

//0.3 hz is too slow, but there tends to be a range of wavelength for each frequency (vice versa)
//Update: 0.5 is too slow
//2 hz is too high
//1.6 hz is too high
void envelope_wavelength_frequency_sweep(float initialWavelength, float endWavelength, float incrementWavelength, 
    float initialFrequency, float endFrequency, float incrementFrequency, int actuationcycle){//wavelength in cm, frequency in Hz

  for (unsigned int wavetype = 0; wavetype <= sizeof(waveshape); wavetype++){
    for (float wavelength = initialWavelength; wavelength <= endWavelength; wavelength += incrementWavelength){
        for(float frequency = initialFrequency; frequency <= endFrequency; frequency += incrementFrequency){
          for(int i = 0; i < actuationcycle; i++){
            undulationLUTmultiplier(wavetophase(wavelength),(1000/frequency),waveshape[wavetype], waveshape[wavetype]);
          }
          alloff();
          delay(600);
        }
        alloff();
        delay(1200);
    }
  }
  alloff();
  while(1){delay(5000);}

}

void lateral_movement_sweep(float initialFrequency, float endFrequency, float incrementFrequency, int actuationcycle){
  for(float frequency = initialFrequency; frequency <= endFrequency; frequency += incrementFrequency){
    DemoFlapping(actuationcycle,(1000/frequency));
    delay(500);
  }
}
//void rotation_sweep(float ){;}

//------------------------Force Measurements----------------------//
void measureForce(){
  int value[] = {255,221,187,153,119,85,51};
  //int value[] = {255,255,255,255,255,255,255};
  int reading = 0;
  int readingoff = 0;
  Serial.println("Value; Reading 1 on; Reading 1 off; Reading 1 Diff; Reading 2 on; Reading 2 off; Reading 2 Diff;Reading 3 on; Reading 3 off; Reading 3 Diff;");
  for (uint8_t k=0; k<(sizeof(value)/sizeof(value[0]));k++){
    Serial.print(value[k]);
    for (uint8_t j= 0; j<3; j++){
      //scale.tare(5);
      //int delaytime = 1000000/value[k];
      //for (int ramp = 0; ramp <= value[k]; ramp++){
        L4.actuatorpush(value[k]);
        //delayMicroseconds(delaytime);
      //}
      delay(200);
      reading = scale.get_value(1);
      L4.actuatorpull();
      delay(5);
      L4.actuatoroff();
      readingoff = scale.get_value(1);

      Serial.print(";");
      Serial.print(reading);
      Serial.print(";");
      Serial.print(readingoff);
      Serial.print(";");
      Serial.print(reading-readingoff);
      delay(2000);
    }
    Serial.println();
    delay(7000);
  }
  while(1){delay(10000);}
}

// void pitchNsurge(int multiplierR, int multiplierL){
//  undulationLUTalternate(phase, delaytime, finMultiplierR, finMultiplierL);
// }

// manual control


//-------------------------------Jellyfish-----------------------//
void jellyfish_standard(finray fr, int num_cycles, int ontime, int offtime){
  for (int n; n<num_cycles; n++){
    fr.actuatorpull();
    delay(ontime);
    fr.actuatoroff();
    delay(offtime);
  }
}


void loop() {
  while(1){
    //Serial.println(scale.get_value(3));
    delay(1000);
    measureForce();
  }
  // put your main code here, to run repeatedly:

  //initialsetup();


  //leftundulation(60,1000);
  //undulationLUT(wavetophase(36),frequencytoMicroSec(2));
  //ReverseUndulationLUT(wavetophase(32),1300);
  
  
  //DemoForwardBackward(5, 5);

  //bothflapping(400,300,255);
  //verticallift(255, 100, 100);

  //undulationLUTalternate(wavetophase(28),1500,uniform,uniform);
  
  //DemoTurn(10);
  //turnright(30,1500);
  //DemoFlapping(4,200);

  //pathDemoSquare();
  
  //pathDemoZigZag();
  //leftundulation(0,800);


  //--------------------------------SINGLE ACTUATOR CHARACTERISATION---------------------------
  //curvemeasurement();
  //curvemeasurementSine();
  

  //--------------------------------FIN CHARACTERISATION---------------------------------------
  //curvemeasurementSineALL();
        //Straight
  //wavelength_frequency_sweep(5.25,42,5.25,0.5,5,0.5,6);
  

  //--------------------------------SWIMMING CHARACTERISATION----------------------------------
        //Straight
  //wavelength_frequency_sweep(5.25,42,5.25,0.5,5,0.5,6);
  //wavelength_frequency_sweep(10.5,10.5,5.25,0.5,5,0.5,6);
        //Turning
  //wavelength_frequency_sweep_turn(5.25,42,5.25,0.5,5,0.5,6);
        //Lateral
  //frequency_sweep_flapping(1,14,0.5,5);
        //Banking
  //pathDemoBank();
        //Diagonal
  //diagonal();
        //SinkFloat
  //floatsink();

/*
  for(int i = 0; i <10; i++){
    undulationLUTadjustedBaseline_alt(wavetophase(36),frequencytoMicroSec(2));
  }
  for(int i = 0; i <10; i++){
    REverseundulationLUTadjustedBaseline_alt(wavetophase(36),frequencytoMicroSec(2));
  }
  alloff();
  while(1);
  undulationLUT(wavetophase(36),frequencytoMicroSec(2));
  undulationLUT(wavetophase(36),frequencytoMicroSec(2));
  for(int i = 0; i <7; i++){
    //bankRight(wavetophase(36),frequencytoMicroSec(2));
    undulationLUT(wavetophase(36),frequencytoMicroSec(2));
  }*/

  //pathDemoALL();







  //delay(2000);
  /*for (int n = 0; n < 14; n++){
    undulationLUTbank(wavetophase(36.75),frequencytoMicroSec(3));
    flashRED(20);
  } 
  delay(1500);
  for (int n = 0; n < 14; n++){
    REverseundulationLUTbank(wavetophase(36.75),frequencytoMicroSec(3));
    flashGREEN(20);
  } */

 /*
  for (int n = 0; n < 15; n++){
    undulationLUT(wavetophase(36.75),frequencytoMicroSec(3));
    flashRED(20);
    if (n%3 ==0){
      undulationLUTleft(wavetophase(36.75),frequencytoMicroSec(3));
      delay(200);
      alloff();
    }
  }

  delay(1500);
  for (int n = 0; n < 15; n++){
    ReverseUndulationLUT(wavetophase(36.75),frequencytoMicroSec(3));
    flashGREEN(20);
    if (n%3 ==0){
      undulationLUTright(wavetophase(36.75),frequencytoMicroSec(3));
      delay(200);
      alloff();
    }
  }
  //undulationLUTleft(wavetophase(36.75),frequencytoMicroSec(3));
  delay(1500);*/

  //reverseundulationLUT(wavetophase(36.75),frequencytoMicroSec(2));

  //envelope_wavelength_frequency_sweep(10.5,42,6.3,0.3,2.1,0.3,5);
  //lateral_movement_sweep(0.6,4.2,0.4,4);

  //jellyfish_standard(R4,3,500, 1000);
  //delay(3000);
  /*for(int i = 255; i>0;i--){
    R4.actuatorpull(i);
    delayMicroseconds(1200);
  }
    R4.actuatoroff();
    delay(200);
    */

   /*R4.actuatorpull();
   delay(300);
   R4.actuatoroff();
   delay(150);*/
}
