<div align="center" markdown="1">
  <img src="../../../.github/LilyGo_logo.png" alt="LilyGo logo" width="100"/>
</div>

<h1 align = "center">🌟Sim Shield Usage Guide🌟</h1>

* The SimShield is an expansion board for the Sim series that includes three-way current detection, LoRa transceiver, and wide-range DC input. It supports the full range of LilyGo T-Sim expansion boards.

> \[!IMPORTANT]
>
> Warning: Jumper caps and DIP switches must be checked when power is on.
> Risk of damage if wrong selection is made
>

### ⚡ Electrical parameters

| Features                                | Details                       |
| --------------------------------------- | ----------------------------- |
| ⚡DC interface allows input voltage      | 7~36V                         |
| ⚡Battery interface allows input voltage | 4.2V                          |
| ⚡Charge Current                         | Provided by the motherboard   |
| ⚡+5V                                    | The maximum current can be 2A |
| ⚡+3V3                                   | Provided by the motherboard   |

* It is recommended not to connect the +3V3 pin to peripheral devices with a current exceeding 100mA, as different motherboards provide different currents.

## SIM7000G/A7670X/A7608X ESP32

![SIM7000_A7608_A7670_ESP32](./images/SIM7000_A7608_A7670_ESP32.png)

1. **J25** short **IO** -> **LP2**
2. **J21** short **RP1** -> **5V**
3. **SW3** is turned to **ON**, and the remaining **SW1, SW2, and SW4** must not be in the ON position.

### Applicable Models

