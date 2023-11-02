#ifndef EPAPER_TYPES_H
#define EPAPER_TYPES_H
typedef struct ImageData {
  bool isUpdated;
  int length;
  UBYTE* data;
} imageData;

#endif