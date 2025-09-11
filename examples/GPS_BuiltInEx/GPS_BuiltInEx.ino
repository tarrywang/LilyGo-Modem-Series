/**
 * @file      GPS_BuiltInEx.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2024  Shenzhen Xin Yuan Electronic Technology Co.,
 * Ltd
 * @date      2024-12-11
 * @note      GPS only supports A7670X/A7608X/SIM7000G/SIM7600 series (excluding A7670G and other
 * versions that do not support positioning).
 *
 * TinyGSM GPS Enhancement Example
 */
#include "utilities.h"

#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS // if enabled it requires the streamDebugger lib
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

String modemName = "UNKNOWN";


void setup()
{
    Serial.begin(115200);
#ifdef BOARD_POWERON_PIN
    /* Set Power control pin output
    * * @note      Known issues, ESP32 (V1.2) version of T-A7670, T-A7608,
    *            when using battery power supply mode, BOARD_POWERON_PIN (IO12) must be set to high level after esp32 starts, otherwise a reset will occur.
    * */
    pinMode(BOARD_POWERON_PIN, OUTPUT);
    digitalWrite(BOARD_POWERON_PIN, HIGH);
#endif

    // Set modem reset pin ,reset modem
#ifdef MODEM_RESET_PIN
    pinMode(MODEM_RESET_PIN, OUTPUT);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL);
    delay(100);
    digitalWrite(MODEM_RESET_PIN, MODEM_RESET_LEVEL);
    delay(2600);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL);
#endif

    // Pull down DTR to ensure the modem is not in sleep state
    pinMode(MODEM_DTR_PIN, OUTPUT);
    digitalWrite(MODEM_DTR_PIN, LOW);

    // Turn on modem
    pinMode(BOARD_PWRKEY_PIN, OUTPUT);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);
    delay(100);
    digitalWrite(BOARD_PWRKEY_PIN, HIGH);
    delay(MODEM_POWERON_PULSE_WIDTH_MS);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);

    // Set modem baud
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);

    Serial.println("Start modem...");
    delay(3000);

    int retry = 0;
    while (!modem.testAT(1000)) {
        Serial.println(".");
        if (retry++ > 30) {
            digitalWrite(BOARD_PWRKEY_PIN, LOW);
            delay(100);
            digitalWrite(BOARD_PWRKEY_PIN, HIGH);
            delay(MODEM_POWERON_PULSE_WIDTH_MS);
            digitalWrite(BOARD_PWRKEY_PIN, LOW);
            retry = 0;
        }
    }
    Serial.println();
    delay(200);

    while (1) {
        modemName = modem.getModemName();
        if (modemName == "UNKNOWN") {
            Serial.println("Unable to obtain module information normally, try again");
            delay(1000);
        } else if (modemName.startsWith("A7670E-FASE") || modemName.startsWith("A7670SA-FASE")) {
            Serial.println("Modem support built-in GPS function, keep running");
            break;
        } else if (modemName.startsWith("A7670E-LNXY-UBL")
                   || modemName.startsWith("A7670SA-LASE")
                   || modemName.startsWith("A7670SA-LASC")
                   ||  modemName.startsWith("A7670G-LLSE")
                   ||  modemName.startsWith("A7670G-LABE")
                   ||  modemName.startsWith("A7670E-LASE ")) {
            while (1) {
                Serial.println("The modem does not have built-in GPS function.");
                Serial.println("If you purchased the A7670G with GPS version, please run this example, https://github.com/Xinyuan-LilyGO/LilyGo-Modem-Series/blob/main/examples/ExternalGPS_A7670G_Only/ExternalGPS_A7670G_Only.ino");
                delay(1000);
            }
        } else {
            Serial.print("Model Name:");
            Serial.println(modemName);
            break;
        }
        delay(5000);
    }

    // Print modem software version
    String res;
    modem.sendAT("+SIMCOMATI");
    modem.waitResponse(10000UL, res);
    Serial.println(res);


#ifdef TINY_GSM_MODEM_SIM7080
    // SIM7080G/SIM7070G GPS and LTE connections cannot be enabled at the same time
    // Disconnect the cellular connection first.
    modem.setNetworkDeactivate();
#endif

    Serial.println("Enabling GPS/GNSS/GLONASS");
    while (!modem.enableGPS(MODEM_GPS_ENABLE_GPIO, MODEM_GPS_ENABLE_LEVEL)) {
        Serial.print(".");
    }
    Serial.println();
    Serial.println("GPS Enabled");

    // Set GPS Baud to 115200
    modem.setGPSBaud(115200);

}


