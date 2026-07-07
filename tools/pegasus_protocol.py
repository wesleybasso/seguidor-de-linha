"""Implementacao Python de referencia do protocolo PEGASUS UART (HCP-001).

Espelha firmware/stm32/Core/Src/uart_protocol.c e
firmware/esp32/communication/uart_protocol.cpp:
frame = SOF(0xA5) | TYPE | LEN | PAYLOAD | CRC8(poly 0x07, init 0x00,
sobre TYPE+LEN+PAYLOAD).
"""

SOF = 0xA5
MAX_PAYLOAD = 64

MSG_TELEMETRY_BASIC = 0x01
MSG_SENSOR_VALUES = 0x02
MSG_SET_PID = 0x03
MSG_SET_MOTOR_CONFIG = 0x04
MSG_SET_FAN_CONFIG = 0x05
MSG_CALIBRATION_START = 0x06
MSG_CALIBRATION_STOP = 0x07
MSG_START_RUN = 0x08
MSG_STOP_RUN = 0x09
MSG_ARM = 0x0A
MSG_DISARM = 0x0B
MSG_HARDWARE_TEST = 0x0C
MSG_ERROR_STATUS = 0x0D
MSG_ACK = 0x0E
MSG_NACK = 0x0F
MSG_PING = 0x10
MSG_PONG = 0x11

MSG_NAMES = {
    0x01: "TELEMETRY_BASIC", 0x02: "SENSOR_VALUES", 0x03: "SET_PID",
    0x04: "SET_MOTOR_CONFIG", 0x05: "SET_FAN_CONFIG",
    0x06: "CALIBRATION_START", 0x07: "CALIBRATION_STOP",
    0x08: "START_RUN", 0x09: "STOP_RUN", 0x0A: "ARM", 0x0B: "DISARM",
    0x0C: "HARDWARE_TEST", 0x0D: "ERROR_STATUS", 0x0E: "ACK",
    0x0F: "NACK", 0x10: "PING", 0x11: "PONG",
}


def crc8(data: bytes) -> int:
    crc = 0
    for byte in data:
        crc ^= byte
        for _ in range(8):
            crc = ((crc << 1) ^ 0x07) & 0xFF if crc & 0x80 else (crc << 1) & 0xFF
    return crc


def encode_packet(msg_type: int, payload: bytes = b"") -> bytes:
    if len(payload) > MAX_PAYLOAD:
        raise ValueError("payload maior que MAX_PAYLOAD")
    body = bytes([msg_type, len(payload)]) + payload
    return bytes([SOF]) + body + bytes([crc8(body)])


class Parser:
    """Maquina de estados identica ao pegasus_parser_push do STM32."""

    WAIT_SOF, READ_TYPE, READ_LEN, READ_PAYLOAD, READ_CRC = range(5)

    def __init__(self):
        self._reset()

    def _reset(self):
        self.state = self.WAIT_SOF
        self.msg_type = 0
        self.length = 0
        self.payload = bytearray()

    def push(self, byte: int):
        """Alimenta um byte; retorna (type, payload) quando fecha um frame valido."""
        if self.state == self.WAIT_SOF:
            if byte == SOF:
                self.msg_type = 0
                self.length = 0
                self.payload = bytearray()
                self.state = self.READ_TYPE
        elif self.state == self.READ_TYPE:
            self.msg_type = byte
            self.state = self.READ_LEN
        elif self.state == self.READ_LEN:
            if byte > MAX_PAYLOAD:
                self._reset()
                return None
            self.length = byte
            self.state = self.READ_CRC if byte == 0 else self.READ_PAYLOAD
        elif self.state == self.READ_PAYLOAD:
            self.payload.append(byte)
            if len(self.payload) >= self.length:
                self.state = self.READ_CRC
        elif self.state == self.READ_CRC:
            body = bytes([self.msg_type, self.length]) + bytes(self.payload)
            ok = crc8(body) == byte
            result = (self.msg_type, bytes(self.payload)) if ok else None
            self._reset()
            return result
        return None
