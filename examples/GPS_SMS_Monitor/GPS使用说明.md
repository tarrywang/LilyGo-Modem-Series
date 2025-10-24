# GPS短信自动发送系统 - 使用说明

## 程序概述

这是一个**GPS定位 + 自动短信发送**的完整系统，程序会：
1. 每**30秒**自动获取一次GPS位置信息
2. 获取成功后立即发送短信到目标手机号
3. 连续发送 **3次** 后自动停止

## 关键配置

### 1. 目标手机号
```cpp
#define SMS_TARGET  "+8615821259813"
```

### 2. 短信中心号码（中国电信 - 上海）
```cpp
#define SMS_CENTER "+8613800210500"
```

### 3. 发送设置
- **发送间隔**: 30秒
- **发送次数**: 3次
- **短信格式**: Text模式（英文）

## GPS工作原理

### 成功的AT命令序列

程序使用您测试成功的AT命令序列来获取GPS数据：

```
1. AT+CGNSPWR?          ← 查询GPS电源状态
2. AT+CGNSPWR=1         ← 启用GPS电源（如果未启用）
3. AT+CGNSSINFO         ← 获取GPS数据
```

### CGNSSINFO数据格式

```
+CGNSSINFO: <mode>,<lon>,<lat>,<date>,<UTC time>,<alt>,<speed>,<course>,<PDOP>,<HDOP>,<VDOP>,<sat_in_use>,<sat_in_view>,<GNSS sat>,<GLONASS sat>,<BEIDOU sat>
```

**字段说明：**
- **mode**: 定位模式 (0=无定位, 1=2D定位, 2=3D定位)
- **lon**: 经度 (DDDMM.MMMM格式，如 12145.8567)
- **lat**: 纬度 (DDMM.MMMM格式，如 3112.3456)
- **date**: 日期 (ddmmyyyy格式，如 24102025)
- **UTC time**: UTC时间 (hhmmss.sss格式，如 123045.000)
- **alt**: 海拔 (米)
- **speed**: 速度 (km/h)
- **course**: 航向 (度)
- **sat_in_use**: 使用的卫星数量

### 坐标转换

程序会自动将CGNSSINFO返回的坐标格式转换为标准十进制度数：

```
DDDMM.MMMM → DDD.DDDDDD

例如：
12145.8567 → 121度 + 45.8567分 → 121 + 45.8567/60 = 121.764278°
```

## 短信内容格式

```
GPS Report #1/3
Time: 2025-10-24 12:30:45 UTC
Lat: 31.205761N
Lon: 121.764278E
Map: https://maps.google.com/?q=31.205761,121.764278
Alt: 15.2m
Speed: 0.0km/h
Sats: 8
```

## 使用步骤

### 1. 硬件准备
- ✅ LilyGo T-Call A7670X V1.0 开发板
- ✅ 中国电信SIM卡（已禁用PIN码，有余额）
- ✅ **5V 2A 外部电源**（重要！USB供电可能不足）
- ✅ GSM天线（必须连接）
- ✅ GPS天线（必须连接）
- ✅ **室外或窗边环境**（GPS需要接收卫星信号）

### 2. 编译上传

```bash
# 确保 platformio.ini 中设置正确：
# src_dir = examples/GPS_SMS_Monitor
# default_envs = T-Call-A7670X-V1-0

# 编译
pio run -e T-Call-A7670X-V1-0

# 上传
pio run -t upload -e T-Call-A7670X-V1-0

# 监视串口输出
pio device monitor --port /dev/cu.usbserial-59691448411 --baud 115200
```

### 3. 启动流程

程序启动后会按顺序执行：

#### Phase 1: 调制解调器初始化
```
====================================
GPS SMS Auto-Send Program
====================================
正在启动调制解调器...
✅ 调制解调器已就绪
```

#### Phase 2: 短信配置
```
Configuring SMS...
Setting SMSC: +8613800210500
✅ SMSC set successfully
```

#### Phase 3: 网络注册
```
等待网络注册...
✅ 网络注册成功
信号质量: 25 (0-31, 越高越好)
```

#### Phase 4: GPS初始化
```
=== Initializing GPS ===
[GPS] Enabling GPS using TinyGSM library...
✅ GPS initialization successful!
```

#### Phase 5: 手动测试模式
```
========================================
=== GPS SMS Auto-Send Ready ===
========================================

💡 Type 'start' to begin auto GPS SMS
========================================

Target: +8615821259813
Sends: 3 times
Interval: 30 seconds
```

### 4. 启动自动发送

