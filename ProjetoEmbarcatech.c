#include <stdio.h>
#include "pico/stdlib.h"

#define GREEN_LED_PIN 11 //LED para menos de 25¢ de capacidade.
#define BLUE_LED_PIN 12 //LED para menos de 50% de capacidade.
#define RED_LED_PIN 13 //LED para menos de 75% de capacidade.
#define BUTTON 5 //Botão que faz a porcentagem mudar.
#define RESET_TIME 10000 // Tempo para reiniciar o volume (em milissegundos)

//função de estado cheio
void volume_total(){
    gpio_put(RED_LED_PIN,false);
    gpio_put(BLUE_LED_PIN,false);
    gpio_put(GREEN_LED_PIN,false);
}

//função de estado em menos 75%.
void volume_75(){
    gpio_put(RED_LED_PIN,false);
    gpio_put(BLUE_LED_PIN,false);
    gpio_put(GREEN_LED_PIN,true);
}

//função de estado em menos de 50%.
void volume_50(){
    gpio_put(RED_LED_PIN,false);
    gpio_put(BLUE_LED_PIN,true);
    gpio_put(GREEN_LED_PIN,false);
}

//função de estado em menos de 25%.
void volume_25(){
    gpio_put(RED_LED_PIN,true);
    gpio_put(BLUE_LED_PIN,false);
    gpio_put(GREEN_LED_PIN,false);
}

void liga_led(int x){
    if(x>75){
        volume_total();
    }else if(x>50){
        volume_75();
    }else if(x>25){
        volume_50();
    }else{
        volume_25();
    }
}

int main()
{
    stdio_init_all();

    // Iniciando o Led vermelho.
    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN,GPIO_OUT);

    // Iniciando o Led azul.
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(BLUE_LED_PIN,GPIO_OUT);

    // Iniciando o Led verde.
    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN,GPIO_OUT);

    // Iniciando o botão.
    gpio_init(BUTTON);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_up(BUTTON);

    // Inicializando o valor do volume inicial em porcentagem(%).
    float volume = 100;

    uint64_t ultimo_acionamento = time_us_64() / 1000; // Armazena o tempo do último botão pressionado.

    while (true) {
        uint64_t tempo_atual = time_us_64() / 1000; // Tempo atual em ms

        // Caso seja 1 -> ele está solto / caso seja 0 -> ele está pressionado.
        if(gpio_get(BUTTON) == 0){
            sleep_ms(50); // Debounce
            // Espera até o botão ser solto para evitar repetições indesejadas
            while (gpio_get(BUTTON) == 0);
            volume-=7.5; // Considerando uma média de 15 L por dia de uma pessoa, considerando uma caixa de água de 2000 L.
            if (volume < 0) volume = 0; //Evitar valores negativos.
            liga_led(volume);
            // Atualiza o tempo do último acionamento
            ultimo_acionamento = tempo_atual;
            
            sleep_ms(300);
        }

        // Se passou mais que RESET_TIME sem pressionar o botão, reseta o volume
        if ((tempo_atual - ultimo_acionamento) >= RESET_TIME) {
            if (volume != 100) {  // Só reinicia se estiver abaixo de 100
                volume = 100;
                liga_led(volume);
            }
            ultimo_acionamento = tempo_atual; // Evita que o reset fique acontecendo em loop
        }

        sleep_ms(100);
    }
}