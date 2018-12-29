#include "fix_fft.h"
#include "FastLED.h"

#define DATA_PIN 4 // Pino onde conectar a Fita de LED endereçavel
#define NUM_LEDS 26 // quantidade de LEDS na fita de LED
#define Led1 3 // LED vermelho da RGB
#define Led2 5 // LED verde da RGB
#define Led3 6 // LED azul da RGB
#define Led11 9 // LED vermelho da RGB
#define Led22 10 // LED verde da RGB
#define Led33 11 // LED azul da RGB
#define Relesom 2 // pino onde é conectado o Rele para ativar o som
#define filtro 5 // filtro de ruido do sinal
#define COLOR_SHIFT 4 // Delay para a velocidade da fita endereçavel

CRGB leds[NUM_LEDS]; // Array de que simboliza a fita de LEDS endereçavel 



unsigned int bass = 0, mid = 0, high = 0; // Niveis de baixo , médio e alto da RGB
unsigned int bass2 = 0, mid2 = 0, high2 = 0; // Niveis de baixo.... da endereçavel
unsigned int on = 0;
unsigned int wait = 0;
unsigned int lwait = 0;
unsigned int lwait2 = 0;
unsigned int Red = 95;
unsigned int Blue = 190;
unsigned int Green = 20;
unsigned int fadeAmount1 = 1;
unsigned int fadeAmount2 = 2;
unsigned int fadeAmount3 = 3;
unsigned long setTime = COLOR_SHIFT;
int i = 0, val;

void setup() {
  FastLED.setBrightness(254);
  pinMode(Relesom, OUTPUT);
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Led3, OUTPUT);
  pinMode(Led11, OUTPUT);
  pinMode(Led22, OUTPUT);
  pinMode(Led33, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(Relesom, HIGH);
  digitalWrite(10, LOW);
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS); // inicia a biblioteca da Fita de LED
  for (i = 0; i < NUM_LEDS ; i++) leds[i] = CRGB(0, 0, 0); // zera todos os LEDS da endereçavel
  FastLED.show(); // Joga o Array leds para a Fita
}



void loop() {
  // Leitura do som e calculos da ftt e pa
  char im[128], data[128];
  unsigned int freqOut[70] = {0};
  int min = 0, max = 1024;
  for (i = 0 ; i < 128; i++)
  {
    val = analogRead(A0);
    data[i] = val / 4 - 128;
    im[i] = 0;
    if (val > max) val = max;
    if (val < min) val = min;
  }
  fix_fft(data, im, 7, 0);
  for (i = 1 ; i < 64; i++)
  {
    int dat = sqrt(data[i] * data[i] + im[i] * im[i]);
    if ( dat > filtro)
    {
      freqOut[i] = dat; // Filtro para retirar ruidos
    }
  }
  // separação das Bandas baixas, médias e altas
  for (i = 0; i < 6; i++)
  {
    bass = bass + freqOut[i];
  }
  for (i = 7; i < 15; i++)
  {
    mid = mid + freqOut[i];
  }
  for (i = 16; i < 35; i++)
  {
    high = high + freqOut[i];
  }

  // detecta se existe música tocando ou não
  if ((high + bass + mid) < 10)
  {
    digitalWrite(Relesom, HIGH);
    wait++;
    if (wait > 15000) // se passar mto tempo sem movimento o som é desligado e o programa espera música
    {

      digitalWrite(Relesom, LOW);
      wait = 15002;
    }
  }
  else wait = 0;
  if (wait >= 2000 && wait <= 15000) // Troca de cor bem devagar esperando para a volta de movimento
  {
    lwait++;
    Red = Red + fadeAmount1;
    Blue = Blue + fadeAmount2;
    Green = Green + fadeAmount3;
    if (Red <= 5 || Red >= 250)  fadeAmount1 = -fadeAmount1;

    if (Blue <= 5 || Blue >= 250)  fadeAmount2 = -fadeAmount2;

    if (Green <= 5 || Green >= 250)  fadeAmount3 = -fadeAmount3;
    if (lwait < 100)
    {
      bass = (lwait * Red) / 100;
      high = (lwait * Blue) / 100;
      mid  = (lwait * Green) / 100;
    }
    else
    {
      bass = Red;
      high = Blue;
      mid  = Green;
    }
  }
  else
  {
    bass = bass;
    high = high;
    mid  = mid;
    lwait = 0;
  }
  // Filtro do baixo e talz para a fita LED endereçavel
  if(bass>1) bass2=bass;
  if(mid>25) mid2=mid;
  if(high>25) high=high;

  // Animação da Fita LED
  leds[11] = CRGB(high/2,bass2*2, mid2);
  leds[12] = CRGB(high/2,bass2*2, mid2);
  unsigned long time = millis();
  if (time / (double)setTime >= 1)
  {
    setTime = time + COLOR_SHIFT;
    for( i = 0; i <= 11; i++) leds[i] = leds[i+1];
    for( i = NUM_LEDS - 1; i >= 12; i--) leds[i] = leds[i - 1];
    FastLED.show();
  }

  // Escreve os niveis de baixo e talz na fita RGB
  analogWrite(Led33, high);
  analogWrite(Led11, bass*2);
  analogWrite(Led22, mid);
  analogWrite(Led3, high);
  analogWrite(Led1, bass*2);
  analogWrite(Led2, mid);

  // Zera várias as variaveis do sistema para evitar bugs
  bass = 0;
  mid  = 0;
  high = 0;
  bass2 = 0;
  mid2  = 0;
  high2 = 0;
  for (i = 0; i < 60; i++)
  {
    freqOut[i] = 0;
  }
  for (i = 0; i < 127; i++)
  {
    im[i] = 0;
  }
  for (i = 0; i < 127; i++)
  {
    data[i] = 0;
  }
}