在串口监视器中输入：
```
start
```

程序会立即开始每30秒循环：

```
🚀 ========================================
🚀 Starting Auto GPS SMS Mode...
🚀 ========================================

========================================
Sending GPS SMS 1/3
========================================

[GPS] Step 1: Checking GPS power status (AT+CGNSPWR?)
[GPS] Response: +CGNSPWR: 1
...
[GPS] Step 2: GPS power already ON
[GPS] Step 3: Getting GPS data (AT+CGNSSINFO)
[GPS] Response: +CGNSSINFO: 2,12145.8567,3112.3456,...
[GPS] Step 4: Parsing GPS data
✅ GPS data parsed successfully
[GPS] Position: 31.205761N, 121.764278E
[GPS] Satellites: 8

[GPS] Current Position:
  Latitude:  31.205761 N
  Longitude: 121.764278 E
  Altitude:  15.2 m
  Speed:     0.0 km/h
  Course:    0.0 deg
  Satellites: 8
  Date/Time: 24102025 123045 UTC

Message content:
--- BEGIN ---
GPS Report #1/3
Time: 2025-10-24 12:30:45 UTC
Lat: 31.205761N
Lon: 121.764278E
Map: https://maps.google.com/?q=31.205761,121.764278
Alt: 15.2m
Speed: 0.0km/h
Sats: 8
--- END ---

Sending to +8615821259813...
Using Text mode with retry mechanism
[SMS] Configuring Text Mode (AT+CMGF=1)...
[SMS] Text mode OK
[SMS] Setting character set (AT+CSCS="GSM")...
[SMS] Character set OK
[SMS] Initiating send (AT+CMGS)...
[SMS] Got '>' prompt, sending message...
[SMS] Waiting for send confirmation...
[SMS] Send confirmed by modem

========== SUCCESS ==========
GPS SMS sent successfully!
Check your phone for GPS location
=============================

========================================
Next GPS SMS in 30s (remaining: 2)
```

### 5. 预期结果

- **第1次**: 30秒后发送第1条GPS短信
- **第2次**: 再过30秒发送第2条GPS短信
- **第3次**: 再过30秒发送第3条GPS短信
- **完成**: 显示 "All 3 GPS SMS sent. Check your phone!"

## 故障排查

### 如果GPS获取失败

**现象：**
```
[GPS] No GPS fix (mode=0)
[GPS] Failed to parse GPS data
[GPS] ERROR: Failed to acquire GPS data after retries
[GPS] Skipping this send cycle
```

**解决方法：**
1. ✅ **确认GPS天线已连接**
2. ✅ **将设备移到室外或窗边** - GPS需要接收卫星信号，室内可能无法定位
3. ✅ **等待更长时间** - 首次定位可能需要1-5分钟
4. ✅ **检查天线质量** - 使用原装GPS天线
5. ✅ **查看卫星数** - 至少需要4颗卫星才能3D定位

**手动测试GPS：**
```
AT+CGNSPWR?       ← 查询GPS电源
AT+CGNSPWR=1      ← 启用GPS电源
AT+CGNSSINFO      ← 获取GPS数据（多次查询，观察mode是否从0变为1或2）
```

### 如果短信发送失败

**现象：**
```
[SMS] Failed to set Text mode
或
[SMS] Failed to get '>' prompt
或
[SMS] Send failed (attempt 3/3)
```

**解决方法：**
1. ✅ **检查信号质量** - 应该 >10，最好 >15
2. ✅ **验证短信中心号码** - 确认 `+8613800210500` 对您的地区是否正确
3. ✅ **检查SIM卡余额** - 确保有足够余额发送短信
4. ✅ **更换短信中心号码** - 可能需要根据地区调整

**常见电信短信中心号码：**
- 上海: +8613800210500 ✅（当前配置）
- 福建: +8618950000321
- 广东: +8613800200500
- 北京: +8613800100500

### 如果网络注册失败

**现象：**
```
❌ 网络注册被拒绝，请检查SIM卡和APN
```

**解决方法：**
1. ✅ SIM卡已插入且未启用PIN码
2. ✅ SIM卡有效且有余额
3. ✅ APN设置正确（中国电信: CTNET）
4. ✅ 天线已连接
5. ✅ 信号覆盖良好

## 核心代码说明

### GPS数据获取（使用成功的AT命令）

