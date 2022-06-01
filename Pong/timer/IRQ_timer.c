/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include <stdio.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include <stdbool.h>
#include "../RIT/RIT.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

uint16_t SinTable[45] =
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

typedef	struct Square 
{
   int x[5];
   int y;
}Square;

typedef	struct Paddle 
{
   int x;
   int y[10];
}Paddle;

extern Paddle pdl;
extern Square sqr;
extern unsigned int points;
extern char bufferPoints[10];
extern unsigned int record;
extern char bufferRecord[10];
extern bool right;
extern bool up;
extern bool defeat;

//logica per la pallina
void TIMER0_IRQHandler (void)
{
	static unsigned int i;
	static unsigned int j;
	
	static unsigned int moveX = 5;
	static unsigned int moveY = 5;
	
	//vecchia posizione della pallina
	Square sqr_old = sqr;
	
	//colori del display, per verificare se la pallina passa sopra la scritta del punteggio
	static uint16_t color;
	static uint16_t color2;
	static bool flagColor = false;
	
	//flag per aggiornare il punteggio
	bool flagPoints = false;
	
	//decido in che direzione deve andare la pallina
	sqr.x[0] =  right ? sqr.x[0] - moveX : sqr.x[0] + moveX;
	sqr.y = up ? sqr.y - moveY : sqr.y + moveY;
	
	//movimento della pallina
	for(i = 1; i <= 4; i++)
	{
		sqr.x[i] = sqr.x[i-1] + 1;
		
		//cade a terra
		if(sqr.y >= 319)
		{
			defeat = true;
			break;
		}
		
		//sbatte alla parete sinistra
		if(sqr.x[i] <= 9)
		{
			//timer 2 che gestisce i suoni
			init_timer(2, 2120); //k = 2120, f = 262 Hz
			enable_timer(2);
			
			right = false;
			moveX = 5;
			moveY = 5;
			break;
		}
		
		//sbatte alla parete destra
		if(sqr.x[i] >= 234)
		{
			//timer 2 che gestisce i suoni
			init_timer(2, 2120); //k = 2120, f = 262 Hz
			enable_timer(2);
			
			right = true;
			moveX = 5;
			moveY = 5;
			break;
		}
		
		//sbatte alla parete superiore
		if(sqr.y <= 9)
		{
			//timer 2 che gestisce i suoni
			init_timer(2, 2120); //k = 2120, f = 262 Hz
			enable_timer(2);
			
			up = false;
			moveX = 5;
			moveY = 5;
			break;
		}
		
		//sbatte al paddle
		if(sqr.y >= 277 && pdl.x <= sqr.x[i] && pdl.x + 70 >= sqr.x[i])
		{
			//timer 2 che gestisce i suoni
			init_timer(2, 1062); //k = 1062, f = 523 Hz
			enable_timer(2);
			
			flagPoints = true;
			up = true;
			
			//paddle left
			if(sqr.x[i] <= pdl.x + 20)
			{
				moveX = 10;
				moveY = 5;
				right = true;
			}
			
			//paddlle right
			else if(sqr.x[i] >= pdl.x + 70 - 20)
			{
				moveX = 10;
				moveY = 5;
				right = false;
			}
			
			//paddle center
			else
			{
				moveX = 5;
				moveY = 5;
				right = true;
			}
			
			break;
		}
	}
	
	//aggiorno il punteggio
	if(flagPoints)
	{
		if(points < 100)
			points += 5;
		
		else
			points += 10;
		
		// score text
		sprintf(bufferPoints, "%d", points);
		GUI_Text(5, 160, (uint8_t *) bufferPoints, White, Black);
		
		if(points > record)
		{
			record = points;
			
			// recortd text
			sprintf(bufferRecord, "%d", record);
			GUI_Text(200, 20, (uint8_t *) bufferRecord, White, Black);
		}
	}
	
	//segnalo la sconfitta
	if(defeat)
	{
		disable_timer(0);
		disable_timer(1);
		// losing text
		GUI_Text(80, 160, (uint8_t *) " YOU LOSE ", White, Black);
		GUI_Text(30, 200, (uint8_t *) " Press INT0 to restart ", White, Black);
	}
	
	//aggiornamento posizione pallina, inoltre
	//verifico se la pallina è passata sopra il punteggio o il record, cancellandolo
	for(i = 0; i < 5; i++)
	{
		for(j = 0; j < 5; j++)
		{
			color = LCD_GetPoint(sqr.x[i], sqr.y + j);
			color2 = LCD_GetPoint(sqr_old.x[i], sqr_old.y +j);
			
			if(color == White || color2 == White)
				flagColor = true;
		}
		
		LCD_DrawLine(sqr_old.x[i], sqr_old.y, sqr_old.x[i], sqr_old.y + 5, Black);
		LCD_DrawLine(sqr.x[i], sqr.y, sqr.x[i], sqr.y + 5, Green);
	}
	
	//stampo di nuovo il punteggio e il record
	if(flagColor)
	{
		// score text
		sprintf(bufferPoints, "%d", points);
		GUI_Text(5, 160, (uint8_t *) bufferPoints, White, Black);
		
		// recortd text
		sprintf(bufferRecord, "%d", record);
		GUI_Text(200, 20, (uint8_t *) bufferRecord, White, Black);
	}
	
	LPC_TIM0->IR = 1;			/* clear interrupt flag */
	return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	/* ADC management */
	ADC_start_conversion();		
	
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

//gestione dei suoni
void TIMER2_IRQHandler (void)
{
	static int ticks=0;
	/* DAC management */	
	LPC_DAC->DACR = SinTable[ticks]<<6;
	ticks++;
	if(ticks==45) 
	{
		disable_timer(2);
		reset_timer(2);
		ticks=0;
	}
	
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
