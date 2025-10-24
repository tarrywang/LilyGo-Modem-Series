/**
 * @file      Check_SMS_Center.ino
 * @brief     查询SIM卡短信中心号码
 * @date      2025-10-23
 * @note      用于确认电信SIM卡的实际短信中心号码
 */

#include "utilities.h"

// 启用调试输出
#define TINY_GSM_DEBUG SerialMon
#define SerialMon Serial

#include <TinyGsmClient.h>
TinyGsm modem(SerialAT);

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("====================================");
    Serial.println("短信中心号码查询工具");
    Serial.println("====================================");

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

    Serial.println("正在启动调制解调器...");
    delay(3000);

    // 等待调制解调器响应
    int retry = 0;
    while (!modem.testAT() && retry < 20) {
        Serial.print(".");
        delay(500);
        retry++;
    }
    Serial.println();

    if (retry >= 20) {
        Serial.println("❌ 调制解调器启动失败！");
        return;
    }
    Serial.println("✅ 调制解调器已就绪");

    // 等待网络注册
    Serial.print("等待网络注册");
    RegStatus status = REG_NO_RESULT;
    int16_t sq;

    while (status == REG_NO_RESULT || status == REG_SEARCHING || status == REG_UNREGISTERED) {
        status = modem.getRegistrationStatus();
        switch (status) {
        case REG_UNREGISTERED:
        case REG_SEARCHING:
            sq = modem.getSignalQuality();
            Serial.printf(".");
            delay(1000);
            break;
        case REG_DENIED:
            Serial.println("\n❌ 网络注册被拒绝，请检查SIM卡");
            return;
        case REG_OK_HOME:
            Serial.println("\n✅ 网络注册成功");
            break;
        case REG_OK_ROAMING:
            Serial.println("\n✅ 网络注册成功（漫游）");
            break;
        default:
            delay(1000);
            break;
        }
    }

    // 获取运营商信息
    Serial.println("\n========================================");
    Serial.println("正在查询SIM卡信息...");
    Serial.println("========================================");

    String operatorName = modem.getOperator();
    Serial.println("运营商名称: " + operatorName);

    // 获取IMSI
    String imsi = modem.getIMSI();
    Serial.println("IMSI: " + imsi);

    // 判断运营商
    if (imsi.startsWith("460") && imsi.length() >= 5) {
        String mcc_mnc = imsi.substring(0, 5);
        Serial.print("运营商代码: " + mcc_mnc + " = ");

        if (mcc_mnc == "46003" || mcc_mnc == "46011" || mcc_mnc == "46005") {
            Serial.println("中国电信 (China Telecom)");
        } else if (mcc_mnc == "46000" || mcc_mnc == "46002" || mcc_mnc == "46007" || mcc_mnc == "46020") {
            Serial.println("中国移动 (China Mobile)");
        } else if (mcc_mnc == "46001" || mcc_mnc == "46006" || mcc_mnc == "46009") {
            Serial.println("中国联通 (China Unicom)");
        } else {
            Serial.println("未知运营商");
        }
    }

    Serial.println("\n========================================");
    Serial.println("查询短信中心号码（方法1: AT+CSCA?）");
    Serial.println("========================================");

    // 方法1：使用AT+CSCA?命令
    modem.sendAT("+CSCA?");
    String response = "";
    if (modem.waitResponse(5000L, response) == 1) {
        Serial.println("✅ 查询成功");
        Serial.println("原始响应: " + response);

        // 解析响应
        int startIdx = response.indexOf("+CSCA: \"");
        if (startIdx >= 0) {
            startIdx += 8;  // 跳过 "+CSCA: ""
            int endIdx = response.indexOf("\"", startIdx);
            if (endIdx > startIdx) {
                String smscNumber = response.substring(startIdx, endIdx);
                Serial.println("\n📱 短信中心号码: " + smscNumber);
            }
        }
    } else {
        Serial.println("⚠️ 查询失败");
    }

    Serial.println("\n========================================");
    Serial.println("查询短信中心号码（方法2: AT+CNUM）");
    Serial.println("========================================");

    // 方法2：获取本机号码（可能包含短信中心信息）
    modem.sendAT("+CNUM");
    if (modem.waitResponse(5000L, response) == 1) {
        Serial.println("✅ 本机号码查询");
        Serial.println("响应: " + response);
    }

    Serial.println("\n========================================");
    Serial.println("常用电信短信中心号码参考");
    Serial.println("========================================");
    Serial.println("北京: +8613800100500");
    Serial.println("上海: +8613800210500");
    Serial.println("广东: +8613800200500");
    Serial.println("江苏: +8613800280500");
    Serial.println("浙江: +8613800330500");
    Serial.println("福建: +8618950000321 或 +8613800350500");
    Serial.println("四川: +8613800280500");
    Serial.println("陕西: +8613800290500");
    Serial.println("湖北: +8613800270500");
    Serial.println("湖南: +8613800731500");

    Serial.println("\n========================================");
    Serial.println("测试SMS模式配置");
    Serial.println("========================================");

    // 查询当前SMS模式
    modem.sendAT("+CMGF?");
    if (modem.waitResponse(1000L, response) == 1) {
        Serial.println("SMS模式: " + response);
        Serial.println("(0=PDU模式, 1=文本模式)");
    }

    // 查询字符集
    modem.sendAT("+CSCS?");
    if (modem.waitResponse(1000L, response) == 1) {
        Serial.println("字符集: " + response);
    }

    Serial.println("\n========================================");
    Serial.println("✅ 查询完成！");
    Serial.println("请记录上面显示的短信中心号码");
    Serial.println("========================================");
}

void loop() {
    // 保持运行
    delay(1000);
}

#ifndef TINY_GSM_FORK_LIBRARY
#error "请将项目的 lib 目录复制到 Arduino 库目录"
#endif
