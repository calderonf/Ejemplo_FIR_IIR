
#ifndef GENERADOR_H
#define GENERADOR_H


/*Definiciones del ejemplo*/
#define IMPULSO 1
#define PASO 2
#define COSENO 4
#define PERSONAL 8
#define ACUMULAR 16

#define ENTERA8BITS 0 // norma definida a 8 bits, salida de 0 a 256
#define UNITARIA 4 // norma punto flotante, salida de 0.0f a 1.0f

#define PERIODOTEST 100

typedef struct generador {// este va a ser el nombre de la estructura
  char entrada;
  char salida;
  char frecuencia;// de 1 a 50
  int cont;
  int periodo;
  int personal;
  char error;
  
} generador;// este va a ser el nombre del typedef

int generadorSenal(generador* gen);
void inicializarGenerador(generador* gen, char entrada, char salida,int periodo,char frecuencia);
void SenalPersonalizada(generador* gen,int personal);

#endif


/***
 *                 _      <-. (`-')_ 
 *       <-.      (_)        \( OO) )
 *    (`-')-----. ,-(`-') ,--./ ,--/ 
 *    (OO|(_\---' | ( OO) |   \ |  | 
 *     / |  '--.  |  |  ) |  . '|  |)
 *     \_)  .--' (|  |_/  |  |\    | 
 *      `|  |_)   |  |'-> |  | \   | 
 *       `--'     `--'    `--'  `--' 
 */