```cpp
bool getGPSInfo(GPSData* gpsData) {
    // Step 1: 查询GPS电源状态
    String pwrStatus = sendATCommand("CGNSPWR?", 2000);

    // Step 2: 确保GPS电源已启用
    if (pwrStatus.indexOf("+CGNSPWR: 0") >= 0) {
        sendATCommand("CGNSPWR=1", 2000);
        delay(1000);
    }

    // Step 3: 获取GPS数据
    String gpsInfo = sendATCommand("CGNSSINFO", 3000);

    // Step 4: 解析GPS数据
    return parseCGNSSINFO(gpsInfo, gpsData);
}
```

### CGNSSINFO数据解析

```cpp
bool parseCGNSSINFO(const String& response, GPSData* gpsData) {
    // 查找 +CGNSSINFO: 并提取数据
    // 分割逗号分隔的字段
    // 转换DDDMM.MMMM格式到十进制度数
    // 提取所有GPS相关信息
}
```

### 短信发送（Text模式，带重试）

```cpp
bool sendSMSText(const String& number, const String& message) {
    for (int attempt = 1; attempt <= 3; attempt++) {
        // 设置Text模式
        modem.sendAT("+CMGF=1");

        // 设置字符集
        modem.sendAT("+CSCS=\"GSM\"");

        // 发送短信
        modem.sendAT("+CMGS=\"" + number + "\"");
        // 等待 > 提示符
        // 发送消息内容
        // 发送 Ctrl+Z (0x1A)
        // 等待确认

        if (success) return true;
    }
    return false;
}
```

## 定制修改

### 修改发送间隔

编辑 `GPS_SMS_Monitor.ino`：
```cpp
#define SEND_INTERVAL 30000  // 30秒（毫秒）
// 改为:
#define SEND_INTERVAL 60000  // 60秒
```

### 修改发送次数

```cpp
#define MAX_SEND_COUNT 3
// 改为:
#define MAX_SEND_COUNT 5  // 发送5次
```

### 修改目标号码

```cpp
#define SMS_TARGET  "+8615821259813"
// 改为你的手机号:
#define SMS_TARGET  "+86138xxxxxxxx"
```

### 修改短信中心

```cpp
#define SMS_CENTER "+8613800210500"  // 上海电信
// 改为你的地区:
#define SMS_CENTER "+8618950000321"  // 福建电信
```

## 注意事项

⚠️ **重要提醒**:

1. **电源要求** - 必须使用外部5V 2A电源，USB供电功率不足
2. **GPS天线** - 必须连接，并放置在室外或窗边
3. **GSM天线** - 必须连接，用于发送短信
4. **SIM卡** - 禁用PIN码，确保有余额
5. **首次定位** - 可能需要1-5分钟获取卫星信号
6. **室内定位** - GPS在室内通常无法定位，需要移到窗边或室外
7. **短信费用** - 每条短信会消耗SIM卡余额

## 技术细节

### AT命令执行流程

```
getGPSInfo() 被调用
    ↓
Step 1: AT+CGNSPWR?
    ↓
检查GPS电源状态
    ↓
如果未启用 → Step 2: AT+CGNSPWR=1
    ↓
Step 3: AT+CGNSSINFO
    ↓
Step 4: 解析响应数据
    ↓
转换坐标格式
    ↓
返回GPS数据结构
```

### 主循环逻辑

```
loop()
    ↓
检查是否输入 "start"
    ↓
进入自动发送模式
    ↓
每30秒执行一次:
    ↓
调用 getGPSInfo()
    ↓
如果获取成功:
    ↓
格式化GPS短信
    ↓
调用 sendSMSText()
    ↓
显示结果
    ↓
重复直到发送3次
```

## 相关文件

1. **主程序**: `examples/GPS_SMS_Monitor/GPS_SMS_Monitor.ino`
2. **硬件配置**: `examples/GPS_SMS_Monitor/utilities.h`
3. **项目配置**: `platformio.ini`
4. **本说明**: `examples/GPS_SMS_Monitor/GPS使用说明.md`
5. **SMS测试**: `examples/GPS_SMS_Monitor/SMS测试说明.md`

## 下一步计划

测试成功后，可以：
1. ✅ 添加接收短信功能，远程控制设备
2. ✅ 实现触发条件（如按键触发发送）
3. ✅ 添加定时唤醒和休眠功能，降低功耗
4. ✅ 存储GPS轨迹到SD卡
5. ✅ 添加MQTT上传GPS数据到云平台

## 问题反馈

如有问题，请提供：
- 完整串口输出日志
- 设备型号和SIM卡运营商
- GPS测试环境（室内/室外）
- 信号质量数值

祝测试顺利！ 🚀
