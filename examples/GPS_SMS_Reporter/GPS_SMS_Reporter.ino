/**
 * @file      GPS_SMS_Reporter.ino
 * @brief     采集GPS数据并通过短信发送
 * @note      支持 A7670E-FASE 模块，使用 AT+CMGS 发送短信
 */

#include "utilities.h"

#define TINY_GSM_RX_BUFFER 1024
#define SerialMon Serial
#define SMS_TARGET  "+8615821259813"
#define SMS_CENTER  "+316540942001"  // 短信中心号码 (实测可用)
#define SMS_INTERVAL_MS 60000  // 每60秒发送一次

// 全局计数器
int smsCount = 0;

#include <TinyGsmClient.h>

TinyGsm modem(SerialAT);

// GPS数据结构
struct GPSData {
    float latitude;
    float longitude;
    float altitude;
    float speed;
    int satellites;
    int year, month, day;
    int hour, minute, second;
    bool valid;
    String location;
};

/**
 * 坐标转换：度分格式 -> 十进制度
 */
float convertToDecimalDegrees(float coord) {
    int degrees = (int)(coord / 100);
    float minutes = coord - (degrees * 100);
    return degrees + (minutes / 60.0);
}

/**
 * 根据经纬度判断位置
 */
String getLocationDescription(float lat, float lon) {
    if (lon >= 121.0 && lon <= 122.0 && lat >= 30.5 && lat <= 31.8) {
        return "中国上海市附近";
    } else if (lon >= 73.0 && lon <= 135.0 && lat >= 18.0 && lat <= 54.0) {
        return "中国境内";
    }
    return "未知位置";
}

/**
 * 发送AT命令并等待响应
 */
String sendATCommand(const String &cmd, unsigned long timeout = 1000) {
    Serial.printf("[AT] %s\n", cmd.c_str());

    // 清空接收缓冲区
    while (SerialAT.available()) {
        SerialAT.read();
    }

    // 发送命令
    SerialAT.println(cmd);

    // 等待响应
    String response = "";
    unsigned long startTime = millis();

    while (millis() - startTime < timeout) {
        if (SerialAT.available()) {
            char c = SerialAT.read();
            response += c;
        }
        delay(10);
    }

    Serial.print(response);
    Serial.println("---");

    return response;
}

/**
 * 采集GPS数据 - 使用逐步AT命令测试
 */
