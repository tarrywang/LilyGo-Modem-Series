# GPS测试程序说明

## 程序概述

已成功创建GPS数据采集测试程序，专注于GPS功能验证。

## 主要功能

### ✅ 已实现功能

1. **GPS初始化**（使用AT+CGNSSPWR=1体系）
   - 严格按照AT测试成功的命令序列
   - 自动启用GPS电源
   - 等待READY响应确认

2. **定时数据采集**
   - 每30秒自动采集一次GPS数据
   - 使用AT+CGNSSINFO命令获取GPS信息
   - 区分有效定位和未定位状态

3. **数据记录**
   - 所有GPS数据写入SPIFFS文件系统的`/gps.log`
   - 记录格式：`时间戳 | 状态 | GPS原始数据`
   - 支持查看、清空日志文件

4. **交互命令**
   - `log` - 显示GPS日志文件内容
   - `now` - 立即执行一次GPS检查
   - `clear` - 清空GPS日志文件

## 测试结果

### 初始化测试 ✅

```
[GPS] Enabling GPS using AT+CGNSSPWR=1
[GPS] Step 1: Checking GPS power status...
[GPS] Step 2: Enabling GPS power (AT+CGNSSPWR=1)...
[GPS] Response: +CGNSSPWR: READY!
✅ GPS power enabled successfully!
✅ GPS system initialized successfully!
```

### 数据采集测试 ✅

程序成功每30秒采集一次GPS数据：
```
GPS Check #1 (every 30s)
[GPS] Querying GPS data (AT+CGNSSINFO)...
[GPS] Full Response: +CGNSSINFO: ,,,,,,,,
[GPS] ⚠️  No GPS fix (waiting for satellites...)
[FILE] ✅ GPS data written to /gps.log
```

## GPS定位说明

### 当前状态
- GPS电源：✅ 已启用
- 数据采集：✅ 正常运行
- 定位状态：⚠️ 未定位（室内环境）

### 获取GPS定位的条件

1. **环境要求**
   - 必须在户外或靠窗位置
   - 需要清晰的天空视野
   - 室内很难接收到GPS卫星信号

2. **时间要求**
   - 首次定位（冷启动）：1-5分钟
   - 后续定位（热启动）：30秒-2分钟

3. **天线要求**
   - 确保GPS天线已正确连接
   - 天线朝向天空

### GPS数据格式

**未定位**（当前状态）：
```
+CGNSSINFO: ,,,,,,,,
```

**已定位**（预期格式）：
```
+CGNSSINFO: mode,lat,latDir,lon,lonDir,date,time,alt,speed,course
示例：
+CGNSSINFO: 2,3113.343286,N,12121.234064,E,091024,080915.0,23.4,0.0,0.0
```

字段说明：
- mode: 定位模式（1=无效, 2=2D定位, 3=3D定位）
- lat: 纬度（度分格式，如3113.343286 = 31°13.343286'）
- latDir: 纬度方向（N/S）
- lon: 经度（度分格式）
- lonDir: 经度方向（E/W）
- date: 日期（DDMMYY）
- time: 时间（HHMMSS.S）
- alt: 海拔高度（米）
- speed: 速度（km/h）
- course: 航向（度）

## 下一步测试计划

### 室外GPS测试

1. **准备工作**
   - 将设备移至室外或窗边
   - 确保天线朝向天空
   - 保持电源连接稳定

2. **测试步骤**
   ```bash
   # 1. 上传并启动程序（已完成）
   pio run -t upload -e T-Call-A7670X-V1-0

   # 2. 监控串口输出
   python3 monitor_serial.py

   # 3. 等待GPS定位（1-5分钟）
   # 观察输出变化：
   # +CGNSSINFO: ,,,,,,,,  ->  +CGNSSINFO: 2,lat,lon,...

   # 4. 查看日志文件
   # 串口输入：log
   ```

3. **成功标志**
   - GPS数据不再为空
   - mode字段 = 2或3
   - 出现有效的经纬度数据

### 后续开发

一旦GPS定位成功，可以：
1. ✅ 验证GPS数据的准确性
2. 📲 添加SMS发送功能
3. 🌐 集成MQTT远程上传
4. 📊 实现GPS数据解析和显示

## 文件说明

- **程序文件**: `examples/GPS_SMS_Monitor/GPS_SMS_Monitor.ino`
- **日志文件**: `/gps.log`（存储在SPIFFS文件系统）
- **板型配置**: `T-Call-A7670X-V1-0`
- **串口波特率**: 115200

## 命令参考

### AT命令（通过串口直接发送）
```
AT+CGNSPWR?       - 查询GPS电源状态
AT+CGNSSPWR=1     - 启用GPS电源
AT+CGNSSINFO      - 获取GPS信息
```

### 程序命令（通过串口监视器发送）
```
log               - 显示GPS日志
now               - 立即检查GPS
clear             - 清空日志
```

## 故障排查

### GPS电源启动失败
- 检查天线连接
- 重启设备
- 确认SIM卡已插入

### 一直无法定位
- 确认在户外或窗边
- 等待更长时间（最多10分钟）
- 检查GPS天线是否损坏

### 日志文件无法写入
- SPIFFS初始化失败
- 存储空间已满
- 重新烧录固件

---

**创建时间**: 2025-10-24
**最后更新**: 2025-10-24
**测试状态**: ✅ 程序运行正常，等待室外GPS定位测试
