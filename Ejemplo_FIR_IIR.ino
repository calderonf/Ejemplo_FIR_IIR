/*
    Instrucciones de uso:
      1. Cambie MEDIRTIEMPOS a 1 o 0 para sacar en 2 columna el tiempo de funcion en microsegundos
      2. Cambie entrada a {PASO,IMPULSO,COSENO,PERSONAL, ACUMULAR} para simular una señal de entrada notese que en PERSONAL Y ACUMULAR hay mas opciones si comenta el condicional de mas abajo en funcion LOOP
      3. Cambie salida a {FIR1,FIR2,FIR3, IIR} para comparar diferentes implementaciones de funciones
      4. Intervalo define el numero de muestras en las que se realiza un cambio, útil para las entradas impulso, paso y coseno
      5. Abra en: ->Herramientas -> monitor serie. o ->Herramientas -> "Serial Plotter"
      6. Las 3 implementaciones de filtros FIR deben tener la misma salida ya que son equivalentes, si no son equivalentes verifique que los tipos de variables dean correctos igual con los castings
      7. Note que al final de las funciones de FIR# hay una division por 256 en forma de corrimiento por 8 (>>8) esto funciona para los filtros diseñados y 
          dados como ejemplo notese que el impulso que entra a los FIR y el paso es de 0 a 255 de esta forma al dividir salen los coeficientes ya normalizados, Diseñe en sus filtros esta normalización
          Calcule o experimente usted mismo la normalización necesaria en los filtros que usted diseñe.
*/
/*
Primera parte del taller/parcial:
para todos los filtros Fs=100Hz.

Diseñar un filtro FIR paso bajo de orden dado por la aplicación anexa, con Fs=100, y corte en (3 dB) a 10Hz. OJO es 3 dB no 6 dB
Diseñar un filtro FIR paso altos de orden dado por la aplicación anexa, con Fs=100, y corte en (3 dB) a 20 Hz. OJO es a 3 dB
Diseñar un filtro IIR paso bajos de segundo orden cuya frecuencia de corte este dada por la aplicación anexa y a esta frecuencia debe tener 3 dB de atenuación, Fs=100.

Para los dos filtros FIR:
a)  Seleccionar la menor cuantización o tamaño de palabra de numerador, de tal manera que el error de cuantización 
    se encuentre siempre por debajo de 60 dB para todas las frecuencias (round-off noise power spectrum)
b)  Seleccionar 10 bits de tamaño de palabra de entrada.
c)  Usar el método de ventanas. experimente con varias, 

Para el IIR:
a)  No olvidar convertir en una sola sección "Edit-> Conver to single section"

Para los 3 filtros:

a) Comparar la respuesta impulso obtenida de esta aplicación con la diseñada en matlab.
b) Comparar la respuesta paso obtenida de esta aplicación con la diseñada en matlab.
c) Usar la señal de COSENO generada por esta herramienta para graficar la salida y entrada a 5 frecuencias: (dos antes de los 3 dB, 1 en el punto, o cercana a 3 dB y 2 después del corte a 3 dB) esto con el objetivo de hallar 5 puntos que puedan ser graficados.
c) Documentar y analizar en su entrega las graficas de group delay, Pole Zero plot, y la estructura del filtro "en la ayuda ver show filter structure" 
*/

#ifdef __cplusplus
extern "C" {
#include "funciones.h"
#include "generador.h"
} // extern "C"
#endif

/*Definiciones del precompilador*/
#define MEDIRTIEMPOS 0//Cambiar a 1 y abrir el monitor serial "de la grafica no se aprecia mucho ya que la columna del centro está en microsegundos"
#define MOSTRARENTRADA 0// cambiar a 1 para mostrar la entrada
#define PERIODOSENAL 100// numero de muestras antes de resetear la respuesta paso e impulso, subir en sistemas lentos

