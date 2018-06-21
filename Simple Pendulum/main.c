#include <iom16v.h>
#include <macros.h>
#include <delay_8m.h>
#include <math.h>
#define uint unsigned int
#define uchar unsigned char
uint k,j=51;
uint numcw,numcww,i;//计算脉冲数
uint a,b,c,d;//功能4、 5用
int num;
uint angle,angle_l,angle_r;//角度显示
uchar FLAG,flag,flag1;
uchar ge,shi,bai,qian;
uchar dua[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
uchar wei[]={0x40,0xc0,0x20,0xa0};
uchar CW[8]={0xaa,0x22,0x66,0x44,0x55,0x11,0x99,0x88};
uchar CWW[8]={0x88,0x99,0x11,0x55,0x44,0x66,0x22,0xaa};
uchar Nfan[]={0,1,2,3,4,5,6,7,
              8,9,10,12,13,14,15,16,
			  17,19,20,21,22,23,25,26,
			  27,29,30,31,33,34,35,37,
			  38,40,41,43,44,46,47,49,
			  50,52,54,55,57,59,60,62,
			  64,66,68,69,71,73,75,77,
			  79,81,83,84,86,88,90,92,
			  94,96,98};
uchar Nzhe[]={0,1,2,3,
              4,5,6,6,7,8,9,10,
			  11,12,13,14,15,16,16,17,
			  18,19,20,21,22,22,23,24,
			  25,26,27,27,28,29,30,31,
			  31,32,33,34,35,35,36,37,
			  38,38,39,40,41,41,42,43,
			  44,44,45,46,47,47,48,49,
			  50,50,51,52,52,53,54,55,
             };
void choose_573(void)								   
{
 PORTC|=BIT(6);
 PORTC&=~BIT(7);
}
void choose_138(void)
{
 PORTC|=BIT(7);
 PORTC&=~BIT(6);
}
void delay(uint mx)
{
 uint a,b;
 for(a=mx;a>0;a--)
  for(b=100;b>0;b--);
}
void display(void)
{
  if(flag==1)
  {
  angle=abs(num)*18/100;
  ge=angle%10;
  shi=angle%100/10;
  bai=angle%1000/100;
  qian=angle/1000;
  
  /*
  ge=numcw%10;
  shi=numcw%100/10;
  bai=numcw%1000/100;
  qian=numcw/1000;
  */
  }
  if(flag1==1)
  {
  angle=abs(num)*18/100;
  ge=angle%10;
  shi=angle%100/10;
  bai=angle%1000/100;
  qian=angle/1000;
  /*
  ge=numcww%10;
  shi=numcww%100/10;
  bai=numcww%1000/100;
  qian=numcww/1000;
  */
  }
  choose_573();
  PORTA=dua[ge];
  choose_138();
  PORTA=wei[0];
  delay_10us();

  choose_573();
  PORTA=dua[shi];
  choose_138();
  PORTA=wei[1];
  delay_10us();

  choose_573();
  PORTA=dua[bai];
  choose_138();
  PORTA=wei[2];
  delay_10us();

  choose_573();
  PORTA=dua[qian];
  choose_138();
  PORTA=wei[3];
  delay_10us();
}
void init(void)
{
 DDRA=0XFF;
 PORTA=0XFF;
 
 DDRB=0X00;
 PORTB=0XFF;

 DDRC=0XFF;
 PORTC=0XFF;			

 
 MCUCR|=0X03;      //设置int1.int0为下降沿触发方式 
 GICR |=BIT(6);//外部中断使能
 MCUCSR=(1<<ISC2);//INT2 enable
 //DDRD|=0X00;
 //PORTD|=0XFF;
 
 DDRD&=~BIT(2); //INT1设置为输入 
 PORTD|=BIT(2); //设置有上拉电阻
 DDRD&=~BIT(3); //INT1设置为输入 
 PORTD|=BIT(3); //设置有上拉电阻
 SREG |=BIT(7);
 
 num=0;
}
/*********电机正反转程序********************/
void motor_shu(uint angle)
{

}
void motor_ni(uint angle)
{

}

/*********功能1：一个周期转一周***********/
/*摆长0.86m，经过计算，周期为1.86s********/
void fun1(void)
{
  for(i=0;i<400;i++)
  {
  PORTC=CWW[i%8]; 
  //delay_nms(4);
  delay_n100us(2);
  //delay_10us();
  }
}
/*****************************************/
/*********功能2：一个硬币*****************/
void fun2(void)
{
 uint temp=abs(num);
 temp=temp+30;
 temp/=5;
 while(temp--)
 { 
  PORTC=CWW[temp%8];
  delay_n100us(2);
  delay_1us();delay_1us();delay_1us();delay_1us();delay_1us();
  /*
  while(i--)
  delay_1us();
  */
 }
}
/*****************************************/
/*********功能2：八个硬币*****************/
void fun3(void)
{
 //uint i=210,j=210;
 uint temp=abs(num);
 temp=temp+5;
 temp/=5;
 while(temp--)
 { 
  PORTC=CWW[temp%8];
  delay_n100us(2);
  delay_50us();
 }
}
/*****************************************/
/***********功能4：静态激光笔*************/
void fun4(void)//目前只能摆逆时针转
{
 b=abs(num);
 if(num<0)
 {
  b=b/5;
  a=Nfan[b];
  while(a--)
  {
   PORTC=CWW[a%8];
   delay_n100us(2);
  }
 }
 if(num>0)
 {
  b=b/5;
  a=Nzhe[b];
  while(a--)
  {
   PORTC=CW[a%8];
   delay_n100us(2);
  }
 }
}
/*****************************************/
/***********功能5：动态激光笔*************/
void fun5(void)//目前只能摆逆时针转
{
 b=abs(num);
 if(num<0)
 {
  b=b/5;
  a=Nfan[b];
  while(a--)
  {
   PORTC=CWW[a%8];
   delay_n100us(2);
  }
  a=Nfan[b];
 }
 if(num>0)
 {
  b=b/5;
  a=Nzhe[b];
  while(a--)
  {
   PORTC=CW[a%8];
   delay_n100us(2);
  }
  a=Nzhe[b];
 }   //在此之上可以完成功能4，接下来是功能5
 c=Nfan[b];
 //d=c;
 while(1)
 {
  b=abs(num); 
  if(num<0)
  { 
   b=b/5;
   for(;c<Nfan[b];c++)
   {
    PORTC=CW[c%8];
    delay_n100us(2);
   }
   for(;c>Nfan[b];c--)
   {
    PORTC=CW[c%8];
    delay_n100us(2);
   }
  }
  if(num>0)
  {
   b=b/5;
   for(;c>Nzhe[b];c--)
   {
	PORTC=CWW[c%8];
    delay_n100us(2);
   }
   for(;c<Nzhe[b];c++)
   {
	PORTC=CWW[c%8];
    delay_n100us(2);
   }
  }
 }
}

/*****************************************/
/*********功能:测试程序*******************/
void test(void)
{
  if(flag==1)
  {
   k=numcw/5;
   PORTC=CWW[k%8];
   display();
  }
  if(flag1==1)
  {
   k=numcww/5;
   PORTC=CW[k%8];
   display();
  }
}
/*****************************************/
void main(void)
{
 uchar temp;
 init();
 while(1)
 {
  temp=PINB&0x1f;
  switch(temp)
  {
   case 0x1e:
            {
             delay_nms(10);
	         if((PINB&0x1f)==0x1e)
	         fun1();
            }break;
   case 0x1d:  //0001 1101
   			{
			 uint time=abs(num);
			 delay_nms(10);
	         if((PINB&0x1f)==0x1d)
			 while(time<=(abs(num)+5));
			 fun2();
			 while(!((PINB&0x1f)==0x1d));
			 delay_nms(50);
			}break;
	case 0x1b:  //0001 1011
   			{
			 uint time=abs(num);
			 delay_nms(10);
	         if((PINB&0x1f)==0x1b)
			 while(time<=abs(num)+5);
			 fun3();
			 while(!((PINB&0x1f)==0x1b));
			 delay_nms(50);
			}break;
	case 0x17:  //0001 0111
   			{
			 delay_nms(10);
	         if((PINB&0x1f)==0x17)
			 {
			  delay_nms(100);
			  while(!((PINB&0x1f)==0x17));
			  fun4();
			  delay_nms(100);		  
			  while(!((PINB&0x1f)==0x17));
			  if(num<0)
			  {
			   for(a=0;a<Nfan[b];a++)
			   {
			    PORTC=CWW[a%8];
                delay_n100us(2);
			   }
			  }
			  if(num>0)
			  {
			   for(a=0;a<Nzhe[b];a++)
			   {
			    PORTC=CWW[a%8];
                delay_n100us(2);
			   }
			  }
			  delay_nms(50);
			  }
			}break;
   case 0x0f:
            {
 			 delay_nms(10);
	         if((PINB&0x1f)==0x0f)
			 {
			  delay_nms(100);
			  while(!((PINB&0x1f)==0x0f));
			  fun5();
             }
            }break;
   default :test();
  }
 }
}
/********中断判断正反转与计数转角**************/
#pragma interrupt_handler int0_isr:2
void int0_isr(void)
{
 if((PIND&BIT(PD3))==0)
 {
  num++;
  flag=1;
  flag1=0;
  numcww=0;
  numcw++; 
 }
 else
 {
  num--;
  flag=0;
  flag1=1;
  numcww++;
  numcw=0;
 }
}
/**************************************************/
/*
#pragma interrupt_handler int1_isr:3
void int1_isr(void)
{
 if(FLAG==1)
 if(flag==0)
 {
  FLAG=0;
  flag1=1;
 }
  numcw++;
  numcww=0;
  i=numcw/5;
  num=numcw;
}

#pragma interrupt_handler int2_isr:19
void int2_isr(void)
{
 FLAG=1;
 flag1=0;
 flag=0;
}
*/