void loopGPS(uint8_t gnss_mode)
{

    Serial.println("=========================");
    Serial.print("Set GPS Mode : ");
    Serial.println(gnss_mode);

    modem.setGPSMode(gnss_mode);

    GPSInfo info;

    Serial.println("Requesting current GPS/GNSS/GLONASS location");

    for (;;) {
        if (modem.getGPS_Ex(info)) {

            Serial.println();

            Serial.print("FixMode:");
            Serial.println(info.isFix);

            Serial.print("Latitude:");
            Serial.println(info.latitude, 6);
            Serial.print("Longitude:");
            Serial.println(info.longitude, 6);
            Serial.print("Speed:");
            Serial.println(info.speed);
            Serial.print("Altitude:");
            Serial.println(info.altitude);

            Serial.println("Visible Satellites:");
            Serial.print(" GPS Satellites:");
            Serial.println(info.gps_satellite_num);
            Serial.print(" BEIDOU Satellites:");
            Serial.println(info.beidou_satellite_num);
            Serial.print(" GLONASS Satellites:");
            Serial.println(info.glonass_satellite_num);
            Serial.print(" GALILEO Satellites:");
            Serial.println(info.galileo_satellite_num);

            Serial.println("Date Time:");
            Serial.print("Year:");
            Serial.print(info.year);
            Serial.print(" Month:");
            Serial.print(info.month);
            Serial.print(" Day:");
            Serial.println(info.day);
            Serial.print("Hour:");
            Serial.print(info.hour);
            Serial.print(" Minute:");
            Serial.print(info.minute);
            Serial.print(" Second:");
            Serial.println(info.second);

            Serial.print("Course:");
            Serial.println(info.course);
            Serial.print("PDOP:");
            Serial.println(info.PDOP);
            Serial.print("HDOP:");
            Serial.println(info.HDOP);
            Serial.print("VDOP:");
            Serial.println(info.VDOP);

            String gps_raw = modem.getGPSraw();
            Serial.print("GPS/GNSS Based Location String:");
            Serial.println(gps_raw);

            break;
        } else {
            // Serial.println("Couldn't get GPS/GNSS/GLONASS location, retrying in
            // 15s.");
            Serial.print(".");
            delay(15000L);
        }
    }
}

void loop()
{
    uint8_t gnss_length = 0;
    uint8_t a76xx_gnss_mode[] = {1, 2, 3, 4};
    uint8_t sim767x_gnss_mode[] = {1, 3, 5, 9, 13, 15}; //SIM7600 series same
    uint8_t sim70xx_gnss_mode[] = {0, 1, 2};
    uint8_t *gnss_mode = NULL;
    if (modemName.startsWith("A767")) {
        gnss_mode = a76xx_gnss_mode;
        gnss_length = sizeof(a76xx_gnss_mode) / sizeof(a76xx_gnss_mode[0]);
    } else if (modemName.startsWith("SIM7080") || modemName.startsWith("SIM7000") || modemName.startsWith("SIM7070") ) {
        gnss_mode = sim70xx_gnss_mode;
        gnss_length = sizeof(sim70xx_gnss_mode) / sizeof(sim70xx_gnss_mode[0]);
    } else {
        gnss_mode = sim767x_gnss_mode;
        gnss_length = sizeof(sim767x_gnss_mode) / sizeof(sim767x_gnss_mode[0]);
    }

    for (int i = 0; i < gnss_length; ++i) {
        /*
         * Model: A76XX
         * 1 - GPS L1 + SBAS + QZSS
         * 2 - BDS B1
         * 3 - GPS + GLONASS + GALILEO + SBAS + QZSS
         * 4 - GPS + BDS + GALILEO + SBAS + QZSS.
         *
         * Model: SIM7670G
         * 1  -  GPS
         * 3  -  GPS + GLONASS
         * 5  -  GPS + GALILEO
         * 9  -  GPS + BDS
         * 13 -  GPS + GALILEO + BDS
         * 15 -  GPS + GLONASS + GALILEO + BDS
         *
         * Model: SIM7600 series
         * Range – 0 to 15
         * Bit0 – GLONASS
         * Bit1 – BEIDOU
         * Bit2 – GALILEO
         * Bit3 – QZSS
         * */
        loopGPS(gnss_mode[i]);
    }


    Serial.println("Disabling GPS");

    modem.disableGPS(MODEM_GPS_ENABLE_GPIO, !MODEM_GPS_ENABLE_LEVEL);

    while (1) {
        if (SerialAT.available()) {
            Serial.write(SerialAT.read());
        }
        if (Serial.available()) {
            SerialAT.write(Serial.read());
        }
        delay(1);
    }
}

#ifndef TINY_GSM_FORK_LIBRARY
#error                                                                         \
"No correct definition detected, Please copy all the [lib directories](https://github.com/Xinyuan-LilyGO/LilyGO-T-A76XX/tree/main/lib) to the arduino libraries directory , See README"
#endif

/*
SIM7600 Version OK 20250709
AT+SIMCOMATI
Manufacturer: SIMCOM INCORPORATED
Model: SIMCOM_SIM7600G-H
Revision: LE20B04SIM7600G22
QCN:
IMEI: xxxxxxxxxxxx
MEID:
+GCAP: +CGSM
DeviceInfo: 173,170
*/
