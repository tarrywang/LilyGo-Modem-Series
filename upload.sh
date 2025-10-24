#!/bin/bash
# GPS短信程序上传脚本 - 使用完整路径

cd "/Users/tarrywang/Library/CloudStorage/OneDrive-个人/项目/LilyGo-Modem-Series/LilyGo-Modem-Series"

echo "================================================================================"
echo "                    GPS短信程序上传工具"
echo "================================================================================"
echo ""
echo "📍 当前目录: $(pwd)"
echo "📦 目标板: T-Call-A7670X-V1-0"
echo "🔧 修复内容: GPS启动命令 (AT+CGPS=1,1)"
echo ""
echo "================================================================================"
echo "                    重要：进入下载模式"
echo "================================================================================"
echo ""
echo "请在开发板上执行以下操作："
echo ""
echo "  1️⃣  按住 BOOT 按钮 (通常标记为 BOOT 或 IO0)"
echo "  2️⃣  按一下 RESET 按钮 (或 EN 按钮)"
echo "  3️⃣  松开 RESET，继续按住 BOOT 2秒"
echo "  4️⃣  松开 BOOT 按钮"
echo ""
echo "💡 提示："
echo "   - BOOT按钮通常在USB接口附近"
echo "   - 进入下载模式后，板载LED可能会闪烁或保持常亮"
echo ""
echo "================================================================================"
echo ""
read -p "按回车键开始上传（确保已进入下载模式）..."

echo ""
echo "🚀 正在上传固件..."
echo ""

# 使用完整路径的PlatformIO
~/.platformio/penv/bin/pio run -t upload -e T-Call-A7670X-V1-0

RESULT=$?

echo ""
echo "================================================================================"
if [ $RESULT -eq 0 ]; then
    echo "✅ 上传成功！"
    echo ""
    echo "接下来的步骤："
    echo "1. 设备会自动重启"
    echo "2. 运行监控脚本查看输出："
    echo "   python3 check_serial.py"
    echo ""
    echo "预期行为："
    echo "  ✓ 调制解调器初始化"
    echo "  ✓ 网络注册（中国电信）"
    echo "  ✓ GPS初始化（使用AT+CGPS=1,1）"
    echo "  ✓ 等待GPS定位（1-5分钟）"
    echo "  ✓ 每60秒发送GPS短信到 +8615821259813"
    echo "  ✓ 共发送3次后停止"
else
    echo "❌ 上传失败！"
    echo ""
    echo "请重试以下步骤："
    echo "1. 确保USB连接稳定"
    echo "2. 重新进入下载模式（重复按钮操作）"
    echo "3. 再次运行: ./upload.sh"
fi
echo "================================================================================"
