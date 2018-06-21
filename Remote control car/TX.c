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

sbit	BUZZA	=P3^7;
sbit	TX_EN	=P1^0;
sbit	TRX_CE	=P3^0;
sbit	PWR_UP	=P1^1;
sbit	MISO	=P1^6;
sbit	MOSI	=P1^3;
sbit	SCK		=P1^5;
sbit	CSN		=P1^4;
sbit	AM		=P1^7;
sbit	DR		=P1^2;			
uchar bdata DATA_BUF;
sbit	flag0	=DATA_BUF^7;
sbit	flag1	=DATA_BUF^0;  

uchar code nrf905_table[10]={0x4c,0x0c,0x44,0x03,0x03,0xcc,0xcc,0xcc,0xcc,0x58};
uchar TX_Data[3];
uchar num0,num1,num2;


void IOInit()
{					
	CSN=1;										
	SCK=0;					
	DR=1;					
	AM=1;					
	PWR_UP=1;					
	TRX_CE=0;					
	TX_EN=0;				
} 

void Delay(uint n)
{
	uint i;
	while(n--)
		for(i=0;i<80;i++);	
} 

void SpiWrite(uchar  byte)
{
	uchar i;	
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

void TxPacket(void)
{
	uchar i;				
	CSN=0;					
	SpiWrite(WTP);			
	for (i=0;i<3;i++)
	{
		SpiWrite(TX_Data[i]);	
	}
	CSN=1;									
	Delay(1);
	CSN=0;						
	SpiWrite(WTA);			
	for (i=0;i<4;i++)		
	{
		SpiWrite(nrf905_table[i+5]);
	}	
	CSN=1;					
	TRX_CE=1;					
	Delay(1);					
	TRX_CE=0;				
}

void SetTxMode(void)				
{	
	TX_EN=1;
	TRX_CE=0;
	Delay(1); 			
} 

void keys()				
{
	uchar temp;
	temp=P2&0x1f;
	while(temp!=0x1f)
	{
		Delay(200);
		while(temp!=0x1f)
		{
			switch(temp)
			{
				case 0x1e: num0=1;	break;
				case 0x1d: 
				{
					if(num1<10)
						num1=num1+1;
				}	break;
				case 0x1b:
				{
					if(num1>2)
						num1=num1-1;
				}	break;
				case 0x17: num2=1;	break;
				case 0x0f: num0=2;	break;	
			}
			temp=0x1f;
			TX_Data[0]=num0;
			TX_Data[1]=num1;
			TX_Data[2]=num2;							
		}
	}	
}  

void main()
{
	IOInit();
	Config905();
	SetTxMode();	
	while(1)
	{
		num0=0;
		keys();
		TxPacket();
		Delay(1000);
	}
}