bool collectGPSData(GPSData &gps) {
    Serial.println("\n========================================");
    Serial.println("开始采集GPS数据 (AT命令逐步测试)...");
    Serial.println("========================================\n");

    gps.valid = false;

    // Step 1: 检查GPS电源状态
    Serial.println("Step 1: 检查GPS电源状态");
    String resp = sendATCommand("AT+CGNSPWR?", 2000);

    // Step 2: 启用GPS电源
    Serial.println("\nStep 2: 启用GPS电源");
    sendATCommand("AT+CGNSPWR=1", 2000);
    delay(1000);

    // Step 3: 验证GPS电源已启用
    Serial.println("\nStep 3: 验证GPS电源状态");
    sendATCommand("AT+CGNSPWR?", 2000);

    // Step 4: 检查CGPS状态
    Serial.println("\nStep 4: 检查CGPS状态");
    sendATCommand("AT+CGPS?", 2000);

    // Step 5: 启用CGPS
    Serial.println("\nStep 5: 启用CGPS");
    sendATCommand("AT+CGPS=1", 2000);
    delay(1000);

    // Step 6: 验证CGPS已启用
    Serial.println("\nStep 6: 验证CGPS状态");
    sendATCommand("AT+CGPS?", 2000);

    Serial.println("\n等待GPS定位...");
    Serial.println("提示: 需要GPS天线 + 室外/窗边 + 30-60秒\n");

    // Step 7: 循环获取GPS数据
    for (int attempt = 1; attempt <= 10; attempt++) {
        Serial.printf("\n[尝试 %d/10] 获取GPS数据...\n", attempt);

        // 方法1: AT+CGPSINFO
        Serial.println("方法1: AT+CGPSINFO");
        String gpsInfo = sendATCommand("AT+CGPSINFO", 3000);

        // 方法2: AT+CGNSSINFO
        Serial.println("\n方法2: AT+CGNSSINFO");
        String gnssInfo = sendATCommand("AT+CGNSSINFO", 3000);

        // 解析CGNSSINFO响应
        // 格式: +CGNSSINFO: <mode>,<lon>,<lat>,<date>,<UTC time>,<alt>,<speed>,<course>,<PDOP>,<HDOP>,<VDOP>,<sat_in_use>,<sat_in_view>,<GNSS sat in use>,<GLONASS sat in use>,<BEIDOU sat in use>

        int infoStart = gnssInfo.indexOf("+CGNSSINFO: ");
        if (infoStart >= 0) {
            String data = gnssInfo.substring(infoStart + 12);
            data.trim();

            Serial.println("\n解析GPS数据:");
            Serial.println(data);

            // 分割字段
            int fieldCount = 0;
            int startPos = 0;
            String fields[16];

            for (int i = 0; i < data.length() && fieldCount < 16; i++) {
                if (data[i] == ',') {
                    fields[fieldCount++] = data.substring(startPos, i);
                    startPos = i + 1;
                }
            }
            fields[fieldCount++] = data.substring(startPos);

            // 检查定位模式
            if (fieldCount > 0 && fields[0].length() > 0 && fields[0] != "0") {
                Serial.println("\n✓ GPS定位成功!");

                // 提取数据
                // fields[1] = 经度, fields[2] = 纬度
                float rawLat = fields[2].toFloat();
                float rawLon = fields[1].toFloat();

                if (rawLat != 0.0 && rawLon != 0.0) {
                    gps.latitude = convertToDecimalDegrees(rawLat);
                    gps.longitude = convertToDecimalDegrees(rawLon);
                    gps.altitude = fields[5].toFloat();
                    gps.speed = fields[6].toFloat();
                    gps.satellites = fields[11].toInt();

                    // 解析日期时间 (fields[3] = ddmmyyyy, fields[4] = hhmmss.sss)
                    String dateStr = fields[3];
                    String timeStr = fields[4];

                    if (dateStr.length() >= 8) {
                        gps.day = dateStr.substring(0, 2).toInt();
                        gps.month = dateStr.substring(2, 4).toInt();
                        gps.year = dateStr.substring(4, 8).toInt();
                    }

                    if (timeStr.length() >= 6) {
                        gps.hour = timeStr.substring(0, 2).toInt();
                        gps.minute = timeStr.substring(2, 4).toInt();
                        gps.second = timeStr.substring(4, 6).toInt();
                    }

                    gps.valid = true;
                    gps.location = getLocationDescription(gps.latitude, gps.longitude);

                    Serial.printf("纬度: %.7f°N\n", gps.latitude);
                    Serial.printf("经度: %.7f°E\n", gps.longitude);
                    Serial.printf("位置: %s\n", gps.location.c_str());
                    Serial.printf("卫星数: %d 颗\n", gps.satellites);
                    Serial.printf("海拔: %.1f 米\n", gps.altitude);
                    Serial.printf("速度: %.3f km/h\n", gps.speed);
                    Serial.printf("时间: %04d-%02d-%02d %02d:%02d:%02d UTC\n\n",
                        gps.year, gps.month, gps.day, gps.hour, gps.minute, gps.second);

                    break;
                }
            }
        }

        if (attempt < 10) {
            Serial.println("等待15秒后重试...\n");
            delay(15000);
        }
    }

    Serial.println("\n关闭GPS模块");
    sendATCommand("AT+CGPS=0", 2000);
    sendATCommand("AT+CGNSPWR=0", 2000);

    return gps.valid;
}

/**
 * 格式化GPS短信 (按照指定格式)
 */
