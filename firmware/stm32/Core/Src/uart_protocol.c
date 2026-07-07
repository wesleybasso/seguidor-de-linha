#include "uart_protocol.h"
#include <string.h>

enum {
    WAIT_SOF = 0,
    READ_TYPE,
    READ_LEN,
    READ_PAYLOAD,
    READ_CRC
};

uint8_t pegasus_crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; ++bit) {
            crc = (crc & 0x80u) ? (uint8_t)((crc << 1) ^ 0x07u) : (uint8_t)(crc << 1);
        }
    }
    return crc;
}

bool pegasus_encode_packet(uint8_t type, const uint8_t *payload, uint8_t len, uint8_t *out, size_t out_capacity, size_t *out_len) {
    if (out == NULL || out_len == NULL || len > PEGASUS_UART_MAX_PAYLOAD ||
        (len > 0 && payload == NULL) || out_capacity < (size_t)len + 4u) {
        return false;
    }
    out[0] = PEGASUS_UART_SOF;
    out[1] = type;
    out[2] = len;
    if (len > 0) {
        memcpy(&out[3], payload, len);
    }
    out[3u + len] = pegasus_crc8(&out[1], (size_t)len + 2u);
    *out_len = (size_t)len + 4u;
    return true;
}

void pegasus_parser_init(pegasus_parser_t *parser) {
    memset(parser, 0, sizeof(*parser));
    parser->state = WAIT_SOF;
}

bool pegasus_parser_push(pegasus_parser_t *parser, uint8_t byte, pegasus_packet_t *out) {
    switch (parser->state) {
    case WAIT_SOF:
        if (byte == PEGASUS_UART_SOF) {
            memset(&parser->packet, 0, sizeof(parser->packet));
            parser->index = 0;
            parser->state = READ_TYPE;
        }
        break;
    case READ_TYPE:
        parser->packet.type = byte;
        parser->state = READ_LEN;
        break;
    case READ_LEN:
        if (byte > PEGASUS_UART_MAX_PAYLOAD) {
            pegasus_parser_init(parser);
            break;
        }
        parser->packet.len = byte;
        parser->state = byte == 0 ? READ_CRC : READ_PAYLOAD;
        break;
    case READ_PAYLOAD:
        parser->packet.payload[parser->index++] = byte;
        if (parser->index >= parser->packet.len) {
            parser->state = READ_CRC;
        }
        break;
    case READ_CRC: {
        uint8_t check[PEGASUS_UART_MAX_PAYLOAD + 2u];
        check[0] = parser->packet.type;
        check[1] = parser->packet.len;
        memcpy(&check[2], parser->packet.payload, parser->packet.len);
        const bool ok = pegasus_crc8(check, (size_t)parser->packet.len + 2u) == byte;
        if (ok) {
            *out = parser->packet;
        }
        pegasus_parser_init(parser);
        return ok;
    }
    default:
        pegasus_parser_init(parser);
        break;
    }
    return false;
}