/*Variables globales de configuración*/
#define ENTRADA IMPULSO // Seleccione IMPULSO, PASO, COSENO, PERSONAL, ACUMULAR ver generador.h
//Si selecciona FIR# y IMPULSO o PASO use ENTERA8BITS en la funcion inicializarGenerador para suponer  una entrada con punto fijo a la izquierda del bit 8
//Si selecciona IIR  y IMPULSO o PASO  use UNITARIA en la funcion inicializarGenerador
#define FRECUENCIA 1 // se usa si se selecciona entrada COSENO puede ser un numero de 1 a 50
#define SALIDA IIR // Seleccione FIR1,FIR2,FIR3, o IIR ver funciones.h
const long interval = 10;           // Intervalo a medir periodico en milisegundos 100Hz=10ms esto asegura una simulación de un tiempo de muestreo


/*****Variables globales del filtro IIR*****/
coef_iir_2_ord ir;/*esta estructura debe ser global y se va a pasar por referencia a la funcion que la inicializa*/


/*****Variables globales del generador de señal sintetica*****/
generador generar;

//Entrada Ruido Gausiano: media de 512, y un sigma de 512/3, truncado a un numero de 10 bits
const int tam = 1000;
const int ruido[tam] PROGMEM = {640, 713, 332, 559, 242, 642, 385, 639, 875, 488, 649, 770, 454, 284, 523, 731, 640, 677, 967, 764, 593, 451, 821, 448, 682, 227, 400, 576, 554, 699, 105, 275, 531, 722, 514, 291, 118, -55, 26, 620, 468, 522, 538, 256, 410, 402, 441, 552, 420, 256, 681, 365, 464, 475, 384, 381, 570, 253, 519, 443, 747, 460, 205, 389, 794, 552, 574, 724, 545, 363, 420, 505, 362, 658, 602, 293, 321, 593, 260, 448, 276, 709, 339, 457, 398, 495,
                                517, 540, 476, 626, 403, 660, 354, 411, 677, 715, 346, 449, 592, 619, 181, 352, 823, 284, 570, 441, 447, 628, 668, 362, 415, 483, 570, 555, 683, 294, 499, 515, 502, 600, 427, 321, 298, 717, 250, 543, 701, 716, 287, 485, 697, 280, 659, 433, 546, 728, 618, 587, 737, 603, 667, 648, 577, 339, 585, 341, 457, 785, 618, 368, 465, 402, 405, 762, 435, 277, 294, 474, 445, 499, 679, 426, 679, 606, 705, 158, 355, 496, 569, 437, 490, 476, 207, 328, 674, 361, 495, 633, 506, 307, 202, 724, 748, 649, 262, 392, 699, 509, 588, 497, 667, 567, 220, 581, 743, 514, 350, 637, 573, 207, 791, 257, 342, 923, 474, 382, 397, 613, 712, 675, 290, 549, 555, 807, 251, 163, 513, 509, 530, 494, 260, 597, 489, 431, 254, 302, 536, 429, 507, 617, 657, 526, 348, 369, 200, 557, 219, 498, 842, 146, 369, 603, 260, 540, 517, 652, 363, 824, 526, 759, 583, 494, 374, 352, 548, 599, 440, 349, 405, 433, 395, 477, 537, 886, 357, 460, 180, 539, 103, 472, 831, 521, 410, 581, 470, 662, 562, 361, 370, 554, 222, 315, 433, 532, 385, 441, 543, 589, 582, 415, 407, 401, 645, 592, 727, 454, 718, 642, 541, 655, 452, 393, 573, 540, 300, 693, 255, 537, 254, 392, 388, 131, 735, 375, 179, 678, 309, 649, 415, 629, 339, 620, 634, 693, 819, 683, 717, 450, 464, 505, 503, 354, 478, 638, 643, 639, 552, 522, 768, 648, 576, 671, 769, 864, 529, 446, 409, 547, 137, 520, 522, 506, 821, 925, 818, 486, 285, 684, 533, 362, 604, 834, 440, 554, 774, 523, 537, 388, 701, 459, 704, 810, 593, 471, 495, 559, 315, 317, 692, 593, -62, 449, 581, 394, 564, 629, 476, 530, 228, 461, 774, 582, 614, 663, 620, 683, 479, 462, 498, 642, 616, 354, 455, 415, 464, 453, 452, 489, 231, 385, 586, 593, 302, 761, 604, 582, 182, 563, 258, 333, 229, 251, 201, 533, 461, 739, 546, 617, 825, 315, 406, 503, 617, 576, 588, 354, 589, 334, 644, 561, 355, 627, 553, 153, 458, 801, 454, 268, 720, 470, 306, 643, 421, 749, 605, 548, 856, 742, 858, 338, 255, 588, 723, 630, 204, 448, 344, 430, 527, 679, 635, 912, 562, 715, 250, 400, 375, 697, 849, 831, 829, 580, -28, 354, 476, 899, 548, 779, 800, 599, 381, 550, 322, 440, 600, 527, 169, 514, 432, 247, 519, 465, 678, 368, 343, 437, 288, 433, 290, 766, 440, 627, 489, 440, 659, 604, 796, 673, 729, 559, 574, 597, 728, 598, 551, 738, 435, 468, 409, 543, 217, 353, 227, 702, 818, 646, 794, 742, 421, 661, 817, 572, 391, 408, 592, 447, 583, 500, 319, 535, 598, 194, 618, 514, 432, 616, 617, 486, 635, 812, 390, 442, 429, 388, 117, 501, 661, 496, 711, 478, 637, 648, 458, 696, 508, 384, 407, 596, 396, 641, 350, 386, 689, 528, 395, 337, 693, 299, 906, 188, 431, 709, 690, 380, 646, 286, 321, 414, 603, 790, 539, 273, 621, 501, 424, 501, 448, 444, 582, 484, 403, 569, 521, 422, 647, 478, 230, 507,
                                542, 431, 362, 742, 106, 520, 265, 598, 175, 736, 375, 631, 549, 141, 583, 435, 671, 705, 569, 600, 437, 554, 237, 691, 478, 455, 385, 880, 752, -10, 582, 403, 258, 452, 609, 699, 568, 417, 436, 674, 564, 491, 357, 671, 198, 502, 306, 527, 414, 425, 328, 241, 717, 470, 572, 519, 488, 731, 343, 736, 678, 298, 550, 338, 351, 582, 709, 410, 468, 292, 226, 391, 933, 558, 280, 344, 716, 631, 585, 383, 800, 645, 674, 675, 685, 584, 531, 512, 427, 444, 542, 553, 343, 508, 373, 451, 411, 618, 601, 398, 262, 416, 455, 222, 897, 479, 166, 259, 463, 675, 446, 625, 316, 702, 424, 293, 428, 346, 442, 412, 175, 297, 589, -82, 501, 572, 557, 590, 903, 795, 435, 600, 424, 724, 474, 528, 452, 808, 286, 307, 530, 580, 423, 600, 585, 366, 544, 330, 496, 636, 291, 801, 583, 513, 525, 554, 603, 430, 248, 503, 388, 600, 678, 536, 439, 524, 621, 609, 451, 429, 563, 411, 546, 515, 658, 769, 510, 428, 474, 858, 539, 522, 164, 564, 358, 312, 731, 250, 389, 464, 432, 188, 505, 592, 290, 301, 563, 388, 274, 586, 789, 453, 490, 464, 517, 516, 696, 741, 630, 564, 748, 368, 710, 442, 334, 562, 880, 692, 405, 224, 500, 342, 541, 711, 725, 500, 857, 606, 525, 768, 227, 969, 649, 341, 499, 558, 689, 614, 453, 494, 49, 891, 677, 573, 452, 358, 506, 346, 421, 571, 715, 864, 526, 249, 596, 596, 443, 248, 468, 581, 564, 601, 491, 710, 296, 370, 593, 738, 697, 540, 627, 462, 614, 452, 465, 263, 628, 445, 407, 438, 592, 272, 759, 620, 563, 380, 581, 751, 611, 524, 551, 389, 809, 824, 330, 123, 309, 299, 397, 594, 216, 463, 583, 278, 733, 487, 466, 952, 514, 682, 442, 320, 531, 641, 535, 557, 445, 442, 406, 346, 274, 420, 497, 444, 669, 295, 418, 543, 464, 273, 367, 707, 358, 487, 512, 462, 271, 623, 783, 537, 744, 922, 133, 416, 924, 529, 131, 446, 509, 813, 274, 589, 296, 560, 496, 766, 829, 283, 792, 645, 402, 582, 344, 545, 623, 313, 837, 304, 474, 609, 495, 553, 584, 503, 468, 591, 483, 554, 382, 570, 278, 412
                               }; 