String formatGPSSMS(const GPSData &gps) {
    String sms = "### 您的当前位置信息:\n\n";
    sms += "- **纬度**: " + String(gps.latitude, 7) + "°N\n";
    sms += "- **经度**: " + String(gps.longitude, 7) + "°E\n";
    sms += "- **位置**: " + gps.location + "\n";
    sms += "- **卫星数**: " + String(gps.satellites) + " 颗\n";
    sms += "- **海拔**: " + String(gps.altitude, 1) + " 米\n";

    // 速度状态
    if (gps.speed < 1.0) {
        sms += "- **速度**: 0.000 km/h (静止)\n";
    } else {
        sms += "- **速度**: " + String(gps.speed, 3) + " km/h\n";
    }

    // 格式化时间
    char timeStr[30];
    snprintf(timeStr, sizeof(timeStr), "%04d-%02d-%02d %02d:%02d:%02d UTC",
             gps.year, gps.month, gps.day, gps.hour, gps.minute, gps.second);
    sms += "- **时间**: " + String(timeStr) + "\n";
    sms += "- **定位状态**: 1 (已成功定位)";

    return sms;
}

/**
 * 使用AT+CMGS发送短信（支持中文UCS2编码）
 */
bool sendSMS(const String &message) {
    Serial.println("\n========================================");
    Serial.println("准备发送短信...");
    Serial.println("========================================");

    // 检查信号质量（物联网卡可能不报告标准网络注册状态）
    Serial.println("\n检查信号质量...");

    // 等待信号稳定（最多30秒）
    int16_t signal = 99;
    for (int i = 0; i < 30; i++) {
        signal = modem.getSignalQuality();
        Serial.printf("信号质量: %d/31 ", signal);

        if (signal > 0 && signal < 99) {
            Serial.println("✓");
            break;
        }

        Serial.println("等待信号...");
        delay(1000);
    }

    if (signal == 0 || signal == 99) {
        Serial.println("X 信号质量差或不可用，但将尝试发送");
    } else {
        Serial.println("✓ 信号质量良好");
    }

    // 检查网络注册状态（但不强制要求）
    Serial.println("\n检查网络状态...");
    RegStatus status = modem.getRegistrationStatus();
    Serial.printf("网络注册状态: %d\n", status);

    if (status == REG_OK_HOME || status == REG_OK_ROAMING) {
        Serial.println("✓ 网络已注册");
    } else {
        Serial.println("! 网络状态异常，但将尝试发送");
    }

    // 等待网络稳定
    delay(2000);

    // 设置短信中心号码
    Serial.println("\n设置短信中心...");
    modem.sendAT("+CSCA=\"" + String(SMS_CENTER) + "\"");
    modem.waitResponse(3000);

    // 设置字符集为UCS2（支持中文）
    Serial.println("设置字符集为UCS2...");
    modem.sendAT("+CSCS=\"UCS2\"");
    if (modem.waitResponse(3000) != 1) {
        Serial.println("! UCS2设置失败，尝试GSM");
        modem.sendAT("+CSCS=\"GSM\"");
        modem.waitResponse(3000);
    } else {
        Serial.println("✓ UCS2字符集已设置");
    }

    // 设置短信为文本模式
    Serial.println("设置短信文本模式...");
    modem.sendAT("+CMGF=1");
    if (modem.waitResponse(3000) != 1) {
        Serial.println("X 文本模式设置失败");
        return false;
    }
    Serial.println("✓ 文本模式已设置");

    // 发送短信
    Serial.printf("\n发送短信到: %s\n", SMS_TARGET);
    Serial.println("\n短信内容:");
    Serial.println("--- 开始 ---");
    Serial.println(message);
    Serial.println("--- 结束 ---\n");

    // 清空串口缓冲区
    Serial.println("[LOG] 清空串口缓冲区...");
    while (SerialAT.available()) {
        SerialAT.read();
    }
    delay(500);

    // 只重试1次（减少重试避免设备重置）
    for (int i = 0; i < 1; i++) {
        Serial.printf("[尝试 %d/1] 发送中...\n", i + 1);

        // 发送 AT+CMGS 命令
        modem.sendAT("+CMGS=\"" + String(SMS_TARGET) + "\"");

        // 等待 > 提示符
        if (modem.waitResponse(10000, ">") == 1) {
            Serial.println("✓ 收到 > 提示符");
            Serial.println("发送短信内容...");

            // 发送短信内容
            SerialAT.print(message);
            delay(200);

            // 发送 Ctrl+Z (0x1A)
            Serial.println("发送 Ctrl+Z...");
            SerialAT.write(0x1A);
            SerialAT.flush();  // 确保数据发送完成

            // 等待发送结果 (60秒超时)
            Serial.println("等待响应 (最多 60 秒)...");
            Serial.println("");

            String response = "";
            unsigned long startTime = millis();
            bool gotOK = false;
            bool gotCMGS = false;

            while (millis() - startTime < 60000) {
                if (SerialAT.available()) {
                    char c = SerialAT.read();
                    response += c;
                    Serial.print(c);  // 实时显示响应

                    // 检查是否收到+CMGS或OK
                    if (response.indexOf("+CMGS:") >= 0) {
                        gotCMGS = true;
                    }
                    if (response.indexOf("OK") >= 0) {
                        gotOK = true;
                    }
                    if (response.indexOf("ERROR") >= 0) {
                        Serial.println("\nX 发送失败: ERROR");
                        break;
                    }

                    // 如果收到完整响应
                    if (gotCMGS && gotOK) {
                        Serial.println("\n====================================");
                        Serial.println("✓✓✓ 短信发送成功! ✓✓✓");
                        Serial.println("====================================");
                        Serial.println("响应: " + response);

                        // 成功后等待5秒让Modem完全恢复
                        Serial.println("\n[LOG] 等待Modem恢复状态...");
                        delay(5000);

                        // 清空可能的残留数据
                        while (SerialAT.available()) {
                            SerialAT.read();
                        }

                        return true;
                    }
                }
                delay(50);
            }

            Serial.println("\nX 等待响应超时或未收到完整确认");
        } else {
            Serial.println("X 未收到 > 提示符");
        }

        // 发送 ESC 取消当前操作
        Serial.println("[LOG] 取消当前操作...");
        SerialAT.write(0x1B);
        delay(1000);
    }

    Serial.println("\n====================================");
    Serial.println("X 短信发送失败");
    Serial.println("====================================");

    // 失败后也要清理状态
    Serial.println("\n[LOG] 清理Modem状态...");

    // 清空缓冲区
    while (SerialAT.available()) {
        SerialAT.read();
    }

    // 发送AT测试连接
    modem.sendAT("");
    delay(500);

    return false;
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  GPS短信测试工具 (AT命令调试版)");
    Serial.println("  LilyGo A7670E-FASE");
    Serial.println("  短信中心: +316540942001");
    Serial.println("========================================\n");

    // 硬件初始化
    Serial.println("[LOG] 开始硬件初始化...");

#ifdef BOARD_POWERON_PIN
    pinMode(BOARD_POWERON_PIN, OUTPUT);
    digitalWrite(BOARD_POWERON_PIN, HIGH);
    Serial.println("[LOG] BOARD_POWERON_PIN -> HIGH");
#endif

#ifdef MODEM_RESET_PIN
    pinMode(MODEM_RESET_PIN, OUTPUT);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL); delay(100);
    digitalWrite(MODEM_RESET_PIN, MODEM_RESET_LEVEL); delay(2600);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL);
    Serial.println("[LOG] MODEM_RESET 完成");
