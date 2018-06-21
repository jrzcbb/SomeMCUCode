#ifndef __LCD12864_h__
#define __LCD12864_h__

#define uchar unsigned char
#define uint  unsigned int
#define delayNOP(); {_NOP();_NOP();_NOP();_NOP();};
/***********msp430IO定义******************/
#define EN BIT5
#define RW BIT4
#define RS BIT3
#define PSB BIT6
void Draw_PM(uchar *ptr1,uchar *ptr2);
void lcd_pos(uchar X,uchar Y);  //确定显示位置
void delay(int ms);
uchar lcd_busy();
void lcd_wcmd(uchar cmd);
void lcd_wdat(uchar dat);
void lcd_init();
void Draw_PM(uchar *ptr1,uchar *ptr2);
void printscreen(uchar *p1,uchar *p2,uchar *p3,uchar *p4);
uint forbit(uchar x);
void draw_point(uchar x,uchar y);
/*******************************************************************/
/*                                                                 */
/*  延时函数                                                       */
/*                                                                 */
/*******************************************************************/
void delay(int ms)
{
    while(ms--)
	{
      uchar i;
	  for(i=0;i<250;i++)  
	   {
	    _NOP();			   
		_NOP();
		_NOP();
		_NOP();
	   }
	}
}		
/*******************************************************************/
/*                                                                 */
/*检查LCD忙状态                                                    */
/*lcd_busy为1时，忙，等待。lcd-busy为0时,闲，可写指令与数据。      */
/*                                                                 */
/*******************************************************************/
uchar lcd_busy()
 {                          
    uchar result;
    P6OUT &=~ RS;
    P6OUT |= RW;
    P6OUT |= EN;
    delayNOP();
    P3DIR = 0X00;
    result = P3IN&BIT7;
    P6OUT &=~ EN;
    return(result); 
 }
