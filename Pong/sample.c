/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "timer/lpc17xx.h"
#include "GLCD/GLCD.h" 
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "adc/adc.h"
#include <stdbool.h>
#include <stdio.h>
#include "button_EXINT/button.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

typedef	struct Paddle 
{
   int x;
   int y[10];
}Paddle;

typedef	struct Square 
{
   int x[5];
   int y;
}Square;

//assets di gioco
volatile Paddle pdl;
volatile Square sqr;

//stampa a schermo del punteggio
volatile unsigned int points;
char bufferPoints[10];

//stampa a schermo del record
volatile unsigned int record;
char bufferRecord[10];

//inizializzazione sistema per paddle
volatile bool init;

//direzioni della pallina
volatile bool right;
volatile bool up;

//flag per segnalare la sconfitta
volatile bool defeat;

//flag per mettere in pausa la partita
volatile bool pause;

int main(void)
{
	//set iniziale delle variabili
	pause = false;
	
	up = false;
	right = true;
	
	record = 100;
	
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	
  LCD_Initialization();
	
	// black screen
	LCD_Clear(Black);
	
	// starting text
	GUI_Text(30, 160, (uint8_t *) " Press KEY1 to start ", White, Black);
	
	BUTTON_init();
	
	//inizializzazione dei timer

	//RIT: handling dei button
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	enable_RIT();													/* RIT enabled												*/
	
	//timer 1: conversione A/D per potenziometro
	init_timer(1, 0x6108);								/* 1ms * 25MHz = 25*10^3 = 0x6108 */
	
	ADC_init();														/* ADC Initialization									*/

	//timer 0: gestione della pallinaì
	init_timer(0, 0x6108 ); 						  /* 1ms * 25MHz = 25*10^3 = 0x6108 */
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
	//loudspeaker
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
