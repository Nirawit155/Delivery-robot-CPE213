#include "stm32l1xx.h"
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_utils.h"
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_dac.h"
#include "stm32l1xx_ll_tim.h"

//value
int N_Station = 1;/*location where it is. {	1 = A, 
																						2 = B, 
																						3 = C, 
																						4 = D} 
																						A is start position*/
																						
int G_Station;		/*location where it go. {	1 = A, 
																						2 = B, 
																						3 = C, 
																						4 = D}*/

int Pline; /*position of robot  on line. {	3 = both sensor is on white, 
																						2 = only right sensor is on black, 
																						1 = only left sensor is on black, 
																						0 = both sensor is on black}*/
																						
int count;	//use for count down
int cnt = 100;	//time count down ( 1/100 )sec
//function																				
	//config
void SystemClock_Config(void);
void TIMBase_Config(void);
void GPIO_MOTOR_Configure(void);
void GPIO_ENCODEDR_Configure(void);
void GPIO_LED_Configure(void);
void GPIO_MATREIX_SWITCH_Configure(void);
void GPIO_INFRARED_Configure(void);
void START(void);
	//basic movement
void F(void);	//motor forward
void L(void);	//motor turn left
void R(void);	//motor turn right
void B(void);	//motor backward
void SS(void);  //stop motor
	//track line
void track();
	//sensor
void infrared(void);
	//step movement
void TF(void);	//forward
void TL(void);	//turn left and go forward until find line
void TR(void);	//turn right and go forward until find line
void TB(void);	//turn backward
	//Input MATREIX SWITCH
void Wait_Input(void);
	//advanced movement
void move();
	//Show LED station
void Show_LED(void);

int main()
{
	START();
	GPIO_MOTOR_Configure();
	GPIO_LED_Configure();
	GPIO_MATREIX_SWITCH_Configure();
	GPIO_ENCODEDR_Configure();
	GPIO_INFRARED_Configure();		
	SystemClock_Config();
	TIMBase_Config();
	
	while(1)
	{
		Wait_Input();
		move();
		Show_LED();
	}
}

void Show_LED(void)
{
	if(N_Station == 1)
	{
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_10);
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_11);
	}
	else if(N_Station == 2)
	{
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_10);
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_11);
	}
	else if(N_Station == 3)
	{
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_10);
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_11);
	}
	else if(N_Station == 4)
	{
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_10);
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_11);
	}
}

void move()
{
	if(N_Station == 1)	//From A
	{
		if(G_Station == 2)	//From A To B
		{
			TF();
			TR();
			N_Station = G_Station;
			TB();
		}
		else if(G_Station == 3)	//From A To C
		{
			TF();
			TF();
			N_Station = G_Station;
			TB();
		}
		else if(G_Station == 4)	//From A To D
		{
			TF();
			TL();
			N_Station = G_Station;
			TB();
		}
	}
	else if(N_Station == 2)	//From B
	{
		if(G_Station == 1)	//From B To A
		{
			TF();
			TL();
			N_Station = G_Station;
			TB();
		}
		else if(G_Station == 3)	//From B To C
		{
			TF();
			TR();
			N_Station = G_Station;
			TB();
		}
		else if(G_Station == 4)	//From B To D
		{
			TF();
			TF();
			N_Station = G_Station;
			TB();
		}
	}
	else if(N_Station == 3)	//From C
	{
		if(G_Station == 1)	//From C To A
		{
			TF();
			TF();
			N_Station = G_Station;
			TB();
		}
		else if(G_Station == 2)	//From C To B
		{
			TF();
			TL();
			N_Station = G_Station;
			TB();
		}
		else if(G_Station == 4)	//From C To D
		{
			TF();
			TR();
			N_Station = G_Station;
			TB();
		}
	}
	else if(N_Station == 4)	//From D
	{
		if(G_Station == 1)	//From D To A
		{
			TF();
			TR();
			N_Station = G_Station;
			TB();
		}
		else if(G_Station == 2)	//From D To B
		{
			TF();
			TF();
			N_Station = G_Station;
			TB();
		}
		else if(G_Station == 3)	//From D To C
		{
			TF();
			TL();
			N_Station = G_Station;
			TB();
		}
	}
}

void Wait_Input()
{
	while(1)
	{
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_5);
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_4);
		if(!LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_6)){}
			
		if(!LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_6))	
		{
			G_Station = 1;
			break;
		}
		if(!LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_7))	
		{
			G_Station = 2;
			break;
		}
		
		LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_4);
		LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_5);
		if(!LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_6)){}
			
		if(!LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_6))	
		{
			G_Station = 3;
			break;
		}
		if(!LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_7))	
		{
			G_Station = 4;
			break;
		}
	}
}

