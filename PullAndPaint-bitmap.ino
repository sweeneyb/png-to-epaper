/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"

#include <stdlib.h>
#include <WiFi.h>
#include <HTTPClient.h>

/*
wifi.h has my creds and web server.  Configure it like:

const char* ssid = "SSID";
const char* password = "WPA2-key";
const String server = "http://localhost:8090";
*/
#include "wifi.h"

#include "upng.h" // https://github.com/lagunax/ESP32-upng TODO fix licensing messages
#include "types.h"

//Your Domain name with URL path or IP address with path
// String blackImage = server + "/static/go-black3.png";
String blackImage = server + "/static/go-black-reduced3.png";
// String blackImage = server + "/static/test.txt";
String redImage = server + "/red.bmp";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;
String sensorReadingsArr[5];
//Create a new image cache named IMAGE_BW and fill it with white
UBYTE *BlackImage, *RYImage;
UWORD Imagesize = ((EPD_7IN5B_V2_WIDTH % 8 == 0) ? (EPD_7IN5B_V2_WIDTH / 8 ) : (EPD_7IN5B_V2_WIDTH / 8 + 1)) * EPD_7IN5B_V2_HEIGHT;

UBYTE *blackHttp, *redHttp;

void debugImageSize(upng_t* upng) {
  Serial.print("width: ");
  Serial.println(upng_get_width(upng));
  Serial.print("height: ");
  Serial.println(upng_get_height(upng));
  Serial.print("epaper size calc: ");
  Serial.println(Imagesize);
  Serial.print("upng size calc: ");
  Serial.println(upng_get_size(upng));
  Serial.print("pixel size: ");
  Serial.println(upng_get_pixelsize(upng));
}

