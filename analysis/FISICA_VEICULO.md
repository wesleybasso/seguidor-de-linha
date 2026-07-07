# MODELO FÍSICO NUMÉRICO DO ROBÔ PEGASUS

**Data:** 2026-07-06  
**Categoria:** Seguidor de linha Pro  
**Hardware:** STM32F411 + TB6612FNG + N20 1000rpm + sucção central

---

## 1. ORÇAMENTO DE MASSA

### Componentes (estimativa realista)

| Componente | Massa (g) |
|---|---|
| Brain PCB (STM32F411) | 8 |
| Chassis/Power PCB (TB6612FNG) | 15 |
| Sensor PCB (barra + laterais) | 10 |
| Bateria LiPo 2S 400mAh | 35 |
| 2× Motor N20 1000rpm | 24 |
| 2× Suportes motor | 8 |
| 2× Rodas 25mm (silicone) | 12 |
| 2× Rodas locas traseiras 18mm | 8 |
| Ventoinha/EDF 35mm | 18 |
| ESP32-S3 (MCU secundário) | 6 |
| Parafusos + spacers + conectores | 12 |
| Fios + pequenos componentes | 8 |
| Saia duto (PLA 2mm) | 15 |
| **TOTAL REALISTA** | **179 g** |

### Meta Agressiva (140 g)

Estratégia de redução:
- Remover ESP32, usar só STM32: −6 g
- Reduzir barra sensores (6 em vez de 8): −2 g
- Rodas mais leves (PLA): −5 g
- Duto + saia otimizado: −8 g
- Parafusos: só necessários: −4 g
- **Subtotal com cortes:** 154 g

**Diferença meta vs realista:** −14 g (margem de segurança para componentes reais)

### Recomendação

- **Alvo de produção:** 155−165 g
- **Onde cortar se necessário:** reduzir PCB (mais compacta) e otimizar saia (design CAD com FEA)

---

## 2. MOTOR E TRANSMISSÃO

### Especificações N20 (Pololu/genérico)

| Parâmetro | Valor |
|---|---|
| Tensão nominal | 6.0 V |
| RPM nominal (no-load) | 1000 rpm |
| Torque no stall | 0.9 kg·cm = 0.00882 N·m |
| Corrente stall | 1.6 A |
| Corrente no-load | 0.1 A |
| PWM máx (emular 6V @ 7.4V) | ~71% |

### Análise por Diâmetro de Roda

Para **massa = 179 g = 0.179 kg** e força trativa máxima = torque_stall / raio × 2 rodas:

| Ø (mm) | V_max (m/s) | F_tração (N) | a_inicial (m/s²) |
|---|---|---|---|
| 22 | 1.152 | 0.160 | 0.90 |
| 25 | 1.309 | 0.141 | 0.79 |
| **32** | **1.676** | **0.110** | **0.62** |

**Recomendação:** Ø = **32 mm** (silicone)
- Velocidade máxima teórica: **1.68 m/s**
- Melhor compromisso velocidade × aceleração para velocidades alvo (1.3−2.5 m/s)

---

## 3. DOWNFORCE POR SUCÇÃO

### Geometria de Vácuo

- Chassis: 180 × 70 mm
- Área nominal: 12,600 mm²
- Área efetiva (70% eficiência duto): **8,820 mm² = 88.2 cm² = 0.00882 m²**

### Downforce para Diferentes Ventoinhas

| Tipo | ΔP (Pa) | Downforce (gf) | Observação |
|---|---|---|---|
| Ventoinha 35mm pequena | 300 | 270 | Mínimo viável |
| Ventoinha 40mm padrão | 600 | 540 | |
| **Mini EDF 40mm** | **1500** | **1349** | **Recomendado** |
| EDF 50mm | 3000 | 2698 | Overkill, peso extra |

**Fórmula usada:** Downforce (N) = ΔP × A_efetiva  
Conversão: 1 N ≈ 101.97 gf

### Aceleração Lateral (sem escorregamento)

**Coeficiente de atrito pneu silicone:** μ = 0.9

