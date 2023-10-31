Display an image onto e-paper.  This repo expects a server to hold images that are sized to the paper and reports Last-Modified headers (as refreshes take a long time with 3-color paper).

The use-case is infrequently updating messages to passers-by, info tags for art, QR displays, etc.  Once this works, I'll give the paper+esp32 pair a GUID, and pull images from the server by GUID to individually address them.  This bypasses any difficulty in text/font rendering that the adafruit libraries address.  Links to a test server to come later.

## Hardware
https://www.aliexpress.us/item/3256805623572150.html
https://www.aliexpress.us/item/3256804935498922.html

## Additional software
https://github.com/lagunax/ESP32-upng - get the .h and .cpp, and add them to the sketch dir.  TBD if I'm going to use those libraries "in prod"