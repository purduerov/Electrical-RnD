// Purdue IEEE ROV Team
// Motor-Testing ESC Black Box
// [v0.1] [2021.01.22] [Tyler Stagge]
//    -This just uses the data from the potentiometer to drive a servo (in preparation for driving the lumineer ESC)
// [v0.2] [2021.01.23] [Tyler Stagge]
//    -Using ESC class instead of Servo
//    -Replaced global constants with #define
// Coded at the Hampton Inn and Suites pool at 1:00am. Copied from other places. Bad.

// ***** ESC *****
#include <ESC.h>

#define PIN_POT (A0)
#define PIN_LED (7)  //ESC LED override (uses I2C)
#define PIN_TLM (1)  //ESC telemetry
#define PIN_PWM (9)
#define EXTRA_5V (2)

#define ESC_MIN (1000)
#define ESC_MAX (2000)
#define ESC_ARM (200)

#define N_ELEMENTS(ARR) (sizeof(ARR) / sizeof(ARR[0]))

ESC esc (PIN_PWM, ESC_MIN, ESC_MAX, ESC_ARM);

int potVal = 0;  //10-bit ADC => value ranges from 0 to 1023
double escSetpoint = 0; //[us]

// ***** Telemetry *****
#include <Wire.h>

static int16_t telemetry[5]; // Temperature, Voltage, Current, used mAh, eRpM
static int16_t telemetry_old[5];

static const uint8_t tel_len = N_ELEMENTS(telemetry);
static uint8_t received_bytes = 0;

static uint16_t request_telemetry = 0;

bool diff = false;

// ***** Load Cell *****
#include "HX711.h"

#define calibration_factor -29200.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
const double leverage = 210/30;

#define DOUT  3 //3
#define CLK  4  //2

HX711 scale;

const int startTime = millis();

void setup(void) {
    Serial.begin(9600);
    pinMode(EXTRA_5V, OUTPUT);
    digitalWrite(EXTRA_5V, HIGH);
    esc.arm();
    delay(1000);
    scale.begin(DOUT, CLK);
    scale.set_scale(calibration_factor);  // This value is obtained by using the SparkFun_HX711_Calibration sketch
    scale.tare();  // Assuming there is no weight on the scale at start up, reset the scale to 0
}

void loop(void) {

    scale.tare();
    for(int i = 512; i < 1024; i++) {
        escSetpoint = map(i, 0, 1023, ESC_MIN, ESC_MAX);
        esc.speed(escSetpoint);
        delay(50);
        receive_telemetry();  // look for incoming bytes

        for(uint8_t i = 0; i < tel_len; i++) {
            diff = telemetry_old[i] != telemetry[i];
            if(diff) {
                break;
            }
        }

        if(diff) {
            Serial.print(millis()-startTime);
            Serial.print("\t");
            Serial.print(escSetpoint); //
            Serial.print("\t");
            Serial.print((scale.get_units()/leverage));
            Serial.print("\t");
            Serial.print(telemetry[0]); //temperature
            Serial.print("\t");
            Serial.print((telemetry[1] / 100.0)); //voltage
            Serial.print("\t");
            Serial.print((telemetry[2] / 100.0)); //current
            Serial.print("\t");
            Serial.print((telemetry[3])); // used mAh
            Serial.print("\t");
            Serial.println((telemetry[4] * 100)); // eRPM
        }

        for(uint8_t i = 0; i < tel_len; i++) {
            telemetry_old[i] = telemetry[i];
        }
        diff = false;

        delay(50);
    }

    // zero the motor for a bit
    escSetpoint = map(512, 0, 1023, ESC_MIN, ESC_MAX);
    esc.speed(escSetpoint);

    delay(5000);

    if(Serial.available()) {
        char temp = Serial.read();
        if(temp == 't' || temp == '0') {
            scale.tare();
        }
    }
}

void receive_telemetry(void) {
    static uint8_t serial_buf[10];

    if(received_bytes < 9) {  // collect bytes
        while(Serial1.available()) {
            serial_buf[received_bytes] = Serial1.read();
            received_bytes++;

            if(received_bytes == 10) {  // transmission complete
                uint8_t crc8 = get_crc8(serial_buf, 9);  // get the 8 bit CRC
                if(crc8 != serial_buf[9]) {
                    while(Serial1.available()) {
                        Serial1.read();
                    }
                    return;
                }

                // slide to the left
                telemetry[0] = serial_buf[0];  // temperature
                telemetry[1] = (serial_buf[1]<<8)|serial_buf[2];  // voltage
                telemetry[2] = (serial_buf[3]<<8)|serial_buf[4];  // Current
                telemetry[3] = (serial_buf[5]<<8)|serial_buf[6];  // used mA/h
                telemetry[4] = (serial_buf[7]<<8)|serial_buf[8];  // eRpM *100
                break;
            }
        }
    }
    received_bytes = 0;
}


uint8_t update_crc8(uint8_t crc, uint8_t crc_seed) {
    uint8_t crc_u;
    crc_u = crc;
    crc_u ^= crc_seed;
    for (uint8_t i = 0; i < 8; i++) {
        crc_u = (crc_u & 0x80) ? 0x7 ^ (crc_u << 1) : (crc_u << 1);
    }
    return crc_u;
}

uint8_t get_crc8(uint8_t *Buf, uint8_t BufLen) {
    uint8_t crc = 0;
    for(uint8_t i = 0; i < BufLen; i++) {
        crc = update_crc8(Buf[i], crc);
    }
    return crc;
}
