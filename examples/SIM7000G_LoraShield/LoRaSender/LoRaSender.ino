/**
 * @file      LoRaSender.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2025  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2025-09-10
 * @note      LoRaSender only support SIM7000G  + LoRa Shield
 */
#include <SPI.h>
#include <SD.h>
#include <LoRa.h>

#define PIN_TX              27
#define PIN_RX              26
#define UART_BAUD           115200
#define PWR_PIN             4

#define SD_MISO             2
#define SD_MOSI             15
#define SD_SCLK             14
#define SD_CS               13

#define LORA_RST            12
#define LORA_DI0            32
#define RADIO_DIO_1         33
#define RADIO_DIO_2         34
#define LORA_SS             5
#define LORA_MISO           19
#define LORA_MOSI           23
#define LORA_SCK            18

#define BAND                470E6

uint32_t counter = 0;
SPIClass SPIRadio(HSPI);

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    Serial.println("LoRaSender only support SIM7000G  + LoRa Shield , see  https://github.com/Xinyuan-LilyGO/LilyGo-Modem-Series/issues/357");

    Serial.println("LoRa Sender");
    // Specify pin to initialize SPI
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS)) {
        Serial.println("SDCard MOUNT FAIL");
    } else {
        uint32_t cardSize = SD.cardSize() / (1024 * 1024);
        String str = "SDCard Size: " + String(cardSize) + "MB";
        Serial.println(str);
    }

    // Specify pin to initialize SPIRadio
    SPIRadio.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
    LoRa.setSPI(SPIRadio);
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
    Serial.printf("Setup Lora freq : %.0f\n", BAND);
    if (!LoRa.begin(BAND)) {
        Serial.println("LoRa init failed. Check your connections.");
        while (1);
    }
    Serial.println("LoRa begin successfully");
}

void loop()
{
    Serial.print("Sending packet: ");
    Serial.println(counter);

    // send packet
    LoRa.beginPacket();
    LoRa.print("hello ");
    LoRa.print(counter);
    LoRa.endPacket();

    counter++;

    delay(5000);
}
