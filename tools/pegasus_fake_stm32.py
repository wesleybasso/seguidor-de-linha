"""Emulador de STM32 PEGASUS sobre porta serial (Fase 0).

Conecta um adaptador USB-serial ao ESP32 (TX/RX cruzados, GND comum) e este
script se comporta como a Black Pill: responde ACK/NACK/PONG e envia
telemetria fake com uma linha varrendo os sensores — permite validar o
dashboard completo sem nenhum STM32 na bancada.

Uso:
    python tools/pegasus_fake_stm32.py COM5
    python tools/pegasus_fake_stm32.py COM5 --baud 115200

Requer: pip install pyserial
"""

import argparse
import struct
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import pegasus_protocol as pp  # noqa: E402

try:
    import serial
except ImportError:
    serial = None  # exigido apenas em main(); permite importar para testes

# Estados iguais ao enum do ESP32 (app_config.h)
ROBOT_IDLE, ROBOT_CALIBRATION, ROBOT_READY = 1, 2, 3
ROBOT_ARMED, ROBOT_RUNNING, ROBOT_SAFE_STOP = 4, 5, 10

SIMPLE_ACK = {
    pp.MSG_SET_PID, pp.MSG_SET_MOTOR_CONFIG, pp.MSG_SET_FAN_CONFIG,
    pp.MSG_HARDWARE_TEST,
}


class FakeRobot:
    def __init__(self):
        self.state = ROBOT_IDLE
        self.run_start = 0.0
        self.phase = 0

    def handle(self, msg_type):
        if msg_type == pp.MSG_PING:
            return pp.MSG_PONG
        if msg_type == pp.MSG_CALIBRATION_START:
            self.state = ROBOT_CALIBRATION
        elif msg_type == pp.MSG_CALIBRATION_STOP:
            self.state = ROBOT_READY
        elif msg_type == pp.MSG_ARM:
            self.state = ROBOT_ARMED
        elif msg_type == pp.MSG_START_RUN:
            if self.state != ROBOT_ARMED:
                return pp.MSG_NACK
            self.state = ROBOT_RUNNING
            self.run_start = time.monotonic()
        elif msg_type == pp.MSG_STOP_RUN:
            self.state = ROBOT_SAFE_STOP
        elif msg_type == pp.MSG_DISARM:
            self.state = ROBOT_IDLE
        elif msg_type not in SIMPLE_ACK:
            return pp.MSG_NACK
        return pp.MSG_ACK

    def sensors(self):
        """Linha triangular varrendo S0..S7, igual ao fake do pegasus_hal.c."""
        self.phase = (self.phase + 120) % 14000
        center = self.phase if self.phase < 7000 else 14000 - self.phase
        norm = []
        for i in range(8):
            dist = abs(i * 1000 - center)
            norm.append(max(0, (1000 * (1500 - dist)) // 1500) if dist < 1500 else 0)
        raw = [200 + v * 36 // 10 for v in norm]
        return raw, norm, center

    def telemetry_basic(self):
        _, norm, center = self.sensors()
        position = center - 3500
        run_ms = int((time.monotonic() - self.run_start) * 1000) if self.state == ROBOT_RUNNING else 0
        flags = 0x01 if any(norm) else 0x10
        return struct.pack(
            "<BHhhhhhHBII",
            self.state, 7800, position, -position, 0,
            300, 300, 0, flags, run_ms, 0,
        )

    def sensor_values(self):
        raw, norm, _ = self.sensors()
        return struct.pack("<16H4H", *raw, *norm, 200, 0, 200, 0)


def main():
    ap = argparse.ArgumentParser(description="Emulador STM32 PEGASUS")
    ap.add_argument("port", help="porta serial, ex.: COM5")
    ap.add_argument("--baud", type=int, default=115200)
    args = ap.parse_args()

    if serial is None:
        sys.exit("pyserial nao instalado. Rode: pip install pyserial")
    ser = serial.Serial(args.port, args.baud, timeout=0)
    parser = pp.Parser()
    robot = FakeRobot()
    last_basic = last_sensor = 0.0
    print(f"Emulando STM32 em {args.port} @ {args.baud}. Ctrl+C para sair.")

    while True:
        for byte in ser.read(256):
            result = parser.push(byte)
            if result:
                msg_type, _ = result
                name = pp.MSG_NAMES.get(msg_type, hex(msg_type))
                reply = robot.handle(msg_type)
                ser.write(pp.encode_packet(reply, bytes([msg_type, 0])))
                print(f"RX {name} -> TX {pp.MSG_NAMES[reply]}")

        now = time.monotonic()
        if now - last_basic >= 0.04:  # 25 Hz
            last_basic = now
            ser.write(pp.encode_packet(pp.MSG_TELEMETRY_BASIC, robot.telemetry_basic()))
        if now - last_sensor >= 0.1:  # 10 Hz
            last_sensor = now
            ser.write(pp.encode_packet(pp.MSG_SENSOR_VALUES, robot.sensor_values()))
        time.sleep(0.005)


if __name__ == "__main__":
    main()