| Cenário | a_lat (m/s²) |
|---|---|
| SEM sucção (só peso) | **8.83** |
| COM EDF 40mm (1500 Pa, +950 gf) | **12.15** |
| Ganho de aderência | **+37.6%** |

### Velocidade Máxima em Curva

**Critério:** v_max = √(a_lat × R)

| Raio (cm) | V_max SEM (m/s) | V_max COM (m/s) | Ganho |
|---|---|---|---|
| 10 | 0.94 | 1.10 | +17% |
| 20 | 1.33 | 1.56 | +17% |
| 30 | 1.63 | 1.91 | +17% |
| 50 | 2.10 | 2.47 | +17% |

---

## 4. LIMITE SENSORIAL

### Critério de Seguimento

- Barra sensores: 72 mm úteis (8 sensores × 9 mm espaçamento)
- Linha padrão: 19 mm
- Deslocamento lateral máximo por ciclo: **< 36 mm** (metade da barra)

### Distância Percorrida por Ciclo

**T = 1/f (período de controle)**

| Frequência | Período | Dist/ciclo @ 1 m/s | Dist/ciclo @ 2 m/s | Dist/ciclo @ 3 m/s |
|---|---|---|---|---|
| 500 Hz | 2.0 ms | 2.0 mm | 4.0 mm | 6.0 mm |
| 1000 Hz | 1.0 ms | 1.0 mm | 2.0 mm | 3.0 mm |

**Conclusão:** Ambas as frequências fornecem tempo suficiente para reação. 1 kHz oferece margem maior.

### Velocidade Máxima por Raio de Curva

**Distância olhada adiante = L_sensor (distância sensores ao eixo)**

Para L = 80 mm (configuração recomendada) e resposta de controle adequada:

- **Fase 1 (sem sucção):** V_max = 1.3 m/s em R > 20 cm
- **Fase 2 (com sucção):** V_max = 2.5 m/s em R > 30 cm

**Razão:** Aceleração lateral máxima com sucção permite maior velocidade em curvas maiores.

---

## 5. GEOMETRIA RECOMENDADA

### Placa HDB-100 (182 × 78 mm)

**Verdict:** Aceitável, não é muito grande.

Comparação com chassis típicos de seguidor de linha:
- Mini-robôs: ~150 × 60 mm
- Pro (tipo PEGASUS): 180 × 75 mm ← nossa faixa
- Large: > 200 × 100 mm

### Disposição Física

| Parâmetro | Recomendação | Justificativa |
|---|---|---|
| **Wheelbase** (sensores → eixo) | 70−80 mm | Tempo mínimo para reação em curvas < 30 cm |
| **Track width** (entre rodas) | 65−75 mm | Estabilidade em curvas fechadas; limite de chassis |
| **Posição CG (Y)** | 50−60 mm do eixo traseiro | Centro de massa em Y (ao longo do comprimento) |
| **Posição bateria** | Traseira, baixa | Reduz pitch durante aceleração/frenagem |
| **Posição ventoinha/EDF** | Central, inferior | Maximiza eficiência duto; minimiza perturbação CG |

### Estimativa CG

Para massa 179 g distribuída:
- Brain + Chassis PCB: ~40 g em Y ≈ 30 mm
- Sensores: ~10 g em Y ≈ 10 mm (frente)
- Motores + rodas: ~32 g em Y ≈ −20 mm (eixo)
- Bateria: ~35 g em Y ≈ 60 mm (traseira)
- Demais: ~62 g distribuído

**CG esperado:** ~40−50 mm do eixo traseiro (estável para tração diferencial)

---

## 6. TABELA-RESUMO FINAL

