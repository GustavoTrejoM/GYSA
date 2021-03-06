#include <16F887.h>
#fuses PROTECT, MCLR
#use delay(internal=4M)

#byte PORTA = 5// dedicado a la entrada y salida de se?ales
#byte PORTB = 6// dedicado al control de los motores
#byte PORTC = 7// dedicado tambien al control de los motores
#byte PORTD = 8// dedicado junto con el PORTA  a la recepci?n de se?ales (limit switch)
/////////////funciones///////////////////////
void cosechar_der();
void cosechar_izq();
void adelante_x();
//////////////entrada de se?ales/////////////
#define  cos_derecha PORTA,0 //se?al para cosechar del lado derecho
#define cos_izquierda PORTA,1// se?al para cosechar del lado izquierdo
#define avanzar_x PORTA,2//se?al para avanzar en x
///////////salida de se?ales///////////////
#define terminado PORTA,3// se?al de que hemos terminado la tarea
#define final_x PORTA,4 // se?al de que hemos llegado al final de camino

//////////////limites de carrera////////////////////
#define ls7 PORTA, 5//lado derecho
#define ls8 PORTA, 6
#define ls9 PORTA, 7
#define ls10 PORTD,0
#define ls11 PORTD,1//lado izquierdo
#define ls12 PORTD,2
#define ls13 PORTD,3
#define ls14 PORTD,4
////////////sensor hall y de fin de carrera x//////////////////////////
#define hall PORTD,5
#define final_carrera PORTD,6
////////////salidas de control a motores/////////
//Para mover el robot en x:
#define motor_x PORTC, 4
// Para cosehcar del lado derecho:
//Motor 5
#define paso_der  PORTB,0
#define dir_der   PORTB,1//0-afuera 1-adentro
#define en_der    PORTB,2
//Motor 6
#define paso_der1 PORTB,3
#define dir_der1  PORTB,4//0-cerrar 1-abrir
#define en_der1   PORTB,5
//Para cosechar del lado izquierdo:
//Motor 7
#define paso_izq  PORTB,6
#define dir_izq   PORTB,7//0-afuera 1-adentro
#define en_izq    PORTC,0
//Motor 8
#define paso_izq1 PORTC,1
#define dir_izq1  PORTC,2//0-cerrar 1-abrir
#define en_izq1   PORTC,3
///////////variables globales////////////
int velocidad=10; // velocidad de avance de los motores
int16 contador=0; // guardamos los pasos al cerra el brazo
void main(){
set_tris_a(0b11100111);
set_tris_b(0x00);
set_tris_c(0x00);
set_tris_d(0b11111111);
portb=0b00100100;
portc=0b00001001;
bit_clear(terminado);
bit_clear(final_x);
//****falta iniciar los enable de cada motor en 1******
while(true){
      if(bit_test(cos_derecha)){
         cosechar_der();// vamos a la funcion para cosechar a la derecha
      }
      if(bit_test(cos_izquierda)){
         cosechar_izq();// vamos a la funcion para cosechar a la izquierda
      }
      if(bit_test(avanzar_x)){
         adelante_x();// vamos a la funcion para avanzar en x
      }
   }
}

void cosechar_der(){
/*aqui cosechamos el fruto de la derecha, primero vamos sacando el brazo hasta que se cierra el limit switch que lo detecta o hasta que llegamos al final**,
despues cerramos el brazo hasta que detectemos que esta bien apretado, aqui damos 10 pasos mas para sujetarlo bien, luego
regresamos el brazo y  cuando se toque el limit switch de inicio de carrera abrimos el brazo la misma cantidad de pasos que lo abrimos
**cuando llegamos al final del brazo solo regresamos el brazo y marcamos como terminada la tarea */
   
   while(!bit_test(ls9) &&  !bit_test(ls8)){//movemos el motor 5 hacia afuera
      bit_clear(en_der);
      bit_clear(dir_der); 
      bit_set(paso_der);
      delay_ms(velocidad);
      bit_clear(paso_der);
      delay_ms(velocidad);
   }// cuando llegamos al fin de carrera o encontramos el tomate nos detenemos y ahora lo cortamos
   bit_set(en_der);
   
   if(bit_test(ls9)){// en caso de llegar al tomate lo cortamos cerrando el brazo
      contador=0;// iniciamos el contador en 0.
      while(!bit_test(ls10)){// cerramos hasta detectar que apretamos el tomate, contamos los pasos que dimos
         bit_clear(en_der1);
         bit_clear(dir_der1); 
         bit_set(paso_der1);
         delay_ms(velocidad);
         bit_clear(paso_der1);
         delay_ms(velocidad);
         contador++;// aqui vamos contando los pasos que di? el motor
      }
      //ahora debemos regresar al inicio de carrera...
      while(!bit_test(ls7)){
         bit_clear(en_der);
         bit_set(dir_der); 
         bit_set(paso_der);
         delay_ms(velocidad);
         bit_clear(paso_der);
         delay_ms(velocidad);  
      }bit_set(en_der);
      //ahora soltamos el tomate
      for(int16 i=0; i<contador;i++){
         bit_clear(en_der1);
         bit_set(dir_der1); 
         bit_set(paso_der1);
         delay_ms(velocidad);
         bit_clear(paso_der1);
         delay_ms(velocidad);
      }bit_set(en_der1);
      // ahora ya soltamos el tomate y podemos continuar con el c?digo y enviar la se?al de terminado
   }else if (bit_test(ls8)){// en caso de llegar al final de carrera sin tomate, solo regresamos al inicio de carrera
      while(!bit_test(ls7)){
         bit_clear(en_der);
         bit_set(dir_der); 
         bit_set(paso_der);
         delay_ms(velocidad);
         bit_clear(paso_der);
         delay_ms(velocidad);  
      }bit_set(en_der);
   }
   //ahora enviamos la se?al de terminado por 200 ms
   bit_set(terminado);
   delay_ms(200);
   bit_clear(terminado);
   // aqui vamos al main de nuevo
}

