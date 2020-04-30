#ifndef IO_H
#define IO_H

#include <Arduino.h>

#include "common.h"
#include "IOBoard.h"

#define PIN_LATCH WD5
#define PIN_CLOCK WD6
#define PIN_DATAOUT WD0
#define PIN_DATAIN WD7
#define PIN_LATCHIN WD8

#define ENCODER_PIN_CLK 2
#define ENCODER_PIN_DIRECTION 3

void setupIOBoard();

IOBoard* getIOBoard();

int32_t getEncoderRotation();

String getIOJsonData(String tabString);

String getEncoderJsonData(String tabString);

#endif