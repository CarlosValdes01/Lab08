/* 
 * Archivo: PostLab08.c
 * Dispositivo: PIC16F887
 * Autor: Carlos Julio Valdés Oajaca
 * Compilador: XC8, MPLABX v6.05
 
 * Programa: Menu en la terminal donde se lee POT o enviar ASCII
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
#include <string.h>

///////////////////////////////////DEFINIR CONSTANTES///////////////////////////////////////////////////////////////////////////////////
#define _XTAL_FREQ 1000000 //1MHz
////////////////////////////////////VARIABLES GLOBALES/////////////////////////////////////////////////////////////////////////////////
uint8_t flag;
uint8_t POT;
char sel;
char RX;
char POT_char [];
/////////////////////////////////////PROTOTIPOS DE FUNCIONES///////////////////////////////////////////////////////////////////////////
void setup(void);
void initUART(void);
void UART_Print(unsigned char *cadena);
void UART_Disp(const char sel);
/////////////////////////////////////INTERRUPCIONES//////////////////////////////////////////////////////////////////////////////////
void __interrupt() isr(void)
{
    if(PIR1bits.RCIF)            //Datos recibidos?
    {    
        RX =RCREG;
    }
    if (PIR1bits.ADIF)           //Interrupcion del ADC
    {
        if (ADCON0bits.CHS == 0) //Seleccion del canal
        {
            POT = ADRESH;
        }
        PIR1bits.ADIF = 0;       //limpiar bandera
        PIR1bits.RCIF = 0;
    }
    
}
///////////////////////////////////////CODIGO PRINCIPAL/////////////////////////////////////////////////////////////////////

void main(void) {
    setup();    //llamar a la configuracion
    initUART(); //llamar inicializacion del UART
    
 ////LOOP PRINCIPAL///// 
    
    while(1)//Loop infinito
    { 
        if (ADCON0bits.GO == 0)  //Verificar si ya inicio la conversion
        {
            ADCON0bits.GO = 1;  //iniciar conversion
        }
    
     UART_Print("\r ---------------------------------------------- \r");  // UART print
     UART_Print("\r Sea bienvenido, que accion desea realizar? \r\n");
     UART_Print(" 1) Leer Potenciometro \r\n");
     UART_Print(" 2) Enviar ASCII \r\n");
     UART_Print("\r ----------------------------------------------  \r");
     flag = 1;
     
     while(flag)
        {
         while(PIR1bits.RCIF == 0); //Esperar a recibir dato
         
           sel = RX;
           
           switch(sel){ 
            case('1'):
                UART_Print("\r El valor del potenciometro es: \r\n");
                itoa(POT_char,POT,10);
                UART_Disp(POT_char);
                UART_Print("\r Listo \r\n");   
                flag = 0;
                break;
                
            case('2'):
                UART_Print("\r Ingrese un caracter de ASCII para mostrar en el puerto B:  \r");
                while(PIR1bits.RCIF == 0); //Esperar a recibir dato
                PORTB = RX;                //Mover dato al puerto B
                UART_Disp(RX);             //Mostrar el caracter en la pantalla
                UART_Print("\r Listo \r\n");   
                flag = 0;
                break;
            
            default:    
                UART_Print(" Error \r\n");
           }
        }
    }
}
//////////////////////////////////////SETUP/////////////////////////////////////////////////////////////////////
void setup(void)
{
    //configuracion de pines y puertos    
    ANSEL= 0b00000001;  //AN0 como entrada analogica
    ANSELH= 0; //configura los pines de portB como digital
    
    TRISB = 0; //configurar salidas

    //Inicializar los puertos en 0
    PORTA = 0;
    PORTB = 0;
    
    //configuracion oscilador
    OSCCONbits.IRCF = 0b0100; //1MHz
    OSCCONbits.SCS = 1;       //Utilizar el reloj interno
    
    //Configuracion de ADC
    ADCON1bits.ADFM = 0;   //Justificado a la izquierda
    ADCON1bits.VCFG0 = 0;  //Referencia en VDD
    ADCON1bits.VCFG1 = 0;  //Referencia en VSS
    
    ADCON0bits.ADCS = 0b01;   //ADCS <1:0> -> 01 Fosc/8
    ADCON0bits.CHS = 0;       //Seleccion de canal AN0
    ADCON0bits.ADON = 1;      //Encender ADC
    __delay_us(50);
    
    //Configuracion de Interrupciones
    INTCONbits.GIE = 1;   //Habilitar interrupciones globales
    INTCONbits.PEIE = 1;  //Habilitar interrupciones perifericas
    
    PIE1bits.ADIE = 1;   //Habilitar interrupcion del ADC
    PIE1bits.RCIE = 1;   //Habilitar interrupcion de Recepcion
    
    PIR1bits.ADIF = 0;   //Flag del ADC en 0
}
//////////////////////////////////////FUNCIONES/////////////////////////////////////////////////////////////////////
    void initUART(void)
    { //Configuracion de la comunicacion serial
    //Paso 1    
     SPBRG = 25;            //Calculo para aprox. 9600 Baud
     SPBRGH = 0;            //% error= 0.16%
     TXSTAbits.BRGH = 1;    //Baud rate de alta velocidad 
     BAUDCTLbits.BRG16 = 1; //Se utilizan 16 bits para Baud rate
    
    //Paso 2
     TXSTAbits.SYNC = 0;  //Comunicacion asincrona (full-duplex)
     RCSTAbits.SPEN = 1;  //Habilita comunicacion (modulo UART)
     
    //Paso 3: 9 bits
     TXSTAbits.TX9 = 0;   //Se utilizan solo 8 bits
     
    //Paso 4
     TXSTAbits.TXEN = 1;  //Habilitar Transmisor
     RCSTAbits.CREN = 1;  //Habilitar Receptor
     TXIF = 0; //apagar bandera TX
     
    }
///////////////////////////////////////////////////////////////////////////////////
void UART_Print(unsigned char *cadena)
{
    uint8_t i = 0;
    while (cadena[i] != '\0')
    UART_Disp (cadena[i++]);
}
////////////////////////////////////////////////////////////////////////////////////////
void UART_Disp(const char sel)
{
  while (TXSTAbits.TRMT == 0);  // esperar a que el bit de transmision este en 0
  TXREG = sel;       // actualizar EUSART registro de transmision
}