// Will work only with ESP8266 firmware 0.9.2.2 or higher
// Some particulars of this sketch are for the Cactus Micro board, 
// but it can be adapted for any Arduino
// Special thanks to David Reeves for his help

// This sketch uses Software Serial, which is the current default for the Cactus Micro board
// If you are using Hardware Serial, you will need to change this line and any with "myserial"
#include <SoftwareSerial.h>


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
#define accessKey "Your_Access_Key_Here"
// How many signals are in your stream? You can have as few or as many as you want
const int NUM_SIGNALS = 3;
// What are the names of your signals (i.e. "Temperature", "Humidity", etc.)
String signalName[NUM_SIGNALS] = {"Signal 1", "Signal 2", "Signal 3"};
// This array is to store our signal data later
String signalData[NUM_SIGNALS];


// WiFi SSID
#define SSID "SSID"
// WiFi password
#define PASS "password"


// CH_PD pin - Used by Cactus Micro to enable ESP8266
#define RESET 13
// RST pin - ESP8266 reset pin
#define RST 5

// Timeout in milliseconds
#define TIMEOUT     5000
#define CONTINUE    false
#define HALT        true

// TCP can only send a payload size of 290 when using Software Serial
// 30 is how many characters are free for bucketKey, signalName, and signalData
const int TCPLengthCap = 30;

// The Cactus Micro's TX and RX
SoftwareSerial mySerial(11, 12);


// This only runs once at the very beginning
void setup()  {

  Serial.begin(9600);
  // Communication with ESP8266
  mySerial.begin(9600);
  
  // Enable RST pin
  digitalWrite(RST, 1);
  // Enable CH_PD pin
  digitalWrite(RESET, 1);     
  // Wait for chip enable
  delay(2000);
  
  Serial.println("ESP8266 A0 Monitor");
  reset();
  // Wait for reset to complete
  delay(5000);

  // Reset & test if the module is ready 
  echoCommand("AT+RST", "Ready", HALT); 
  delay(5000);
  echoCommand("AT+CSYSWDTENABLE", "WDT Enabled", HALT);
  delay(500);
  Serial.println("Module is ready.");
  
  // Set up connection modes
  // Retrieves the firmware ID (version number) of the module.
  echoCommand("AT+GMR", "OK", CONTINUE);
  // Station mode    
  echoCommand("AT+CWMODE=3", "", HALT);
  // Allow one connection    
  echoCommand("AT+CIPMUX=1", "", HALT);    

  // Connect to the wifi
  boolean connection_established = false;
  for(int i=0;i<5;i++)
  {
    if(connectWiFi())
    {
      connection_established = true;
      delay(5000);
      break;
    }
  }
  if (!connection_established) errorHalt("Connection failed");
  delay(5000);

  // Echo IP address.
  echoCommand("AT+CIFSR", "", HALT);            
  
  // Initialize random number generation for our example signals
  randomSeed (millis());

  // The postBucket() function creates a bucket 
  // (unnecessary if the bucket already exists)
  while (!postBucket()) {};
}

// This repeats
void loop() 
{
  // Reset ESP8266 each time around
  reset();                                
  delay(5000);
 
  // Gather Data
  // Read from a port for input or output or generate your own values/messages
  // These signals are random numbers for the sake of example
  signalData[0] = String(random(18,25));
  signalData[1] = String(random(1, 100));
  signalData[2] = String("Alert");

  // The postData() function streams our events
  while(!postData());   

  // Wait for 5 seconds before collecting and sending the next batch
  delay(5000);
}



// Here are the data bucket creation, event data posting, error handling,
// and WiFi connection functions
// They do not need to be edited - everything you would need to change for 
// your project can be found above

boolean postBucket () 
{
  // Must be connected to your destination URL
  while(!connectService(ISDestURL,80)) {};

  // Get connection status 
  if (!echoCommand("AT+CIPSTATUS", "OK", CONTINUE)) return false;
 
  // Build HTTP request.
  String toSend = "POST /api/buckets HTTP/1.1\r\n";
  toSend += "Host: "ISDestURL"\r\n" ;
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
  
  Serial.println(toSend);

  // Ready the module to receive raw data
  if (!echoCommand("AT+CIPSEND="+String(toSend.length()), ">", CONTINUE))
  {
    echoCommand("AT+CIPCLOSE", "", CONTINUE);
    Serial.println("Connection timeout.");
    return false;
  }
  
  // Send the raw HTTP request
  // POST
  if(!echoCommand(toSend,"20", CONTINUE)) return false;  
  Serial.println("Bucket Created or Exists");
   
  return true;
} 
  
