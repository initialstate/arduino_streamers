/*
// Will work with Ethernet Shield
// Thanks to Gaetano Carlucci
*/

/////////////////////
// Ethernet Shield //
/////////////////////
#include <SPI.h>
#include <Ethernet.h>

////////////////////////////
// Initial State Streamer //
////////////////////////////
 
// Data destination
// https can't be handled by the ESP8266, thus "insecure"
#define ISDestURL "insecure-groker.initialstate.com"
// Bucket key (hidden reference to your bucket that allows appending):
#define bucketKey "arduino"
// Bucket name (name your data will be associated with in Initial State):
#define bucketName "Arduino Stream"
// Access key (the one you find in your account settings):
#define accessKey "enter your key"
// How many signals are in your stream? You can have as few or as many as you want
const int NUM_SIGNALS = 2;
// What are the names of your signals (i.e. "Temperature", "Humidity", etc.)
String signalName[NUM_SIGNALS] = {"Signal 1", "Signal 2"};
// This array is to store our signal data later
String signalData[NUM_SIGNALS];

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

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192, 168, 0, 177);

// fill in your Domain Name Server address here:
IPAddress myDns(8, 8, 8, 8);

// initialize the library instance:
EthernetClient client;

void setup() {
  // start serial port:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection using a fixed IP address and DNS server:
  Ethernet.begin(mac, ip, myDns);
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  // The postBucket() function creates a bucket 
  // (unnecessary if the bucket already exists)
  while (!postBucket()) {};
}

void loop() {

  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // these lines gererate a random signal that can be easily understood on the dashboard
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
    // Build HTTP request.

  for (int i=0; i<NUM_SIGNALS; i++){
    String toSend = "POST /api/events HTTP/1.1\r\n";
    toSend += "Host:";
    toSend += ISDestURL;
    toSend += "\r\n" ;
    toSend += "Content-Type: application/json\r\n";
    toSend += "User-Agent: Arduino\r\n";
    toSend += "Accept-Version: ~0\r\n";
    toSend += "X-IS-AccessKey:  " accessKey "\r\n";
    toSend += "X-IS-BucketKey:  " bucketKey "\r\n";
    
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