void TF(void)
{
	while(!(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) && LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7)))
		track();
	while(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) && LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7))
		track();
	SS();
}

void TL(void)
{
	count = cnt;
	while(count > 0)
	{
		track();
		if(LL_TIM_IsActiveFlag_UPDATE(TIM2) == SET)	//Activeflag has set? Overflow?
		{
			LL_TIM_SetCounter(TIM2, 0);
			count--;
			LL_TIM_ClearFlag_UPDATE(TIM2);
		}
	}
	
	L();
	while(!LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7));
	while(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7));
	while(!LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6));
	while(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6));
	
	while(!(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) && LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7)))
		track();
	while(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) && LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7))
		track();
	
	SS();
}

void TR(void)
{
	count = cnt;
	while(count > 0)
	{
		track();
		if(LL_TIM_IsActiveFlag_UPDATE(TIM2) == SET)	//Activeflag has set? Overflow?
		{
			LL_TIM_SetCounter(TIM2, 0);
			count--;
			LL_TIM_ClearFlag_UPDATE(TIM2);
		}
	}
	R();
	while(!LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6));
	while(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6));
	while(!LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7));
	while(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7));
	
	while(Pline != 0)
		track();
	while(Pline == 0)
		track();
	
	SS();
}

void TB(void)
{
	count = 50;
	B();
	while(count > 0)
	{
		if(LL_TIM_IsActiveFlag_UPDATE(TIM2) == SET)	//Activeflag has set? Overflow?
		{
			LL_TIM_SetCounter(TIM2, 0);
			count--;
			LL_TIM_ClearFlag_UPDATE(TIM2);
		}
	}
	R();
	while(!LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6));
	while(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6));
	while(!LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7));
	while(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7));
}

void track()
{
	infrared();
	if(Pline == 0 || Pline == 3)
	{
		F();
	}
	else if(Pline == 1)
	{
		L();
	}
	else if(Pline == 2)
	{
		R();
	}
}

void infrared(void)
{
	if(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) && LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7))
	{	
		Pline = 0;
	}	//b b
	else if(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) && !LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7))
	{	
		Pline = 1;
	}//b w
	else if(!LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) && LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7))
	{	
		Pline = 2;
	}//w b
	else if(!LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) && !LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_7))
	{	
		Pline = 3;
	}//w w

}

void F(void)
{
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_13);
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_14);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_15);
}

void L(void)
{
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_12);
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_13);
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_14);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_15);
}

void R(void)
{
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_13);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_14);
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_15);
}

void B(void)
{
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_12);
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_13);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_14);
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_15);
}

void SS(void)
{
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_12);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_13);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_14);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_15);
}

void GPIO_INFRARED_Configure(void)
{
	LL_GPIO_InitTypeDef gpio_conf;
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	
	gpio_conf.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_conf.Pull = LL_GPIO_PULL_NO;
	gpio_conf.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	gpio_conf.Mode = LL_GPIO_MODE_INPUT;
	
	gpio_conf.Pin = LL_GPIO_PIN_6;
	LL_GPIO_Init(GPIOC, &gpio_conf);
	gpio_conf.Pin = LL_GPIO_PIN_7;
	LL_GPIO_Init(GPIOC, &gpio_conf);
}

void GPIO_LED_Configure(void)
{
	LL_GPIO_InitTypeDef gpio_conf;
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	gpio_conf.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_conf.Pull = LL_GPIO_PULL_NO;
	gpio_conf.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	gpio_conf.Mode = LL_GPIO_MODE_OUTPUT;
	
	gpio_conf.Pin = LL_GPIO_PIN_10;
	LL_GPIO_Init(GPIOB, &gpio_conf);
	gpio_conf.Pin = LL_GPIO_PIN_11;
	LL_GPIO_Init(GPIOB, &gpio_conf);
}

void GPIO_MATREIX_SWITCH_Configure(void)
{
	LL_GPIO_InitTypeDef gpio_conf;
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	gpio_conf.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_conf.Pull = LL_GPIO_PULL_NO;
	gpio_conf.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_conf.Mode = LL_GPIO_MODE_OUTPUT;
	
	gpio_conf.Pin = LL_GPIO_PIN_4;
	LL_GPIO_Init(GPIOB, &gpio_conf);
	gpio_conf.Pin = LL_GPIO_PIN_5;
	LL_GPIO_Init(GPIOB, &gpio_conf);
	
	gpio_conf.Mode = LL_GPIO_MODE_INPUT;
	
	gpio_conf.Pin = LL_GPIO_PIN_6;
	LL_GPIO_Init(GPIOB, &gpio_conf);
	gpio_conf.Pin = LL_GPIO_PIN_7;
	LL_GPIO_Init(GPIOB, &gpio_conf);
	
	
}

