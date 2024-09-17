#include<Servo.h>
#include"command.h"
#define LED
//Serial Communication
#define baud_rate 57600

//Acceleration
#define accel 10

//Define Thrusters
// #undef THRUSTER1
// #undef THURSTER2
#define THRUSTER1
#define THRUSTER2

//LED
#ifdef LED
  #define led 13
#endif  
//Servo
Servo servo;
//Thrusters
#ifdef THRUSTER1
  Servo thruster1;
  #define thruster1_pin 9
  #define thrust1_normal 1000 
#endif  

#ifdef THRUSTER2
  Servo thruster2;
  #define thruster2_pin 10
  #define thrust2_normal 1000
#endif


//---------------Variables---------------
//Thrust varibale
int thrust1 = 1000;
int thrust2 = 1000;
//Serial parsing variables

int arg = 0;
int index = 0;

//Serial read variable
char chr;

//Command variable
char cmd;

// Character arrays to hold the first and second arguments
char argv1[16];
char argv2[16];

// The arguments converted to integers
long arg1;
long arg2;
int mapped_args1;
int mapped_args2;


float map_float(float x,float x1,float x2,long y1,long y2 ){

  return (x-x1)*(y2-y1)/(x2-x1) + (float)y1;
}

void resetCommand() {
  cmd = NULL;
  memset(argv1, 0, sizeof(argv1));
  memset(argv2, 0, sizeof(argv2));
  arg1 = 0;
  arg2 = 0;
  arg = 0;
  index = 0;
}
#ifdef THRUSTER1 && THRUSTER2
void forward_thrust(long args1,long args2){
  mapped_args1 = map(args1,-10,10,1300,1700);
  mapped_args2 = map(args2,-10,10,1300,1700);
  // Serial.println(mapped_args1);
  // Serial.println(mapped_args2);
  // while(thrust1<args1 || thrust2<args2){

  //   if(thrust1<args1){
  //     thrust1 += accel;
  //   }
  //   if(thrust2<args2){
  //     thrust2 += accel;
  //   }
  if(thrust1<mapped_args1 && thrust2<mapped_args2){
    while(thrust1<mapped_args1 || thrust2<mapped_args2){
          if(thrust1<mapped_args1){
            thruster1.writeMicroseconds(thrust1);
            thrust1 += accel;
            
          }
          if(thrust2<mapped_args2){
            thruster2.writeMicroseconds(thrust2);  
            thrust2 += accel;
            
          }
        delay(50);
    }
  }
    else if(thrust1>mapped_args1 && thrust2>mapped_args2){
    while(thrust1>mapped_args1 || thrust2>mapped_args2){
          if(thrust1>mapped_args1){
            thruster1.writeMicroseconds(thrust1);
            thrust1 -= accel;
            
          }
          if(thrust2>mapped_args2){
            thruster2.writeMicroseconds(thrust2);  
            thrust2 -= accel;
            
          }
        delay(50);
    }
  }

  else{
    thrust1 = mapped_args1;
    thrust2 = mapped_args2;
    thruster1.writeMicroseconds(thrust1);
    thruster2.writeMicroseconds(thrust2);  
      }
    
  }

void stop(){
  servo.write(170);
  delay(800);
  thrust1=1000;
  thrust2=1000;
    thruster1.writeMicroseconds(thrust1);
    thruster2.writeMicroseconds(thrust2);  


}
#endif

int run(){
  arg1 = atoi(argv1);
  arg2 = atoi(argv2);

  switch(cmd){
    case GET_BAUDRATE:
      Serial.println(baud_rate);
      break;
  #ifdef THRUSTER1 && THRUSTER2
    case FORWARD:
      forward_thrust(arg1,arg2);
      Serial.println("ACTIVATED");
      break;
  
    case MOTOR_COMMAND:
      Serial.println(thrust1);
      Serial.println(thrust2);
      break;
    case STOP:
      stop();
      Serial.println("STOPPED");
      break;  
  #endif

  #ifdef LED
    case ON:
      digitalWrite(led,1);
      break;
    case OFF:
      digitalWrite(led,0);
      break;
  #endif  
  default:
    Serial.println("Invalid Command");
    break;

  }

}

void setup(){

Serial.begin(baud_rate);
servo.attach(11);
servo.write(0);

#ifdef THRUSTER1
  thruster1.attach(thruster1_pin);
  thruster1.writeMicroseconds(thrust1_normal);
#endif  

#ifdef THRUSTER2
  thruster2.attach(thruster2_pin);
  thruster2.writeMicroseconds(thrust2_normal);
#endif  

#ifdef LED
  pinMode(led,OUTPUT);
#endif

Serial.println("STARTING");
//Serial.write("STARTING");
delay(7000);
Serial.println("OK");
//Serial.write("OK");


}


void loop(){

  while (Serial.available() > 0){

    chr = Serial.read();
    // Serial.println(chr);
        if (chr == 13) {
      if (arg == 1) argv1[index] = NULL;
      else if (arg == 2) argv2[index] = NULL;
      run();
      resetCommand();
    }
    // Use spaces to delimit parts of the command
    else if (chr == ' ') {
      // Step through the arguments
      if (arg == 0) arg = 1;
      else if (arg == 1)  {
        argv1[index] = NULL;
        arg = 2;
        index = 0;
      }
      continue;
    }
    else {
      if (arg == 0) {
        // The first arg is the single-letter command
        cmd = chr;
      }
      else if (arg == 1) {
        // Subsequent arguments can be more than one character
        argv1[index] = chr;
        index++;
      }
      else if (arg == 2) {
        argv2[index] = chr;
        index++;
      }
    }
  }



}




