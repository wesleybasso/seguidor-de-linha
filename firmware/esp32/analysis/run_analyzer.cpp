#include "run_analyzer.h"

String analyze_run(const run_summary_t &summary) {
    String text;
    const int32_t avg_error = summary.samples ? summary.error_sum_abs / static_cast<int32_t>(summary.samples) : 0;
    text += "Diagnostico:\\n";
    if (summary.samples == 0) {
        text += "- Sem amostras de corrida.\\n";
        return text;
    }
    if (avg_error > 900) {
        text += "- Erro medio alto; robo pode estar atrasado na linha.\\n";
    }
    if (summary.error_max_abs > 2500) {
        text += "- Picos de erro grandes detectados.\\n";
    }
    if (summary.line_lost_count > 0) {
        text += "- Linha perdida em algumas amostras.\\n";
    }
    if (summary.pwm_saturation_count > summary.samples / 5) {
        text += "- PWM saturou com frequencia.\\n";
    }
    text += "Sugestoes:\\n";
    if (avg_error > 900) {
        text += "- Reduzir base_speed ou aumentar Kp em pequenos passos.\\n";
    }
    if (summary.error_max_abs > 2500) {
        text += "- Aumentar Kd ou reduzir Kp se houver oscilacao.\\n";
    }
    if (summary.line_lost_count > 0) {
        text += "- Recalibrar sensores e reduzir velocidade antes de aumentar fan.\\n";
    }
    text += "- Aplicar ajustes somente com confirmacao; nunca automaticamente em corrida.\\n";
    return text;
}