/*******************************************************************/
/*                                                                 */
/*写指令数据到LCD                                                  */
/*RS=L，RW=L，E=高脉冲，D0-D7=指令码。                             */
/*                                                                 */
/*******************************************************************/
void lcd_wcmd(uchar cmd)
{                          
   while(lcd_busy());
    P6OUT &=~ RS;
    P6OUT &=~ RW;
    P6OUT &=~ EN;
    _NOP();
    _NOP(); 
    P3DIR = 0XFF;
    P3OUT = cmd;
    delayNOP();
    P6OUT |= EN;
    delayNOP();
    P6OUT &=~EN;  
}
/*******************************************************************/
/*                                                                 */
/*写显示数据到LCD                                                  */
/*RS=H，RW=L，E=高脉冲，D0-D7=数据。                               */
/*                                                                 */
/*******************************************************************/
void lcd_wdat(uchar dat)
{                          
   while(lcd_busy());
    P6OUT |= RS;
    P6OUT &=~RW;
    P6OUT &=~EN;
    P3DIR = 0XFF;
    P3OUT = dat;
    delayNOP();
    P6OUT |= EN;
    delayNOP();
    P6OUT &=~EN;
}
/*******************************************************************/
/*                                                                 */
/*  LCD初始化设定                                                  */
/*                                                                 */
/*******************************************************************/
void lcd_init()
{ 
    P6DIR = 0XFF;
    P6OUT = 0X00;
    P6OUT |= PSB;        //并口方式
    
    lcd_wcmd(0x34);      //扩充指令操作
    delay(5);
    lcd_wcmd(0x30);      //基本指令操作
    delay(5);
    lcd_wcmd(0x0C);      //显示开，关光标
    delay(5);
    lcd_wcmd(0x01);      //清除LCD的显示内容
    delay(5);
}
void Draw_PM(uchar *ptr1,uchar *ptr2)
{
    uchar i,j,k;
	lcd_wcmd(0x34);        //打开扩展指令集
	i = 0x80;            
	for(j = 0;j < 32;j++)
	{
        lcd_wcmd(i++);
        lcd_wcmd(0x80);
  		for(k = 0;k < 16;k++)
  		{
  		    lcd_wdat(*ptr1++);
  		}
	}
	i = 0x80;

 //如果是全副图片，应删去此句
 	for(j = 0;j < 32;j++)
	{
 	    lcd_wcmd(i++);
            lcd_wcmd(0x88);	   
  		for(k = 0;k < 16;k++)
  		{
   		    lcd_wdat(*ptr1++);
   		} 
	}  
    lcd_wcmd(0x36);        //打开绘图显示
	lcd_wcmd(0x30);        //回到基本指令集
}
/*
void draw_block(uchar *ptr1)
{
  lcd_wcmd(0x34);
  uchar i,j,k;
  i = 0x80;            
  for(j = 0;j < 32;j++)
  {
    lcd_wcmd(i++);
    lcd_wcmd(0x80);
    for(k = 0;k < 16;k++)
    {
      lcd_wdat(*ptr1++);
    }
  }
}
*/
uint forbit(uchar x)
{
  uint a;
  switch(x%16)
  {
  case 1: a = 0x0001;break;
  case 2: a = 0x0002;break;
  case 3: a = 0x0004;break;
  case 4: a = 0x0008;break;
  case 5: a = 0x0010;break;
  case 6: a = 0x0020;break;
  case 7: a = 0x0040;break;
  case 8: a = 0x0080;break;
  case 9: a = 0x0100;break;
  case 10: a = 0x0200;break;
  case 11: a = 0x0400;break;
  case 12: a = 0x0800;break;
  case 13: a = 0x1000;break;
  case 14: a = 0x2000;break;
  case 15: a = 0x4000;break;
  case 16: a = 0x8000;break;
  }
  return a;
}
void draw_point(uchar x,uchar y)
{
  lcd_wcmd(0x34);
  lcd_wcmd(0x34);
  /********写垂直地址*********/
  if(y/32 == 0)
    lcd_wcmd(0x80 + y);
  else
    lcd_wcmd(0x88 + y%32);
  /********写水平地址*********/
    lcd_wcmd(0x80 + x/16);
  /**********写数据***********/
    lcd_wcmd(forbit(16-(x%16))/256);
    lcd_wcmd(forbit(16-(x%16))%256);
   lcd_wcmd(0x36);
   lcd_wcmd(0x30);
}
void printscreen(uchar *p1,uchar *p2,uchar *p3,uchar *p4)
{
    uchar i;
    lcd_init();                //初始化LCD             
    lcd_pos(0,0);             //设置显示位置为第一行的第1个字符
     i = 0;
    while(*p1 != '\0')
     {                         //显示字符
       lcd_wdat(*p1++);
       i++;
     }
    lcd_pos(1,0);             //设置显示位置为第二行的第1个字符
     i = 0;
    while(*p2 != '\0')
     {
       lcd_wdat(*p2++);      //显示字符
       i++;
     }
 	 lcd_pos(2,0);             //设置显示位置为第三行的第1个字符
     i = 0;
    while(*p3 != '\0')
     {
       lcd_wdat(*p3++);      //显示字符
       i++;
     }
	 lcd_pos(3,0);             //设置显示位置为第四行的第1个字符
     i = 0;
    while(*p4 != '\0')
     {
       lcd_wdat(*p4++);      //显示字符
       i++;
     }
}
/*********************************************************/
/*                                                       */
/* 设定显示位置                                          */
/*                                                       */
/*********************************************************/
void lcd_pos(uchar X,uchar Y)
{                          
   uchar  pos;
   if (X==0)
     {X=0x80;}
   else if (X==1)
     {X=0x90;}
   else if (X==2)
     {X=0x88;}
   else if (X==3)
     {X=0x98;}
   pos = X+Y ;  
   lcd_wcmd(pos);     //显示地址
}
#endif