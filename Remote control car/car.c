#include<reg51.h>
#include<intrins.h>
#define uchar unsigned char 
#define uint  unsigned int
#define WC		0x00		
#define RC		0x10 	
#define WTP		0x20 		
#define RTP		0x21			
#define WTA		0x22		
#define RTA		0x23		
#define RRP		0x24

sbit	BUZZA	=P0^0;

sbit	front	=P2^1;
sbit	behind	=P2^2;

sbit 	left_pwm	=P0^2;
sbit 	right_pwm	=P0^3;
sbit 	left_a	=P0^4;
sbit 	left_b	=P0^5;
sbit 	right_a	=P0^6;
sbit 	right_b	=P0^7;

sbit	TX_EN	=P3^0;
sbit	TRX_CE	=P1^3;
sbit	PWR_UP	=P1^7;
sbit	MISO	=P1^1;
sbit	MOSI	=P1^5;
sbit	SCK		=P1^0;
sbit	CSN		=P1^4;
sbit	AM		=P1^2;
sbit	DR		=P1^6;
			
uchar bdata DATA_BUF;
sbit	flag0	=DATA_BUF^7;
sbit	flag1	=DATA_BUF^0;  

uchar code nrf905_table[10]={0x4c,0x0c,0x44,0x03,0x03,0xcc,0xcc,0xcc,0xcc,0x58};
uchar nrf905_data[3];
uchar left_speed,right_speed,speed,num0,num1,num2;
bit flag,flag2,flag3,flag4,flag5;

void IOInit()
{					
	CSN=1;										
	SCK=0;					
	DR=1;					
	AM=1;					
	PWR_UP=1;					
	TRX_CE=0;					
	TX_EN=0;
	left_a=0;
	left_b=0;
	right_a=0;
	right_b=0;
	num1=5;				
}

void timer_init(void)	
{
	EA=1;
	ET0=1;
	TMOD=0x01;
	TH0=(65536-500)/256;
	TL0=(65536-500)%256;
	TR0=0;	
} 

void Delay(uchar n)
{
	uint i;
	while(n--)
	for(i=0;i<80;i++);	
} 

void SpiWrite(uchar  byte)
{
	uchar i;
	EA=0;	
	DATA_BUF=byte;						
	for(i=0;i<8;i++)		
	{	
		
		if (flag0)			
			MOSI=1;
		else
			MOSI=0;
		SCK=1;				
		DATA_BUF=DATA_BUF<<1;	
		SCK=0;				
	}
	EA=1;	
}

uchar SpiRead(void)
{
	uchar i;
	EA=0;	
	for(i=0;i<8;i++)		
	{	
		DATA_BUF=DATA_BUF<<1;
		SCK=1;				
		if (MISO)
			flag1=1;			
		else
			flag1=0;		
		SCK=0;				
	}
	EA=1;
	return DATA_BUF;		
}

void Config905(void)
{
	uchar i;					
	CSN=0;					
	SpiWrite(WC);				
	for (i=0;i<10;i++)	
	{
		SpiWrite(nrf905_table[i]);
	}
	CSN=1;					
}

void RxPacket(void)
{
	uchar i;	
	TRX_CE=0;					
	CSN=0;					
	SpiWrite(RRP);				
	for (i=0;i<3;i++)
	{
		nrf905_data[i]=SpiRead();			
	}
	CSN=1;					
	while(DR||AM);			
	TRX_CE=1;										
}

void SetRxMode(void)
{
	TX_EN=0;
	TRX_CE=1;
	Delay(1); 							
}

uchar CheckDR(void)		
{
	if (DR==1)
		return 1;
	else
		return 0;
}

void car(void)
{
	if(!num2)
	{
		flag4=flag5=0;
		if(!flag3)
		{
			switch(num1)
			{
				case 1:
				{
					right_speed=0;
					left_speed=0;			
				}	break;
				case 2:
				{
					right_speed=2;
					left_speed=2;
				}	break;
				case 3:
				{
					right_speed=3;
					left_speed=3;
				}	break;
				case 4:
				{
					right_speed=4;
					left_speed=4;
				}	break;
				case 5:
				case 6:
				{
					_nop_();
				}	break;
				case 7:
				{
					right_speed=4;
					left_speed=4;
				}	break;
				case 8:
				{
					right_speed=3;
					left_speed=3;
				}	break;
				case 9:
				{
					right_speed=2;
					left_speed=2;
				}	break;
				case 10:
				{
					right_speed=0;
					left_speed=0;
				}	break;
			}
		}
		if(num0==1)
		{
			if(!flag)
			{
				left_speed=left_speed+2;
				flag=1;
				flag3=1;
			}
		}
		else if(num0==2)
		{
			if(!flag2)
			{
				right_speed=right_speed+2;
				flag2=1;
				flag3=1;
			}
		}	
		else
		{
			flag=flag2=flag3=0;
			if(left_speed<right_speed)
				right_speed=left_speed;	
			else
				left_speed=right_speed;		
		}
		if(num1>6)
		{
			if(!front)
			{
				left_b=0;
				right_b=0;
				BUZZA=0;	
			}
			else
			{
				left_a=0;
				left_b=1;
				right_a=0;
				right_b=1;
				BUZZA=1;
			}
		}
		else if(num1<5)
		{
			if(!behind)
			{
				left_a=0;
				right_a=0;
				BUZZA=0;	
			}
			else
			{
				left_a=1;
				left_b=0;
				right_a=1;
				right_b=0;
				BUZZA=1;
			}
		}
		else
		{
			left_a=0;
			left_b=0;
			right_a=0;
			right_b=0;
			BUZZA=1;
		}
	}
	else
	{
		if((!front)&&(!flag4))
		{
			flag4=1;
			flag5=0;
			left_b=0;
			right_b=0;
			left_a=1;
			right_a=1;	
		}
		else if((!behind)&&(!flag5))
		{
			flag5=1;
			flag4=0;
			left_a=0;
			right_a=0;
			left_b=1;
			right_b=1;	
		}
		right_speed=3;
		left_speed=3;	
	}
}

void main()                                                                                                                                                                                          
{
	IOInit();
	timer_init();
	Config905();
	SetRxMode();
	TR0=1;		
	while(1)
	{
		while(CheckDR()==0)
		{
			car();
		}
		RxPacket();
		num0=nrf905_data[0];
		num1=nrf905_data[1];
		num2=nrf905_data[2];
	}
}

void Timer0(void)	interrupt 1
{
	TH0=(65536-500)/256;
	TL0=(65536-500)%256;	
	if(speed>=left_speed)
		left_pwm=1;
	if(speed>=right_speed)
		right_pwm=1;
	speed++;
	if(speed==11)
	{
		speed=0;
		right_pwm=0;
		left_pwm=0;
	}
} 

