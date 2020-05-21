#include "generador.h"
/*una tabla así lo ideal seria leerla y gardarla en memoria de programa usando const PROGMEM*/
int coseno[100] = {
  1023, 1021, 1015, 1005, 991,  973,  951,  926,    896,    864,    828,    788,    746,    700,    652,  601,  548,  493,  436,  377,
  316,  254,  192,  128,  64,   0,    -64,  -128,   -192,   -254,   -316,   -377,   -436,   -493,   -548, -601, -652, -700, -746, -788,
  -828, -864, -896, -926, -951, -973, -991, -1005,  -1015,  -1021,  -1023,  -1021,  -1015,  -1005,  -991, -973, -951, -926, -896, -864,
  -828, -788, -746, -700, -652, -601, -548, -493,   -436,   -377,   -316,   -254,   -192,   -128,   -64,  -0,   64,   128,  192,  254,
  316,  377,  436,  493,  548,  601,  652,  700,    746,    788,    828,    864,    896,    926,    951,  973,  991,  1005, 1015, 1021
};
/*
  Esta es una tabla generada así:
  F=1;lo mismo que decir k=1
  Fs=100; lo mismo que decir N=100 o periodo =100
  n=0:99;
  y=cos(2*pi*F*n/Fs)
  por lo que puede aumentarse el F desde 1 a 50 tecnicamente soporta F=0, y saca un DC de 1023
*/

void SenalPersonalizada(generador* gen,int personal)
{
  gen->personal= (gen->entrada==ACUMULAR)? gen->personal+personal : personal;
}

int generadorSenal(generador* gen)
{
  int walk = -1;
  switch (gen->entrada)
  {
    case PASO:
      if (gen->cont > gen->periodo)
      {
        gen->cont = 0;
        if (walk)
          walk = 0;
        else if (gen->salida == UNITARIA)
          walk = 1; // para el filtro IIR la respuesta impulso es de una unidad pr eso es SINNORMALIZAR
        else
          walk = 256; // para el filtro FIR la respuesta impulso es de 256 ya que esta escalizada, mire al final de las funciones FIR# que se hace un corrimiento de 8 bits 2^8-1=256
      }
      gen->cont++;
      break;
    case PERSONAL:
    case ACUMULAR:
      walk =gen->personal;
      break;
    case IMPULSO:
      if (gen->cont > gen->periodo)
      {
        gen->cont = 0;
        if (gen->salida == UNITARIA)
          walk = 1; // para el filtro IIR la respuesta paso es de una unidad
        else
          walk = 256; //para el filtro FIR la respuesta paso es de 256 ya que esta escalizada, mire al final de las funciones FIR# que se hace un corrimiento de 8 bits 2^8-1=256
      }
      else {
        walk = 0;
      }
      gen->cont++;
      break;
    case COSENO:
      walk = coseno[gen->cont];
      gen->cont += gen->frecuencia;
      gen->cont = (gen->cont >= 100) ? 0 : gen->cont ;
    //se lee: si (gen->cont>=100) haga gen->cont igual a 0 de lo contrario hagalo igual a gen->cont
    //salida = (condicion) ? (si si):(si no);
    default:
      gen->error = 1;
  }
  return walk;
}

void inicializarGenerador(generador* gen, char entrada, char salida, int periodo, char frecuencia)
{
  gen->entrada = entrada;
  gen->salida = salida;
  gen->periodo = periodo;
  gen->personal=-1;
  gen->frecuencia = frecuencia;
  gen->cont = 0;
  gen->error = 0;
}
