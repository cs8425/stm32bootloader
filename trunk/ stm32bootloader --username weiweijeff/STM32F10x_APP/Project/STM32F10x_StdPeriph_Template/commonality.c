
#include "commonality.h"


struct tm time_now;
unsigned char time_buffer[20]="2011-11-07 09:17:10";
unsigned char Scan_Channels[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};//扫描通道
unsigned char ADC_R_Value[6];
unsigned char ADC_V_Value[6];
u16 ADCConvertedValue_R[ADC_R_DMA_BufferSize];
u16 ADCConvertedValue_V[ADC_V_DMA_BufferSize];


uint8_t     tp_flag=0;
uint16_t    tp_x[5],tp_y[5],x,y;
uint8_t     sys_flag=0;





void RCC_Configuration(void)
{
  
  ErrorStatus HSEStartUpStatus;
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);
    
    RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
    
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); 

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
       User can add here some code to deal with this error */    

    /* Go to infinite loop */
    while (1)
    {
    }
  }
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
//  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0);
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0); 
//  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xF000);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
//  NVIC_InitStructure.NVIC_IRQChannel = ADC_R_DMA_Channel_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
  
//  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
}


void TIM2_Config(void)
{
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  /* TIM2 configuration */
  TIM_TimeBaseStructure.TIM_Period = 24000-1;          
  TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock/120000) - 1);
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM2->EGR |=(1<<0);

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);
}

void TIM3_Config(void)
{
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  /* TIM3 configuration */
  TIM_TimeBaseStructure.TIM_Period = 2000-1;          
  TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock/120000) - 1);
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  
  TIM3->EGR |=(1<<0);

  /* TIM3 enable counter */
//  TIM_Cmd(TIM3, ENABLE);
}

void delay_nus(vu32 nCount)
{
  u16 TIMCounter = nCount;
  TIM_Cmd(TIM2, ENABLE);
  TIM_SetCounter(TIM2, TIMCounter);
  while (TIMCounter)
  {
    TIMCounter = TIM_GetCounter(TIM2);
  }
  TIM_Cmd(TIM2, DISABLE);
}

void  delay(u32 t)
{
 uint32_t i;
 for(i=0;i<t;i++);
}




unsigned char *Get_ADC_R_Value()
{

  u16 j=0,i=0,temp=0;
  u32 k=0,n=0;
  for(j=0;j<=ADC_R_DMA_BufferSize;j+=1)
  {
    for (i=0;i<ADC_R_DMA_BufferSize-j;i+=1)
    {
      if (ADCConvertedValue_R[i]>ADCConvertedValue_R[i+1])
      {
        temp=ADCConvertedValue_R[i];
        ADCConvertedValue_R[i]=ADCConvertedValue_R[i+1];
        ADCConvertedValue_R[i+1]=temp;
      }
    }
  }
  for(i=ADC_R_DMA_BufferSize>>2;i<(ADC_R_DMA_BufferSize>>2)*3;i+=1)
  {
    n+=ADCConvertedValue_R[i];
  }
  k= ((n*3277)>>17);
  n=0;
  ADC_R_Value[0]=k/1000+'0';
  ADC_R_Value[1]='.';
    ADC_R_Value[2]=(k%1000)/100+'0';
      ADC_R_Value[3]=(k%100)/10+'0';
        ADC_R_Value[4]=k%10+'0';
          ADC_R_Value[5]='\0';  
  return ADC_R_Value;
}


void ADC_GPIO_Config(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}


void ADC_R_Config(void)
{
  RCC_APB2PeriphClockCmd(ADC_R_RCC, ENABLE);
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitTypeDef ADC_InitStructure;

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC_R, &ADC_InitStructure);

  /* ADC_R regular channel1 configuration */
  ADC_RegularChannelConfig(ADC_R, ADC_Channel_13, 1, ADC_SampleTime_1Cycles5);
  /* ADC_R regular channel2 configuration */