1. [SIM7000G-ESP32 Version](https://lilygo.cc/products/t-sim7000g?_pos=1&_sid=d79d65953&_ss=r)
2. [A7608X-ESP32 Version](https://lilygo.cc/products/t-a7608e-h?variant=43275846090933)
3. [A7670X-ESP32 Version](https://lilygo.cc/products/t-sim-a7670e?_pos=1&_sid=f2986df37&_ss=r&variant=42534734495925)

### Pinout

| SIM7000G/A7670X/A7608X ESP32 | GPIO |
| ---------------------------- | ---- |
| SIMSHIELD_MOSI               | 23   |
| SIMSHIELD_MISO               | 19   |
| SIMSHIELD_SCK                | 18   |
| SIMSHIELD_SD_CS              | 32   |
| SIMSHIELD_RADIO_BUSY         | 39   |
| SIMSHIELD_RADIO_CS           | 5    |
| SIMSHIELD_RADIO_IRQ          | 34   |
| SIMSHIELD_RADIO_RST          | 15   |
| SIMSHIELD_RS_RX              | 13   |
| SIMSHIELD_RS_TX              | 14   |
| SIMSHIELD_SDA                | 21   |
| SIMSHIELD_SCL                | 22   |

## SIM7600X ESP32

![SIM7600_ESP32](./images/SIM7600_ESP32.png)

1. **J25** short **IO** -> **BAT**
2. **J21** short **RP2** -> **5V**
3. **SW3** is turned to **ON**, and the remaining **SW1, SW2, and SW4** must not be in the ON position.

> \[!IMPORTANT]
> The SIM7600 pin header is 15 pins, and the insertion position must be where the arrow points.
>

### Applicable Models

1. [SIM7600X-ESP32 Version](https://lilygo.cc/products/t-sim7600?_pos=1&_sid=7474cd3cf&_ss=r&variant=42358717939893)

### Pinout

| SIM7600X ESP32       | GPIO |
| -------------------- | ---- |
| SIMSHIELD_MOSI       | 23   |
| SIMSHIELD_MISO       | 19   |
| SIMSHIELD_SCK        | 18   |
| SIMSHIELD_SD_CS      | 32   |
| SIMSHIELD_RADIO_BUSY | 39   |
| SIMSHIELD_RADIO_CS   | 5    |
| SIMSHIELD_RADIO_IRQ  | 34   |
| SIMSHIELD_RADIO_RST  | 14   |
| SIMSHIELD_RS_RX      | 12   |
| SIMSHIELD_RS_TX      | 13   |
| SIMSHIELD_SDA        | 21   |
| SIMSHIELD_SCL        | 22   |

## SIM7670G ESP32S3

![SIM7670G_ESP32S3](./images/SIM7670G_ESP32S3.png)

1. **J25** short **IO** -> **LP2**
2. **J21** short **RP1** -> **5V**
3. **SW2** is turned to **ON**, and the remaining **SW1, SW3, and SW4** must not be in the ON position.

### Applicable Models

1. [SIM7670G-ESP32S3 Version](https://lilygo.cc/products/t-sim-7670g-s3?_pos=1&_sid=2be878e69&_ss=r)

### Pinout

| SIM7670G ESP32S3     | GPIO |
| -------------------- | ---- |
| SIMSHIELD_MOSI       | 15   |
| SIMSHIELD_MISO       | 7    |
| SIMSHIELD_SCK        | 16   |
| SIMSHIELD_SD_CS      | 46   |
| SIMSHIELD_RADIO_BUSY | 38   |
| SIMSHIELD_RADIO_CS   | 39   |
| SIMSHIELD_RADIO_IRQ  | 6    |
| SIMSHIELD_RADIO_RST  | 40   |
| SIMSHIELD_RS_RX      | 41   |
| SIMSHIELD_RS_TX      | 42   |
| SIMSHIELD_SDA        | 2    |
| SIMSHIELD_SCL        | 1    |

## A7608X ESP32S3

![A7608_ESP32S3](./images/A7608_ESP32S3.png)

1. **J25** short **IO** -> **LP2**
2. **J21** not connected to any
3. **SW2** is turned to **ON**, and the remaining **SW1, SW3, and SW4** must not be in the ON position.

### Applicable Models

1. [A7608X-ESP32S3 Version](https://lilygo.cc/products/t-a7608e-h?_pos=1&_sid=e4fd02f43&_ss=r)

### Pinout

| SIM7670G ESP32S3     | GPIO |
| -------------------- | ---- |
| SIMSHIELD_MOSI       | 11   |
| SIMSHIELD_MISO       | 10   |
| SIMSHIELD_SCK        | 12   |
| SIMSHIELD_SD_CS      | 45   |
| SIMSHIELD_RADIO_BUSY | 38   |
| SIMSHIELD_RADIO_CS   | 39   |
| SIMSHIELD_RADIO_IRQ  | 9    |
| SIMSHIELD_RADIO_RST  | 40   |
| SIMSHIELD_RS_RX      | 41   |
| SIMSHIELD_RS_TX      | 42   |
| SIMSHIELD_SDA        | 2    |
| SIMSHIELD_SCL        | 1    |

## SIM7080G ESP32S3

![SIM7080G_ESP32S3](./images/SIM7080G_ESP32S3.png)

1. **J25** not connected to any
2. **J21** not connected to any
3. **SW4** is turned to **ON**, and the remaining **SW1, SW2, and SW3** must not be in the ON position.

> \[!IMPORTANT]
>
> 1. The SIM7080G-ESP32S3 version does not expose the battery external pin. The battery external function of SimShield cannot be used. The positive pole of the 18650 battery holder needs to be welded to the left pin.In the picture, +BAT points to the welding direction of the positive pole of the battery.
> 2. The SIM7080G-ESP32S3 version has an onboard 18650 battery holder that conflicts with SimShield. The battery holder must be removed before installation
>

### Applicable Models

1. [SIM7080G-ESP32S3 Version](https://lilygo.cc/products/t-sim7080-s3?_pos=1&_sid=7d3cc194f&_ss=r)

### Pinout

| SIM7080G ESP32S3     | GPIO |
| -------------------- | ---- |
| SIMSHIELD_MOSI       | 11   |
| SIMSHIELD_MISO       | 13   |
| SIMSHIELD_SCK        | 12   |
| SIMSHIELD_SD_CS      | 21   |
| SIMSHIELD_RADIO_BUSY | 48   |
| SIMSHIELD_RADIO_CS   | 45   |
| SIMSHIELD_RADIO_IRQ  | 8    |
| SIMSHIELD_RADIO_RST  | 47   |
| SIMSHIELD_RS_RX      | 2    |
| SIMSHIELD_RS_TX      | 1    |
| SIMSHIELD_SDA        | 44   |
| SIMSHIELD_SCL        | 43   |

## Monitor battery charging and discharging jumper settings

* The vertical jumper cap in the figure below routes the battery current to INA3221 channel 2, and the battery charging and discharging current can be monitored by reading the INA3221 sensor. **At this time, do not connect anything to the crimp terminal interface.**
* If the jumper cap is not connected, the crimp terminal interface current is monitored

![Monitor battery charging and discharging jumper](./images/battery_current_detection_mode.png)

## 5V 2.00mm 2Pin power supply interface

* This interface is used to supply external power and has no other functions.

## SD interface

* SimShield remaps the SPI interface. After using SimShield, do not insert the SD card into the motherboard. Instead, insert the SD card into SimShield.

## RS485 interface

* RS485 uses hardware automatic sending and receiving control, and it is recommended to use a communication baud rate below 115200

## I2C interface

* [0.96-inch OLED display](https://lilygo.cc/products/0-96-inch-oled?_pos=1&_sid=61234aa32&_ss=r)
* The I2C interface is suitable for OLED screens connected to conventional interfaces. Please note that the OLED interface has two line sequences. Please make sure that the line sequence is correct. Please refer to this [link](https://lilygo.cc/products/0-96-inch-oled?_pos=1&_sid=61234aa32&_ss=r) for the OLED interface

## SPI interface

* SPI interface exposes the SPI bus interface of SD card/Radio

## Current detection

![intervface](./images/intervface.png)

* If the jumper of current detection channel 2 is selected as the battery detection channel, the external wiring terminal is unavailable.
* Channel 3 and channel 1 are free to use. The **IN+** of the terminal is the direction of current inflow, **G** is the **GND** of the load, and **IN-** is the direction of current outflow from the load.

## Interface Description

| Name          | Details                                         |
| ------------- | ----------------------------------------------- |
| INPUT:7~36V   | DC input, range 7 ~ 36 V                        |
| OUTPUT:5V     | Only when DC is input will 5V voltage be output |
| VBAT:+4.2V-   | Battery input interface, maximum 4.2V           |
| IN+ G IN- CH2 | Current detection channel 2                     |
| IN+ G IN- CH3 | Current detection channel 3                     |
| IN+ G IN- CH1 | Current detection channel 1                     |

* **The battery input interface is connected in parallel with the onboard 18650. If an external battery is connected, do not install the battery on the 18650 socket.**
* **Current detection channel 1 (this channel is unavailable if a battery is connected via a jumper cap)**

## Examples

1. [SimShield_LoRaReceive](../../../examples/SimShield_LoRaReceive/SimShield_LoRaReceive.ino)
2. [SimShield_LoRaTransmit](../../../examples/SimShield_LoRaTransmit/SimShield_LoRaTransmit.ino)
3. [SimShieldCurrentSensor](../../../examples/SimShieldCurrentSensor/SimShieldCurrentSensor.ino)
4. [SimShieldFactory](../../../examples/SimShieldFactory/SimShieldFactory.ino)
5. [SerialRS485](../../../examples/SerialRS485/SerialRS485.ino)
