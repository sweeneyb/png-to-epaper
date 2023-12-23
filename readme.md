Display an image onto e-paper.  This repo expects a server to hold images that are sized to the paper and reports Last-Modified headers (as refreshes take a long time with 3-color paper).
For a sample server, see https://github.com/sweeneyb/epaper-images.

The use-case is infrequently updating messages to passers-by, info tags for art, QR displays, etc.  Once this works, I'll give the paper+esp32 pair a GUID, and pull images from the server by GUID to individually address them.  This bypasses any difficulty in text/font rendering that the adafruit libraries address.  Links to a test server to come later.

## Hardware
https://www.aliexpress.us/item/3256805623572150.html
https://www.aliexpress.us/item/3256804935498922.html

## Additional software
https://github.com/lagunax/ESP32-upng - get the .h and .cpp, and add them to the sketch dir.  TBD if I'm going to use those libraries "in prod"

## Image gen
I'm using golang or python to generate the images, and currently passing them through imagemagick.

For testing:
```convert go-black3.png go-black.xbm```

Currently, to reduce the bit depth:
```convert go-black3.png -colors 2 -depth 1 go-black-reduced3.png```
todo: figure out how to create low bit depth greyscale pngs in a programming language that can handle fonts.