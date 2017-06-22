# Streaming with Arduino

arduinoyunstream.ino (cURL)
--------------------

Arduino Yun Sketch implementation of the Initial State events api to easily send data using the Process class.

You are required to provide your accessKey, bucketKey, bucketName, number of signals to be streamed, and signal names.

The sketch will not currently return the cURL output, so be sure to check your Initial State account to ensure that your data is actually sending.

arduinoESP8266wifishieldstream.ino / arduinoESP8266wifishieldstream-AT.ino (HTTP POST)
---------------------------

Arduino Sketch implementation of the Initial State events api to easily send data using an ESP8266 WiFi shield.

The first sketch handles ESP8266 boards that do not require AT commands. The second sketch uses the ESP8266 connected to the Cactus Micro board, which requires AT commands. Both should be easily adaptable to whatever Arduino/Shield combo you are using.

You are required to provide your accessKey, bucketKey, bucketName, number of signals to be streamed, signal names, WiFi SSID and password.

You may need to wait a minute or so for the module to connect to the groker.initialstate.com service.

NOTE: This uses the "insecure" Initial State API endpoint since many Arduinos can't handle https. For more secure streaming, route through a device capable of encryption (like a node.js hub).

arduinoethernetshieldstream.ino (HTTP GET)
---------------------------

Arduino Sketch implementation of the Initial State events api to easily send data using an Ethernet shield.

You are required to provide your accessKey, bucketKey, bucketName, number of signals to be streamed, signal names.

NOTE: This uses the "insecure" Initial State API endpoint since many Arduinos can't handle https. For more secure streaming, route through a device capable of encryption (like a node.js hub).

arduinociaostream.ino (URL Parameters)
---------------------------

Arduino Sketch implementation of the Initial State events api to easily send data using the [Ciao library](http://www.arduino.org/learning/tutorials/advanced-guides/ciao).

Current Ciao compatible boards include: Arduino Yun, Arduino Yun Mini, Linino One, Arduino Tian, Arduino Industrial 101

You are required to provide your accessKey, bucketKey, signal names.

NOTE: This uses the "insecure" Initial State API endpoint since many Arduinos can't handle https. For more secure streaming, route through a device capable of encryption (like a node.js hub).

