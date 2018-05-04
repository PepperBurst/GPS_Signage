#include <SD.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define  PRINT(s, v) { Serial.print(F(s)); Serial.print(v); }
#define MAX_DEVICES 8
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

const uint16_t WAIT_TIME = 1000;
static const int RXPin = 2, TXPin = 3;
static const uint32_t GPSBaud = 9600;
bool switchFlag = false;
int prevRoute = 0;
int currRoute = 0;
int prevPoint1 = 0;
int prevPoint2 = 0;
int currPoint1 = 0;
int currPoint2 = 0;
float prevDist1 = 0;
float prevDist2 = 0;
float currDist1 = 0;
float currDist2 = 0;
unsigned long previousMillis = 0;
const long interval = 5000;
const int pDelayMs = 1000;
int indexDistanceMin;
int loopCounter;

MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
//Point A = 14.588185, 121.021542
//Point B = 14.589435, 121.022893
//Point C = 14.590562, 121.024286
//Point D = 14.591606, 121.025618
const float A[] = {14.595085, 121.027841};
const float B[] = {14.596923, 121.029395};
const float C[] = {14.601695, 121.027566};
const float D[] = {14.604197, 121.023864};
//const float testLat[] = {14.599879, 14.599596, 14.59938, 14.599147, 14.598882, 14.598712, 14.598477, 14.598285, 14.598237, 14.598225, 14.598211, 14.598172, 14.598012, 14.59781, 14.597605, 14.597373, 14.59721, 14.597146, 14.59715};
//const float testLong[] = {121.028137, 121.028205, 121.028274, 121.028373, 121.028572, 121.028747, 121.028976, 121.02935, 121.029594, 121.029769, 121.029838, 121.029953, 121.030059, 121.030143, 121.030143, 121.029983, 121.029846, 121.029792, 121.029785};
int testCnt = 0;
void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);
  P.begin();
}

void loop() {
   while(ss.available() > 0){
       gps.encode(ss.read());
   }
   float tempLat = gps.location.lat();
   float tempLong = gps.location.lng();
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
//    if(testCnt>=19){
//      testCnt = 0;
//    }
//    float tempLat = testLat[testCnt];
//    float tempLong = testLong[testCnt];
//    testCnt++;
    previousMillis = currentMillis;
    getDistance(tempLat, tempLong);
    Serial.println("----------------------------------------");
    Serial.println("LOCATION:");
    Serial.print("Latitude = ");
    Serial.println(tempLat, 6);
    Serial.print("Longtitude = ");
    Serial.println(tempLong, 6);
    getRoute();
    Serial.print("Point1: ");
    Serial.print(prevPoint1);
    Serial.print(", ");
    Serial.println(currPoint1);
    Serial.print("Point2: ");
    Serial.print(prevPoint2);
    Serial.print(", ");
    Serial.println(currPoint2);
    Serial.print("Distance1: ");
    Serial.print(prevDist1);
    Serial.print(", ");
    Serial.println(currDist1);
    Serial.print("Distance2: ");
    Serial.print(prevDist2);
    Serial.print(", ");
    Serial.println(currDist2);
    String outPoint = "Point: " + currPoint1;
    if(currRoute==0){
      Serial.print("Route = 1: ");
      Serial.println("San Juan");
      P.print("San Juan");
    }else{
      Serial.print("Route = 2: ");
      Serial.println("Kalentong");
      P.print("Kalentong");
    }
    delay(pDelayMs);
    P.print(outPoint);
    prevPoint1 = currPoint1;
    prevPoint2 = currPoint2;
    prevDist1 = currDist1;
    prevDist2 = currDist2;
  }
}

void getDistance(float tempLat, float tempLong){
  float calDistance[] = {0.0, 0.0, 0.0, 0.0};
  int lbl[] = {0, 1, 2, 3};
  calDistance[0] = sqrt((pow((tempLat-A[0]), 2)) + (pow((tempLong-A[1]), 2))) * 1000;
  calDistance[1] = sqrt((pow((tempLat-B[0]), 2)) + (pow((tempLong-B[1]), 2))) * 1000;
  calDistance[2] = sqrt((pow((tempLat-C[0]), 2)) + (pow((tempLong-C[1]), 2))) * 1000;
  calDistance[3] = sqrt((pow((tempLat-D[0]), 2)) + (pow((tempLong-D[1]), 2))) * 1000;
  int i;
  int j;
  float temp;
  int min1;
  int min2;
  int lblSwap;
  for(i=0; i<3; i++){
    for(j=0; j<3-i; j++){
      if(calDistance[j] > calDistance[j+1]){
        temp = calDistance[j];
        lblSwap = lbl[j];
        calDistance[j] = calDistance[j+1];
        lbl[j] = lbl[j+1];
        calDistance[j+1] = temp;
        lbl[j+1] = lblSwap;
      }
    }
  }
  currPoint1 = lbl[0];
  currPoint2 = lbl[1];
  currDist1 = calDistance[0];
  currDist2 = calDistance[1];
}

void getRoute(){
  if(currPoint1==prevPoint1){
    prevRoute = currRoute;
    if(switchFlag){
      switchFlag=false;
    }else if(prevPoint2 != currPoint2){
      switchFlag = true;
      if(prevPoint2==0 && currPoint2==2){
        currRoute = 0;
      }else if(prevPoint2==1 && currPoint2==3){
        currRoute = 0;
      }else if(prevPoint1==2 && currPoint2==0){
        currRoute = 1;
      }else if(prevPoint1==3 && currPoint2==1){
        currRoute = 1;
      }
    }else if(currDist1 >= prevDist1 && currDist2 <=  prevDist2){
      if(currPoint1==0){
        currRoute = 0;
      }else if(currPoint1==1){
        if(currPoint2==0){
          currRoute = 0;
        }else{
          currRoute = 1;
        }
      }else if(currPoint1==2){
        if(currPoint2==3){
          currRoute = 0;
        }else{
          currRoute = 1;
        }
      }else{
        currRoute = 1;
      }
    }else if(currDist1 <= prevDist1 && currDist2 >= prevDist2){
      if(currPoint2==0){
        currRoute = 0;
      }else if(currPoint2==1){
        if(currPoint1==0){
          currRoute = 0;
        }else{
          currRoute = 1;
        }
      }else if(currPoint2==2){
        if(currPoint1==3){
          currRoute = 0;
        }else{
          currRoute = 1;
        }
      }else{
        currRoute = 1;
      }
    }
  }else{
    prevRoute = currRoute;
    if(prevPoint1==0){
      currRoute = 0;
    }else if(prevPoint1==1){
      if(currPoint1==0){
        currRoute = 1;
      }else{
        currRoute = 0;
      }
    }else if(prevPoint1==2){
      if(currPoint1==3){
        currRoute = 0;
      }else{
        currRoute = 1;
      }
    }else{
      currRoute = 1;
    }
  }
}

String getPointName(int value){
  switch(value){
      case 0:
        return "Kalentong";
      case 1:
        return "Aquinas";
      case 2:
        return "Petron";
      defualt:
        return "Puregold";
    }
}
