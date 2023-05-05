#pragma once

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

  size_t bytes;
};