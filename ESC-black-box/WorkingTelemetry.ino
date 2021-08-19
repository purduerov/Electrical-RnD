#include <Wire.h>

#define N_ELEMENTS(ARR) (sizeof(ARR) / sizeof(ARR[0]))

static int16_t telemetry[5];  // Temperature, Voltage, Current, used mAh, eRpM
static int16_t telemetry_old[5];

static const uint8_t tel_len = N_ELEMENTS(telemetry);
static uint8_t received_bytes = 0;

static uint16_t request_telemetry = 0;

bool diff = false;

void setup() {
    pinMode(9,OUTPUT);

    Serial.begin(9600);  // open seria0 for serial monitor
    Serial1.begin(115200);  // open Serial1 for ESC communication
}

void loop() {
    static uint32_t loop_time = 0;
    if((micros() - loop_time) > 2000) {
        loop_time = micros();
        receive_telemetry();  // look for incoming bytes

        for(uint8_t i = 0; i < tel_len; i++) {
            diff = telemetry_old[i] != telemetry[i];
            if(diff) {
                break;
            }
        }

        if(diff) {
            Serial.print(" Temperature(C):");
            Serial.print(telemetry[0]);
            Serial.print(" Voltage(V):");
            Serial.print(telemetry[1] / 100.0);
            Serial.print(" Current(A):");
            Serial.print(telemetry[2] / 100.0);
            Serial.print(" used_mA/h:");
            Serial.print(telemetry[3]);
            Serial.print(" eRpM/10:");
            Serial.println(telemetry[4] * 100);
        }

        for(uint8_t i = 0; i < tel_len; i++) {
            telemetry_old[i] = telemetry[i];
        }

        diff = false;
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