//  ADC_RegularChannelConfig(ADC_R, ADC_Channel_12, 2, ADC_SampleTime_239Cycles5);//

  /* Enable ADC_R DMA */
  ADC_DMACmd(ADC_R, ENABLE);
  /* Enable ADC_R */
  ADC_Cmd(ADC_R, ENABLE);

  /* Enable ADC_R reset calibaration register */
  ADC_ResetCalibration(ADC_R);
  /* Check the end of ADC_R reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC_R));
    /* Start ADC_R calibaration */
  ADC_StartCalibration(ADC_R);
  /* Check the end of ADC_R calibration */
  while(ADC_GetCalibrationStatus(ADC_R));
  /* Start ADC_R Software Conversion */
  ADC_SoftwareStartConvCmd(ADC_R, ENABLE);
}

void ADC_R_DMA_Config(void)
{
  RCC_AHBPeriphClockCmd(ADC_R_DMA_RCC, ENABLE);
  DMA_InitTypeDef DMA_InitStructure;

  /* DMA1 channel1 configuration ----------------------------------------------*/
  DMA_DeInit(ADC_R_DMA_Channel);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_R_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue_R;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = ADC_R_DMA_BufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(ADC_R_DMA_Channel, &DMA_InitStructure);

  /* Enable DMA1 channel1 */
  DMA_Cmd(ADC_R_DMA_Channel, ENABLE);
//  DMA_ITConfig(ADC_R_DMA_Channel,DMA_IT_TC,ENABLE);
}


//-------------------------------------------------------//



unsigned char *Get_ADC_V_Value()
{

  u16 j=0,i=0;
  vu32 k=0,n=0,temp=0;

  for(j=0;j<=ADC_V_DMA_BufferSize;j+=1)
  {
    for (i=0;i<ADC_V_DMA_BufferSize-j;i+=1)
    {
      if (ADCConvertedValue_V[i]>ADCConvertedValue_V[i+1])
      {
        temp=ADCConvertedValue_V[i];
        ADCConvertedValue_V[i]=ADCConvertedValue_V[i+1];
        ADCConvertedValue_V[i+1]=temp;
      }
    }
  }
  for(i=ADC_V_DMA_BufferSize>>2;i<(ADC_V_DMA_BufferSize>>2)*3;i+=1)
  {
    n+=ADCConvertedValue_V[i];
  }
  k=((n*3277)>>17);
  n=0;
  ADC_V_Value[0]=k/1000+'0';
  ADC_V_Value[1]='.';
    ADC_V_Value[2]=(k%1000)/100+'0';
      ADC_V_Value[3]=(k%100)/10+'0';
        ADC_V_Value[4]=k%10+'0';
          ADC_V_Value[5]='\0';  
  return ADC_V_Value;
  
}

void ADC_V_Config(void)
{
  RCC_APB2PeriphClockCmd(ADC_V_RCC, ENABLE);
  /* ADC configuration ------------------------------------------------------*/
  ADC_InitTypeDef ADC_InitStructure;

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC_V, &ADC_InitStructure);

  /* ADC regular channel8 configuration */
//  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_239Cycles5);
  /* ADC regular channel9 configuration */
  ADC_RegularChannelConfig(ADC_V, ADC_Channel_10, 1, ADC_SampleTime_1Cycles5);

  /* Enable ADC_V DMA */
  ADC_DMACmd(ADC_V, ENABLE);
  /* Enable ADC_V */
  ADC_Cmd(ADC_V, ENABLE);

  /* Enable ADC_V reset calibaration register */
  ADC_ResetCalibration(ADC_V);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC_V));
    /* Start ADC_V calibaration */
  ADC_StartCalibration(ADC_V);
  /* Check the end of ADC_V calibration */
  while(ADC_GetCalibrationStatus(ADC_V));
  /* Start ADC_V Software Conversion */
  ADC_SoftwareStartConvCmd(ADC_V, ENABLE);
}

