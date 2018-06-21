#include "msp430x14x.h"
#include "LCD12864.h"
#include "math.h"
#include "stdlib.h"
#define uchar unsigned char
#define uint unsigned int

//-------------alex-pid-------
/*
#define kp 6.5//12     //0.05比较合适
#define ki 1
#define kd 2//10
*/
#define N  8 // num of filter in
#define set_pos 1000
int last_err = 913,pre_err = 913,this_err = 913;
int pid_cal();
void zhongduan2();
void zhongduan3();
void zhongduan4();
void zhongduan6();
float kp = 6.5,ki = 1,kd = 2;
int pid_cal()
{
  int p_err, d_err, i_err, delta_control;
  
  p_err = this_err - last_err;
  i_err = this_err;
  d_err = this_err - 2*last_err + pre_err;
  
  //电机输入的控制增量计算
  delta_control =(int)(kp*p_err) + (int)(ki*i_err) + (int)(kd*d_err);
  return delta_control;
}

int num,max,//记录光电编码器输出脉冲数，以计算角度
    maxspeed = 300,//电机最大转速限制
    speed = 0;
uchar disnum[]="0123456789.";
uchar  fmotor=0,//将电机脉冲分频
       flag1=1,flag2=1,flag3=1,flag4= 1,//记录正反转
       zhongduanstop,//用来开关中断中的一条语句
       zhongduan,//用来选择终端中要执行的程序
       dir,//方向
       dir1;//电机转向
float  kiofpid = 1;//用于调节比例系数
int kongbaizhi1,kongbaizhi2,timeravla = 370,//左右空白值，初速
    deadroom;
