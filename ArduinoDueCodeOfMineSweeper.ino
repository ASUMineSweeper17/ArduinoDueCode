#include <Wire.h>
//#include <LSM303.h>

//////////////////////////////////////////////////////
//communication
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define ce 6
#define csn 7

bool listenIndicator =true;
String go;
char come;

void trans (String n);
char rec ();

const uint64_t DuePipe = 0xA2A2A2A2B4LL;
const uint64_t UnoPipe = 0xE5E5E5E5E5LL;

RF24 radio(ce,csn);
///////////////////////////////////////////////////////

#define PWM1F 8   //forward right
#define DIR1F 9 
#define DIR2F 10   //forward left
#define PWM2F 11   


#define PWM1B 22    //backword left
#define DIR1B 23
#define DIR2B 24    //backword right
#define PWM2B 25    

//Encoder data
#define encoder_pin 2
#define arcLength 0.78
double x_distance = 0, y_distance = 0;

//Compass data
//LSM303 compass;
// float heading;
float angle = 0;
// float reference;

char old_data;
uint32_t t;



char data = '3'; 
int Speed = 130;


String text = "Sent data is : ";

void setup() {
  // put your setupcode here, to run once:
  pinMode(PWM1F, OUTPUT);
  pinMode(DIR1F, OUTPUT);
  pinMode(PWM2F, OUTPUT);
  pinMode(DIR2F, OUTPUT);
  pinMode(PWM1B, OUTPUT);
  pinMode(DIR1B, OUTPUT);
  pinMode(PWM2B, OUTPUT);
  pinMode(DIR2B, OUTPUT);

  Serial.begin(9600);
  attachInterrupt(encoder_pin, encoderPlus, FALLING);
///////////////////////////////////////////////////////
//Communication Setup
  radio.begin();
  radio.enableAckPayload();
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(DuePipe);
  radio.openReadingPipe(1,UnoPipe);
  radio.startListening();
///////////////////////////////////////////////////////
 
 /* Wire.begin();
  compass.init();
  compass.enableDefault();
  compass.m_min = (LSM303::vector<int16_t>){-791,   -802,  -1046};
  compass.m_max = (LSM303::vector<int16_t>){+673,   +613,   +333};

  compass.read();
  reference = compass.heading();*/ 
}

void loop() {
  // put your main code here, to run repeatedly:
  come=rec();
  while(come=='F'){
    come=rec();
    forward(Speed);
    /*if(mine==1){
      trans(go);
    }*/
  }
}  
  
  /*//calc();
  if(Serial.available()){
    data = Serial.read();
    //Serial.println(data);
    //Serial1.write(data);
  radio.write(&text,sizeof(text));
    radio.write(&data,sizeof(data));
    if(data == '4' || data == '2'){
      t = millis();
      old_data = data;
      }
    if (data == '1' && old_data == '2'){
      old_data = '1';
      t = millis() - t;
      if (t > 250)
        angle += 90;
      
    }
    else if (data == '1' && old_data == '4'){
      old_data = '1';
      t = millis() - t;
      if (t > 250)
        angle += 90;
      
    } 
  }

 switch (data){
    case '1':{
      forward(Speed);
      break;
 }

    case '2':{
      turnRight(Speed);
      break;
    }

    case '4':{
      turnLeft(Speed);
      break;
    }
    case '5':{
      backward(Speed);
      break;
    }

    case '3':{
      stopp();
      break;   
    }
 }

 Serial.print("X Distance = ");
 Serial.print(x_distance);
 Serial.print(",    Y Distance = ");
 Serial.print(y_distance);

 Serial.print(",    Heading = ");
 heading = compass.heading();
 Serial.print(heading);*/
 //Serial.print(",    Angle = ");
 //Serial.println(angle);
 //delay(10);*/


void forward(int Speed){  
  attachInterrupt(2, encoderPlus, FALLING);
  analogWrite(PWM1F, Speed);
  analogWrite(PWM2F, Speed);
  analogWrite(PWM1B, Speed);
  analogWrite(PWM2B, Speed); 
  digitalWrite(DIR1F, HIGH);
  digitalWrite(DIR2F, HIGH);
  digitalWrite(DIR1B, HIGH);
  digitalWrite(DIR2B, HIGH);
  }
  
void backward(int Speed){
  attachInterrupt(2, encoderMinus, FALLING);
  analogWrite(PWM1F, Speed);
  analogWrite(PWM2F, Speed);
  analogWrite(PWM1B, Speed);
  analogWrite(PWM2B, Speed); 
  digitalWrite(DIR1F, LOW);
  digitalWrite(DIR2F, LOW);
  digitalWrite(DIR1B, LOW);
  digitalWrite(DIR2B, LOW);
  }
  
void turnRight(int Speed){
  detachInterrupt(2);
  analogWrite(PWM1F, Speed);
  analogWrite(PWM2F, Speed);
  analogWrite(PWM1B, Speed);
  analogWrite(PWM2B, Speed); 
  digitalWrite(DIR1F, LOW);
  digitalWrite(DIR2F, HIGH);
  digitalWrite(DIR2B, LOW);
  digitalWrite(DIR1B, HIGH);
   }
   
void turnLeft(int Speed){
  detachInterrupt(2);
  analogWrite(PWM1F, Speed);
  analogWrite(PWM2F, Speed);
  analogWrite(PWM1B, Speed);
  analogWrite(PWM2B, Speed); 
  digitalWrite(DIR1F, HIGH);
  digitalWrite(DIR2F, LOW);
  digitalWrite(DIR2B, HIGH);
  digitalWrite(DIR1B, LOW);
  }
  
/*void rightForward(){
  analogWrite(motorR1, 170);
  digitalWrite(motorR2, LOW);
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  }
void leftForward(){
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(motorL1, 170);
  digitalWrite(motorL2, LOW);
  }*/
  
void stopp(){
  detachInterrupt(2);
  digitalWrite(PWM1F, LOW);
  digitalWrite(PWM1B, LOW);
  digitalWrite(PWM2F, LOW);
  digitalWrite(PWM2B, LOW);
  }

void encoderPlus(){
  if(angle ==0)
    x_distance += arcLength;

  else if(angle == 90)
    y_distance += arcLength;

  else if(angle == 180)
    x_distance -= arcLength;

  
}  
  
void encoderMinus(){
  if(angle ==0)
    x_distance -= arcLength;

  else if(angle == 90)
    y_distance -= arcLength;

  else if(angle == 180)
    x_distance += arcLength;
  }

/*void calc(){
  compass.read();
  angle = compass.heading() - reference;
  
  }*/
////////////////////////////////////////////
//Communication Functions:
void trans (String n){
  if(listenIndicator==true){
    radio.stopListening();
    listenIndicator=false;
  }
  bool done=false;
  while (!done){
    done = radio.write(&n,sizeof(n));
  }
}

char rec (){
  char n;
  if(listenIndicator==false){
    radio.startListening();
    listenIndicator=true;
  }
  if(radio.available()){
    bool done=false;
    while(!done){
      done=radio.read(&n,sizeof(n));
    }
    return n;
  } 
}
////////////////////////////////////////////
