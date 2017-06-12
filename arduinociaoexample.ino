#include <Wire.h>
#include <Ciao.h>

////////////////////////////
// Initial State Streamer //
////////////////////////////

// Data destination
// https can't be handled by most Arduinos, thus "insecure"
#define CONNECTOR     "rest" 
#define SERVER_ADDR   "insecure-groker.initialstate.com"

// Access key (the one you find in your account settings):
#define ACCESSKEY  "Your_IS_Access_Key"
// Bucket key (hidden reference to your bucket that allows appending):
#define BUCKETKEY  "ciao_stream"

void setup() {
  // CIAO init
  Ciao.begin();
}

void loop() {
    
  // read temperature, pressure, altitude, humidity
  int number = random(18,25);
  String alert = "Alert";
  
  // Build HTTP request.
  String uri = "/api/events?accessKey=";
  uri += ACCESSKEY;
  uri += "&bucketKey=";
  uri += BUCKETKEY;
  uri += "&number=";
  uri += String(number);
  uri += "&alert=";
  uri += String(alert);

  
  Ciao.println("Send data to Initial State"); 
  
  // Send the raw HTTP request
  CiaoData data = Ciao.write(CONNECTOR, SERVER_ADDR, uri);

  if (!data.isEmpty()){
    Ciao.println( "State: " + String (data.get(1)) );
    Ciao.println( "Response: " + String (data.get(2)) );
  }
  else{ 
    Ciao.println("Write Error");
  }    
 
  delay(300000); // wait 5 minutes between reads

}
