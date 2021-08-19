// Purdue IEEE ROV Team
// Motor-Testing ESC Black Box
// [v0.1] [2021.01.22] [Tyler Stagge]
//    -This just uses the data from the potentiometer to drive a servo (in preparation for driving the lumineer ESC)
// [v0.2] [2021.01.23] [Tyler Stagge]
//    -Using ESC class instead of Servo
//    -Replaced global constants with #define

#include <ESC.h>

#define PIN_POT (A0)
#define PIN_LED (7)  // ESC LED override (uses I2C)
#define PIN_TLM (1)  // ESC telemetry
#define PIN_PWM (9)
#define EXTRA_5V (2)

#define ESC_MIN (1000)
#define ESC_MAX (2000)
#define ESC_ARM (200)

ESC esc (PIN_PWM, ESC_MIN, ESC_MAX, ESC_ARM);

int potVal = 0;  // 10-bit ADC => value ranges from 0 to 1023
double escSetpoint = 0; // [us]


// Load Cell
#include "HX711.h"

#define calibration_factor -29200.0 // This value is obtained using the SparkFun_HX711_Calibration sketch
const double leverage = 210/30;

#define DOUT  3 // 3
#define CLK   4 // 2

const int startTime = millis();
int currentTime = millis();

HX711 scale;

void setup(void) {
    Serial.begin(9600);
    pinMode(EXTRA_5V, OUTPUT);
    digitalWrite(EXTRA_5V, HIGH);
    esc.arm();
    delay(1000);

    Serial.println("hello");
    Serial.println(startTime);

    scale.begin(DOUT, CLK);
    scale.set_scale(calibration_factor); // This value is obtained by using the SparkFun_HX711_Calibration sketch
    scale.tare(); // Assuming there is no weight on the scale at start up, reset the scale to 0
    // esc.speed(1000);
}

void loop(void) {
    if(currentTime - 10000 < startTime){
        escSetpoint = map(512, 0, 1023, ESC_MIN, ESC_MAX);
        // Serial.print("first 10 seconds");
        // Serial.print("\t");
        delay(15);
        currentTime = millis();
    } else if (currentTime - 300000 < startTime){
        potVal = round(512 + 0.15 * 512);
        escSetpoint = map(potVal, 0, 1023, ESC_MIN,ESC_MAX);
        delay(15);
        currentTime = millis();
    } else {
        escSetpoint = map(512, 0, 1023, ESC_MIN,ESC_MAX);
    }

    Serial.print(millis() - startTime);
    Serial.print("\t");
    Serial.print(escSetpoint);
    Serial.print("\t");
    esc.speed(escSetpoint);
    delay(15);

    Serial.print(scale.get_units()/leverage, 2); // scale.get_units() returns a float
    Serial.print("\t");
    Serial.print(" kg"); 
    Serial.println();
}
