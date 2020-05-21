#include "funciones.h"

//#define BL 8
//const char B[BL] = {
//      0,   16,   48,   64,   64,   48,   16,    0
//};

#define BL 41
const char B[BL] = {
      0,    0,    0,   -1,   -1,   -1,   -1,   -2,   -2,   -1,    0,    2,
      4,    7,   10,   14,   18,   21,   23,   25,   25,   25,   23,   21,
     18,   14,   10,    7,    4,    2,    0,   -1,   -2,   -2,   -1,   -1,
     -1,   -1,    0,    0,    0
};

//#define BL 12
//const char  B[BL] = {
//     -8,   -8,    8,   32,   48,   56,   56,   48,   32,    8,   -8,   -8
//};

//#define BL 18
//const char B[BL] = {
//      0,   -8,  -16,   -8,    0,   16,   32,   48,   64,   64,   48,   32,
//     16,    0,   -8,  -16,   -8,    0
//};

//#define BL 31
//const char  B[BL] = {
//  0,    0,    0,    8,    8,    0,    0,   -8,   -8,   -8,    0,    8,
//  24,   40,   48,   56,   48,   40,   24,    8,    0,   -8,   -8,   -8,
//  0,    0,    8,    8,    0,    0,    0
//};

//#define BL 40
//const char  B[BL]={1,-1,1,0,0,1,-1,2,-2,1,0,-1,3,-5,5,-4,2,4,-16,64,64,-16,4,2,-4,5,-5,3,-1,0,1,-2,2,-1,1,0,0,1,-1,1};

/*Inicio de variables volatiles para pasar datos entre funciones*/
volatile int x[BL];
volatile int k=0;

long filtrarFIR1(int in)
{
  int i = 1;
  x[k] = (int)in;
  long y = 0;
  for (i = 1; i <= BL; i++) // NOTA, DEBE INICIAR EN 1. EJERCICIO: haga una prueba de escritorio con una respuesta impulso y compruebe...
  {
    y += (long)B[i - 1] * (long)x[(i + k) % BL]; // verifique que para su filtro no exista overflow.
  }

  k = (k + 1) % BL;
  return y>>8 ; //si no es multiplo de 2^n divida por el factor de normalización adecuado a su filtro.
}

long filtrarFIR2(int in)
{
  int i = 0;
  x[k] = in;
  int inx = k;
  long y = 0;
  for (i = 0; i < BL; ++i) {
    y += (long)x[inx] * (long)B[i];// verifique que para su filtro no exista overflow.
    inx = inx != 0 ? inx - 1 : BL - 1;
  }
  k++;
  k = (k >= BL) ? 0 : k;
  return y >> 8; //si no es multiplo de 2^n divida por el factor de normalización adecuado a su filtro.

}

long filtrarFIR3(int in)
{
  int i = 0;
  x[k] = in;
  int inx = k;
  char *apuntadorcoef = &B[0];
  int *apuntadorarrc = &x[inx];
  // mucho cuidado con el tamaño de los apuntadores DEBE COINCIDIR CON EL DEL ARREGLO o no va a funcionar.
  
  long y = 0;
  for (i = 0; i < BL; ++i) {
    y += (long)(*apuntadorarrc) * (long)(*apuntadorcoef);// verifique que para su filtro no exista overflow.
    apuntadorcoef++;
    if (inx != 0) {
      apuntadorarrc--;
      inx--;
    }
    else {
      apuntadorarrc = &x[BL - 1];
      inx = BL - 1;
    }
  }
  k++;
  k = (k >= BL) ? 0 : k;
  return y>>8 ; //si no es multiplo de 2^n divida por el factor de normalización adecuado a su filtro.
}


/*Funciones del filtro IIR*/

void inicializar_iir_2_ord(float*num, float*den, float*w, coef_iir_2_ord* ir)
{
  for (int i = 0; i < 3; i++) {
    ir->num[i] = num[i];
    ir->den[i] = den[i];
    ir->w[i] = w[i];
  }
}

/*ver la ayuda de matlab de dfilt.df2 para ver la estructura implementada en este filtro*/
float filtrarIIR(float in, coef_iir_2_ord* ir) {
  float y;
  /*Ecuacion implementada:
  w0=a0*x-a1*w1-a2*w2 donde x es la entrada  a denota a los denominadores notese que a0 es siempre 1 por lo que da lo mismo quitarlo
  y =b0*w0+b1*w1+b2*w2
  w2=w1
  w1=w0
  */
  ir->w[0] = (ir->den[0] * in) - (ir->den[1] * ir->w[1]) - (ir->den[2] * ir->w[2]); // OJO QUE EL MENOS YA ESTA EN LA ECUACION ver en  la ayuda de filterDesign en "show filter structure" si esta es o no la ecuación que implementan en ese software
  y = ((ir->num[0] * ir->w[0]) + (ir->num[1] * ir->w[1]) + (ir->num[2] * ir->w[2]));
  ir->w[2] = ir->w[1];//debe ser primero este
  ir->w[1] = ir->w[0];// y luego este para no sobreescribir
  return y;
}


/***
                   _      <-. (`-')_
         <-.      (_)        \( OO) )
      (`-')-----. ,-(`-') ,--./ ,--/
      (OO|(_\---' | ( OO) |   \ |  |
       / |  '--.  |  |  ) |  . '|  |)
       \_)  .--' (|  |_/  |  |\    |
        `|  |_)   |  |'-> |  | \   |
         `--'     `--'    `--'  `--'
*/
