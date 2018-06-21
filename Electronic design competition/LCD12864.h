#ifndef __LCD12864_h__
#define __LCD12864_h__

#define uchar unsigned char
#define uint  unsigned int
#define delayNOP(); {_NOP();_NOP();_NOP();_NOP();};
/***********msp430IO����******************/
#define EN BIT5
#define RW BIT4
#define RS BIT3
#define PSB BIT6
void Draw_PM(uchar *ptr1,uchar *ptr2);
void lcd_pos(uchar X,uchar Y);  //ȷ����ʾλ��
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
/*  ��ʱ����                                                       */
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
/*���LCDæ״̬                                                    */
/*lcd_busyΪ1ʱ��æ���ȴ���lcd-busyΪ0ʱ,�У���дָ�������ݡ�      */
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
/*дָ�����ݵ�LCD                                                  */
/*RS=L��RW=L��E=�����壬D0-D7=ָ���롣                             */
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
/*д��ʾ���ݵ�LCD                                                  */
/*RS=H��RW=L��E=�����壬D0-D7=���ݡ�                               */
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
/*  LCD��ʼ���趨                                                  */
/*                                                                 */
/*******************************************************************/
void lcd_init()
{ 
    P6DIR = 0XFF;
    P6OUT = 0X00;
    P6OUT |= PSB;        //���ڷ�ʽ
    
    lcd_wcmd(0x34);      //����ָ�����
    delay(5);
    lcd_wcmd(0x30);      //����ָ�����
    delay(5);
    lcd_wcmd(0x0C);      //��ʾ�����ع��
    delay(5);
    lcd_wcmd(0x01);      //���LCD����ʾ����
    delay(5);
}
void Draw_PM(uchar *ptr1,uchar *ptr2)
{
    uchar i,j,k;
	lcd_wcmd(0x34);        //����չָ�
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

 //�����ȫ��ͼƬ��Ӧɾȥ�˾�
 	for(j = 0;j < 32;j++)
	{
 	    lcd_wcmd(i++);
            lcd_wcmd(0x88);	   
  		for(k = 0;k < 16;k++)
  		{
   		    lcd_wdat(*ptr1++);
   		} 
	}  
    lcd_wcmd(0x36);        //�򿪻�ͼ��ʾ
	lcd_wcmd(0x30);        //�ص�����ָ�
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
  /********д��ֱ��ַ*********/
  if(y/32 == 0)
    lcd_wcmd(0x80 + y);
  else
    lcd_wcmd(0x88 + y%32);
  /********дˮƽ��ַ*********/
    lcd_wcmd(0x80 + x/16);
  /**********д����***********/
    lcd_wcmd(forbit(16-(x%16))/256);
    lcd_wcmd(forbit(16-(x%16))%256);
   lcd_wcmd(0x36);
   lcd_wcmd(0x30);
}
void printscreen(uchar *p1,uchar *p2,uchar *p3,uchar *p4)
{
    uchar i;
    lcd_init();                //��ʼ��LCD             
    lcd_pos(0,0);             //������ʾλ��Ϊ��һ�еĵ�1���ַ�
     i = 0;
    while(*p1 != '\0')
     {                         //��ʾ�ַ�
       lcd_wdat(*p1++);
       i++;
     }
    lcd_pos(1,0);             //������ʾλ��Ϊ�ڶ��еĵ�1���ַ�
     i = 0;
    while(*p2 != '\0')
     {
       lcd_wdat(*p2++);      //��ʾ�ַ�
       i++;
     }
 	 lcd_pos(2,0);             //������ʾλ��Ϊ�����еĵ�1���ַ�
     i = 0;
    while(*p3 != '\0')
     {
       lcd_wdat(*p3++);      //��ʾ�ַ�
       i++;
     }
	 lcd_pos(3,0);             //������ʾλ��Ϊ�����еĵ�1���ַ�
     i = 0;
    while(*p4 != '\0')
     {
       lcd_wdat(*p4++);      //��ʾ�ַ�
       i++;
     }
}
/*********************************************************/
/*                                                       */
/* �趨��ʾλ��                                          */
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
   lcd_wcmd(pos);     //��ʾ��ַ
}
#endif