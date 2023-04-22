/* 
 * Archivo: Lab08.c
 * Dispositivo: PIC16F887
 * Autor: Carlos Julio Valdés Oajaca
 * Compilador: XC8, MPLABX v6.05
 
 * Programa: Enviar cadena de caracteres a la terminal
 * Hardware: 

 * Creado: 13 de abril, 2023
 * Última modificación: 13 de abril, 2023
 */

// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <pic16f887.h>
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

///////////////////////////////////DEFINIR CONSTANTES///////////////////////////////////////////////////////////////////////////////////
#define _XTAL_FREQ 8000000 //8MHz
////////////////////////////////////VARIABLES GLOBALES/////////////////////////////////////////////////////////////////////////////////
//uint8_t caracter;
//char txt[];
/////////////////////////////////////PROTOTIPOS DE FUNCIONES///////////////////////////////////////////////////////////////////////////
void setup(void);
void initUART(void);
void UART_Print(const char *cadena);
void UART_Put(const char cadena);
/////////////////////////////////////INTERRUPCIONES//////////////////////////////////////////////////////////////////////////////////
void __interrupt() isr(void)
{

}
///////////////////////////////////////CODIGO PRINCIPAL/////////////////////////////////////////////////////////////////////

void main(void) {
    setup(); //llamar a la configuracion
    initUART();
 ////LOOP PRINCIPAL/////   
    while(1) //Loop infinito
    {
     UART_Print("Hola mundo ;)\r\n");  // UART print
        __delay_ms(1000);  // esperar 1seg
    }
}
//////////////////////////////////////SETUP/////////////////////////////////////////////////////////////////////
void setup(void)
{
    //configuracion de pines y puertos    
    ANSEL= 0b00000000;  //
    ANSELH= 0; //configura los pines de portB como digital
    
    TRISB = 0; //configurar salidas
    TRISD = 0;

    //Inicializar los puertos en 0
    PORTB = 0;
    PORTD = 0;
    
    //configuracion oscilador
    OSCCONbits.IRCF = 0b0111; //8MHz
    OSCCONbits.SCS = 1;       //Utilizar el reloj interno
}
//////////////////////////////////////FUNCIONES/////////////////////////////////////////////////////////////////////
    void initUART(void)
    { //Asynchronous EUSART
    //Paso 1    
     SPBRG = 12; //calculo
     SPBRGH = 0; //% error= 0.16%
     BRGH = 0;
     BRG16 = 0;
    
    //Paso 2
     SYNC = 0; //Modo asincrono
     SPEN = 1; //Habilita modulo UART
     
    //Paso 3: 9 bits
     
    //Paso 4
     TXEN = 1; //habilitar transmision
     TXIF = 0; //apagar bandera TX
     
    }
///////////////////////////////////////////////////////////////////////////////////
void UART_Print(const char *cadena)
{
  uint8_t i = 0;
  while (cadena[i] != '\0')
    UART_Put (cadena[i++]);
}
////////////////////////////////////////////////////////////////////////////////////////
void UART_Put(const char cadena)
{
  while (TRMT == 0);  // esperar a que el bit de transmision este en 0
  TXREG = cadena;       // actualizar EUSART registro de transmision
}