# Análise de Energia e Componentes - PEGASUS

Nota: estimativa preliminar para orientar escolhas de bancada. Preços, massas,
correntes e capacidade de bateria precisam ser confirmados com componentes reais
antes de virarem BOM final ou criterio de competicao.

**Data:** 2025-02-20 | **Equipe:** PEGASUS (Seguidor de Linha) | **Hardware Base:** STM32F411 + ESP32-S3, TB6612FNG, 2x N20, 2S LiPo

---

## 1. ORÇAMENTO DE CORRENTE POR TRILHO

### Cenário VBAT (Bateria 2S Direto)

| Condição | Corrente | Descrição |
|----------|----------|-----------|
| No-load | 0.20 A | 2× N20 em repouso |
| Típico corrida | 2.8 A | 2× N20 (0.4A cada) + EDF (2.0A min) |
| Pico | 9.2 A | 2× N20 stall (1.6A cada) + EDF máx (6.0A) |

### Cenário 5V (Buck Mini360)

| Condição | Corrente | Componentes |
|----------|----------|-------------|
| Base | 0.060 A | STM32F411 + TB6612 lógica |
| WiFi + sensores | 0.430 A | + ESP32-S3 WiFi avg + 10× IR LEDs ligados |
| Pico TX | 0.660 A | WiFi peak (350mA) + IR ligados |

**Análise Mini360:**
- Contínuo: 1.0 A (suficiente para 0.430 A típico)
- Pico: 2.5 A (suficiente para 0.660 A)
- **Conclusão:** Mini360 OK, mas margem apertada. Se IR sempre ligados causarem problemas, implementar PWM dos LEDs (ligar só durante leitura ADC ~5ms a cada 50ms).

---

## 2. ANÁLISE DE BATERIA (2S LiPo)

### Especificações de Candidatas

| Capacidade | C-rate | Max Contínuo | Sag @13A | Massa | Status |
|------------|--------|--------------|----------|-------|--------|
| 300 mAh | 25C | 7.5 A | 1.32 V | 15.2 g | INSUFICIENTE |
| 300 mAh | 50C | 15.0 A | 0.92 V | 15.2 g | ✓ |
| 450 mAh | 25C | 11.2 A | 1.32 V | 15.3 g | INSUFICIENTE |
| 450 mAh | 50C | 22.5 A | 0.92 V | 15.3 g | ✓ |
| 650 mAh | 25C | 16.2 A | 1.32 V | 15.5 g | ✓ |
| 650 mAh | 50C | 32.5 A | 0.92 V | 15.5 g | ✓ |

### Estimativa de Sag de Tensão

**Fórmula:** V_sag = I_pico × R_interna

- **450 mAh 50C:** R_int ≈ 70 mΩ → Sag @13A = 0.92 V (7.5V mínimo, aceitável)
- **650 mAh 25C:** R_int ≈ 100 mΩ → Sag @13A = 1.32 V (7.08V mínimo, crítico)

### Autonomia Estimada

- **Corrida típica:** 3.8 A @ VBAT × 60 s = 0.063 Ah consumido
- **450 mAh @ 50C:** ~6 corridas (6 min total) antes de troca
- **Recomendação:** **450 mAh 50C** (melhor balanço massa/performance)

### Proteção em Firmware

```
LOW_VOLTAGE_CUTOFF = 3.3V por célula (6.6V para 2S)
- Implementar monitoramento ADC de VBAT
- Hard-stop de motores se V_cell < 3.2V
```

---

## 3. QUALIDADE ANALÓGICA - Ruído Motor/Buck nos Canais ADC

### Problema

- **TB6612FNG PWM:** ~20 kHz (gera harmônicos até 1 MHz)
- **Mini360 buck:** ~340 kHz (ripple característico)
- **IR LEDs:** Corrente pulsada → acoplamento capacitivo em PA0-PA7

### Solução: Filtro RC por Canal