void cosechar_izq(){
/*aqui cosechamos el fruto de la derecha, primero vamos sacando el brazo hasta que se cierra el limit switch que lo detecta o hasta que llegamos al final**,
despues cerramos el brazo hasta que detectemos que esta bien apretado, aqui damos 10 pasos mas para sujetarlo bien, luego
regresamos el brazo y  cuando se toque el limit switch de inicio de carrera abrimos el brazo la misma cantidad de pasos que lo abrimos
**cuando llegamos al final del brazo solo regresamos el brazo y marcamos como terminada la tarea */
   
   while(!bit_test(ls13) &&  !bit_test(ls12)){//movemos el motor 7 hacia afuera
      bit_clear(en_izq);
      bit_clear(dir_izq); 
      bit_set(paso_izq);
      delay_ms(velocidad);
      bit_clear(paso_izq);
      delay_ms(velocidad);
   }// cuando llegamos al fin de carrera o encontramos el tomate nos detenemos y ahora lo cortamos
   bit_set(en_izq);
   
   if(bit_test(ls13)){// en caso de llegar al tomate lo cortamos cerrando el brazo
      contador=0;// iniciamos el contador en 0.
      while(!bit_test(ls14)){// cerramos hasta detectar que apretamos el tomate, contamos los pasos que dimos
         bit_clear(en_izq1);
         bit_clear(dir_izq1); 
         bit_set(paso_izq1);
         delay_ms(velocidad);
         bit_clear(paso_izq1);
         delay_ms(velocidad);
         contador++;// aqui vamos contando los pasos que di? el motor
      }
      //ahora debemos regresar al inicio de carrera...
      while(!bit_test(ls11)){
         bit_clear(en_izq);
         bit_set(dir_izq); 
         bit_set(paso_izq);
         delay_ms(velocidad);
         bit_clear(paso_izq);
         delay_ms(velocidad);  
      }bit_set(en_izq);
      //ahora soltamos el tomate
      for(int16 i=0; i<contador;i++){
         bit_clear(en_izq1);
         bit_set(dir_izq1); 
         bit_set(paso_izq1);
         delay_ms(velocidad);
         bit_clear(paso_izq1);
         delay_ms(velocidad);
      }bit_set(en_izq1);
      // ahora ya soltamos el tomate y podemos continuar con el c?digo y enviar la se?al de terminado
   }else if (bit_test(ls12)){// en caso de llegar al final de carrera sin tomate, solo regresamos al inicio de carrera
      while(!bit_test(ls11)){
         bit_clear(en_izq);
         bit_set(dir_izq); 
         bit_set(paso_izq);
         delay_ms(velocidad);
         bit_clear(paso_izq);
         delay_ms(velocidad);  
      }bit_set(en_izq);
   }
   //ahora enviamos la se?al de terminado por 200 ms
   bit_set(terminado);
   delay_ms(200);
   bit_clear(terminado);
   // aqui vamos al main de nuevo
}

void adelante_x(){
//aqui activamos el motor para mover el robot adelante, en este caso es un motor de CD
// si detectamos que ya no hay bancales de jitomates, hemos llegado al final
/*las llantas son de 4 1/2", entonces cada vuelta avanza aprox 36 cm, por lo que necesitamos media vuelta para
avanzar los 18 cm que necesitamos, por ello colocamos dos imanes en la llanta, para detectar cada media vuelta un pulso
 con el sensor hall*/
   while(bit_test(hall)){//esperamos a 'liberar' la se?al antigua del sensor hall, luego esperamos la nueva y apagamos
      bit_set(motor_x);
   }
   //luego de liberar la se?al antigua seguimos avanzando solo si no llegamos al final de carrera
   if(bit_test(final_carrera)){
      while(true){// se llegamos al final de x nos quedamos en un bucle infinito, hasta apagar el equipo
         bit_set(final_x);
         bit_clear(terminado);
         bit_clear(motor_x);
      }
   }else{
      while(!bit_test(hall)){// esperamos la nueva se?al del hall
         bit_set(motor_x);
      }
      bit_clear(motor_X);// luego de tener la se?al apagamos el motor y regresamos al main
         //ahora enviamos la se?al de terminado por 200 ms
         bit_set(terminado);
         delay_ms(200);
         bit_clear(terminado);
         // aqui vamos al main de nuevo
   }
}

