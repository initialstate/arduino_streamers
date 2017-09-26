// Should work with most ESP8266 boards.
// Special thanks to Kurt Lanes for providing the example

#include <Arduino.h>

#include <ESP8266WiFi.h>

////////////////////////////
// Initial State Streamer //
////////////////////////////
// Data destination
#define ISDestURL "insecure-groker.initialstate.com" // https can't be handled by the ESP8266, thus "insecure"
#define bucketKey "arduino" // Bucket key (hidden reference to your bucket that allows appending):
#define bucketName "Arduino Stream" // Bucket name (name your data will be associated with in Initial State):
#define accessKey "Your_IS_Access_Key" // Access key (the one you find in your account settings):

const int NUM_SIGNALS = 2; // How many signals are in your stream? You can have as few or as many as you want
String signalName[NUM_SIGNALS] = {"Signal 1", "Signal 2"}; // What are the names of your signals (i.e. "Temperature", "Humidity", etc.)

String signalData[NUM_SIGNALS]; // This array is to store our signal data later

/////////////
// Signals //
////////////

//Signal number 1
int i = 0;
//Signal number 2
bool increase = true;

//////////////////////
// Network Settings //
/////////////////////

char ssid[] = "WiFi_SSID"; // your network SSID (name)
char password[] = "WiFi_PW"; // your network password

WiFiClient client; // initialize the library instance:

void setup() {
Serial.begin(115200);
delay(10);

// We start by connecting to a WiFi network

Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}

Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());

// The postBucket() function creates a bucket
// (unnecessary if the bucket already exists)
while (!postBucket()) {};
}

void loop() {

// if there's incoming data from the net connection.
// send it out the serial port. This is for debugging
// purposes only:
if (client.available()) {
  char c = client.read();
  Serial.write(c);
}

// these lines generate a random signal that can be easily understood on the dashboard
if (increase)
  {
    i = i + 1;
  if (i == 10)
    {
      increase = false;
    }
  }
else
  {
    i = i - 1;
    if (i == 0)
    {
      increase = true;
    }
  }

// Gather Data
// Read from a port for input or output or generate your own values/messages
signalData[0] = String(i);
signalData[1] = String(increase);

// The postData() function streams our events
while(!postData());

// Wait for 1 seconds before collecting and sending the next batch
delay(1000);

}



// this method makes a HTTP connection to the server and creates a bucket is it does not exist:
bool postBucket() {
// close any connection before send a new request.
// This will free the socket on the WiFi shield
client.stop();

// if there's a successful connection:
if (client.connect(ISDestURL, 80) > 0) {
  Serial.println("connecting...");
  // send the HTTP PUT request:
  // Build HTTP request.
  String toSend = "POST /api/buckets HTTP/1.1\r\n";
  toSend += "Host:";
  toSend += ISDestURL;
  toSend += "\r\n" ;
  toSend += "User-Agent:Arduino\r\n";
  toSend += "Accept-Version: ~0\r\n";
  toSend += "X-IS-AccessKey: " accessKey "\r\n";
  toSend += "Content-Type: application/json\r\n";
  String payload = "{\"bucketKey\": \"" bucketKey "\",";
  payload += "\"bucketName\": \"" bucketName "\"}";
  payload += "\r\n";
  toSend += "Content-Length: "+String(payload.length())+"\r\n";
  toSend += "\r\n";
  toSend += payload;

  client.println(toSend);
  Serial.println(toSend);

  return true;
} else {
  // if you couldn't make a connection:
  Serial.println("connection failed");
  return false;
  }
}


// this method makes a HTTP connection to the server and sends the signals measured:
bool postData() {
// close any connection before send a new request.
// This will free the socket on the WiFi shield
client.stop();

// if there's a successful connection:
if (client.connect(ISDestURL, 80) > 0) {
  Serial.println("connecting...");
  // send the HTTP PUT request:

  // Build HTTP request.
  for (int i=0; i<NUM_SIGNALS; i++){
    String toSend = "POST /api/events HTTP/1.1\r\n";
    toSend += "Host:";
    toSend += ISDestURL;
    toSend += "\r\n" ;
    toSend += "Content-Type: application/json\r\n";
    toSend += "User-Agent: Arduino\r\n";
    toSend += "Accept-Version: ~0\r\n";
    toSend += "X-IS-AccessKey: " accessKey "\r\n";
    toSend += "X-IS-BucketKey: " bucketKey "\r\n";

    String payload = "[{\"key\": \"" + signalName[i] + "\", ";
    payload +="\"value\": \"" + signalData[i] + "\"}]\r\n";

    toSend += "Content-Length: "+String(payload.length())+"\r\n";
    toSend += "\r\n";
    toSend += payload;
    Serial.println(toSend);
    client.println(toSend);
  }
  return true;
} else {
  // if you couldn't make a connection:
  Serial.println("connection failed");
  return false;
}
}
