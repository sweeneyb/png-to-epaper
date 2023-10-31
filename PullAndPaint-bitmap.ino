/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"

#include <stdlib.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "wifi.h"

#include "upng.h"

//Your Domain name with URL path or IP address with path
String blackImage = server + "/static/go-black3.png";
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

UBYTE *blackHttp, *redHttp;

String artist = "";
String work = "";



String unquote(String inputString) {
  String result = "";
  
  for (int i = 0; i < inputString.length(); i++) {
    char currentChar = inputString.charAt(i);
    
    if (currentChar != '"') {
      result += currentChar;
    }
  }
  
  return result;
}

/* Entry point ----------------------------------------------------------------*/
void setup()
{
  printf("EPD_7IN5B_V2_test Demo\r\n");
  DEV_Module_Init();

  printf("e-Paper Init and Clear...\r\n");
  EPD_7IN5B_V2_Init();
  // EPD_7IN5B_V2_Clear();
  // DEV_Delay_ms(500);



  UWORD Imagesize = ((EPD_7IN5B_V2_WIDTH % 8 == 0) ? (EPD_7IN5B_V2_WIDTH / 8 ) : (EPD_7IN5B_V2_WIDTH / 8 + 1)) * EPD_7IN5B_V2_HEIGHT;
  if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    printf("Failed to apply for black memory...\r\n");
    while(1);
  }
  if ((RYImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    printf("Failed to apply for red memory...\r\n");
    while(1);
  }
  printf("NewImage:BlackImage and RYImage\r\n");
  Paint_NewImage(BlackImage, EPD_7IN5B_V2_WIDTH, EPD_7IN5B_V2_HEIGHT , 0, WHITE);
  Paint_NewImage(RYImage, EPD_7IN5B_V2_WIDTH, EPD_7IN5B_V2_HEIGHT , 0, WHITE);

  //Select Image
  Paint_SelectImage(BlackImage);
  Paint_Clear(WHITE);
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

unsigned char* readData(HTTPClient *http, int contentLength){
  unsigned char* data = new unsigned char[contentLength];
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
                        int c = stream->readBytes(data, ((size > sizeof(data)) ? sizeof(data) : size));
                        if(len > 0) {
                            len -= c;
                        }
                    }
                    delay(1);
                }

  }
  return data;
}

char c2h(char c)
{  return "0123456789ABCDEF"[0x0F & (unsigned char)c];
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
      
      if (httpResponseCode == 200) {
        Serial.println("would re-render");
        modifiedSince = http.header("Last-Modified");
        String contentLength = http.header("Content-Length");
        Serial.print("content length:");
        Serial.println(contentLength);
        int imageLength = contentLength.toInt();
        unsigned char *imageData = readData(&http, imageLength);

        upng_t* upng;

        for (int i =0;i<imageLength;i++) {
          Serial.write(c2h(imageData[i]));
        }

        upng = upng_new_from_bytes(imageData, imageLength);
        if (upng != NULL) {
          // Decode PNG image.
          upng_decode(upng);
          if (upng_get_error(upng) == UPNG_EOK) {
            Serial.println("UPNG_EOK");
            // Get pointer to bitmap buffer.
            const uint8_t *bitmap = upng_get_buffer(upng);

          } else {
            Serial.print("upng_get_error: ");
            Serial.println(upng_get_error(upng));
          }

          upng_free(upng);
        }



      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
