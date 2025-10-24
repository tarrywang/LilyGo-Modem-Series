#!/usr/bin/env python3
"""
串口监视器脚本
用于监控GPS短信程序的运行状态
"""

import serial
import time
import sys

PORT = '/dev/cu.usbserial-59691448411'
BAUD = 115200

def main():
    print(f"正在连接串口: {PORT} @ {BAUD} baud")
    print("=" * 70)

    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
        print("✅ 串口已打开")
        print("📡 正在读取数据...\n")
        print("=" * 70)

        while True:
            if ser.in_waiting > 0:
                try:
                    data = ser.readline().decode('utf-8', errors='ignore').strip()
                    if data:
                        # 添加时间戳
                        timestamp = time.strftime("%H:%M:%S")
                        print(f"[{timestamp}] {data}")
                        sys.stdout.flush()
                except Exception as e:
                    print(f"读取错误: {e}")

            time.sleep(0.01)

    except serial.SerialException as e:
        print(f"❌ 串口错误: {e}")
        return 1
    except KeyboardInterrupt:
        print("\n\n程序被用户中断")
        return 0
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("串口已关闭")

if __name__ == "__main__":
    sys.exit(main())
