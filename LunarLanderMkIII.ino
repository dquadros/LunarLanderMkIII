#include <MAX7219Display.h>
#include <TM1637Display.h>

// Autor: dqsoft.blogspot@gmail.com
//
// Lunar Lander Mk3 - Simulação de pouso na Lua para Arduino
// https://garoa.net.br/wiki/Lunar_Lander_Mk_III


// Hardware

const uint8_t pinCLK_A = 4;
const uint8_t pinLOAD_A = 5;
const uint8_t pinDIN_A = 6;

MAX7219Display dispAlt(pinCLK_A, pinDIN_A, pinLOAD_A, 8);

const uint8_t pinCLK_V = 7;
const uint8_t pinLOAD_V = 8;
const uint8_t pinDIN_V = 9;

MAX7219Display dispVeloc(pinCLK_V, pinDIN_V, pinLOAD_V, 8);

const uint8_t pinCLK_C = 2;
const uint8_t pinDIO_C = 3;

TM1637Display dispComb(pinCLK_C, pinDIO_C);

const int Dispara = 10;
const int Reset = 11;
const int Buzzer = 12;
const int Led = 13;
const int Queima = A0;

// Variáveis da simulação

static const int grav = 5;         // acelaração da gravidade

static long altitude;              // altitude atual
static long comb;                  // combustível disponível
static long veloc;                 // velocidade atual
static int burn;                   // taxa de queima do combustível

// Valores iniciais para a simulação
static const long altInicial = 5000000L; 
static const long combInicial = 19000L;
static const long velocInicial = 8500L;

// Acender o LED de combustível baixo quando ficar abaixo deste valor
static const long combAlerta = 500L;


// Iniciação
void setup()
{
  pinMode (Buzzer, OUTPUT);
  pinMode (Led, OUTPUT);
  pinMode (Dispara, INPUT);
  pinMode (Reset, INPUT);
  digitalWrite (Dispara, HIGH);
  digitalWrite (Reset, HIGH);
  dispComb.setBrightness(0xF);
  Restart();
}

// Laço principal do jogo
void loop()
{
  // Reiniciar se botão Reset apertado
  if (!digitalRead (Reset))
    Restart();
    
  // Apresentar a situação atual
  AtlDisplay ();
  
  // Tratar queima de combustível
  if (digitalRead (Dispara))
    burn = 0;
  else
    burn = analogRead (Queima) >> 5;
    
  // Executar a simulação
  Simula ();
  
  // Tempo entre os passos da simulação
  delay (100);
}

// Reinicia a simulação
void Restart()
{
  digitalWrite (Buzzer, LOW);
  digitalWrite (Led, LOW);
  altitude = altInicial;
  comb = combInicial;
  veloc = velocInicial;
  burn = 0;
  dispComb.showNumberDec (0);
  dispAlt.showNumberDec (0);
  dispVeloc.showNumberDec (0);
  while (!digitalRead(Reset))
    ;
}

// Atualiza o display
static void AtlDisplay (void)
{
  dispComb.showNumberDec (comb >> 1);
  dispAlt.showNumberDec (altitude);
  dispVeloc.showNumberDec (veloc);
  if (comb < combAlerta)
    digitalWrite (Led, HIGH); 
  if ((altitude == 0) && (veloc > 99))
     digitalWrite (Buzzer, HIGH);
}

// Simula o movimento
static void Simula (void)
{
  signed int newv;
  
  if (altitude > 0)
  {
     if (burn)
     {
        if (burn > comb)
           burn = comb;
        comb -= burn;
        newv = veloc - burn + grav;
        burn = 0;
     }
     else
        newv = veloc + grav;
     
     altitude -= (veloc + newv) >> 1;
     if (altitude < 0)
        altitude = 0;
     veloc = newv;
  }
}

