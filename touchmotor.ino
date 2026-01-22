#include "unit_rolleri2c.hpp"
//#include <M5Unified.h>
UnitRollerI2C RollerI2C;  // Create a UNIT_ROLLERI2C object
//uint32_t p, i, d;         // Defines a variable to store the PID value
//uint8_t r, g, b;


void setup()
{
    Serial.begin(9600);
    delay(300);
    
    //M5.begin();
    RollerI2C.begin(&Wire, 0x64, 5, 6, 400000);
    RollerI2C.setOutput(0);
    RollerI2C.setMode(ROLLER_MODE_POSITION);
    RollerI2C.setPosMaxCurrent(100000);
    RollerI2C.setOutput(1);
}

void loop()
{
    if (!Serial.available()) return;
    int deg = Serial.parseInt();
    while (Serial.available()) Serial.read();
    //move motor
    RollerI2C.setPos(deg * 100);
    
}
