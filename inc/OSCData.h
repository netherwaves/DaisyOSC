#pragma once

#include "daisy_core.h"
#include "OSCTiming.h"

typedef enum
{
  OSC_OK = 0,
  BUFFER_FULL,
  INVALID_OSC,
  ALLOCFAILED,
  INDEX_OUT_OF_BOUNDS
} OSCErrorCode;

class OSCData
{
private:
  // friends
  friend class OSCMessage;

  // should only be used while decoding
  // leaves an invalid OSCMessage with a type, but no data
  OSCData(char t);

public:
  // an error flag
  OSCErrorCode error;

  // the size (in bytes) of the data
  int bytes;

  // the type of the data
  uint8_t type;

  // the data
  union
  {
    char *s;    // string
    int32_t i;  // int
    float f;    // float
    double d;   // double
    uint64_t l; // long
    uint8_t *b; // blob
    osctime_t time;
  } data;

  // overload the constructor to account for all the types and sizes
  OSCData(const char *s);
  OSCData(int);
  OSCData(int32_t);
  OSCData(float);
  OSCData(double);
  OSCData(uint8_t *, int);
  // accepts another OSCData objects and clones it
  OSCData(OSCData *);
  OSCData(osctime_t);

  // destructor
  ~OSCData();

  // GETTERS
  int32_t getInt();
  float getFloat();
  double getDouble();
  int getString(char *);
  int getString(char *, int);
  int getString(char *, int, int, int);
  int getBlob(uint8_t *);
  int getBlob(uint8_t *, int);
  int getBlob(uint8_t *, int, int, int);
  int getBlobLength();
  bool getBoolean();
  osctime_t getTime();

  // constructor from byte array with type and length
  OSCData(char, uint8_t *, int);
  // fill the passed in buffer with the data
  // uint8_t * asByteArray();
};

template <typename T>
static inline T BigEndian(const T &x)
{
  const int one = 1;
  const char sig = *(char *)&one;
  if (sig == 0)
    return x; // for big endian machine just return the input
  T ret;
  int size = sizeof(T);
  char *src = (char *)&x + sizeof(T) - 1;
  char *dst = (char *)&ret;
  while (size-- > 0)
  {
    *dst++ = *src--;
  }
  return ret;
}