void ADC_V_DMA_Config(void)
{
  RCC_AHBPeriphClockCmd(ADC_V_DMA_RCC, ENABLE);
  DMA_InitTypeDef DMA_InitStructure;

  /* DMA2 channel5 configuration ----------------------------------------------*/
  DMA_DeInit(ADC_V_DMA_Channel);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_V_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue_V;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = ADC_V_DMA_BufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(ADC_V_DMA_Channel, &DMA_InitStructure);

  /* Enable DMA2 channel5 */
  DMA_Cmd(ADC_V_DMA_Channel, ENABLE);
//  DMA_ITConfig(ADC_V_DMA_Channel,DMA_IT_TC,ENABLE);
}


void PowerA_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(PowerA_RCC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = PowerA_pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
  GPIO_Init(PowerA_PORT,&GPIO_InitStructure);
}



void CD4067_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(CD4067_RCC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = CD4067_E|CD4067_s0|CD4067_s1|CD4067_s2|CD4067_s3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
  GPIO_Init(CD4067_PORT,&GPIO_InitStructure);

}




void draw_choosemodel(void)
{
  ;
}


unsigned char *get_time_now()
{

  time_now = Time_GetCalendarTime();//获得当前时间
  //转换年月日时分秒
  time_buffer[0]=time_now.tm_year/1000+'0';
  time_buffer[1]=(time_now.tm_year%1000)/100+'0';
  time_buffer[2]=(time_now.tm_year%100)/10+'0';
  time_buffer[3]=time_now.tm_year%10+'0';
  //年
  time_buffer[5]=((time_now.tm_mon+1)%100)/10+'0';
  time_buffer[6]=(time_now.tm_mon+1)%10+'0';
  //月,时间戳是0-11,显示+1
  time_buffer[8]=(time_now.tm_mday%100)/10+'0';
  time_buffer[9]=time_now.tm_mday%10+'0';
  //日
  time_buffer[11]=(time_now.tm_hour%100)/10+'0';
  time_buffer[12]=time_now.tm_hour%10+'0';
  //时
  time_buffer[14]=(time_now.tm_min%100)/10+'0';
  time_buffer[15]=time_now.tm_min%10+'0';
  //分
  time_buffer[17]=(time_now.tm_sec%100)/10+'0';
  time_buffer[18]=time_now.tm_sec%10+'0';
  //秒

  return time_buffer;
}

void LCD_Set_Time(unsigned char *set_buffer)
{
  time_now.tm_year = (*set_buffer-'0')*1000+(*(set_buffer+1)-'0')*100+(*(set_buffer+2)-'0')*10+(*(set_buffer+3)-'0');
  time_now.tm_mon =  (*(set_buffer+5)-'0')*10+(*(set_buffer+6)-'0'-1);
  time_now.tm_mday = (*(set_buffer+8)-'0')*10+(*(set_buffer+9)-'0');
  time_now.tm_hour = (*(set_buffer+11)-'0')*10+(*(set_buffer+12)-'0');
  time_now.tm_min =  (*(set_buffer+14)-'0')*10+(*(set_buffer+15)-'0');
  time_now.tm_sec =  (*(set_buffer+17)-'0')*10+(*(set_buffer+18)-'0');
  Time_SetCalendarTime(time_now);
}



void Set_Scan_Channel(unsigned char x)
{
  if((Scan_Channels[x]>>3)==0)
  {
    GPIO_ResetBits(CD4067_PORT,CD4067_s3);
  }
  else
  {
    GPIO_SetBits(CD4067_PORT,CD4067_s3);
  }
  if(((Scan_Channels[x]&0x04)>>2)==0)
  {
    GPIO_ResetBits(CD4067_PORT,CD4067_s2);
  }
  else
  {
    GPIO_SetBits(CD4067_PORT,CD4067_s2);
  }
  if(((Scan_Channels[x]&0x02)>>1)==0)
  {
    GPIO_ResetBits(CD4067_PORT,CD4067_s1);
  }
  else
  {
    GPIO_SetBits(CD4067_PORT,CD4067_s1);
  }
  if((Scan_Channels[x]&0x01)==0)
  {
    GPIO_ResetBits(CD4067_PORT,CD4067_s0);
  }
  else
  {
    GPIO_SetBits(CD4067_PORT,CD4067_s0);
  }
}




