#endif

#ifdef MODEM_FLIGHT_PIN
    pinMode(MODEM_FLIGHT_PIN, OUTPUT);
    digitalWrite(MODEM_FLIGHT_PIN, HIGH);
    Serial.println("[LOG] MODEM_FLIGHT_PIN -> HIGH");
#endif

    pinMode(MODEM_DTR_PIN, OUTPUT);
    digitalWrite(MODEM_DTR_PIN, LOW);
    Serial.println("[LOG] MODEM_DTR_PIN -> LOW");

    pinMode(BOARD_PWRKEY_PIN, OUTPUT);
    digitalWrite(BOARD_PWRKEY_PIN, LOW); delay(100);
    digitalWrite(BOARD_PWRKEY_PIN, HIGH);
    delay(MODEM_POWERON_PULSE_WIDTH_MS);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);
    Serial.println("[LOG] PWRKEY 脉冲完成");

#ifdef MODEM_RING_PIN
    pinMode(MODEM_RING_PIN, INPUT_PULLUP);
    Serial.println("[LOG] MODEM_RING_PIN 配置完成");
#endif

    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
    Serial.println("[LOG] SerialAT 已启动 (115200 baud)");

    Serial.println("\n[LOG] 正在启动Modem...");
    delay(3000);

    int retry = 0;
    while (!modem.testAT(1000)) {
        Serial.print(".");
        if (retry++ > 30) {
            Serial.println("\n[ERROR] Modem启动失败!");
            while(1) delay(1000);  // 停止运行
        }
    }
    Serial.println("\n[LOG] ✓ Modem启动成功\n");

    String modemName = modem.getModemName();
    Serial.printf("[INFO] 模块型号: %s\n", modemName.c_str());

    String imei = modem.getIMEI();
    Serial.printf("[INFO] IMEI: %s\n\n", imei.c_str());

    Serial.println("========================================");
    Serial.println("[LOG] 初始化完成");
    Serial.println("========================================\n");

    Serial.println("💡 使用说明:");
    Serial.println("  - 输入 'test' 或 'start' 开始GPS短信测试");
    Serial.println("  - 输入AT命令进行手动调试");
    Serial.println("  - AT+CGNSPWR? - 查询GPS电源状态");
    Serial.println("  - AT+CGPS? - 查询GPS状态");
    Serial.println("  - AT+CGNSSINFO - 获取GPS数据");
    Serial.println("========================================\n");
}

