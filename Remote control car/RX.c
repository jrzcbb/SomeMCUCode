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

void Delay(uchar n)
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

uchar SpiRead(void)
{
	uchar i;	
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

void main()
{
	IOInit();
	Config905();
	SetRxMode();
	while(1)
	{
		while(CheckDR()==0);
		RxPacket();
		P0=nrf905_data[1];
	}
}