/*Variables especificas del ejemplo:*/
volatile int walk;
volatile int cont;
unsigned long previousMillis;
unsigned long ts1, ts2;


void setup() {
  cont = 0;
  walk = 0;
  previousMillis = 0;
  pinMode(LED_BUILTIN, OUTPUT); //LED_BUILTIN

  Serial.begin(1000000);/*velocidad maxima para que no interfiera en tiempos*/
  while (!Serial) {
    ; // Esperar a que el puerto inicie
  }
  previousMillis = 0;
  /*el filtro que se pone acá es un Butter con Fs en 100 y corte en 5Hz @ 3dB*/
  float num[3] = {0.02008336596190929412841796875,0.0401667319238185882568359375,0.02008336596190929412841796875}; // ponga aquí su numerador, por defecto para ejemplo un oscilador sería: {1.0, -2.0, 1.0}
  float den[3] = {1,-1.56101810932159423828125,0.641351521015167236328125}; // ponga aquí su denominador, por defecto para ejemplo                       {1,   0, -1}
  float w[3] = {0, 0, 0};// condición inicial punto intermedio,  por defecto para ejemplo                   {0, 0, 0}
  inicializar_iir_2_ord(num, den, w, &ir);
  inicializarGenerador(&generar, ENTRADA, UNITARIA, PERIODOSENAL,FRECUENCIA); /*si es iir use UNITARIA si es FIR use ENTERA8BITS para usar paso o impulso de 0 a 255*/
  if (MOSTRARENTRADA)
  {
    Serial.print("Entrada");
    Serial.print(" ");
  }
  if (MEDIRTIEMPOS)
  {
    Serial.print("Tiempo_us");
    Serial.print(" ");
  }
  Serial.println("Salida");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    long sal;//para FIR
    float saliir;// para IIR

    if (generar.entrada&(PERSONAL+ACUMULAR)){ // Ejercicio: averiguar como funciona este condicional? ver en generador.h
      /*Descomente uno de estos a la vez para obtener una de estas respuestas: */
      //SenalPersonalizada(&generar,pgm_read_word_near(&ruido[random(0,1000)]));// lee al azar un lugar en la tabla de numeros al azar distribuidos gausiannos "distribución normal"
      //SenalPersonalizada(&generar,5);// prenda ACUMULAR para hallar una respuesta rampa
      SenalPersonalizada(&generar,random(-3,4));// prenda ACUMULAR para hacer una caminata aleatoria apague para numero aleatorio entre -3 y 3
    }
    walk = generadorSenal(&generar);
    if (MOSTRARENTRADA)
    {
      Serial.print(walk);
      Serial.print(" ");
    }
    switch (SALIDA)
    {
      case FIR1:
        ts1 = micros();
        sal = filtrarFIR1(walk);
        ts2 = micros();
        if (MEDIRTIEMPOS) {
          Serial.print(ts2 - ts1); //timepo en microsegundos
          Serial.print(" ");
        }
        Serial.print(sal);
        Serial.print(" ");
        break;
      case FIR2:
        ts1 = micros();
        sal = filtrarFIR2(walk);
        ts2 = micros();
        if (MEDIRTIEMPOS) {
          Serial.print(ts2 - ts1); //timepo en microsegundos
          Serial.print(" ");
        }
        Serial.print(sal);
        Serial.print(" ");
        break;
      case FIR3:
        ts1 = micros();
        sal = filtrarFIR3(walk);
        ts2 = micros();
        if (MEDIRTIEMPOS) {
          Serial.print(ts2 - ts1); //timepo en microsegundos
          Serial.print(" ");
        }
        Serial.print(sal);
        Serial.print(" ");
        break;
      case IIR:
        ts1 = micros();
        saliir = filtrarIIR((float)walk, &ir);
        ts2 = micros();
        if (MEDIRTIEMPOS) {
          Serial.print(ts2 - ts1); //timepo en microsegundos
          Serial.print(" ");
        }
        Serial.print(saliir, 4); //pinta 4 decimales
        Serial.print(" ");
        break;
    }
    Serial.println("");
  }
}
/***
        ___   ___   _  _
       | __| |_ _| | \| |
       | _|   | |  | .` |
       |_|   |___| |_|\_|

*/
