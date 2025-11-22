//
//    FILE: HX_kitchen_scale.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: HX711 demo
//     URL: https://github.com/RobTillaart/HX711


#include "HX711.h"
#include "scale.h"

HX711 scale;

//  adjust pins if needed
uint8_t dataPin = 6;
uint8_t clockPin = 7;
float calibration = -108.207550;

float w1, w2, previous = 0;


void scale_init()
{
  //Serial.begin(115200);
  Serial.println();
  Serial.println(__FILE__);
  Serial.print("HX711_LIB_VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  scale.begin(dataPin, clockPin);

  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));

  //  load cell factor 20 KG
  //  scale.set_scale(127.15);
  //  load cell factor 5 KG
  scale.set_scale(calibration);       //  TODO you need to calibrate this yourself.
  scale.tare();

  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));
}

float scale_read_delta()
{
    // read until stable
    float w1 = scale.get_units(10);
    delay(100);
    float w2 = scale.get_units();

    while (fabs(w1 - w2) > 10)
    {
        w1 = w2;
        w2 = scale.get_units();
        delay(100);
    }

    // compute delta
    float delta = w1 - previous;

    // update previous reading
    previous = w1;

    // return the result
    return delta;
}


//  -- END OF FILE --