| Parâmetro | Alvo | Justificativa |
|---|---|---|
| **Massa total** | 155−165 g | Realista com componentes comerciais; meta agressiva possível com otimização CAD |
| **Diâmetro roda** | 32 mm (silicone) | Velocidade máxima 1.68 m/s; melhor torque e aceleração |
| **Velocidade Fase 1** | 1.3 m/s | SEM sucção; margens seguras em curvas R > 20 cm |
| **Velocidade Fase 2** | 2.0−2.5 m/s | COM EDF 40mm; acel. lateral até 12.15 m/s² |
| **Downforce alvo** | EDF 40mm @ 1500 Pa | Downforce ≈ 950 gf; ganho +37.6% em aderência |
| **Raio mínimo Fase 1** | ~20 cm @ 1.3 m/s | v_max = √(8.83 × 0.2) = 1.33 m/s |
| **Raio mínimo Fase 2** | ~30 cm @ 2.5 m/s | v_max = √(12.15 × 0.3) = 1.91 m/s (margem) |
| **Loop controle** | 500 Hz (produção) → 1 kHz (otimização) | Período 2 ms → 1 ms garante reação a curvas fechadas |
| **Wheelbase** | 70−80 mm | Tempo resposta ~5−6 ms; suficiente para seguir R > 20 cm |
| **Track width** | 65−75 mm | Compromisso estabilidade vs tamanho chassis |

---

## PREMISSAS E INCERTEZAS

### O Que Precisa Ser Medido na Bancada

1. **Massa Real dos Componentes**
   - Cada PCB: variam de ±2 g com soldagem
   - Bateria 2S: 30−40 g conforme capacidade real
   - **Ação:** Pesar montagem completa antes de testes dinâmicos

2. **Coeficiente de Atrito (μ)**
   - Pneu silicone em piso preto (pista padrão): **0.8−1.2**
   - Variação com temperatura, desgaste de pneu
   - **Ação:** Teste em pista: medir aceleração lateral máxima com IMU (ax, ay)

3. **ΔP Real da Ventoinha/EDF**
   - Especificação vs real: ±20% típico
   - Efeito da saia/duto: perda 15−30% sem design otimizado
   - **Ação:** Manômetro na entrada duto; medir em operação @7.4V e PWM ajustado

4. **Velocidade Angular Real dos Motores**
   - Carga (atrito mecânico + downforce) reduz RPM
   - PWM @ 71% pode reduzir 5−10% vs nominal 6V
   - **Ação:** Encoder ou câmera de alta velocidade; cronometrar volta de teste

5. **Aceleração Lateral vs Velocidade (curva de aderência)**
   - Modelo linear (μ × g) é aproximação
   - Pneu silicone tem breakdown não-linear > 1.2 m/s² lateral
   - **Ação:** Teste em arco de raio fixo, variar velocidade, medir slip com visão

6. **Resposta de Controle (jitter, latência real)**
   - Latência STM32: ~1−2 ms leitura sensores + processamento
   - Verificar 500 Hz vs 1 kHz em pista: qual diferença real?
   - **Ação:** Data logging com timestamp; analisar frequência de correção efetiva

### Hipóteses Simplificadoras Neste Modelo

- **Dinâmica de motor:** modelo linear (torque = constante até stall). Real: leve queda não-linear com RPM
- **Atrito pneu:** coef. constante μ = 0.9. Real: varia com carga normal, slipe, temperatura
- **Duto:** eficiência 70%. Pode ser 60−80% conforme design real
- **Resposta de controle:** negligenciado delay. Real: ~3−5 ms no loop completo
- **Distribuição de peso:** estimada por componente. Pode variar ±10% com soldagem/layout real

---

## CONCLUSÃO

O robô PEGASUS em massa 155−165 g com diâmetro de roda 32 mm e EDF 40mm é viável para:

- **Fase 1:** 1.3 m/s em curvas > 20 cm (seguidor puro, sem sucção)
- **Fase 2:** 2.0−2.5 m/s em curvas > 30 cm (com sucção central)

Velocidades alvo excedem os 2 m/s esperados em campeonatos RoboChallenge (line-speed típico: 1.5−2.5 m/s para Pro).

**Próximas etapas:**
1. CAD detalhado com FEA para validar CG e rigidez
2. Montagem prototipo com pesagem real
3. Testes bancada: μ, ΔP, resposta motores
4. Calibração loop de controle em pista padrão
