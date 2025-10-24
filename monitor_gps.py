#!/usr/bin/env python3
"""
GPS数据采集监控脚本
用于监控T-Call A7670X GPS测试程序的串口输出
"""

import serial
import time
import sys
from datetime import datetime

# 串口配置
PORT = '/dev/cu.usbserial-59691448411'
BAUDRATE = 115200

def print_header():
    """打印监控头部信息"""
    print("\n" + "=" * 70)
    print("GPS数据采集监控器")
    print("=" * 70)
    print(f"串口: {PORT}")
    print(f"波特率: {BAUDRATE}")
    print(f"开始时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print("=" * 70)
    print("\n可用命令（在串口监视器中输入）：")
    print("  log   - 显示GPS日志文件")
    print("  now   - 立即检查GPS")
    print("  clear - 清空GPS日志")
    print("\n按 Ctrl+C 停止监控\n")
    print("=" * 70)

def highlight_gps_data(line):
    """高亮显示GPS关键信息"""
    # GPS定位成功
    if "GPS Fix Acquired" in line or "VALID" in line:
        return f"\033[92m{line}\033[0m"  # 绿色
    # GPS未定位
    elif "No GPS fix" in line or "NO_FIX" in line:
        return f"\033[93m{line}\033[0m"  # 黄色
    # 错误信息
    elif "ERROR" in line or "❌" in line or "Failed" in line:
        return f"\033[91m{line}\033[0m"  # 红色
    # GPS数据行
    elif "+CGNSSINFO:" in line:
        return f"\033[96m{line}\033[0m"  # 青色
    # 成功信息
    elif "✅" in line or "SUCCESS" in line:
        return f"\033[92m{line}\033[0m"  # 绿色
    # 日志文件操作
    elif "[FILE]" in line:
        return f"\033[94m{line}\033[0m"  # 蓝色
    else:
        return line

def monitor_serial():
    """监控串口数据"""
    try:
        # 打开串口
        ser = serial.Serial(PORT, BAUDRATE, timeout=1)
        print_header()

        line_buffer = ""
        gps_fix_count = 0
        no_fix_count = 0

        while True:
            if ser.in_waiting > 0:
                # 读取数据
                data = ser.read(ser.in_waiting)
                try:
                    decoded = data.decode('utf-8', errors='replace')
                except:
                    continue

                line_buffer += decoded

                # 处理完整的行
                while '\n' in line_buffer:
                    line, line_buffer = line_buffer.split('\n', 1)
                    line = line.rstrip()

                    if line:
                        # 统计GPS状态
                        if "GPS Fix Acquired" in line:
                            gps_fix_count += 1
                        elif "No GPS fix" in line:
                            no_fix_count += 1

                        # 高亮显示并输出
                        colored_line = highlight_gps_data(line)
                        timestamp = datetime.now().strftime('%H:%M:%S')
                        print(f"[{timestamp}] {colored_line}")
                        sys.stdout.flush()

            time.sleep(0.01)

    except serial.SerialException as e:
        print(f"\n\033[91m错误: 无法打开串口 {PORT}\033[0m")
        print(f"详细信息: {e}")
        print("\n请检查：")
        print("1. 设备是否已连接")
        print("2. 串口是否被其他程序占用")
        print("3. 串口路径是否正确")
        sys.exit(1)

    except KeyboardInterrupt:
        print("\n\n" + "=" * 70)
        print("监控已停止")
        print("=" * 70)
        print(f"GPS定位成功次数: {gps_fix_count}")
        print(f"GPS未定位次数: {no_fix_count}")
        print(f"结束时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("=" * 70)
        ser.close()
        sys.exit(0)

    except Exception as e:
        print(f"\n\033[91m未知错误: {e}\033[0m")
        sys.exit(1)

def main():
    """主函数"""
    print("\n正在连接设备...")
    time.sleep(0.5)
    monitor_serial()

if __name__ == "__main__":
    main()
