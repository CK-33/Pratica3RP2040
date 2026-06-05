# Prática de Laboratório – RP2040: Controle de Servomotor com Joystick Analógico

**Disciplina:** Microcontroladores e Microprocessadores – ELET0021  
**Instituição:** Universidade Federal do Vale do São Francisco – UNIVASF  
**Curso:** Engenharia Elétrica  
**Professor:** Prof. Dr. Ricardo Menezes Prates  
**Data:** 13/05/2026  
**Turma:** E5  

---

##  Integrantes

| Nome |
|------|
| Anderson Luiz Barreto |
| Ítalo Gustavo Vieira Souza |
| Maria Clara de Souza Silva |
| Thyago Torres de Castro Gama | 

---

##  Descrição do Projeto

Este projeto foi desenvolvido como atividade prática da disciplina de Microcontroladores e Microprocessadores (ELET0021). O objetivo é projetar, com o auxílio da ferramenta **Pico SDK**, um sistema capaz de controlar o **ângulo de um servomotor** por meio de um **joystick analógico**, utilizando os periféricos **ADC (Conversor Analógico-Digital)** e **PWM (Pulse Width Modulation)** presentes no microcontrolador **RP2040**.

> **Observação importante:** Em virtude da ausência de aulas de laboratório presencial no período de realização desta atividade, não foi possível utilizar a placa física **BitDogLAB**. Dessa forma, **toda a prática foi realizada integralmente por simulação computacional no ambiente Wokwi**, incluindo tanto o joystick analógico quanto o servomotor, ambos simulados dentro do VS Code com a extensão Wokwi integrada.

---

##  Objetivos

- Compreender o funcionamento do conversor A/D presente no RP2040.
- Configurar e utilizar o módulo PWM para controle de servomotores.
- Integrar leitura de joystick analógico com geração de sinal PWM proporcional.
- Desenvolver código em linguagem C utilizando o Pico SDK.
- Simular o circuito no ambiente Wokwi integrado ao VS Code.

---

##  Hardware e Software Utilizados

### Hardware (Simulado no Wokwi)
| Item | Descrição |
|------|-----------|
| Microcontrolador | Raspberry Pi Pico W (RP2040) – simulado |
| Joystick Analógico | Joystick analógico padrão Wokwi (eixos VRX/VRY) – simulado |
| Servomotor | Motor micro servo padrão Wokwi – simulado |

> **Nota:** A placa BitDogLAB foi prevista no enunciado para uso com hardware físico, porém não foi utilizada devido à indisponibilidade de aulas de laboratório.

### Software
| Ferramenta | Finalidade |
|------------|-----------|
| VS Code | Ambiente de desenvolvimento |
| Pico SDK | Kit de desenvolvimento para RP2040 |
| Wokwi (extensão VS Code) | Simulação do servomotor |
| CMake | Sistema de build |
| Git / GitHub | Versionamento e submissão |

---

##  Esquema de Conexões

```
Raspberry Pi Pico W          Joystick Analógico
────────────────────         ──────────────────
GP26 (ADC0)        ◄──────── VRY  (eixo Y)
GP27 (ADC1)        ◄──────── VRX  (eixo X)
3V3                ─────────► VCC
GND                ─────────► GND

Raspberry Pi Pico W          Servomotor
────────────────────         ──────────
GP22 (PWM)         ─────────► Sinal (laranja/amarelo)
3V3                ─────────► V+    (vermelho)
GND                ─────────► GND   (marrom/preto)
```

> **Obs.:** Não é necessário circuito de interface entre o Pico W e o servomotor nesta prática, pois o servo é simulado no Wokwi.

---

##  Funcionamento do Sistema

### 1. Leitura do Joystick (ADC)

O joystick analógico possui dois potenciômetros que variam sua resistência conforme o movimento nos eixos X e Y. O RP2040 lê essa tensão variável (0 a 3,3 V) através do seu ADC de **12 bits**, gerando valores entre **0 e 4095**.

- **GP26 → ADC0** → Eixo Y do joystick
- **GP27 → ADC1** → Eixo X do joystick

### 2. Mapeamento do Valor ADC

O valor lido pelo ADC é mapeado linearmente para uma largura de pulso PWM correspondente ao ângulo do servo:

```
pulse_us = SERVO_MIN_US + (adc_val × (SERVO_MAX_US - SERVO_MIN_US)) / ADC_MAX

Onde:
  SERVO_MIN_US = 500  µs  →  0°
  SERVO_MAX_US = 2500 µs  →  180°
  ADC_MAX      = 4095
```

### 3. Geração do Sinal PWM

O servomotor padrão opera com sinal PWM de **50 Hz** (período de 20 ms). A largura do pulso determina o ângulo:

