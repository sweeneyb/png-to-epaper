Display an image onto e-paper.  This repo expects a server to hold images that are sized to the paper and reports Last-Modified headers (as refreshes take a long time with 3-color paper).
For a sample server, see https://github.com/sweeneyb/epaper-images.

The use-case is infrequently updating messages to passers-by, info tags for art, QR displays, etc.  Once this works, I'll give the paper+esp32 pair a GUID, and pull images from the server by GUID to individually address them.  This bypasses any difficulty in text/font rendering that the adafruit libraries address.  Links to a test server to come later.

## Hardware
~~https://www.aliexpress.us/item/3256805623572150.html~~
The board I ordered seems to have been displaced by a cheaper option.  https://www.aliexpress.us/item/2251832767050812.html.  I can't attest that I've verified that board, though.
https://www.aliexpress.us/item/3256804935498922.html


## Image gen
Use golang's image tools, then a bit of custom processing to get into something like an xbm.  It's really a bit-field (as XBM is a text format that embeds into C code). The greyscaling could also probably be better.


## Mentions
https://github.com/lagunax/ESP32-upng - For a while, I wanted to decode on the embedded system. Turns out, I didn't have enough memory for 2 images.  But I appreciate this library being available.