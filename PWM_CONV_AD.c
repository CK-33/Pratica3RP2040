/**
 * Controle de Servomotor com Joystick Analógico
 * Raspberry Pi Pico – Pico SDK
 * * Placa: BitDogLAB / Simulador: Wokwi
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

/* ──────────────────────────────────────────────
   Pinos
   ────────────────────────────────────────────── */
#define JOYSTICK_VRY_PIN  26   // ADC0 – eixo Y do joystick (Avança/Recua)
#define JOYSTICK_VRX_PIN  27   // ADC1 – eixo X do joystick (Esquerda/Direita)
#define JOYSTICK_SEL_PIN  28   // Digital – Botão de clique do joystick
#define SERVO_PIN         22   // Saída PWM para o servomotor

/* ──────────────────────────────────────────────
   Parâmetros do Servo e ADC
   ────────────────────────────────────────────── */
#define PWM_FREQ_HZ       50
#define SERVO_MIN_US      1000    // µs → 0°
#define SERVO_MAX_US      2000   // µs → 180°
#define ADC_MAX           4095   // 12 bits

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
    stdio_init_all();   // Inicializa a USB para o printf funcionar

    /* ── Inicializa o ADC para os dois Eixos ── */
    adc_init();
    adc_gpio_init(JOYSTICK_VRY_PIN);   // Habilita GP26 (Eixo Y)
    adc_gpio_init(JOYSTICK_VRX_PIN);   // Habilita GP27 (Eixo X)

    /* ── Inicializa o Pino do Botão (Digital) ── */
    gpio_init(JOYSTICK_SEL_PIN);
    gpio_set_dir(JOYSTICK_SEL_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_SEL_PIN);    // Garante nível alto quando solto

    /* ── Inicializa PWM do Servo ── */
    servo_init(SERVO_PIN);

    printf("Sistema iniciado – Monitoramento Completo do Joystick\n");
    
    while (true)
    {
        /* 1. Lê valor ADC do eixo Y (Canal 0 = GP26) */
        adc_select_input(0);                      
        uint16_t adc_y = adc_read();            

        /* 2. Lê valor ADC do eixo X (Canal 1 = GP27) */
        adc_select_input(1);                      
        uint16_t adc_x = adc_read();            

        /* 3. Lê o estado do Botão (0 = Pressionado, 1 = Solto) */
        bool btn_pressionado = !gpio_get(JOYSTICK_SEL_PIN);

        /* 4. Converte o eixo Y para controlar o servo com inversão para ficar intuitivo */
        uint32_t adc_invertido = 4095 - adc_y;
        uint32_t pulse_us = map_adc_to_servo(adc_invertido);
        servo_set_us(SERVO_PIN, pulse_us);

        /* 5. Nova Matemática: Mapeia de -45° a +45° baseado no range de 1000µs a 2000µs */
        int32_t angle = (((int32_t)(pulse_us - SERVO_MIN_US) * 90) / (int32_t)(SERVO_MAX_US - SERVO_MIN_US)) - 45;

        /* 6. Print formatado com `%+3ld` para forçar a exibição dos sinais de + e - */
        printf("[Eixo Y]: %4u | [Eixo X]: %4u | [Botão]: %s | [Servo]: %+3ld°\n",
               adc_y, adc_x, (btn_pressionado ? "CLICADO" : "SOLTO    "), angle);

        sleep_ms(20);   
    }

    return 0;
}

/* ══════════════════════════════════════════════
   FUNÇÕES AUXILIARES
   ══════════════════════════════════════════════ */

void servo_init(uint gpio)
{
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_config cfg = pwm_get_default_config();

    pwm_config_set_clkdiv(&cfg, 64.0f);
    pwm_config_set_wrap(&cfg, 39062 - 1);
    pwm_init(slice, &cfg, true);

    servo_set_us(gpio, 1500); // Começa centralizado
}

void servo_set_us(uint gpio, uint32_t pulse_us)
{
    if (pulse_us < SERVO_MIN_US) pulse_us = SERVO_MIN_US;
    if (pulse_us > SERVO_MAX_US) pulse_us = SERVO_MAX_US;

    uint32_t level = (pulse_us * 125ul) / 64ul;
    pwm_set_chan_level(pwm_gpio_to_slice_num(gpio), pwm_gpio_to_channel(gpio), (uint16_t)level);
}

uint32_t map_adc_to_servo(uint16_t adc_val)
{
    uint32_t range  = SERVO_MAX_US - SERVO_MIN_US;          
    return SERVO_MIN_US + ((uint32_t)adc_val * range) / ADC_MAX;
}
