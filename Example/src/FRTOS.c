/*
===============================================================================
 Name        : FRTOS.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

TaskHandle_t		Handle_Tarea_Lectura;
TaskHandle_t		Handle_Tarea_Escritura;

SemaphoreHandle_t 	Semaforo_1;
SemaphoreHandle_t 	Semaforo_2;

QueueHandle_t 		Queue_Ejemplo;
QueueHandle_t 		Queue_Valores;

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
#define PORT(x) 	((uint8_t) x)
#define PIN(x)		((uint8_t) x)

/*	Definiciones pines led RGB	*/
#define RED_PORT	((uint8_t) 2)
#define BLUE_PORT	((uint8_t) 0)
#define GREEN_PORT	((uint8_t) 2)

#define RED_PIN		((uint8_t) 0)
#define BLUE_PIN	((uint8_t) 26)
#define GREEN_PIN	((uint8_t) 1)
/*	Fin Definiciones pines led RGB	*/

/*	Definiciones pines joystick	*/
#define UP_KEY_PORT		((uint8_t) 2)
#define DOWN_KEY_PORT	((uint8_t) 2)
#define LEFT_KEY_PORT	((uint8_t) 0)
#define RIGHT_KEY_PORT	((uint8_t) 0)

#define UP_KEY_PIN		((uint8_t) 4)
#define DOWN_KEY_PIN	((uint8_t) 3)
#define LEFT_KEY_PIN	((uint8_t) 16)
#define RIGHT_KEY_PIN	((uint8_t) 15)
/*	Fin Definiciones pines joystick	*/

#define OUTPUT		((uint8_t) 1)
#define INPUT		((uint8_t) 0)

#define ON			((uint8_t) 1)
#define OFF			((uint8_t) 0)


#define PRESSED		((uint8_t) 0)
#define NOT_PRESSED	((uint8_t) 1)

#define ALL_LEDs	((uint32_t) 5000)

typedef struct
{
	uint8_t 	puerto;
	uint8_t 	pin;
	uint32_t 	tiempo;
}LED_GPIO;

LED_GPIO LED_Struct;

void uC_StartUp (void)
{
	//INICIALIZACION PERIFERICO GPIO
	Chip_GPIO_Init (LPC_GPIO);

	/* CONFIGURACION DE PINES */

	//LED
	Chip_IOCON_PinMux (LPC_IOCON , PORT(0) , PIN(22), IOCON_MODE_INACT , IOCON_FUNC0);

	//LED RGB
	Chip_IOCON_PinMux (LPC_IOCON , PORT(RED_PORT) ,   PIN(RED_PIN),  IOCON_MODE_INACT , IOCON_FUNC0);
	Chip_IOCON_PinMux (LPC_IOCON , PORT(BLUE_PORT) ,  PIN(BLUE_PIN), IOCON_MODE_INACT , IOCON_FUNC0);
	Chip_IOCON_PinMux (LPC_IOCON , PORT(GREEN_PORT) , PIN(GREEN_PIN),  IOCON_MODE_INACT , IOCON_FUNC0);

	//JOYSTICK
	Chip_IOCON_PinMux (LPC_IOCON , PORT(UP_KEY_PORT) , PIN(UP_KEY_PIN), IOCON_MODE_INACT , IOCON_FUNC0);
	Chip_IOCON_PinMux (LPC_IOCON , PORT(DOWN_KEY_PORT) , PIN(DOWN_KEY_PIN), IOCON_MODE_INACT , IOCON_FUNC0);
	Chip_IOCON_PinMux (LPC_IOCON , PORT(LEFT_KEY_PORT) , PIN(LEFT_KEY_PIN),  IOCON_MODE_INACT , IOCON_FUNC0);
	Chip_IOCON_PinMux (LPC_IOCON , PORT(RIGHT_KEY_PORT) , PIN(RIGHT_KEY_PIN),  IOCON_MODE_INACT , IOCON_FUNC0);

	/* FIN CONFIGURACION DE PINES */

	/* SETEO DE PINES */

	//LED
	Chip_GPIO_SetDir (LPC_GPIO , PORT(0) , PIN(22) , OUTPUT);

	//LED RGB
	Chip_GPIO_SetDir (LPC_GPIO , PORT(RED_PORT) ,   PIN(RED_PIN), OUTPUT);
	Chip_GPIO_SetDir (LPC_GPIO , PORT(BLUE_PORT) ,  PIN(BLUE_PIN), OUTPUT);
	Chip_GPIO_SetDir (LPC_GPIO , PORT(GREEN_PORT) , PIN(GREEN_PIN), OUTPUT);

	//JOYSTICK
	Chip_GPIO_SetDir (LPC_GPIO , PORT(UP_KEY_PORT) , 	PIN(UP_KEY_PIN), INPUT);
	Chip_GPIO_SetDir (LPC_GPIO , PORT(DOWN_KEY_PORT) , 	PIN(DOWN_KEY_PIN), INPUT);
	Chip_GPIO_SetDir (LPC_GPIO , PORT(LEFT_KEY_PORT) , 	PIN(LEFT_KEY_PIN), INPUT);
	Chip_GPIO_SetDir (LPC_GPIO , PORT(RIGHT_KEY_PORT) , PIN(RIGHT_KEY_PIN), INPUT);

	/* FIN SETEO DE PINES */
}

/* LED1 toggle thread */
static void vTaskRGB(void *pvParameters)
{
	LED_GPIO * Buffer;
	Buffer = (LED_GPIO *) pvParameters;

	while(1)
	{
		Chip_GPIO_SetPinToggle( LPC_GPIO, Buffer->puerto, Buffer->pin );
		vTaskDelay( Buffer->tiempo / portTICK_PERIOD_MS );
	}
}