**Frequência de corte alvo:** 10 kHz (atenua ruído de chaveamento, preserva leitura sensores)

**Cálculo:**
```
f_c = 1 / (2π × R × C)
C = 1 / (2π × 10kHz × 10kΩ) ≈ 1.6 nF → usar 10 nF (E12)
```

**Componentes por canal:** R = 10 kΩ, C = 10 nF

### Checklist Plano Analógico (para Codex/KiCAD)

- [ ] **Filtro RC (10k + 10nF)** em cada entrada: PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7
- [ ] **Capacitor 100nF desacoplamento** próximo a cada sensor (máx 2cm de fio)
- [ ] **Plano GND contínuo** — sem cortes sob drivers/buck
- [ ] **Trilho 5V (buck) isolado** de VBAT (retornos separados até star point)
- [ ] **Estrela de GND:** ponto único de retorno para VBAT, 5V, sensores
- [ ] **Cabo VBAT blindado** de bateria até TB6612 (XT30 → PCB)
- [ ] **Via tecelada** em volta de PA0-PA7 (ligação GND próxima)

---

## 4. SELEÇÃO DE COMPONENTES - PREÇOS BRASIL/ALIEXPRESS

### Rodas de Silicone

| Opção | Preço Estimado | Massa | Nota |
|-------|----------------|-------|------|
| **Silicone 26 mm** | R$ 25–35 un | 4 g un | Menor inércia, mais rápido |
| Silicone 32 mm | R$ 35–50 un | 7 g un | Maior tração, mais peso |

**Recomendação:** 26 mm (4 un = R$ 100–140, 16 g total)

### Ventoinha de Sucção

| Opção | Preço | Massa | Consumo | Vantagem |
|-------|-------|-------|---------|----------|
| **Blower 5V 3010** | R$ 45–60 | 28 g | 0.2–0.4 A | Direto em buck, simples |
| Axial 4020 12V | R$ 35–50 | 35 g | ~0.3 A @ 8.4V | Menos potência em 2S |
| EDF 30mm + ESC30A | R$ 180–220 | 92 g | 2–6 A | Máxima sucção, peso alto |

**Recomendação:** **Blower 5V 3010** (custo-benefício, baixo consumo 5V)

### Sensores Frontais

| Opção | Preço | Massa | Acurácia | Nota |
|-------|-------|-------|----------|------|
| **Barra IR 8ch PCB** | R$ 45 | 12 g | ±2° | Atual, validado |
| QTR-8A Pololu | R$ 120 | 8 g | ±0.5° | Comercial, mais preciso |

**Recomendação:** Manter **8ch PCB** (custo 3×, massa -4g insuficiente para justificar)

### Conectores e Chave

| Item | Opção | Preço | Massa |
|------|-------|-------|-------|
| **Bateria** | XT30 macho/fêmea | R$ 8–12/par | 1.2 g par |
| **Liga/Desliga** | Chave micro 2P deslizante | R$ 10–15 | 3 g |

### IMU Opcional (Ajuda em Curva)

| Chip | Preço | Massa | Interface | Nota |
|------|-------|-------|-----------|------|
| MPU6050 | R$ 20–30 | 4 g | I2C | 6-DOF, viável |
| ICM42688 | R$ 35–50 | 3.5 g | SPI/I2C | 6-DOF, melhor ruído |

**Recomendação:** IMU **opcional** em Rev 2 (teste estabilidade sem antes)

---

## 5. TABELA FINAL DE POTÊNCIA

### Cenário de Corrida Típica (60 segundos)

**Condição:** Velocidade média, motores 40% PWM + EDF 3.0 A (média entre mín/máx)

| Trilho | Corrente | Tensão | Potência |
|--------|----------|--------|----------|
| VBAT | 3.80 A | 8.4 V | 31.9 W |
| 5V | 0.43 A | 5.0 V | 2.15 W |
| **TOTAL** | — | — | **34.1 W** |

