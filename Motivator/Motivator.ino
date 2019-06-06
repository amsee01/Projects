#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68
byte byteReadh;
byte byteReadm;
byte byteReadr;
int check=0;
int checkl=0;
int checkc=0;
int alarmh;
int alarmh1;
int alarmm;
int alarmm1;
int alarmm2;
int alarmam;
int checkdisp=0;
int motivate=0;
int timea=0;
int timeb=0;
int checktime=0;
int globehour=0;
int globemin=0;
char bt; 
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}
//replace here


//The RTC and OLED setup ends here

//the autodetect setup starts from below
#include <NewPing.h>
#include <IRremote.h>

int us1Pin = 8;
int us2Pin = 9;
int relayPin = 13;

#define MAX_DISTANCE 200
#define OK_POS1 0x499B750A
#define OK_POS2  0x9E856ADE
#define RECV_PIN 10
#define OK_ON 0x7D03709
#define OK_OFF 0x7AD7A7D9

byte ledState;
IRrecv irrecv(RECV_PIN);
decode_results results;
boolean power_state = LOW;


class USSensor {
public:
  USSensor(int trigPin, int echoPin, String aName) {
    trig = trigPin;
    echo = echoPin;
    sonar = new NewPing(trigPin, echoPin, MAX_DISTANCE);
    isActivated = false;
    name = aName;
    time = 0;
  }
  
  void update() {
    updateSensor();
    if (value < 30) {
      setActive(true);
    } else {
      if (millis() - time > 500) {
        setActive(false);
      } 
    }      
  }
  
  void reset() {
    setActive(false);
    time = 0;
  }
  
  void setActive(boolean state) {
    if (!isActivated && state) {
      time = millis();
    }
    if (isActivated != state) {
      isActivated = state;
      Serial.print(name + ": ");
      Serial.println(state);
    }
  }
  
  boolean wasActive() {
    return (time != 0) && !isActivated;
  }
  
  int lastActive() {
    return millis() - time;
  }
  
private:
  int trig;
  int echo;
  int led;
  String name;
  boolean isActivated;
  unsigned long time;
  float value;
  NewPing *sonar;
  
  void updateSensor() {
    
    delay(25);
    unsigned int uS = sonar->ping();
    value = uS / US_ROUNDTRIP_CM;
    
//    delay(5);
//    pinMode(trig, OUTPUT);
//    digitalWrite(trig, LOW);
//    delayMicroseconds(50);
//    digitalWrite(trig, HIGH);
//    delayMicroseconds(200);
//    digitalWrite(trig, LOW);
//    pinMode(echo, INPUT);
//    int pulseLen = pulseIn(echo, HIGH);
//    float currentValue = pulseLen / 29.387 /2; // [voloshyn] convert into cm
//    if (value == 0.0f) value = currentValue;
//    else value = (value + currentValue) / 2;
//    value = pulseLen / 29.387 /2; // [voloshyn] convert into cm
  }
};

int peopleInRoom = 0;
USSensor *A;
USSensor *B;

void setup() {
  irrecv.enableIRIn(); // Start the receiver
  Serial.begin(9600);
  A = new USSensor(us1Pin, us1Pin, "A");
  B = new USSensor(us2Pin, us2Pin, "B");
  pinMode(relayPin, OUTPUT);
  Wire.begin();
   // set the initial time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
  //setDS3231time(30,25,25,2,2,6,15);
}


void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void displayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);

globehour = hour;
globemin = minute;

 /* if(second > 25 && second < 27){
  // send it to the serial monitor
 Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute<10)
  {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second<10)
  {
    Serial.print("0");
  }
  Serial.println(second, DEC);
  }*/
  checkdisp=minute;
  if (hour==alarmh){
    if(minute==alarmm){
      Serial.println("Alarm!");
      digitalWrite(13,HIGH);
      
    }
  }
}


void loop() {
  byte byteReadh, byteReadma, byteReadmb, byteReadam, byteReadh1;
     if( Serial.available() )       // if data is available to read
  {
    ;
  }
  bt = Serial.read();         // read it and store it in 'val'

  if( bt == 'A' )               // if 'a' was received led 2 is switched off
  {
    digitalWrite(13, HIGH);    // turn Off pin 2
  }
 
  if( bt == 'a' )               // if 'A' was received led 2 on
  {
    digitalWrite(13, LOW);  // turn ON pin 2a
  }
  if( bt == 'b')
  {
    relayPin=13;
  }
  
  if(bt == 'c')
  {
    relayPin=16;
  }
  
  if(bt == 'h')
  {
  // byteReadh= Serial.read();
  //  alarmh=byteReadh; 
    alarmh= Serial.read();
    Serial.println(alarmh);
  }

  
  if( bt == 'm' )               
  {
    Serial.println(motivate);    
  }
  
  if(bt == 'i')
  {
  // byteReadma= Serial.read();
    //alarmm1=byteReadma; 
    alarmm= Serial.read();
    Serial.println(alarmm);
  }
  
//Serial.println(bt);
  
 
 if (irrecv.decode(&results)) {  //If IR receive results are detected
   Serial.println(results.value, HEX);
    switch (results.value) {

     //Power
    case OK_POS1:
      //   Serial.println("Power");
         digitalWrite(13,LOW);
       // turn LED ON
       
    break;
    
     case OK_POS2:
      //   Serial.println("Power");
         digitalWrite(13, HIGH);  // turn LED ON
         
    break;
    
    case OK_ON:
      //   Serial.println("Power");
         relayPin=16;  // turn LED ON
         
    break;
    
    case OK_OFF:
      //   Serial.println("Power");
         relayPin=13;  // turn LED ON
         
    break;
    
    
    }

    irrecv.resume(); // Receive the next value
  }
  
     
    
    A->update();
    B->update();
    
    if (A->wasActive() && B->wasActive()) {
      int a_time = A->lastActive();
      int b_time = B->lastActive();
      if (a_time < 5000 && b_time < 5000) {
        if (a_time > b_time) {
          peopleInRoom++;
        } else {
          peopleInRoom--;
        }
        Serial.print("People in room: ");
        Serial.println(peopleInRoom);
      }
      A->reset();
      B->reset();
    }
   displayTime(); 
   byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
    if (peopleInRoom < 0) { // [voloshyn] in case when someone was already in the room when system was activated
      peopleInRoom = 0;
    }
    
    if (peopleInRoom > 0) {
      digitalWrite(relayPin, HIGH);
      if (checktime==1){
      timea = globehour*60+globemin;
      Serial.println(timea);
      motivate = motivate + timea - timeb;
      }
      checktime = 0;
      
    } else {
      digitalWrite(relayPin, LOW);
      if (checktime==0){
      delay (100);
      timeb = globehour*60+globemin;  
      Serial.println(timeb);
      }
      checktime=1;
    }
  /* displayTime(); 
   byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);*/
  
}


