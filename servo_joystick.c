/**
 * Controle de Servomotor com Joystick Analógico
 * Raspberry Pi Pico W – Pico SDK
 * 
 * Placa: BitDogLAB
 * Simulador: Wokwi
 *
 * Descrição:
 *   Lê o eixo Y (ou X) do joystick analógico via ADC e converte
 *   o valor para um pulso PWM correspondente ao ângulo do servo
 *   (0° a 180°).
 *
 * Pinagem (BitDogLAB / Wokwi):
 *   - GP26 (ADC0) → eixo VRY do joystick
 *   - GP27 (ADC1) → eixo VRX do joystick  (opcional)
 *   - GP22        → sinal PWM do servomotor
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

/* ──────────────────────────────────────────────
   Pinos
   ────────────────────────────────────────────── */
#define JOYSTICK_VRY_PIN  26   // ADC0 – eixo Y do joystick
#define JOYSTICK_VRX_PIN  27   // ADC1 – eixo X (não usado, mas declarado)
#define SERVO_PIN         22   // Saída PWM para o servomotor

/* ──────────────────────────────────────────────
   Parâmetros do Servo (Motor Micro Servo Padrão)
   Frequência PWM: 50 Hz  →  período = 20 ms
   Pulso mínimo : ~0,5 ms → 0°
   Pulso médio  : ~1,5 ms → 90°
   Pulso máximo : ~2,5 ms → 180°
   ────────────────────────────────────────────── */
#define PWM_FREQ_HZ       50
#define SERVO_MIN_US      500    // µs → 0°
#define SERVO_MAX_US      2500   // µs → 180°

/* ──────────────────────────────────────────────
   Resolução ADC
   O RP2040 possui ADC de 12 bits → 0 a 4095
   ────────────────────────────────────────────── */
#define ADC_MAX           4095

/* ──────────────────────────────────────────────
   Protótipos
   ────────────────────────────────────────────── */
void    servo_init(uint gpio);
void    servo_set_us(uint gpio, uint32_t pulse_us);
uint32_t map_adc_to_servo(uint16_t adc_val);

/* ══════════════════════════════════════════════
   MAIN
   ══════════════════════════════════════════════ */
int main(void)
{
    stdio_init_all();   // Inicializa UART para debug (opcional)

    /* ── Inicializa ADC ── */
    adc_init();
    adc_gpio_init(JOYSTICK_VRY_PIN);   // Habilita GP26 como entrada analógica

    /* ── Inicializa PWM do Servo ── */
    servo_init(SERVO_PIN);

    printf("Sistema iniciado – Controle de Servo por Joystick\n");

    while (true)
    {
        /* 1. Lê valor ADC do eixo Y (canal 0 = GP26) */
        adc_select_input(0);                      // ADC0 → GP26
        uint16_t adc_val = adc_read();            // 0 – 4095

        /* 2. Converte para largura de pulso em µs */
        uint32_t pulse_us = map_adc_to_servo(adc_val);

        /* 3. Aplica ao servomotor */
        servo_set_us(SERVO_PIN, pulse_us);

        /* 4. Debug via serial */
        uint32_t angle = ((pulse_us - SERVO_MIN_US) * 180) /
                         (SERVO_MAX_US - SERVO_MIN_US);
        printf("ADC: %4u | Pulso: %4lu µs | Ângulo: %3lu°\n",
               adc_val, pulse_us, angle);

        sleep_ms(20);   // Taxa de atualização ~50 Hz (sincroniza com o PWM)
    }

    return 0;
}

/* ══════════════════════════════════════════════
   FUNÇÕES AUXILIARES
   ══════════════════════════════════════════════ */

/**
 * servo_init – Configura o slice PWM para gerar 50 Hz no pino especificado.
 *
 * O RP2040 roda a 125 MHz. Para obter 50 Hz com resolução suficiente:
 *   - Divisor de clock: 64
 *   - Wrap (top)      : 39062  - 1  →  125_000_000 / 64 / 39062 ≈ 50,00 Hz
 */
void servo_init(uint gpio)
{
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint slice = pwm_gpio_to_slice_num(gpio);

    pwm_config cfg = pwm_get_default_config();

    /*
     * clock_sys = 125 MHz
     * Divisor fracionário = 64.0  →  f_contador = 125e6 / 64 = 1 953 125 Hz
     * wrap = 39062 - 1 = 39061   →  f_PWM = 1 953 125 / 39062 ≈ 50,00 Hz
     * Resolução por µs = 1 953 125 / 1_000_000 ≈ 1,953 contagens/µs
     */
    pwm_config_set_clkdiv(&cfg, 64.0f);
    pwm_config_set_wrap(&cfg, 39062 - 1);

    pwm_init(slice, &cfg, true);

    /* Começa no centro (90°) */
    servo_set_us(gpio, 1500);
}

/**
 * servo_set_us – Define a largura do pulso em microssegundos.
 *
 * Conversão:
 *   level = pulse_us * (f_contador / 1_000_000)
 *         = pulse_us * (125_000_000 / 64) / 1_000_000
 *         = pulse_us * 1953125 / 1000000
 *         ≈ pulse_us * 1.953125
 *
 * Simplificado como: (pulse_us * 125) / 64
 */
void servo_set_us(uint gpio, uint32_t pulse_us)
{
    /* Garante que o pulso fica nos limites do servo */
    if (pulse_us < SERVO_MIN_US) pulse_us = SERVO_MIN_US;
    if (pulse_us > SERVO_MAX_US) pulse_us = SERVO_MAX_US;

    uint32_t level = (pulse_us * 125ul) / 64ul;

    uint slice   = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    pwm_set_chan_level(slice, channel, (uint16_t)level);
}

/**
 * map_adc_to_servo – Mapeia valor ADC (0–4095) para pulso em µs
 *                    (SERVO_MIN_US a SERVO_MAX_US).
 *
 * Fórmula de mapeamento linear:
 *   pulse = MIN + (adc_val * (MAX - MIN)) / ADC_MAX
 */
uint32_t map_adc_to_servo(uint16_t adc_val)
{
    uint32_t range  = SERVO_MAX_US - SERVO_MIN_US;          // 2000 µs
    uint32_t pulse  = SERVO_MIN_US + ((uint32_t)adc_val * range) / ADC_MAX;
    return pulse;
}
