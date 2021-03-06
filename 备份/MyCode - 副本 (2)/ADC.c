#include "ADC.h"

//使用ADC1通道8、9   PB0,PB1

void ADC_init(void)
{
	//adc上电，执行校准（校准前adon=0，即关闭adc至少2个adc周期）
	//等待Tstab时常
	//14个时钟周期后 EOC 被置位，DR中存入转换结果
	//adc转换时间=采样时间（例：1.5周期）+12.5个adc时钟周期=14周期（若时钟频率为14MHz，则为1us）
	
	RCC->APB2RSTR |=   1<<9;//!(RCC_APB2RSTR)复位ADC1
	RCC->APB2RSTR &= ~(1<<9);//!ADC1复位结束
	
	RCC->CFGR &= ~(3<<14);//!(RCC_CFGR)清除ADC预分频因子
	RCC->CFGR |=   2<<14;//!设定分频因子
	
	ADC1->CR1 |= //!(ADC_CR1)
		 0<<16//独立工作模式
		|0<<8//非扫描模式
		;//!END
	
	ADC1->CR2|= //!(ADC_CR2)
		 0<<1//关闭连续转换
		|0<<11//设置对齐模式为右对齐
		|7<<17//设定规则通道启动事件为软件启动：SWSTART
		|1<<8//使能DMA
		|1<<20//使用外部事件启动转换
		;//!END
	
	ADC1->SQR1|= 1<<20;//!(ADC_SQR1)规则序列数量为1（一个序列号可以对应任意一个通道）
	
	//!提高采样时间可以提高采样精度
	ADC1->SMPR2|= //!(ADC_SMPR2)
		 7<<24//通道8采样时间(7.5s=1)28.5周期
		|7<<27//通道9采样时间(7.5s=1)28.5周期
		;//!END
	
	ADC1->CR2 |= 1<<0;//启动ADC1
	ADC1->CR2 |= 1<<3;//使能复位校准，由硬件清零 
	while((ADC1->CR2)&(1<<3));//等待校准结束
	ADC1->CR2|= 1<<2;//开始AD校准，由硬件清零 
	while((ADC1->CR2)&(1<<2));//等待校准结束
}

u16 ADC_getVal(u8 CHx)
{
	ADC1 -> SQR3 &= 0xFFFFFFE0;          //(ADC_SQR3)清除通道选择
    ADC1 -> SQR3 |= CHx;                //选择通道
    ADC1 -> CR2  |= 1<<22;             //开启AD转换
    while(!(ADC1 -> SR & 1<<1));           //等待转换结束
 
    return ADC1->DR;
}
