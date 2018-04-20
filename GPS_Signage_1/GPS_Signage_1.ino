#include <SoftwareSerial.h>
#include <TinyGPS++.h>
static const int RXPin = 2, TXPin = 3;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
//Point A = 14.588185, 121.021542
//Point B = 14.589435, 121.022893
//Point C = 14.590562, 121.024286
//Point D = 14.591606, 121.025618
const float A[] = {14.588185, 121.021542};
const float B[] = {14.589435, 121.022893};
const float C[] = {14.590562, 121.024286};
const float D[] = {14.591606, 121.025618};
int indexDistanceMin;
int loopCounter;
float minDist[100];
int minLbl[100];
int arrCnt = 0;

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);
}

void loop() {
  while (ss.available() > 0){
    gps.encode(ss.read());
  }
  float tempLat = gps.location.lat();
  float tempLong = gps.location.lng();
  float tempMin;
  int tempInd;
  Serial.println("----------------------------------------");
  Serial.println("LOCATION:");
  Serial.print("Latitude = ");
  Serial.println(tempLat, 6);
  Serial.print("Longtitude = ");
  Serial.println(tempLong, 6);
  tempMin = getDistance(tempLat, tempLong);
  tempInd = indexDistanceMin;
//  Serial.print("tempInd :");
//  Serial.println(tempInd);
//  Serial.print("tempMin :");
//  Serial.println(tempMin);
  if(arrCnt>=999){
    for(loopCounter = 0; loopCounter <300; loopCounter++){
       minDist[loopCounter] = minDist[loopCounter + 700];
       minLbl[loopCounter] = minLbl[loopCounter + 700];
    }
    arrCnt = 300;
  }
  minDist[arrCnt] = tempMin;
  minLbl[arrCnt] = tempInd;
  Serial.print("Nearest point = ");
  Serial.println(minLbl[arrCnt]);
//  Serial.println(minDist[arrCnt]);
//  Serial.println(minLbl[arrCnt]);
//  Serial.println(arrCnt);
  switch(getRoute(arrCnt)){
    case 1:
      Serial.println("Route = 1");
      break;
    case 2:
      Serial.println("Route = 2");
      break;
    default:
      Serial.println("Calculating Route...");
      break;
  }
  arrCnt ++;
  delay(5000);
}
float getDistance(float tempLat, float tempLong){
  float calDistance[] = {0.0, 0.0, 0.0, 0.0};
  calDistance[0] = sqrt((pow((tempLat-A[0]), 2)) + (pow((tempLong-A[1]), 2))) * 1000;
  calDistance[1] = sqrt((pow((tempLat-B[0]), 2)) + (pow((tempLong-B[1]), 2))) * 1000;
  calDistance[2] = sqrt((pow((tempLat-C[0]), 2)) + (pow((tempLong-C[1]), 2))) * 1000;
  calDistance[3] = sqrt((pow((tempLat-D[0]), 2)) + (pow((tempLong-D[1]), 2))) * 1000;
  int minDistanceIndex = 0;
  for(loopCounter = 0; loopCounter<4; loopCounter++){
    Serial.print("Distance: ");
    Serial.println(calDistance[loopCounter]);
    if(loopCounter!=0){
      if(calDistance[loopCounter] < calDistance[minDistanceIndex]){
        minDistanceIndex = loopCounter;
      }  
    }
  }
  indexDistanceMin = minDistanceIndex;
//  int returnInt = minDistanceIndex;
//  Serial.print("Return Value: ");
//  Serial.println(returnInt);
//  Serial.print("Minimal Distance: ");
//  Serial.println(minDistanceIndex);
  return(calDistance[minDistanceIndex]);
}

int getRoute(int index){
  int route = 3;
  if(index!=0){
    // Check from A
    if(minLbl[index - 1] = 0){
      if(minLbl[index] = 0){
        if(minDist[index] <= minDist[index - 1]){
          route = 2;
        }else{
          route = 1;
        }
      }else{
        route = 2;
      }
    // Check from B
    }else if(minLbl[index - 1] = 1){
      if(minLbl[index] = 1){
        int tempIndex = 0;
        while(minLbl[index - tempIndex] == 1){
          tempIndex++;
        }
        if(minLbl[index - tempIndex] == 0){
          route = 1;
        }else{
          route = 2;
        }
      }else if(minLbl[index] = 0){
        route = 1;
      }else{
        route = 2;
      }
    // Check from C
    }else if(minLbl[index - 1] = 2){
      if(minLbl[index] = 2){
        int tempIndex = 0;
        while(minLbl[index - tempIndex] == 2){
          tempIndex++;
        }
        if(minLbl[index - tempIndex] == 3){
          route = 2;
        }else{
          route = 1;
        }
      }else if(minLbl[index] = 3){
        route = 2;
      }else{
        route = 1;
      }
    //Check from D
    }else{
      if(minLbl[index] = 3){
        if(minDist[index] <= minDist[index -1]){
          route = 1;
        }else{
          route = 2;
        }
      }else{
        route = 1;
      }
    }
  }
  return(route);
}

