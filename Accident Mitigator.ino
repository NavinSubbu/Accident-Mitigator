#include <SoftwareSerial.h>
//#define rx 11
//#define tx 10
SoftwareSerial sim808(10,11); 
int lastState = 0; //For running loop once
int runOnce = 0; 
int currentState = 1;     

char phone_no[] = "9619884810"; // Phone number to which Distress Message has to be sent
String data[5];
#define DEBUG true
#define button 6
String state,timegps,latitude,longitude;

const int groundpin = A4;             // analog input pin 4 -- ground
const int powerpin = A5;              // analog input pin 5 -- voltage
const int xpin = A3;                  // x-axis of the accelerometer
const int ypin = A2;                  // y-axis
const int zpin = A1;                  // z-axis (only on 3-axis models)
int x;
int y;
int z;


const int FLEX_PIN_L = A6; // Pin connected to voltage divider output
const int FLEX_PIN_R = A7; // Pin connected to voltage divider output

const float VCC = 4.98; // Measured voltage of Ardunio 5V line
const float R_DIV = 33000.0; // Measured resistance of 33k resistor

const float STRAIGHT_RESISTANCE = 13000.0; // resistance when straight
const float BEND_RESISTANCE = 25000.0; // resistance at 90 deg


String sendData (String command , const int timeout ,boolean debug){
  String response = "";
  sim808.println(command);
  long int time = millis();
  int i = 0;

  while ( (time+timeout ) > millis()){
    while (sim808.available()){
      char c = sim808.read();
      response +=c;
    }
  }
  if (debug) {
     Serial.print(response);
     }
     return response;
}

void sendTabData(String command , const int timeout , boolean debug)
{
    sim808.println(command);
    long int time = millis();
    int i = 0;

    while((time+timeout) > millis())
    {
      while(sim808.available()){
      char c = sim808.read();
      if (c != ',') {
         data[i] +=c;
         delay(100);
      } else {
        i++;  
      }
      if (i == 5) {
        delay(100);
        goto exitL;
      }
    }
  }exitL:
  if (debug) {
    state = data[1];
    timegps = data[2];
    latitude = data[3];
    longitude =data[4];  
  }
}
void sending(){
  sim808.print("AT+CMGF=1\r"); 
    delay(400);
    sim808.print("AT+CMGS=\"");
    sim808.print(phone_no);
    sim808.println("\"");
    delay(300);
    sim808.print("My car has crashed here.. please come to rescue http://maps.google.com/maps?q=loc:");
    sim808.print(latitude);
    sim808.print(",");
    sim808.print (longitude);
    delay(200);
    sim808.println((char)26); // End AT command with a ^Z, ASCII code 26
    delay(200);
    sim808.println();
    delay(200);
}

void SMS(){
Serial.println("SMS Loop");
    sendTabData("AT+CGNSINF",1000,DEBUG);
  if (state !=0) {
    Serial.println("State  :"+state);
    Serial.println("Time  :"+timegps);
    Serial.println("Latitude  :"+latitude);
    Serial.println("Longitude  :"+longitude);
    delay(400);
    Serial.println("SMS Sending");
    sending();
    Serial.println("SMS Sent"); 
    delay(200);
    sim808.flush();
   
  } else {
    Serial.println("GPS Initialising...");
  }
  delay(3000);
}


void Trigger()
{
  if (currentState == 1) {

        if (currentState = !lastState) {
            if (runOnce == 0)
           SMS();
            Serial.println("ACCIDENT");    
        }
        lastState = !lastState;
    }
//    Serial.println("Occured");     
    delay(2000);
}

void setup() {
 sim808.begin(9600);
    Serial.begin(9600);
    delay(50);
    pinMode(groundpin, OUTPUT);
    pinMode(powerpin, OUTPUT);
    digitalWrite(groundpin, LOW);
    digitalWrite(powerpin, HIGH);
    pinMode(FLEX_PIN_L, INPUT);
    pinMode(FLEX_PIN_R, INPUT);
//    pinMode(button,INPUT_PULLUP);

    sim808.println("AT+CSMP=17,167,0,0");  // set this parameter if empty SMS received
    delay(100);
    Serial.println("Sim module");
    sendData("AT+CGNSPWR=1",1000,DEBUG); 
    delay(50);
    sendData("AT+CGNSSEQ=RMC",1000,DEBUG);
    delay(150);
}

void loop() {
  
  x=analogRead(xpin);
  y=analogRead(ypin);
  z=analogRead(zpin);
//  Serial.println("");
//  Serial.print(x);
//  Serial.print("\t"); // print a tab between values:
//  delay(1000);
//  Serial.print(y);
//  Serial.print("\t"); // print a tab between values:
//  delay(1000);
//  Serial.print(z);
//  delay(1000);
 
//Voltage Divider for Flex
//For LEFT
  int flexADC_L = analogRead(FLEX_PIN_L);
  
  float flexV_L = flexADC_L * VCC / 1023.0;
  float flexR_L = R_DIV * (VCC / flexV_L - 1.0);

//For RIGHT
  int flexADC_R = analogRead(FLEX_PIN_R);
  float flexV_R = flexADC_R * VCC / 1023.0;
  float flexR_R = R_DIV * (VCC / flexV_R - 1.0);
//  Serial.println("");
//  Serial.println(flexADC_L);
//  Serial.println(flexR_L);
  delay(1000);
  //Serial.println("Resistance: " + String(flexR) + " ohms");

  // Use the calculated resistance to estimate the sensor's
  // bend angle:
//  float angle = map(flexR_L, STRAIGHT_RESISTANCE, BEND_RESISTANCE,
//                   0, 90.0);
 //Serial.println("Bend: " + String(angle) + " degrees");
 // Serial.println();

 if (x <300 && x > 100 ) 
{
  Serial.print("X-Axis Backwards");
   Trigger();
}
else if (x >400) 
{
  Serial.print("X-Axis Forwards");
  Trigger();
}

else if (y >410) 
{
  Serial.print("Y-Axis Forwards");
  Trigger();
}

else if ( y<300 && y > 100) 
{
  Trigger();
  Serial.print("Y-Axis Backwards");
  
}

else if (z < 300 && z > 100) 
{
  Serial.print("Z-Axis Tumble");
  Trigger();
}

else if (flexR_L >15000) 
{
  Serial.println("Flex");
  Trigger();
}

else if (flexR_R >15000) 
{
  Serial.println("Flex");
  Trigger();
}
 delay(100);

}