| Largura do Pulso | Ângulo |
|-----------------|--------|
| ~0,5 ms (500 µs) | 0° |
| ~1,5 ms (1500 µs) | 90° (centro) |
| ~2,5 ms (2500 µs) | 180° |

**Configuração do PWM no RP2040:**
- Clock do sistema: 125 MHz
- Divisor de clock: 64
- Wrap (TOP): 39061
- Frequência resultante: ≈ 50 Hz

---

##  Estrutura do Repositório

```
.
├── servo_joystick.c      # Código principal em C
├── CMakeLists.txt        # Arquivo de build (CMake + Pico SDK)
├── diagram.json          # Diagrama de simulação para o Wokwi
└── README.md             # Este arquivo
```

---

##  Como Compilar e Executar

### Pré-requisitos

- [Pico SDK](https://github.com/raspberrypi/pico-sdk) instalado e a variável `PICO_SDK_PATH` configurada.
- CMake ≥ 3.13
- Compilador ARM GCC (`arm-none-eabi-gcc`)
- VS Code com extensão [Wokwi](https://marketplace.visualstudio.com/items?itemName=wokwi.wokwi-vscode)

### Compilação

```bash
# Clone o repositório
git clone https://github.com/<seu-usuario>/servo_joystick.git
cd servo_joystick

# Crie a pasta de build e compile
mkdir build && cd build
cmake .. -DPICO_SDK_PATH=$PICO_SDK_PATH
make -j4
```

Após a compilação, o arquivo `servo_joystick.uf2` estará disponível na pasta `build/`.

### Simulação no Wokwi (VS Code)

1. Abra a pasta do projeto no VS Code.
2. Certifique-se de que o arquivo `diagram.json` está na raiz do projeto.
3. Pressione `F1` → **Wokwi: Start Simulator**.
4. A simulação iniciará mostrando o Pico W, o joystick e o servomotor.
5. Interaja com o joystick clicando e arrastando no simulador para controlar o ângulo do servo.

> **Obs.:** A etapa de gravação física na BitDogLAB não foi realizada, pois toda a prática foi conduzida em ambiente simulado.

---

##  Testes Realizados

### Teste 1 – Verificação do ADC
- **Objetivo:** Confirmar que o ADC lê corretamente os valores do joystick.
- **Procedimento:** Mover o joystick nas posições extremas (mínimo e máximo) e verificar via serial os valores ADC impressos.
- **Resultado esperado:** Valores próximos de `0` na posição mínima e `4095` na posição máxima.

### Teste 2 – Verificação do PWM
- **Objetivo:** Confirmar que o sinal PWM é gerado com frequência de 50 Hz.
- **Procedimento:** Observar no simulador Wokwi o comportamento do servo com pulso fixo de 500 µs, 1500 µs e 2500 µs.
- **Resultado esperado:** Servo posicionado em 0°, 90° e 180°, respectivamente.

### Teste 3 – Controle Proporcional
- **Objetivo:** Validar que o movimento do joystick controla proporcionalmente o ângulo do servo.
- **Procedimento:** Mover o joystick suavemente do mínimo ao máximo e observar o servo.
- **Resultado esperado:** Servo se move suavemente de 0° a 180° acompanhando o joystick.

---

##  Resultados

Toda a validação do sistema foi conduzida no simulador Wokwi, dado que as aulas de laboratório presencial não foram realizadas no período desta atividade. Dentro do ambiente simulado, o sistema funcionou conforme esperado: a leitura do ADC respondeu linearmente ao movimento do joystick virtual, e o mapeamento para largura de pulso PWM resultou em controle proporcional e suave do ângulo do servomotor. O debug via serial (USB) do próprio Wokwi permitiu verificar em tempo real os valores de ADC, pulso em µs e ângulo calculado.

Exemplo de saída serial observada no simulador:

```
Sistema iniciado – Controle de Servo por Joystick
ADC:    0 | Pulso:  500 µs | Ângulo:   0°
ADC: 2047 | Pulso: 1500 µs | Ângulo:  90°
ADC: 4095 | Pulso: 2500 µs | Ângulo: 180°
```

---

##  Vídeo de Demonstração

> 🔗 Link do vídeo: *(a ser preenchido após gravação)*

O vídeo apresenta a solução desenvolvida, demonstrando o funcionamento completo do sistema no simulador Wokwi (joystick + servomotor), com duração máxima de 5 minutos.

---

##  Referências

- [Raspberry Pi Pico SDK Documentation](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Wokwi Simulator](https://wokwi.com)
- Vídeo de apoio da prática: [Link Dropbox](https://www.dropbox.com/scl/fi/i32f4t9dqggn4c1durb4o/2025-02-04-08-52-05.mkv?rlkey=s6ofq9yumuts3h8chte052cuj&dl=0)

---

**Prazo de submissão:** 29/05/2026  

