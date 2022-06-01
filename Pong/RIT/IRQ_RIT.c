/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "../led/lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"
#include "../adc/adc.h"
#include <stdbool.h>
#include <stdio.h>

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

int down_INT0=0;
int down_KEY1=0;
int down_KEY2=0;

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

extern Paddle pdl;
extern Square sqr;

extern unsigned int points;
extern char bufferPoints[10];

extern unsigned int record;
extern char bufferRecord[10];

extern bool init;

extern bool right;
extern bool up;

extern bool pause;

extern bool defeat;

void RIT_IRQHandler (void)
{			
	static unsigned int i = 0;
	
  /* INT0 */
	if(down_INT0!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* INT0 pressed */
			down_INT0++;				
			switch(down_INT0){
				case 2:
					//int0 handler
					// black screen
					LCD_Clear(Black);
					
					// starting text
					GUI_Text(30, 160, (uint8_t *) " Press KEY1 to start ", White, Black);
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_INT0=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	/* KEY1 */
	if(down_KEY1!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */
			down_KEY1++;				
			switch(down_KEY1){
				case 2:
					//key1 handler
					// black screen
					LCD_Clear(Black);
				
					defeat = false;
					points = 0;
					// red lines
					for(i = 0; i <= 4; i++)
					{
						//left one
						LCD_DrawLine(i, 0, i, 287, Red);
						
						// upper one
						LCD_DrawLine(0, i, 239, i, Red);
						
						// right one
						LCD_DrawLine(239 - i, 0, 239 - i, 287, Red);
					}
					
					// green square
					sqr.x[0] = 220;
					sqr.y = 160;
					LCD_DrawLine(sqr.x[0], sqr.y, sqr.x[0], sqr.y + 5, Green);
					
					for(i = 1; i <= 4; i++)
					{
						sqr.x[i] = sqr.x[i-1] + 1;
						
						LCD_DrawLine(sqr.x[i], sqr.y, sqr.x[i], sqr.y + 5, Green);
					}
					
					// score text
					sprintf(bufferPoints, "%d", points);
					GUI_Text(5, 160, (uint8_t *) bufferPoints, White, Black);
					
					// recortd text
					sprintf(bufferRecord, "%d", record);
					GUI_Text(200, 20, (uint8_t *) bufferRecord, White, Black);
					
					init = true;
					
					right = true;
					up = false;
					
					enable_timer(0);
					enable_timer(1);
					
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_KEY1=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	/* KEY2 */
	if(down_KEY2!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* KEY2 pressed */
			down_KEY2++;				
			switch(down_KEY2){
				case 2:
					//key2 handler
					if(!pause)
					{
						// pause text
						GUI_Text(90, 120, (uint8_t *) " PAUSE ", White, Black);
						GUI_Text(40, 160, (uint8_t *) " Press KEY2 to resume ", White, Black);
						pause = true;
						disable_timer(0);
						disable_timer(1);
					}
					
					else
					{
						//cancel pause text
						GUI_Text(90, 120, (uint8_t *) " PAUSE ", Black, Black);
						GUI_Text(40, 160, (uint8_t *) " Press KEY2 to resume ", Black, Black);
						pause = false;
						enable_timer(0);
						enable_timer(1);
					}
					
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_KEY2=0;			
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	}
	
	LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
}

/******************************************************************************
**                            End Of File
******************************************************************************/