boolean postData() 
{  
  // Must be connected to your destination URL
  if (!connectService(ISDestURL,80)) return false;

  // Get connection status 
  if (!echoCommand("AT+CIPSTATUS", "OK", CONTINUE)) return false;

  // Build HTTP request.
  for (int i=0; i<NUM_SIGNALS; i++)
  {
    // Shorten signalName and signalData if toSend will be over 290 characters
    truncateString(i);
    
    String toSend = "POST /api/events HTTP/1.1\r\n";
    toSend += "Host: "ISDestURL"\r\n";
    toSend += "Content-Type: application/json\r\n";
    toSend += "User-Agent: Arduino\r\n";
    toSend += "Accept-Version: ~0\r\n";
    toSend += "X-IS-AccessKey:  " accessKey "\r\n";
    toSend += "X-IS-BucketKey:  " bucketKey "\r\n";
    String payload ="[{\"key\": \"" + signalName[i] + "\", "; 
    payload +="\"value\": \"" + signalData[i] + "\"}]";
    payload +="\r\n"; 
    toSend += "Content-Length: "+String(payload.length())+"\r\n";
    toSend += "\r\n";
    toSend += payload;

    Serial.println(toSend);

    // Ready the module to receive raw data
    if (!echoCommand("AT+CIPSEND="+String(toSend.length()), ">", CONTINUE))
    {
      echoCommand("AT+CIPCLOSE", "", CONTINUE);
      Serial.println("Connection timeout.");
      return false;
    }
    
    // Send the raw HTTP request
    // POST
    if(!echoCommand(toSend,"204", CONTINUE)) return false; 
    String toPrint = "Data batch ";
    toPrint += i;
    toPrint += " posted!";
    Serial.println(toPrint);
  }

  return true;
}

void truncateString(int sigNum)
{
  int variableLength = (sizeof(bucketKey) + signalName[sigNum].length() + signalData[sigNum].length());
  int eventValueLength = (30 - sizeof(bucketKey));

  if (variableLength > TCPLengthCap)
  {
    int halfEventValueLength;
    
    if ((eventValueLength & 1) == 0)
      halfEventValueLength = (eventValueLength/2);
    else
      halfEventValueLength = ((eventValueLength-1)/2);

    signalName[sigNum] = signalName[sigNum].substring(0,halfEventValueLength);
    signalData[sigNum] = signalData[sigNum].substring(0,halfEventValueLength);
  }
}

// Print error message and loop stop.
void errorHalt(String msg)
{
  Serial.println(msg);
  Serial.println("HALT");
  while(true){};
}

// Read characters from WiFi module and echo to serial until keyword occurs or timeout.
boolean echoFind(String keyword)
{
  byte current_char   = 0;
  byte keyword_length = keyword.length();
  
  // Fail if the target string has not been sent by deadline.
  unsigned long deadline = millis() + TIMEOUT;
  while(millis() < deadline)
  {
    if (mySerial.available())
    {
      char ch = mySerial.read();
      Serial.write(ch);

      if (ch == keyword[current_char])
        if (++current_char == keyword_length)
        {
          Serial.println();
          return true;
        }
    }
  }
  // Timed out
  return false;  
}

// Read and echo all available module output.
// (Used when we're indifferent to "OK" vs. "no change" responses or to get around firmware bugs.)
void echoFlush()
  {while(mySerial.available()) Serial.write(mySerial.read());}
  
// Echo module output until 3 newlines encountered.
// (Used when we're indifferent to "OK" vs. "no change" responses.)
void echoSkip()
{
  echoFind("\n");        // Search for nl at end of command echo
  echoFind("\n");        // Search for 2nd nl at end of response.
  echoFind("\n");        // Search for 3rd nl at end of blank line.
}

// Send a command to the module and wait for acknowledgement string
// (or flush module output if no ack specified).
// Echoes all data received to the serial monitor.
boolean echoCommand(String cmd, String ack, boolean halt_on_fail)
{
  mySerial.println(cmd);
  #ifdef ECHO_COMMANDS
    Serial.print("--"); Serial.println(cmd);
  #endif
  
  // If no ack response specified, skip all available module output.
  if (ack == "")
    echoSkip();
  else
    // Otherwise wait for ack.
    // Timed out waiting for ack string
    if (!echoFind(ack))           
      if (halt_on_fail)
        // Critical failure halt.
        errorHalt(cmd+" failed");
      else
        // Let the caller handle it.
        return false;      
  // Ack blank or ack found      
  return true;                   
}

// Connect to the specified wireless network.
boolean connectWiFi()
{
  String cmd = "AT+CWJAP=\"" SSID "\",\"" PASS "\"";
  // Join Access Point
  if (echoCommand(cmd, "OK", CONTINUE)) 
  {
    Serial.println("Connected to WiFi.");
    return true;
  }
  else
  {
    Serial.println("Connection to WiFi failed.");
    return false;
  }
}

boolean connectService(String service, int port) 
{  
  String serviceConnect = "AT+CIPSTART=\"TCP\",\"" + service + "\"," + port;
  //  Handle connection errors
  if (!echoCommand(serviceConnect, "Linked", CONTINUE)) {          
    if (echoCommand(serviceConnect, "ALREADY CONNECT", CONTINUE)){
      return true;
    }
    if (echoCommand(serviceConnect, "busy p...", CONTINUE)) {
      reset();
      delay(5000);
    }
    if (echoCommand(serviceConnect, "ERROR", CONTINUE)) {
      reset();
      delay(5000);
    }
    delay(2000);
    return false;
  }
  delay(2000);
  return true;
}

void reset()
{
  digitalWrite(RESET,LOW);
  delay(1000);
  digitalWrite(RESET,HIGH);
  delay(1000);
}