void schedule(uint16_t x,uint16_t y)
{
  TP_stop();
  
  
  
  
  if((x<0)||(x>799)||(y>479)||(y<0))
  {
    TP_restart();
    return;
  }
  
  
  if(sys_flag==choose_model)
  {
    if(x>280&&x<320&&y>200&&y<239)
    {
      TP_restart();
      return;
    }
    //第一列
    if((x<80)&&(y<48))
    {
      TP_restart();
      return;
    }
    if((x<80)&&(y>48)&&(y<96))
    {

      sys_flag=TKS14A_I;
      TP_restart();
      return;
    }
    if((x<80)&&(y>96)&&(y<144))
    {
      sys_flag=TKS14A_II;
      TP_restart();
      return;
    }
    if((x<80)&&(y>144)&&(y<192))
    {
      sys_flag=TKS15A_I;
      TP_restart();
      return;
    }
    if((x<80)&&(y>192&&y<239))
    {
      sys_flag=TKS15A_II;
      TP_restart();
      return;
    }
    //第二列
    if((x>80)&&(x< 200)&&(y<48))
    {
      TP_restart();
      return;
    }
    if((x>80)&&(x<200)&&(y>48)&&(y<96))
    {
      sys_flag=TKS231_1_II;
      TP_restart();
      return;
    }

    if((x>200&&x<319)&&(y>144)&&(y<192))
    {
      sys_flag=set_time;
      TP_restart();
      return;
    }
  }
/***********************************************************/
  if(sys_flag==TKS14A_I)
  {
    if(x>280&&x<320&&y>200&&y<239)
    {
      sys_flag=choose_model;
      CD4067_DIS();
      TP_restart();
      return;
    }
  }
/************************************************************/
  if(sys_flag==TKS14A_II)
  {
    if(x>280&&x<320&&y>200&&y<239)
    {
      sys_flag=choose_model;
      CD4067_DIS();
      TP_restart();
      return;
    }
    measure_TKS14A_II(x,y);
  }
/************************************************************/
  if(sys_flag==TKS15A_I)
  {
    if(x>280&&x<320&&y>200&&y<239)
    {
      sys_flag=choose_model;
      CD4067_DIS();
      TP_restart();
      return;
    }
    measure_TKS15A_I(x,y);
  }
/************************************************************/
  if(sys_flag==TKS15A_II)
  {
    if(x>280&&x<320&&y>200&&y<239)
    {
      sys_flag=choose_model;
      CD4067_DIS();
      TP_restart();
      return;
    }
    measure_TKS15A_II(x,y);
  }
  if(sys_flag==TKS231_1_II)
  {
    if(x>280&&x<320&&y>200&&y<239)
    {
      sys_flag=choose_model;
      CD4067_DIS();
      TP_restart();
      return;
    }
    measure_TKS231_1_II(x,y);
  }
  if(sys_flag==set_time)
  {
    if(x>280&&x<320&&y>200&&y<239)
    {
      sys_flag=choose_model;
      TP_restart();
      return;
    }
    time_manager(x,y);
  }



TP_restart();
}








unsigned int time_coordinate[14][4]={
{84,5,92,20},{92,5,100,20},{100,5,108,20},{108,5,116,20},
{124,5,132,20},{132,5,140,20},
{148,5,156,20},{156,5,164,20},
{172,5,180,20},{180,5,188,20},
{196,5,204,20},{204,5,212,20},
{220,5,228,20},{228,5,235,20},
};
unsigned int action_tp[5][4]={
{130,20,190,80},{60,90,120,150},{130,90,190,150},{200,90,260,150},{130,160,190,220}
};//up,left,ok,right,down
enum action{up,left,ok,right,down};
unsigned char time_set_flag=0,key_flag=0xff;
unsigned char time_bit[14]={0,1,2,3,5,6,8,9,11,12,14,15,17,18};
void draw_time_manger(void)
{
  ;
}