### Energia por Corrida

- **Duração:** 60 segundos
- **Energia consumida:** 34.1 W × 60 s ÷ 3600 = **0.568 Wh**
- **Bateria 450 mAh @ 8.4 V:** 3.78 Wh
- **Autonomia:** 3.78 ÷ 0.568 ≈ **6.6 corridas** (≈ 6–7 minutos total)

### Margem de Segurança

| Aspecto | Valor | Status |
|---------|-------|--------|
| Corrente pico estimada | 9.2 A | 70% da capacidade bateria 450mAh 50C ✓ |
| Sag mínimo em pico | 7.5 V | Acima do limite STM32 (6.3V) ✓ |
| Tempo entre trocas | ~7 min | Aceitável para sessão de testes |
| Peso total estimado | ~280 g | Dentro de margens (com blower) |

---

## 6. RECOMENDAÇÕES PARA O CODEX (BOM/PCB)

### Decisões Estruturais

- **Buck:** Manter Mini360 (5V/1A), validado, backup: TPS563201 (2A, síncrono) se futuro requerer
- **Bateria:** **450 mAh 50C** (balance peso/potência, ~R$ 60–80)
- **Ventoinha:** **Blower 5V 3010** (R$ 50–60, 0.3A integrado ao 5V)
- **Rodas:** Silicone 26 mm (R$ 120–140 set 4un)

### PCB/Analógico (Imediato para Rev 1.1)

- [ ] Adicionar filtro RC **10k + 10nF** em **PA0–PA7** (8 canais motores + 2 sensores laterais)
- [ ] **100nF desacoplamento** imediato na entrada de cada sensor IR (máx 2 cm)
- [ ] Separar retorno **5V (buck)** de **VBAT** até ponto único no conector de bateria (star point GND)
- [ ] Blindagem ou tweits em **VBAT → TB6612** (reduz acoplamento capacitivo)
- [ ] Plano GND contínuo, sem cortes

### Firmware (Próximos Sprints)

- [ ] **ADC:** ativar averaging (4–8 amostras) + filtro FIR na leitura IR
- [ ] **PWM IR LEDs:** ligar só durante janela ADC (5–10 ms a cada 50 ms) → reduz consumo 5V para ~250 mA
- [ ] **Monitoramento VBAT:** ADC entrada + firmware cutoff 3.3V/célula (6.6V 2S)
- [ ] **Telemetria:** logar potência/corrente para validação em campo

### Massa Estimada (Sem Chassis)

| Subsistema | Massa |
|-----------|-------|
| Bateria 450mAh | 15.3 g |
| Blower + duct | 40 g |
| Rodas (4×) | 16 g |
| Motores N20 | 20 g |
| PCB + eletrônica | 35 g |
| Conector/chave | 5 g |
| **TOTAL** | **~131 g** |
| + Chassis alumínio | ~120–150 g |
| **FINAL** | **~250–280 g** |

---

## Referências de Cálculo

- **Motor N20 6V 1000rpm:** I_no_load=0.1A, I_típico=0.4A, I_stall=1.6A (datasheet genérico)
- **TB6612FNG:** PWM ~20 kHz, dropout ~0.5V @ 2A
- **Mini360:** 1.0 A contínuo, 2.5 A pico, R_int ~100 mΩ, dropout ~0.3V
- **2S LiPo 450mAh 50C:** R_int ~70 mΩ, sag=13A×0.07Ω=0.91V
- **ESP32-S3 WiFi AP:** 120 mA média, 350 mA pico TX

---

**Próximas Ações:**
1. Fabricar nova PCB Rev 1.1 com filtros RC analógicos
2. Testar consumo 5V com PWM IR (expectativa: -50 mA)
3. Validar sag de tensão em corrida com osciloscopio
4. Avaliar IMU (MPU6050) em Rev 2 se estabilidade exigir