uint fofsample = 10000;
void isrinit(void)   //外部中断
{
  P2DIR &=~ BIT0 + BIT1 + BIT4;
  P2IE = BIT1 + BIT4;
  P2IES = BIT1 + BIT4;
  P2OUT = BIT1;
  P2IFG &=~ BIT1 + BIT4;
  //P2DIR = 0XFF;   //初始化流水灯
}
void clockinit(void)     //高频时钟
{  
  unsigned int i;
  BCSCTL1 &=~ XT2OFF;   //打开高频晶体振荡器
  do
  {
  IFG1 &= ~OFIFG;                           // Clear OSCFault flag
  for (i = 0xFF; i > 0; i--);               // Time for flag to set
  }
  while ((IFG1 & OFIFG));                   // OSCFault flag still set?
  BCSCTL1 |= XTS;                           // ACLK= LFXT1= HF XTAL
  BCSCTL2 |= SELM_2 + SELS + SELM_3;                         // MCLK= LFXT1 (safe)
}
void motorstart()
{
  /*
  if(zhankongbi>=1600)
    zhankongbi = 1600;
  */
  P1DIR |= 0x0C;                            // P1.2 and P1.3 output
  P1SEL |= 0x0C;                            // P1.2 and P1.3 TA1/2 otions
  CCR0 = timeravla; //400.390                             // PWM Period
  CCTL1 = OUTMOD_7;                         // CCR1 reset/set
  CCR1 = 0;                               // CCR1 PWM duty cycle
  CCTL2 = OUTMOD_7;                         // CCR2 reset/set
  CCR2 = 0;                               // CCR2 PWM duty cycle
  TACTL = TASSEL_2 + MC_1;                  // SMCLK, up mode
}
void shiftspeed(int x)
{
  if(abs(x)> maxspeed)
  {
    if(x>=0) x = maxspeed;
    if(x <0) x = (-1*maxspeed);
  }
  if(abs(1000 + num)<=deadroom)
    x = 0;//限定中点在+-0.18*7°之间
  if(x >= 0)
  {
    CCTL1 = OUTMOD_7;                         // CCR1 reset/set
    CCR1 = (int)((x + kongbaizhi1)*kiofpid);  //90.80.70.30
  }
  if(x < 0)
  {
    CCTL2 = OUTMOD_7;                         // CCR1 reset/set
    CCR2 = (int)((-1*x + kongbaizhi2)*kiofpid);  //60.70.30
  }
}
void motorstop()
{
  P1DIR |= BIT2 + BIT3;
  P1OUT &=~ BIT2 + BIT3; 
  P1SEL &=~BIT2 + BIT3; 
}
void display(void)
{
      unsigned int temp;
      temp = abs(num);
      lcd_pos(1,3);
      if(num>=0)
      lcd_wdat('+');
      else       lcd_wdat('-');
      lcd_wdat(disnum[temp/1000]);
      lcd_wdat(disnum[temp%1000/100]);
      lcd_wdat(disnum[temp%100/10]);
      lcd_wdat(disnum[temp%10]); 

      temp = abs((-1*(speed + 4000)));
      lcd_pos(2,3);
      if((-1*(speed + 4000 + 10))>=0)
      lcd_wdat('+');
      else       lcd_wdat('-');
      lcd_wdat(disnum[temp/1000]);
      lcd_wdat(disnum[temp%1000/100]);
      lcd_wdat(disnum[temp%100/10]);
      lcd_wdat(disnum[temp%10]); 
      
      lcd_pos(0,3);
      lcd_wdat(disnum[flag1]);
      lcd_wdat(disnum[flag2]);           
}
void timerainit(void)
{
  /*
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  CCR0 = 50000;
  TACTL = TASSEL_2 + MC_2 + ID_3;                  // SMCLK, contmode ,8FEN 
  */
}
void timerbinit(void)   //PID采样频率为50hz
{
  //TBCCTL0 = CCIE;                           // TBCCR0 interrupt enabled
  TBCCR0 = fofsample;/*原来为50000，换为10000后不会抖动*/    //PID采样频率
  TBCTL = TBSSEL_2 + MC_1 + ID_3;                  // SMCLK, upmode
}
void delayst(unsigned long int time)
{
  while(time--);
}
void startpendulum(void)
{
  
}
/**********功能1****************/
void function1(void)
{
  deadroom = 15;
  zhongduan = 4;//选择外部中断程序4
  zhongduanstop = 0;
  motorstart();
  kongbaizhi1 = 0;//原来两个为50
  kongbaizhi2 = 0;
  timeravla  = 370;
  //speed = 0;
  while(1)
  {
    display();
    motorstart();                //必须执行
    shiftspeed(200);
    delayst(3000);
    motorstop();

    while(flag1);
    motorstart();                //必须执行
    shiftspeed(-200);
    delayst(4000);
    motorstop();

    while(flag2);
    motorstart();                //必须执行
    shiftspeed(250);
    delayst(5000);
    motorstop();
    while(1)
   {
      while(flag1);
      motorstart();                //必须执行
      shiftspeed(-120);
      delayst(4000);
      motorstop();

      while(flag2);
      motorstart();                //必须执行
      shiftspeed(120);
      delayst(5000);
      motorstop();
   }
  }  
}
/**********功能2****************/
void function2(void)
{
  deadroom = 0;
  zhongduan = 2;//选择外部中断程序2
  zhongduanstop = 0;
  
  kongbaizhi1 = 0;//原来两个为50
  kongbaizhi2 = 0;
  timeravla  = 370;
  motorstart();
  //speed = 0;
  while(1)
  {
    display();
    motorstart();                //必须执行
    shiftspeed(200);
    delayst(3000);
    motorstop();

    display();
    while(flag1);
    motorstart();                //必须执行
    shiftspeed(-200);
    delayst(4000);
    motorstop();

    display();  
    while(flag2);
    motorstart();                //必须执行
    shiftspeed(250);
    delayst(5000);
    motorstop();
     
    display();
    while(flag1);
      motorstart();                //必须执行
      shiftspeed(-250);
      delayst(5000);
      motorstop();

    display();
      while(flag2);
      motorstart();                //必须执行
      shiftspeed(250);
      delayst(5000);
      motorstop();

    display();
      while(flag1);
      motorstart();                //必须执行
      shiftspeed(-280);
      delayst(5000);
      motorstop();

    display();
      while(flag2);
      motorstart();                //必须执行
      shiftspeed(280);
      delayst(4000);
      motorstop();
      /*    
      display();
      while(flag1);
      motorstart();                //必须执行
      shiftspeed(-300);
      delayst(5000);
      motorstop();
      */
      
   while(1)
   {
      //while(1);
      //while(1)
      display();
      //while((num>0&&num<1000)||(num>-1000&&num<-900));
      while(num%2000<1000);
      motorstart();                //必须执行
      shiftspeed(200);
      delayst(4000);
      motorstop();
      while(num%2000>1000);      
   }
  }  
}
/**********功能3****************/
void function3(void)
{
  kp = 1.5;//5.9
  ki = 1.0;
  kd = 1.3;
  fofsample = 1000;
  deadroom = 5;
  kiofpid = 1;
  zhongduan = 3; //选择中断程序3
  zhongduanstop = 1;

  kongbaizhi1 = 30;//10
  kongbaizhi2 = 50;//20
  maxspeed = 350;
  timeravla  = 450;
  motorstart();
  //speed = 0;
  while(1)
  {
    while(flag1);
    while(flag3);
    display();
    motorstart();
    shiftspeed((-1*(speed + 4000)));
  }
}
/******************功能4**********************/
void function4(void)  //改过判断语句大于小于号
{
  deadroom = 15;
  zhongduan = 4;//选择外部中断程序4
  zhongduanstop = 0;
  motorstart();
  kongbaizhi1 = 0;//原来两个为50
  kongbaizhi2 = 0;
  timeravla  = 370;
  //speed = 0;
  while(1)
  {
    display();
    motorstart();                //必须执行
    shiftspeed(200);
    delayst(3000);
    motorstop();

    while(flag1);
    motorstart();                //必须执行
    shiftspeed(-200);
    delayst(4000);
    motorstop();

    while(flag2);
    motorstart();                //必须执行
    shiftspeed(250);
    delayst(5000);
    motorstop();

    while(flag1);
    motorstart();                //必须执行
    shiftspeed(-250);
    delayst(5000);
    motorstop();

      display();
      while(flag2);
      motorstart();                //必须执行
      shiftspeed(100);
      delayst(4000);
      motorstop();
     
      display();
      while(flag1);
      motorstart();                //必须执行
      shiftspeed(-140);           //换电池是，调节此参数
      delayst(4000);
      motorstop();            
      
      //while(1);
    num +=180;//作为丢失脉冲补偿
      while(abs(num)>=900);
      flag1 = 0;
      flag3 = 0;
    zhongduan = 3;

    while(1)
    {
      display();
      //motorstart();
      if((abs(num)>=800/*900*/)&&(abs(num)<=1200/*1100*/))
      {
        motorstart();
        shiftspeed((/*-1**/(speed + 4000 + 10)));
      }
      zhongduan = 3;
        kongbaizhi1 = 50;
        kongbaizhi2 = 50;
        timeravla  = 370;    
      //else motorstop();
    }      
  }
}
/****************功能5********************/
void funciton5(void)
{
  fofsample = 1000;
  deadroom = 15;
  kiofpid = 1.3;
  zhongduan = 3; //选择中断程序3
  zhongduanstop = 1;

  kongbaizhi1 = 20;//70
  kongbaizhi2 = 20;//75
  maxspeed = 450;
  timeravla  = 450;
  motorstart();
  //speed = 0;
  while(1)
  {
    while(flag1);
    while(flag3);
    display();
    motorstart();
    shiftspeed((-1*(speed + 4000)));
  } 
}
/****************功能6********************/
void function6(void)
{
  kp = 5.5;
  ki = 1.0;
  kd = 2.0;
  fofsample = 5000;
  deadroom = 15;
  kiofpid = 1.3;
  zhongduan = 6; //选择中断程序3
  zhongduanstop = 1;

  kongbaizhi1 = 20;//70
  kongbaizhi2 = 20;//75
  maxspeed = 450;
  timeravla  = 450;
  motorstart();
  //speed = 0;
  while(1)
  {
    while(flag1);
    while(flag3);
    display();
    motorstart();
    shiftspeed((-1*(speed + 4000)-60));
  }
}
void keyscan(void)
{
  unsigned char temp;
  P5DIR = 0X00;
  temp = P5IN & 0xff;
  switch(temp)
  {
  case 0xfe:function1();break;
  case 0xfd:function2();break;
  case 0xfb:function3();break;
  case 0xf7:function4();break;
  case 0xef:funciton5();break;
  case 0xdf:function6();break;
  case 0xbf:shiftspeed(130);break;
  case 0x7f:shiftspeed(-130);break;
  }
}
void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;
  lcd_init();     //LCD初始化
  clockinit();    //高频时钟
  isrinit();      //P1外部中断 
  timerbinit();
   _BIS_SR(GIE);
  //TBCCTL0 = CCIE;
  
  motorstart();
  timeravla = 370;
  
  while(1)
  {
    //function3();
    //function4();
    //function6();
    //shiftspeed(51);
    //display();zhongduan = 4;    
    keyscan();
  }
}
#pragma vector = PORT2_VECTOR
__interrupt void isr(void)
{
  _EINT();
  //motorstart();
  //shiftspeed(speed);
  P2IFG = 0X00;
    if((P2IN&0X01)==1)
    {
     num ++;
    }
    else 
    {
     num --;
    }
  switch(zhongduan)
  {   
    case 2:zhongduan2();break;
    case 3:zhongduan3();break;
    case 4:zhongduan4();break;
    case 6:zhongduan6();break;   
  } 
  max = num;
  P2IFG = 0X00;
}
// Timer B0 interrupt service routine
#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_B (void)
{
  _EINT();
  long temp = num;
  if(temp >=0)
    temp %= 2000;
  else temp %= -2000;
  pre_err  = last_err;  
  last_err = this_err;
  this_err = temp - set_pos;
  speed = pid_cal();
  //if(speed>1000&&speed<2000)
  //speed = speed - 2000;
  //if(speed>0&&speed<1000);
  speed = speed/5*10; 
}
void zhongduan2(void)  //write for function2
{
    if((max>num)&&(num>0)&&(abs(num)<500))
    {
     flag1 = 0;//从左往右下落，下平面
     flag2 = 1;
     //if(num ==0)
     //motorstop();
    }
    if((max<num)&&(num<0)&&(abs(num)<900))
    {
     flag2 = 0;//从右往左下落，下平面
     flag3 = 0;
     flag4 = 1;
     flag1 = 1;
     //if(num ==0)
     //motorstop();
    }
    else 
    {
      flag3 = 1;
      flag4 = 0;
    }
    max = num;
    /*
    if(abs(num)>=1000)
    num = -1000;
    */
}
void zhongduan3(void)  //write for function3
{   
    if((max<num)&&(num<0))
    {
     flag2 = 1;//从右往左下落，下平面
     flag1 = 0;
     dir = 1;
    } 
    if((max<num)&&(num>0))
    {
     flag2 = 1;//从右往左下落，下平面
     flag1 = 0;
     dir = 0;
    } 
    if(num<0)
      dir = 1;
    else dir =0;
    if(abs(num)>=900&&abs(num)<=1100)
    {
      //motorstart();  // 打开震动，关闭平稳
      TBCCTL0 = CCIE;
      flag3 = 0; 
    }
    else 
    {
      TBCCTL0 |= CCIE;
      //TBCCTL0 &=~ CCIE;
      motorstop();
      flag3 = 1;
      /*   //是否能解决回过负向脉冲  是
      pre_err = 0;
      last_err = 0;
      this_err = 0;
      */
    }
  if(abs(num)>=4000)
    num=0;
}
void zhongduan4(void) //write for function4
{
    if((max<num)&&(num<0))
    {
     flag2 = 0;//从右往左下落，下平面
     flag1 = 1;//功能3时需要改正
     dir = 1;
    } 
    if((max>num)&&(num>0))
    {
     flag2 = 1;//从左往右下落，下平面
     flag1 = 0;
     dir = 0;
    }
    /*
    if(num<0)
      dir = 1;
    else dir =0;
    */
    /*
    if(abs(num)>=900&&abs(num)<=1100)
    {
      motorstart();
      TBCCTL0 = CCIE;
      flag3 = 0;
    }
    else 
    {
      TBCCTL0 |= CCIE;
      //TBCCTL0 &=~ CCIE;
      if(zhongduanstop == 1)
      {
        motorstop();
      }
      flag3 = 1;
      pre_err = 0;
      last_err = 0;
      this_err = 0;
    }
    */
  if(abs(num)>=4000)
    num=0;
  //num %= 1000;  
}
void zhongduan6(void)
{
    if((max<num)&&(num<0))
    {
     flag2 = 1;//从右往左下落，下平面
     flag1 = 0;
     dir = 1;
    } 
    if((max<num)&&(num>0))
    {
     flag2 = 1;//从右往左下落，下平面
     flag1 = 0;
     dir = 0;
    } 
    if(num<0)
      dir = 1;
    else dir =0;
    if(abs(num)>=700&&abs(num)<=1400)
    {
      //motorstart();  // 打开震动，关闭平稳
      TBCCTL0 = CCIE;
      flag3 = 0; 
    }
    else 
    {
      TBCCTL0 |= CCIE;
      //TBCCTL0 &=~ CCIE;
      motorstop();
      flag3 = 1;
      /*   //是否能解决回过负向脉冲  是
      pre_err = 0;
      last_err = 0;
      this_err = 0;
      */
    }
  if(abs(num)>=4000)
    num=0;  
}