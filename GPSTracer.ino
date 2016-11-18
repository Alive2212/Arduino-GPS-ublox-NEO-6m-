#include <TimerOne.h>

String GPSString;   // for incoming serial data
int counter=0;
String GPSData;
String GPSDataType[]={"GPGGA","GPGSA","GPGSV","GPRMC","GPGLL"};
String latitude,longitude;
String lastLatitude,lastLongitude;



void setup() {
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
  pinMode(13, OUTPUT);  // Pin 13 has an LED connected on most Arduino boards
  Timer1.initialize(1000); // set a timer of length 1000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
} 

void loop() {
  String strLatitdue,strLongitude;
  // send data only when you receive data:
  if (Serial.available() > 0) {    
    GPSString.concat((char) Serial.read());// read the incoming byte and add it into GPSString
    counter=100;//wait 100 mili second for next serial input
  }
  if(counter==0 && GPSString.length()>0){//when recived all data
    if(GPSString.length()>0){
      //Serial.println(GPSString);
      GPSData=NMEA0183GetData(GPSString,GPSDataType[4]);
      strLatitdue=(GPGGAParser(GPSData,",",1));
      latitude=strLatitdue;
      strLongitude=(GPGGAParser(GPSData,",",3));
      longitude=strLongitude;
      if(latitude!=lastLatitude || longitude!=lastLongitude){
        lastLatitude=latitude;
        lastLongitude=longitude;
        Serial.println("");
        Serial.println("time:"+GPGGAParser(GPSData,",",5));
        Serial.println(latitude);
        Serial.println("N/S Indicator:"+GPGGAParser(GPSData,",",2));
        Serial.println(longitude);
        Serial.println("E/W Indicator:"+GPGGAParser(GPSData,",",4));
        digitalWrite(13, HIGH);
        counter=100;//wait 100 mili second for next serial input
      }
    }
    GPSString.remove(0);    
  }        
}

/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr()
{
  if(counter>0){
    counter--;
  }
  else{  
    digitalWrite(13, LOW);
  };
}
String GPGGAParser(String data,String delimiter,int index){
  String parsedData="";
  int i;
  int startDataIndex=-1;
  int endDataIndex=data.length();
  for(i=0;i<index;i++){
    startDataIndex=data.indexOf(delimiter,startDataIndex+1);
  }
  endDataIndex=data.indexOf(delimiter,startDataIndex+1);
  if(endDataIndex==-1){
    endDataIndex=data.length();
  }
  if(startDataIndex>-1){
    parsedData=data.substring(startDataIndex+1,endDataIndex);
  }
  return parsedData;  
}
String NMEA0183GetData(String data, String dataType){
  String parsedData="";
  String startData=String("$"+dataType);
  int startDataIndex=data.indexOf(startData);
  int endDataIndex=data.indexOf('$',startDataIndex+1);
  if(endDataIndex==-1){
    endDataIndex=data.length();
  }
  if(startDataIndex>-1){
    parsedData=data.substring(startDataIndex,endDataIndex);
  }
  return parsedData;
}

