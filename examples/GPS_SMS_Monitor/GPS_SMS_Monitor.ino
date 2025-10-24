/**
 * @file      GPS_SMS_Monitor.ino
 * @brief     GPS数据采集测试程序（使用AT+CGNSSPWR=1体系）
 * @date      2025-10-24
 * @note      每30秒采集一次GPS数据，输出到串口和gps.log文件
 */

#include "utilities.h"
#include "FS.h"
#include "SPIFFS.h"

#define SerialMon Serial

// 定时间隔（毫秒）
#define GPS_CHECK_INTERVAL 30000  // 30秒

uint32_t lastCheckTime = 0;
int checkCount = 0;

// GPS数据结构
struct GPSData {
    String rawData;     // CGNSSINFO原始响应
    bool valid;         // 数据有效性
    uint32_t timestamp; // 采集时间戳
};

/**
 * @brief 发送AT命令并获取响应
 * @param cmd AT命令（完整命令，包含AT+前缀）
 * @param timeout 超时时间（毫秒）
 * @return 响应字符串
 */
String sendATCommand(const String& cmd, unsigned long timeout = 5000) {
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
        delay(1);
    }

    return response;
}

/**
 * @brief 启用GPS功能 - 使用AT+CGNSSPWR=1体系
 * @return true 成功, false 失败
 */
bool enableGPS() {
    Serial.println("\n[GPS] ========================================");
    Serial.println("[GPS] Enabling GPS using AT+CGNSSPWR=1");
    Serial.println("[GPS] ========================================");

    // Step 1: 查询当前GPS电源状态
    Serial.println("[GPS] Step 1: Checking GPS power status...");
    String pwrStatus = sendATCommand("AT+CGNSPWR?", 2000);
    Serial.print("[GPS] Response: ");
    Serial.println(pwrStatus);

    // Step 2: 启用GPS电源
    Serial.println("[GPS] Step 2: Enabling GPS power (AT+CGNSSPWR=1)...");
    String pwrOn = sendATCommand("AT+CGNSSPWR=1", 30000);  // 30秒超时，等待READY
    Serial.print("[GPS] Response: ");
    Serial.println(pwrOn);

    // 检查是否收到 READY 响应
    if (pwrOn.indexOf("READY") >= 0 || pwrOn.indexOf("OK") >= 0) {
        Serial.println("[GPS] ✅ GPS power enabled successfully!");

        // Step 3: 验证GPS电源状态
        Serial.println("[GPS] Step 3: Verifying GPS power status...");
        delay(1000);
        pwrStatus = sendATCommand("AT+CGNSPWR?", 2000);
        Serial.print("[GPS] Status: ");
        Serial.println(pwrStatus);

        return true;
    } else {
        Serial.println("[GPS] ❌ Failed to enable GPS power");
        return false;
    }
}

/**
 * @brief 获取GPS数据 - 使用AT+CGNSSINFO命令
 * @param gpsData GPS数据结构指针
 * @return true 获取成功, false 失败
 */
bool getGPSData(GPSData* gpsData) {
    gpsData->valid = false;
    gpsData->rawData = "";
    gpsData->timestamp = millis();

    // 发送AT+CGNSSINFO命令
    Serial.println("\n[GPS] Querying GPS data (AT+CGNSSINFO)...");
    String response = sendATCommand("AT+CGNSSINFO", 3000);

    Serial.println("[GPS] Full Response:");
    Serial.println("--- BEGIN ---");
    Serial.println(response);
    Serial.println("--- END ---");

    // 提取 +CGNSSINFO: 数据行
    int infoStart = response.indexOf("+CGNSSINFO:");
    if (infoStart < 0) {
        Serial.println("[GPS] ❌ No +CGNSSINFO in response");
        return false;
    }

    // 提取从 +CGNSSINFO: 开始到行尾的数据
    String rawData = response.substring(infoStart);
    int lineEnd = rawData.indexOf('\r');
    if (lineEnd > 0) {
        rawData = rawData.substring(0, lineEnd);
    }
    rawData.trim();

    // 检查是否是空数据（没有定位数据）
    // 空数据格式：+CGNSSINFO: ,,,,,,,,
    // 或者 mode=0：+CGNSSINFO: 0,,,,,,,
    if (rawData.indexOf("+CGNSSINFO: ,,") >= 0 ||
        rawData.indexOf("+CGNSSINFO: 0,") >= 0) {
        Serial.println("[GPS] ⚠️  No GPS fix (waiting for satellites...)");
        gpsData->rawData = rawData;
        return false;
    }

    // 保存原始数据
    gpsData->rawData = rawData;
    gpsData->valid = true;

    Serial.println("[GPS] ✅ GPS data extracted successfully");
    return true;
}

