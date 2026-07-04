# HDB-100 — Chassis / Power Board

## Função

Placa inferior estrutural e de potência.

## Dimensões

- Comprimento máximo: 182 mm.
- Largura máxima: 78 mm.
- Furo central de sucção: Ø22 mm inicial.

## Componentes

| Ref | Componente | Função |
|-----|------------|--------|
| JBAT | XT30 | entrada LiPo 2S |
| SW_PWR | chave | liga/desliga |
| Cbulk | 470–1000uF | filtro VBAT |
| UDRV | TB6612FNG | motores |
| UBUCK | Mini360/buck | 5V |
| J_ML | motor L | saída motor esquerdo |
| J_MR | motor R | saída motor direito |
| J_FAN | fan/ESC | sucção |
| Q_FAN | MOSFET opcional | motor DC sucção |
| J1/J2 | board-to-board | Brain Board |
| JSENS | sensor board | HDB-101 |

## Observação

Um único TB6612 é suficiente para os dois motores.