#if 0
void time_manager(int x,int y)
{
//  printf("\r\nx=%d,y=%d",x,y);
  unsigned char n;
  for(n=0;n<5;n++)
  {
    if(x>action_tp[n][0]&&x<action_tp[n][2]&&y>action_tp[n][1]&&y<action_tp[n][3])
    {
      key_flag=n;
    }
  }
//  printf("\r\n%d",n);
  switch(key_flag)
  {
  case up:
    {
//      printf("\r\nup");
      if((time_buffer[time_bit[time_set_flag]]=='0')||(time_buffer[time_bit[time_set_flag]]>'0')&&(time_buffer[time_bit[time_set_flag]]<'9'))
      {
        time_buffer[time_bit[time_set_flag]]+=1;
        ili9320_writestr(84,5,time_buffer,Cyan,Black);
        GUI_Rectangle(time_coordinate[time_set_flag][0],time_coordinate[time_set_flag][1],time_coordinate[time_set_flag][2],time_coordinate[time_set_flag][3],Red,0);
      }
      else
      {
        time_buffer[time_bit[time_set_flag]]='0';
        ili9320_writestr(84,5,time_buffer,Cyan,Black);
        GUI_Rectangle(time_coordinate[time_set_flag][0],time_coordinate[time_set_flag][1],time_coordinate[time_set_flag][2],time_coordinate[time_set_flag][3],Red,0);
      }
      break;
    }
  case left:
    {
//      printf("\r\nleft");
      if(time_set_flag>0&&time_set_flag<=13)
      {
        time_set_flag-=1;
        ili9320_writestr(84,5,time_buffer,Cyan,Black);
        GUI_Rectangle(time_coordinate[time_set_flag][0],time_coordinate[time_set_flag][1],time_coordinate[time_set_flag][2],time_coordinate[time_set_flag][3],Red,0);
      }
      else
      {
        time_set_flag=0;
        ili9320_writestr(84,5,time_buffer,Cyan,Black);
        GUI_Rectangle(time_coordinate[time_set_flag][0],time_coordinate[time_set_flag][1],time_coordinate[time_set_flag][2],time_coordinate[time_set_flag][3],Red,0);
      }
      break;
    }
  case ok:
    {
//      printf("\r\nok");
      LCD_Set_Time(time_buffer);
      ili9320_writestr(84,5,time_buffer,Cyan,Black);
      break;
    }
  case right:
    {
//      printf("\r\nright");
      if((time_set_flag==0)||(time_set_flag>0)&&(time_set_flag<13))
      {
        time_set_flag+=1;
        ili9320_writestr(84,5,time_buffer,Cyan,Black);
        GUI_Rectangle(time_coordinate[time_set_flag][0],time_coordinate[time_set_flag][1],time_coordinate[time_set_flag][2],time_coordinate[time_set_flag][3],Red,0);
      }
      else
      {
        time_set_flag=0;
        ili9320_writestr(84,5,time_buffer,Cyan,Black);
        GUI_Rectangle(time_coordinate[time_set_flag][0],time_coordinate[time_set_flag][1],time_coordinate[time_set_flag][2],time_coordinate[time_set_flag][3],Red,0);
      }
      break;
    }
  case down:
    {
//      printf("\r\ndown");
      if((time_buffer[time_bit[time_set_flag]]>'0')&&(time_buffer[time_bit[time_set_flag]]<='9'))
      {
        time_buffer[time_bit[time_set_flag]]-=1;
        ili9320_writestr(84,5,time_buffer,Cyan,Black);
        GUI_Rectangle(time_coordinate[time_set_flag][0],time_coordinate[time_set_flag][1],time_coordinate[time_set_flag][2],time_coordinate[time_set_flag][3],Red,0);
      }
      else
      {
        time_buffer[time_bit[time_set_flag]]='0';
        ili9320_writestr(84,5,time_buffer,Cyan,Black);
        GUI_Rectangle(time_coordinate[time_set_flag][0],time_coordinate[time_set_flag][1],time_coordinate[time_set_flag][2],time_coordinate[time_set_flag][3],Red,0);
      }
      break;
    }
  default:break;
  }
}
#endif