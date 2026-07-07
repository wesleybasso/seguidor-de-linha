"""Testes do protocolo PEGASUS UART (HCP-001).

Roda com:  python -m unittest discover tests/protocol
Valida a implementacao de referencia Python (tools/pegasus_protocol.py),
que espelha byte a byte o parser C do STM32 e o C++ do ESP32.
"""

import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools"))

import pegasus_protocol as pp  # noqa: E402


class TestCrc8(unittest.TestCase):
    def test_vetores_conhecidos(self):
        # CRC-8/ATM (poly 0x07, init 0x00): vetor classico "123456789" -> 0xF4
        self.assertEqual(pp.crc8(b"123456789"), 0xF4)
        self.assertEqual(pp.crc8(b""), 0x00)
        self.assertEqual(pp.crc8(b"\x00"), 0x00)
        self.assertEqual(pp.crc8(b"\x01"), 0x07)

    def test_frame_ping_bytes_exatos(self):
        # PING sem payload: A5 10 00 CRC(10 00)
        frame = pp.encode_packet(pp.MSG_PING)
        self.assertEqual(frame[:3], bytes([0xA5, 0x10, 0x00]))
        self.assertEqual(frame[3], pp.crc8(bytes([0x10, 0x00])))
        self.assertEqual(len(frame), 4)


class TestRoundTrip(unittest.TestCase):
    def _roundtrip(self, msg_type, payload=b""):
        parser = pp.Parser()
        result = None
        for byte in pp.encode_packet(msg_type, payload):
            result = parser.push(byte) or result
        self.assertIsNotNone(result, "frame valido nao foi decodificado")
        self.assertEqual(result, (msg_type, payload))

    def test_todos_os_tipos_sem_payload(self):
        for t in (pp.MSG_ARM, pp.MSG_DISARM, pp.MSG_START_RUN, pp.MSG_STOP_RUN,
                  pp.MSG_CALIBRATION_START, pp.MSG_CALIBRATION_STOP,
                  pp.MSG_HARDWARE_TEST, pp.MSG_PING, pp.MSG_PONG):
            self._roundtrip(t)

    def test_set_pid_12_bytes(self):
        import struct
        payload = struct.pack("<fff", 1.5, 0.0, 12.0)
        self._roundtrip(pp.MSG_SET_PID, payload)

    def test_telemetry_basic_24_bytes(self):
        self._roundtrip(pp.MSG_TELEMETRY_BASIC, bytes(range(24)))

    def test_sensor_values_40_bytes(self):
        self._roundtrip(pp.MSG_SENSOR_VALUES, bytes(range(40)))

    def test_payload_maximo(self):
        self._roundtrip(pp.MSG_ERROR_STATUS, bytes(pp.MAX_PAYLOAD))


class TestRobustez(unittest.TestCase):
    def test_crc_corrompido_descarta(self):
        frame = bytearray(pp.encode_packet(pp.MSG_ARM))
        frame[-1] ^= 0xFF
        parser = pp.Parser()
        results = [parser.push(b) for b in frame]
        self.assertTrue(all(r is None for r in results))

    def test_payload_corrompido_descarta(self):
        frame = bytearray(pp.encode_packet(pp.MSG_SET_PID, bytes(12)))
        frame[5] ^= 0x55
        parser = pp.Parser()
        results = [parser.push(b) for b in frame]
        self.assertTrue(all(r is None for r in results))

    def test_len_invalido_reseta_parser(self):
        parser = pp.Parser()
        for b in (0xA5, 0x10, 0xFF):  # LEN 255 > MAX_PAYLOAD
            self.assertIsNone(parser.push(b))
        # parser deve estar pronto para o proximo frame valido
        result = None
        for b in pp.encode_packet(pp.MSG_PING):
            result = parser.push(b) or result
        self.assertEqual(result, (pp.MSG_PING, b""))

    def test_recuperacao_apos_lixo_e_frame_parcial(self):
        parser = pp.Parser()
        ruido = bytes([0x00, 0xFF, 0x42, 0xA5, 0x03])  # lixo + frame abortado
        stream = ruido + pp.encode_packet(pp.MSG_STOP_RUN)
        results = [parser.push(b) for b in stream]
        decodificados = [r for r in results if r is not None]
        # O frame abortado (A5 03 ...) consome bytes do frame seguinte ate o
        # parser detectar CRC invalido; o segundo frame completo deve passar.
        parser2 = pp.Parser()
        stream2 = ruido + pp.encode_packet(pp.MSG_STOP_RUN) * 2
        decod2 = [r for b in stream2 for r in [parser2.push(b)] if r]
        self.assertIn((pp.MSG_STOP_RUN, b""), decod2 or decodificados)

    def test_dois_frames_colados(self):
        parser = pp.Parser()
        stream = pp.encode_packet(pp.MSG_PING) + pp.encode_packet(pp.MSG_ARM)
        decodificados = [r for b in stream for r in [parser.push(b)] if r]
        self.assertEqual(decodificados, [(pp.MSG_PING, b""), (pp.MSG_ARM, b"")])


if __name__ == "__main__":
    unittest.main()