void START(void)
{
	LL_GPIO_InitTypeDef gpio_conf;
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	
	gpio_conf.Pin = LL_GPIO_PIN_0;
	gpio_conf.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_conf.Pull = LL_GPIO_PULL_NO;
	gpio_conf.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	gpio_conf.Mode = LL_GPIO_MODE_INPUT;
	
	LL_GPIO_Init(GPIOA, &gpio_conf);

	while(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0) == 0);
}

void GPIO_MOTOR_Configure(void)
{
	LL_GPIO_InitTypeDef gpio_conf;
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	//motor
	gpio_conf.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_conf.Pull = LL_GPIO_PULL_NO;
	gpio_conf.Speed = LL_GPIO_SPEED_FREQ_LOW;
	gpio_conf.Mode = LL_GPIO_MODE_OUTPUT;
	
	gpio_conf.Pin = LL_GPIO_PIN_12;
	LL_GPIO_Init(GPIOB, &gpio_conf);
	gpio_conf.Pin = LL_GPIO_PIN_13;
	LL_GPIO_Init(GPIOB, &gpio_conf);
	gpio_conf.Pin = LL_GPIO_PIN_14;
	LL_GPIO_Init(GPIOB, &gpio_conf);
	gpio_conf.Pin = LL_GPIO_PIN_15;
	LL_GPIO_Init(GPIOB, &gpio_conf);
	
	//speed motor
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	
	gpio_conf.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	gpio_conf.Mode = LL_GPIO_MODE_ANALOG;
	
	gpio_conf.Pin = LL_GPIO_PIN_4;
	LL_GPIO_Init(GPIOA, &gpio_conf);
	gpio_conf.Pin = LL_GPIO_PIN_5;
	LL_GPIO_Init(GPIOA, &gpio_conf);
	
	LL_DAC_InitTypeDef DAC_config;
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);
	
	LL_DAC_Init(DAC, LL_DAC_CHANNEL_1, &DAC_config);
	LL_DAC_SetOutputBuffer(DAC, LL_DAC_CHANNEL_1, LL_DAC_OUTPUT_BUFFER_ENABLE);
	LL_DAC_DisableTrigger(DAC, LL_DAC_CHANNEL_1);
	LL_DAC_Enable(DAC, LL_DAC_CHANNEL_1);
	
	LL_DAC_Init(DAC, LL_DAC_CHANNEL_2, &DAC_config);
	LL_DAC_SetOutputBuffer(DAC, LL_DAC_CHANNEL_2, LL_DAC_OUTPUT_BUFFER_ENABLE);
	LL_DAC_DisableTrigger(DAC, LL_DAC_CHANNEL_2);
	LL_DAC_Enable(DAC, LL_DAC_CHANNEL_2);
}

void GPIO_ENCODEDR_Configure(void)
{
	LL_GPIO_InitTypeDef gpio_conf;
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	
	gpio_conf.Pin = LL_GPIO_PIN_0;
	gpio_conf.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_conf.Pull = LL_GPIO_PULL_UP;
	gpio_conf.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	gpio_conf.Mode = LL_GPIO_MODE_INPUT;
	LL_GPIO_Init(GPIOC, &gpio_conf);
	
	gpio_conf.Pin = LL_GPIO_PIN_1;
	LL_GPIO_Init(GPIOC, &gpio_conf);
	
	gpio_conf.Pin = LL_GPIO_PIN_2;
	LL_GPIO_Init(GPIOC, &gpio_conf);
	
	gpio_conf.Pin = LL_GPIO_PIN_3;
	LL_GPIO_Init(GPIOC, &gpio_conf);
}

void TIMBase_Config(void) 
{
	LL_TIM_InitTypeDef timbase_initstructure;
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	timbase_initstructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;	//Set Divistion 1
	timbase_initstructure.CounterMode = LL_TIM_COUNTERMODE_UP;	//Set counter mode up
	timbase_initstructure.Autoreload = 1000 - 1; 
	timbase_initstructure.Prescaler = 320 - 1;
	
	LL_TIM_Init(TIM2, &timbase_initstructure); 
	LL_TIM_EnableCounter(TIM2);
}

void SystemClock_Config(void)
{
  /* Enable ACC64 access and set FLASH latency */ 
  LL_FLASH_Enable64bitAccess();
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  /* Set Voltage scale1 as MCU will run at 32MHz */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  
  /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
  while (LL_PWR_IsActiveFlag_VOSF() != 0)
  {
  }
  
  /* Enable HSI if not already activated*/
  if (LL_RCC_HSI_IsReady() == 0)
  {
    /* HSI configuration and activation */
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1)
    {
    };
  }
  
	
  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3);

  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  };
  
  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };
  
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 32MHz                             */
  LL_Init1msTick(32000000);
  
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(32000000);
}