/* Entry point ----------------------------------------------------------------*/
void setup()
{
  printf("EPD_7IN5B_V2_test Demo\r\n");
  DEV_Module_Init();

  printf("e-Paper Init and Clear...\r\n");
  EPD_7IN5B_V2_Init();

  // if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
  //   printf("Failed to apply for black memory...\r\n");
  //   while(1);
  // }
  if ((RYImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    printf("Failed to apply for red memory...\r\n");
    while(1);
  }
  Paint_NewImage(RYImage, EPD_7IN5B_V2_WIDTH, EPD_7IN5B_V2_HEIGHT , 0, WHITE);
  Paint_SelectImage(RYImage);
  Paint_Clear(WHITE);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}
const char * headerKeys[] = {"Last-Modified", "Content-Length"} ;
const size_t numberOfHeaders = 2;

String modifiedSince = "Sun, 15 Oct 2023 00:59:39 GMT";

UBYTE* readData(HTTPClient *http, UBYTE* data, int contentLength){
  Serial.print("content-length");
  Serial.println(contentLength);
  // UBYTE* data = new unsigned char[contentLength]; // TODO free this memory
  Serial.print("data size");
  Serial.println(sizeof(data));
  //todo: read data
  if(data == 0){
    Serial.println("could not allocate");
  } else {
    Serial.println("seemingly allocated");
    int len = contentLength;
    WiFiClient* stream = http->getStreamPtr();
     // read all data from server
                while(http->connected() && (len > 0 || len == -1)) {
                    // get available data size
                    size_t size = stream->available();

                    if(size) {
                        // read up to 128 byte
                        int c = stream->readBytes(&(data[contentLength-len]), ((size > contentLength) ? contentLength : size));
                        Serial.print("read: ");
                        Serial.println(c);
                        if(len > 0) {
                            len -= c;
                        }
                    }
                    delay(1);
                }

  }
  return data;
}



void updateImageIfNeeded(String url, imageData* imageDataStruct  ) {
  Serial.print("start fetch of ");
  Serial.println(url);
  HTTPClient http;
  http.begin(url.c_str());
  http.addHeader("If-Modified-Since", modifiedSince );
  http.collectHeaders(headerKeys, numberOfHeaders);
  int httpResponseCode = http.GET();
  Serial.print("status code: ");
  Serial.println(httpResponseCode);
  UBYTE* imageData;
  int imageLength;
  bool doUpdate = false;
  if (httpResponseCode == 200) {
    doUpdate = true;
    Serial.println("would re-render");
    modifiedSince = http.header("Last-Modified");
    String contentLength = http.header("Content-Length");
    Serial.print("content length: ");
    Serial.println(contentLength);
    imageLength = contentLength.toInt();
    imageData = new unsigned char[imageLength];
    for (int i = 0; i< imageLength;i++) {
      imageData[i] = 0;
    }
    readData(&http, imageData, imageLength);    
    for (int i = 0; i < imageLength; i++) {
      Serial.print(imageData[i], HEX);
      Serial.print(' '); // Add a space to separate the values
    }
  }
  http.end();
  Serial.print("end fetch of ");
  Serial.println(url);
}




/* The main loop -------------------------------------------------------------*/
void loop()
{
  //Send an HTTP POST request every 1 minutes
  if ((millis() - lastTime) > timerDelay) {

    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      String serverPath = blackImage;


      struct ImageData blackImageData;
      updateImageIfNeeded(serverPath, &blackImageData );
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      http.addHeader("If-Modified-Since", modifiedSince );
      http.collectHeaders(headerKeys, numberOfHeaders);
      int httpResponseCode = http.GET();
      Serial.print("status code: ");
      Serial.println(httpResponseCode);
      
      for(int i = 0; i< http.headers(); i++){
          Serial.println(http.header(i));
      }
      UBYTE* imageData;
      int imageLength;
      bool doUpdate = false;
      if (httpResponseCode == 200) {
        doUpdate = true;
        Serial.println("would re-render");
        modifiedSince = http.header("Last-Modified");
        String contentLength = http.header("Content-Length");
        Serial.print("content length:");
        Serial.println(contentLength);
        imageLength = contentLength.toInt();
        imageData = new unsigned char[imageLength];
        for (int i = 0; i< imageLength;i++) {
          imageData[i] = 0;
        }
        readData(&http, imageData, imageLength);


        unsigned char dataArray[] = {0x12, 0x34, 0xAB, 0xCD};
        for (int i = 0; i < sizeof(dataArray); i++) {
          Serial.print(dataArray[i], HEX);
          Serial.print(' '); // Add a space to separate the values
        }
        for (int i = 0; i < imageLength; i++) {
          Serial.print(imageData[i], HEX);
          Serial.print(' '); // Add a space to separate the values
        }
        Serial.println(' ');
        // for (int i =0;i<imageLength;i++) {
        //   // Serial.write(c2h(imageData[i]));

          
        // }
      }
      // Free resources
      http.end();

      Serial.println("heap avail: ");
      size_t available = heap_caps_get_free_size(MALLOC_CAP_8BIT);
      Serial.println(available);
      if (doUpdate) {
        upng_t* upng;
        upng = upng_new_from_bytes(imageData, imageLength);
        if (upng != NULL) {
          // Decode PNG image.
          upng_decode(upng);
          if (upng_get_error(upng) == UPNG_EOK) {
            Serial.println("UPNG_EOK");
          
            debugImageSize(upng);

            const uint8_t *bitmap = upng_get_buffer(upng);

          } else {
            Serial.print("upng_get_error: ");
            Serial.println(upng_get_error(upng));
            ESP.restart();
          }

          Serial.println("Painting...");
          // Paint_NewImage(upng_get_buffer(upng), EPD_7IN5B_V2_WIDTH, EPD_7IN5B_V2_HEIGHT , 0, WHITE);
          EPD_7IN5B_V2_Clear();
          EPD_7IN5B_V2_Display(upng_get_buffer(upng), RYImage);
        
        upng_free(upng);
        free(imageData);
      }
      }
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