void loop() {
    static String inputBuffer = "";
    static bool testRunning = false;

    // 处理串口输入命令
    while (Serial.available()) {
        char c = Serial.read();

        if (c == '\n' || c == '\r') {
            if (inputBuffer.length() > 0) {
                String cmd = inputBuffer;
                cmd.trim();
                inputBuffer = "";

                // 处理命令
                if (cmd.equalsIgnoreCase("test") || cmd.equalsIgnoreCase("start")) {
                    testRunning = true;
                    smsCount++;

                    Serial.println("\n\n");
                    Serial.println("########################################");
                    Serial.printf("[TASK] 开始GPS短信测试 #%d\n", smsCount);
                    Serial.println("########################################\n");

                    // 采集GPS数据
                    GPSData gps;
                    bool gpsOK = collectGPSData(gps);

                    if (gpsOK) {
                        // 格式化GPS短信
                        String message = formatGPSSMS(gps);

                        Serial.println("\n[LOG] GPS短信内容:");
                        Serial.println("--- 开始 ---");
                        Serial.println(message);
                        Serial.println("--- 结束 ---\n");

                        // 发送短信
                        bool success = sendSMS(message);

                        if (success) {
                            Serial.println("\n========================================");
                            Serial.printf("[SUCCESS] GPS短信发送成功!\n");
                            Serial.println("========================================");
                        } else {
                            Serial.println("\n========================================");
                            Serial.printf("[FAILED] GPS短信发送失败\n");
                            Serial.println("========================================");
                        }
                    } else {
                        Serial.println("\n========================================");
                        Serial.println("[ERROR] GPS数据采集失败，不发送短信");
                        Serial.println("========================================");
                    }

                    Serial.println("\n输入 'test' 或 'start' 开始新的测试");
                    Serial.println("或输入AT命令进行测试\n");

                    testRunning = false;
                } else {
                    // 转发AT命令到modem
                    SerialAT.println(cmd);
                }
            }
        } else {
            inputBuffer += c;
        }
    }

    // 转发modem响应到串口监视器
    if (SerialAT.available()) {
        Serial.write(SerialAT.read());
    }

    delay(10);
}

#ifndef TINY_GSM_FORK_LIBRARY
#error "No correct definition detected"
#endif