/**
 * @brief 将GPS数据写入文件
 * @param gpsData GPS数据
 */
void writeGPSToFile(const GPSData& gpsData) {
    File file = SPIFFS.open("/gps.log", FILE_APPEND);
    if (!file) {
        Serial.println("[FILE] ❌ Failed to open gps.log for writing");
        return;
    }

    // 写入时间戳和GPS数据
    String logEntry = String(gpsData.timestamp) + "ms | " +
                      (gpsData.valid ? "VALID" : "NO_FIX") + " | " +
                      gpsData.rawData + "\n";

    file.print(logEntry);
    file.close();

    Serial.println("[FILE] ✅ GPS data written to /gps.log");
}

/**
 * @brief 读取并显示gps.log文件内容
 */
void showGPSLog() {
    if (!SPIFFS.exists("/gps.log")) {
        Serial.println("[FILE] gps.log does not exist yet");
        return;
    }

    File file = SPIFFS.open("/gps.log", FILE_READ);
    if (!file) {
        Serial.println("[FILE] ❌ Failed to open gps.log for reading");
        return;
    }

    Serial.println("\n========================================");
    Serial.println("GPS Log File Contents:");
    Serial.println("========================================");

    while (file.available()) {
        Serial.write(file.read());
    }

    Serial.println("========================================");
    file.close();
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n\n");
    Serial.println("====================================");
    Serial.println("GPS Data Collection Test");
    Serial.println("Using AT+CGNSSPWR=1 System");
    Serial.println("====================================");

    // 初始化SPIFFS文件系统
    Serial.println("\n[FILE] Initializing SPIFFS...");
    if (!SPIFFS.begin(true)) {
        Serial.println("[FILE] ❌ SPIFFS initialization failed!");
    } else {
        Serial.println("[FILE] ✅ SPIFFS initialized");

        // 显示SPIFFS信息
        size_t totalBytes = SPIFFS.totalBytes();
        size_t usedBytes = SPIFFS.usedBytes();
        Serial.printf("[FILE] Total: %u bytes, Used: %u bytes, Free: %u bytes\n",
                     totalBytes, usedBytes, totalBytes - usedBytes);

        // 清空旧的日志文件
        if (SPIFFS.exists("/gps.log")) {
            Serial.println("[FILE] Removing old gps.log...");
            SPIFFS.remove("/gps.log");
        }
    }

#ifdef BOARD_POWERON_PIN
    pinMode(BOARD_POWERON_PIN, OUTPUT);
    digitalWrite(BOARD_POWERON_PIN, HIGH);
#endif

#ifdef MODEM_RESET_PIN
    pinMode(MODEM_RESET_PIN, OUTPUT);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL);
    delay(100);
    digitalWrite(MODEM_RESET_PIN, MODEM_RESET_LEVEL);
    delay(2600);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL);
#endif

#ifdef MODEM_FLIGHT_PIN
    pinMode(MODEM_FLIGHT_PIN, OUTPUT);
    digitalWrite(MODEM_FLIGHT_PIN, HIGH);
