/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "../led/lpc17xx.h"
#include "adc.h"
#include "../led/led.h"
#include "../GLCD/GLCD.h" 
#include <stdbool.h>
#include <stdlib.h>

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short AD_current ;   
unsigned short AD_last = 0xFF;     /* Last converted value               */

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

extern volatile Paddle pdl;
extern volatile Square sqr;
extern volatile bool init;

void ADC_IRQHandler(void) {
	
	static unsigned int i;
	static int sampledValues[3];
	static unsigned int j;
	static int sum;
	static int sumOld;
	
	AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */
  	
	//inizializzo il paddle
	if(init)
	{
		sum = 0;
		sumOld = 0;
		j = 0;
		
		for(i = 0; i < 10; i++)
			sampledValues[i] = 0;
		
		pdl.x = 119;
		pdl.y[0] = 287;
		LCD_DrawLine(pdl.x, pdl.y[0], pdl.x + 70, pdl.y[0], Green);
		
		for(i = 1; i <= 9; i++)
		{
			pdl.y[i] = pdl.y[i-1] + 1;
	
			LCD_DrawLine(pdl.x, pdl.y[i], pdl.x + 70, pdl.y[i], Green);
		}
		
		init = false;
		AD_last = AD_current;
	}
	
	else
	{
		//evito di considerare variazioni troppo piccole nella conversione AD
		sampledValues[j] = AD_current;
		
		for(i = 0; i < 3; i++)
		{
			sum += sampledValues[i];
		}
		
		if(abs(sum/3 - sumOld/3) > 300)
		{
			LCD_DrawLine(pdl.x, pdl.y[0], pdl.x + 70, pdl.y[0], Black);
			for(i = 1; i <= 9; i++)
			{
				pdl.y[i] = pdl.y[i-1] + 1;
		
				LCD_DrawLine(pdl.x, pdl.y[i], pdl.x + 70, pdl.y[i], Black);
			}
			// green paddle	
			pdl.x = (sum/3)*164/0xFFF;
			
			for(i = 1; i <= 9; i++)
			{
				if(pdl.x < 5)
				{
					LCD_DrawLine(5, pdl.y[i], 5 + 70, pdl.y[i], Green);
					continue;
				}
				
				LCD_DrawLine(pdl.x, pdl.y[i], pdl.x + 70, pdl.y[i], Green);
			}
			
			AD_last = AD_current;
			sumOld = sum;			
		}
		
		sum = 0;
		
		j = j == 2 ? 0 : j+1;
	}
}
