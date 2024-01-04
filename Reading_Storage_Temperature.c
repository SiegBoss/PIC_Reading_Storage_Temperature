// Lectura y Almacenamiento de la Temperatura | Reading and Storage Temperature

// Librerias a utilizar | Libraries to use
#include <16F877A.h>
#device adc = 10
#fuses XT, NOWDT, NOPROTECT, NOLVP
#use delay(clock = 4MHz)
#use rs232(baud = 9600, xmit = pin_c6, rcv = pin_c7, bits = 8, parity = N)
#include <lcd.c>
#include <2432.c>
#include <stdlib.h>
#use standard_io(C)
#use fast_io(D)

// Variables a utilizar | Variables to use
char option2[2];
int i = 0, i2 = 0, i3 = 0, seconds, data = 0;
int16 temperatureRead, option;
float temperature, time, dataMemory;

// Funcion para imprimir en el Puerto Serial | Function to print on the Serial Port
void printText()
{
    printf("\f .: Lectura y Almacenamiento de la Temperatura :. \r\n\n");
    printf(" 1) Leer los Datos Almacenados \n\r");
    printf(" 2) Fijar el Tiempo De Muestreo \r\n\n");
}

// Funcion para leer la temperatura | Function to read the temperature
void temperatureFunction()
{
    set_adc_channel(0);
    temperatureRead = read_adc();
    temperature = (temperatureRead) * (500 / 1024.0);
}

// Funcion para la interrupcion del Timer1 | Function for the Timer1 interruption
#int_TIMER1
void timeFunction(void)
{
    set_timer1(3200);

    i3++;

    if (i3 == 2)
    {
        i3 = 0;

        // Muestra los segundos | Show the seconds
        if (time != 0)
        {
            seconds++;

            lcd_gotoxy(1, 2);
            printf(lcd_putc, "Seg. %i           ", seconds);
        }

        // Guarda los datos | Save the data
        if (seconds == time && time != 0)
        {
            lcd_gotoxy(1, 1);
            printf(lcd_putc, "\fGuardando Dato...");
            delay_ms(1000);

            // Leer la temperatura | Read the temperature
            temperatureFunction();

            // Guardar los datos | Save the data
            WRITE_EXT_EEPROM(i2, temperature);

            i2++;
            data++;
        }

        // Reinicia los segundos | Restart the seconds
        if (seconds == time && time != 0)
        {
            seconds = 0;
        }
    }
}

// Interrupcion por Recepcion de Datos del Puerto Serial | Interruption by Reception of Data from the Serial Port
#int_rda
void opcion()
{
    // Leer la opcion | Read the option
    option = getchar();

    //Si la opcion es 1 muestra los datos almacenados | If the option is 1 shows the stored data
    if (option == '1')
    {
        for (i = 0; i < data; i++)
        {
            // Leer los datos almacenados | Read the stored data
            dataMemory = READ_EXT_EEPROM(i);
            printf("\r\nDato No. %i = %f\r", i, dataMemory);
        }

        delay_ms(3000);

        // Imprimir el Puerto Serial | Print the Serial Port
        printf("\f");
        printText();
    }

    //Si la opcion es 2 fija el tiempo de muestreo | If the option is 2 sets the sampling time
    else if (option == '2')
    {
        printf("\r\n\n ::  Nuevo Tiempo de Muestreo  ::");
        printf("\r\n ::  Valor de 0 a 60 Segundos  ::");
        printf("\r\n Ingrese el Nuevo Valor : ");
        gets(option2);
        time = atoi(option2);

        printf("\r\n\n El Tiempo De Muestreo es de %2.0f Segundos \r\n", time);
        delay_ms(2000);

        // Imprimir el Puerto Serial | Print the Serial Port
        printf("\f");
        printText();
    }
}

// Funcion Principal | Main Function
void main()
{
    // Configuracion de Puertos de entrada y salida | Configuration of input and output ports
    set_tris_D(0b00001000);
    // Inicializar el ADC | Initialize the ADC
    setup_adc(ADC_CLOCK_INTERNAL);
    setup_adc_ports(AN0_AN1_AN3);
    // Inicializar el Timer1 | Initialize the Timer1
    setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
    set_timer1(3200);
    // Inicializar las Interrupciones | Initialize the Interruptions
    enable_interrupts(INT_TIMER1);
    enable_interrupts(int_rda);
    enable_interrupts(global);
    // Inicializar el LCD | Initialize the LCD
    lcd_init();
    // Imprimir el Puerto Serial | Print the Serial Port
    printText();

    // Ciclo Infinito | Infinite Cycle
    while (true)
    {
        // Leer la temperatura | Read the temperature
        temperatureFunction();

        // Imprimir en el LCD | Print on the LCD
        lcd_gotoxy(1, 1);
        printf(lcd_putc, "Temperatura=%3.0fC", temperature);
        printf(" Temperatura=%3.0fC\r", temperature);
    }
}