#endif

    // DTR引脚设置（防止进入睡眠）
    pinMode(MODEM_DTR_PIN, OUTPUT);
    digitalWrite(MODEM_DTR_PIN, LOW);

    // 启动调制解调器
    pinMode(BOARD_PWRKEY_PIN, OUTPUT);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);
    delay(100);
    digitalWrite(BOARD_PWRKEY_PIN, HIGH);
    delay(MODEM_POWERON_PULSE_WIDTH_MS);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);

    // 初始化串口
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);

    Serial.println("\n正在启动调制解调器...");
    delay(3000);

    // 等待调制解调器响应
    Serial.print("等待AT响应");
    int retry = 0;
    while (retry < 20) {
        SerialAT.println("AT");
        delay(500);

        String resp = "";
        unsigned long start = millis();
        while (millis() - start < 500) {
            if (SerialAT.available()) {
                resp += (char)SerialAT.read();
            }
        }

        if (resp.indexOf("OK") >= 0) {
            Serial.println(" OK");
            break;
        }

        Serial.print(".");
        retry++;
    }

    if (retry >= 20) {
        Serial.println("\n❌ 调制解调器启动失败！");
        while(1) delay(1000);
    }

    Serial.println("✅ 调制解调器已就绪");

    // 启用GPS
    if (enableGPS()) {
        Serial.println("\n✅ GPS system initialized successfully!");
        Serial.println("GPS is warming up, waiting for satellite fix...");
        Serial.println("This may take 1-5 minutes in open sky");
    } else {
        Serial.println("\n❌ GPS initialization failed!");
        Serial.println("Check antenna connection and try again");
    }

    lastCheckTime = millis();

    Serial.println("\n========================================");
    Serial.println("GPS Data Collection Started");
    Serial.println("========================================");
    Serial.println("Interval: 30 seconds");
    Serial.println("Log file: /gps.log");
    Serial.println("");
    Serial.println("Commands:");
    Serial.println("  'log' - Show GPS log file");
    Serial.println("  'now' - Check GPS immediately");
    Serial.println("  'clear' - Clear GPS log");
    Serial.println("========================================\n");

    Serial.flush();
}

void loop() {
    // 处理串口命令
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();

        if (cmd.equalsIgnoreCase("log")) {
            showGPSLog();
        } else if (cmd.equalsIgnoreCase("now")) {
            Serial.println("\n[USER] Manual GPS check requested");
            GPSData gps;
            if (getGPSData(&gps)) {
                Serial.println("[GPS] 📍 Data: " + gps.rawData);
                writeGPSToFile(gps);
            } else {
                Serial.println("[GPS] No fix or data unavailable");
                writeGPSToFile(gps);  // 记录未定位状态
            }
        } else if (cmd.equalsIgnoreCase("clear")) {
            if (SPIFFS.exists("/gps.log")) {
                SPIFFS.remove("/gps.log");
                Serial.println("[FILE] ✅ GPS log cleared");
            } else {
                Serial.println("[FILE] GPS log is already empty");
            }
        } else if (cmd.length() > 0) {
            Serial.println("[INFO] Unknown command: " + cmd);
        }
    }

    // 定时GPS检查
    if (millis() - lastCheckTime >= GPS_CHECK_INTERVAL) {
        lastCheckTime = millis();
        checkCount++;

        Serial.println("\n========================================");
        Serial.printf("GPS Check #%d (every 30s)\n", checkCount);
        Serial.println("========================================");

        GPSData gps;
        if (getGPSData(&gps)) {
            Serial.println("\n[GPS] ✅ GPS Fix Acquired!");
            Serial.println("[GPS] 📍 Data: " + gps.rawData);
            writeGPSToFile(gps);
        } else {
            Serial.println("\n[GPS] ⚠️  No GPS fix yet, still searching...");
            writeGPSToFile(gps);  // 记录未定位状态
        }

        Serial.println("========================================");
        Serial.printf("Next check in 30s (Total checks: %d)\n", checkCount);
        Serial.println("========================================\n");
    }

    delay(100);
}
