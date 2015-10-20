# arduino_streamers

arduinoyunstream.ino
--------------------

Arduino Yun Sketch implementation of the Initial State events api to easily send data using the Process class.

You are required to provide your accessKey, bucketKey, bucketName, number of signals to be streamed, and signal names.

The sketch will not currently return the cURL output, so be sure to check your Initial State account to ensure that your data is actually sending.

arduinowifishieldstream.ino
---------------------------

Arduino Sketch implementation of the Initial State events api to easily send data using a WiFi shield.

This particular sketch uses the ESP8266 connected to the Cactus Micro board, but is easily adaptable to whatever Arduino/Shield combo you are using.

You are required to provide your accessKey, bucketKey, bucketName, number of signals to be streamed, signal names, WiFi SSID and password.

You may need to wait a minute or so for the module to connect to the groker.initialstate.com service.

arduinoethernetshieldstream.ino
---------------------------

Arduino Sketch implementation of the Initial State events api to easily send data using an Ethernet shield.

You are required to provide your accessKey, bucketKey, bucketName, number of signals to be streamed, signal names.

NOTE: This uses the "insecure" Initial State API endpoint since Arduinos can't handle https. For more secure streaming, route through a device capable of encryption (like a node.js hub).