/* Inputs read thread */
static void vTaskLectura(void *pvParameters)
{
	LED_GPIO Buffer;

	while (1)
	{
		Buffer.tiempo = 0;

		if( Chip_GPIO_GetPinState( LPC_GPIO , PORT(UP_KEY_PORT) , PIN(UP_KEY_PIN)) == (bool)PRESSED )
		{
			Buffer.puerto = PORT(RED_PORT);
			Buffer.pin = 	PIN(RED_PIN);
			Buffer.tiempo = 1000;
		}

		if( Chip_GPIO_GetPinState( LPC_GPIO , PORT(DOWN_KEY_PORT) , PIN(DOWN_KEY_PIN)) == (bool)PRESSED )
		{
			Buffer.puerto = PORT(BLUE_PORT);
			Buffer.pin = 	PIN(BLUE_PIN);
			Buffer.tiempo = 2000;
		}

		if( Chip_GPIO_GetPinState( LPC_GPIO , PORT(LEFT_KEY_PORT) , PIN(LEFT_KEY_PIN)) == (bool)PRESSED )
		{
			Buffer.puerto = PORT(GREEN_PORT);
			Buffer.pin = 	PIN(GREEN_PIN);
			Buffer.tiempo = 2000;
		}

		if( Chip_GPIO_GetPinState( LPC_GPIO , PORT(RIGHT_KEY_PORT) , PIN(RIGHT_KEY_PIN)) == (bool)PRESSED )
		{
			Buffer.puerto = PORT(RED_PORT);
			Buffer.pin = 	PIN(RED_PIN);
			Buffer.tiempo = ALL_LEDs;
		}

		if( Buffer.tiempo != 0 )
		{
			xQueueSendToBack( Queue_Valores , &Buffer , portMAX_DELAY );
		}

		/*
		xSemaphoreTake(Semaforo_2 , portMAX_DELAY );

		Chip_GPIO_SetPinOutHigh (LPC_GPIO , PORT(0) , PIN(22));

		vTaskDelay( 500 / portTICK_PERIOD_MS );

		xSemaphoreGive(Semaforo_1 );
		*/
	}
}

/* Led RGB thread */
static void xTaskEscritura(void *pvParameters)
{
	LED_GPIO LED_RGB;

	uint8_t 	Puerto;
	uint8_t 	Pin;
	uint32_t 	Tiempo;
	uint32_t 	Veces;

	while (1)
	{
		xQueueReceive( Queue_Valores , &LED_RGB , portMAX_DELAY );

		if ( LED_RGB.tiempo != ALL_LEDs )
		{
			Puerto = 	LED_RGB.puerto;
			Pin = 		LED_RGB.pin;
			Tiempo = 	LED_RGB.tiempo;

			Chip_GPIO_SetPinState( LPC_GPIO , Puerto , Pin , ON );

			vTaskDelay( Tiempo / portTICK_PERIOD_MS );

			Chip_GPIO_SetPinState( LPC_GPIO , Puerto , Pin , OFF );
		}
		else
		{
			for( Veces = 6 ; Veces ; Veces-- )
			{
				Chip_GPIO_SetPinToggle( LPC_GPIO , PORT(RED_PORT) ,   PIN(RED_PIN) );
				Chip_GPIO_SetPinToggle( LPC_GPIO , PORT(BLUE_PORT) ,  PIN(BLUE_PIN) );
				Chip_GPIO_SetPinToggle( LPC_GPIO , PORT(GREEN_PORT) , PIN(GREEN_PIN) );

				vTaskDelay( 500 / portTICK_PERIOD_MS );
			}
		}

		/*
		xSemaphoreTake(Semaforo_1 , portMAX_DELAY );

		Chip_GPIO_SetPinOutLow (LPC_GPIO , PORT(0) , PIN(22));

		vTaskDelay( 500 / portTICK_PERIOD_MS );

		xSemaphoreGive(Semaforo_2 );
		*/
	}
}


int main(void)
{
	//Inicializacion de bajo nivel
	uC_StartUp ();
	SystemCoreClockUpdate();

	//Inicializacion de GPIO
	LED_Struct.pin = PORT(0);
	LED_Struct.puerto = PIN(22);
	LED_Struct.tiempo = 500;

	//Semaforos
	vSemaphoreCreateBinary(Semaforo_1);
	vSemaphoreCreateBinary(Semaforo_2);

	xSemaphoreTake( Semaforo_1 , portMAX_DELAY );

	//Colas
	Queue_Ejemplo = xQueueCreate( 5 , sizeof(LED_GPIO) );
	Queue_Valores = xQueueCreate( 5 , sizeof(LED_GPIO) );

	xQueueSendToBack( Queue_Ejemplo , &LED_Struct , portMAX_DELAY );
	xQueueReceive( Queue_Ejemplo , &LED_Struct , portMAX_DELAY );

	//Tareas
	xTaskCreate(	vTaskLectura,
					(char *) "vTaskLectura",
					configMINIMAL_STACK_SIZE,
					&LED_Struct,
					(tskIDLE_PRIORITY + 1UL),
					(xTaskHandle *) Handle_Tarea_Lectura
	);

	xTaskCreate(	xTaskEscritura,
					(char *) "xTaskEscritura",
					configMINIMAL_STACK_SIZE,
					&LED_Struct,
					(tskIDLE_PRIORITY + 1UL),
					(xTaskHandle *) Handle_Tarea_Escritura
	);

	//Manejador
	vTaskStartScheduler();

	/* Nunca debería arribar aquí */

    return 0;
}

