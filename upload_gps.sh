#!/bin/bash
# GPS测试程序快速上传脚本

echo "=========================================="
echo "GPS测试程序 - 快速编译上传"
echo "=========================================="
echo ""

# 显示当前配置
echo "📋 当前配置："
echo "  板型: T-Call-A7670X-V1-0"
echo "  程序: GPS_SMS_Monitor (GPS测试版本)"
echo "  功能: GPS数据采集 + 日志记录"
echo ""

# 编译并上传
echo "🔨 开始编译..."
~/.platformio/penv/bin/pio run -e T-Call-A7670X-V1-0 -t upload

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "✅ 上传成功！"
    echo "=========================================="
    echo ""
    echo "下一步："
    echo "  1. 启动串口监控: python3 monitor_gps.py"
    echo "  2. 或手动监控: pio device monitor"
    echo ""
    echo "测试提示："
    echo "  - 将设备放在户外或窗边"
    echo "  - 等待1-5分钟进行GPS定位"
    echo "  - 使用'log'命令查看历史记录"
    echo "  - 使用'now'命令立即检查GPS"
    echo ""
else
    echo ""
    echo "=========================================="
    echo "❌ 上传失败"
    echo "=========================================="
    echo ""
    echo "请检查："
    echo "  - 设备是否连接"
    echo "  - 串口是否被占用"
    echo "  - USB线缆是否正常"
    echo ""
    exit 1
fi
