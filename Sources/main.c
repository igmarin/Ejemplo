
/*   
  size is 2*16 
  if do not need to read busy, then you can tie R/W=ground 
  ground  pin 3  = pin 1    Vss
  power   pin 1  = pin 2    Vdd   +5V
  ground  pin 3  = pin 3    Vlc   grounded for highest contrast
  PM5     pin 21 = pin 4    RS    (1 for data, 0 for control/status)
  PAD0    pin 18 = pin 5    R/W   (1 for read, 0 for write)
  PM4     pin 17 = pin 6    E     (enable)
  PM3     pin 23 = pin 14   DB7   (4-bit data)
  PM2     pin 19 = pin 13   DB6
  PM1     pin 26 = pin 12   DB5
  PM0     pin 28 = pin 11   DB4
 
  PT0     pin 13 = LED
  PT1     pin 15 = Switch 1
  PT2     pin 30 = Switch 2
  
16 characters are configured as 2 rows of 8
addr  00 01 02 03 04 05 06 07 40 41 42 43 44 45 46 47
*/
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      // Definiciones de la familia del micro 
#include <mc9s12c32.h>       /******* Definiciones específicas del micro *******/
#include "LCD.H"
#include "PLL.H"


#pragma LINK_INFO DERIVATIVE "mc9s12c32"

//--------------------TimerInit---------------
// initialize timer module to 0.667us(Boot Mode) TCNT clock
// inputs: none
// outputs: none
void TimerInit(void){
  DDRT  = 0x01;   // PT.0 como salida (LED), PT.1 y PT2 como entradas (Switch)
  DDRA  = 0x01;    // PA.0 como salida (LED)
  DDRB  = 0x04;    // PB.4 como salida (LED)
  TSCR1 = 0x80;   // Enable TCNT, 24MHz in both boo tand run modes
  TSCR2 = 0x04;   // divide by 16 TCNT prescale, TCNT at 667nsec
  PACTL = 0;      // timer prescale used for TCNT
/* Bottom three bits of TSCR2 (PR2,PR1,PR0) determine TCNT period
    divide  24MHz E clock    
000   1     42ns  TOF  2.73ms  
001   2     84ns  TOF  5.46ms   
010   4    167ns  TOF  10.9ms       
011   8    333ns  TOF  21.8ms 	 
100  16    667ns  TOF  43.7ms 	 
101  32   1.33us  TOF  87.4ms		 
110  64   2.67us  TOF 174.8ms   
111 128   5.33us  TOF 349.5ms    */ 
// Be careful, TSCR1 and TSCR2 maybe set in other rituals
}

//---------------------mwait---------------------
// wait specified number of msec
// Input: number of msec to wait
// Output: none
// assumes TCNT timer is running at 667ns
void mwait(unsigned short msec){ 
unsigned short startTime;
  for(; msec>0; msec--){
    startTime = TCNT;
    while((TCNT-startTime) <= 1500){} 
  }
}

//---------------------check---------------------
// if LCD broken toggle LED1 at 2Hz
// Input: last LCD status, 0 means bad
// Output: none
// Error: if status is zero, this function will not return
void check(short status){	 // 0 if LCD is broken
  if(status ==0){		   
    for(;;) {
      PORTA ^= 0x01;   // fast toggle LEDA
      mwait(250);    // 0.25 sec wait
    }
  }
}
//---------------------swait---------------------
// wait specified 2 sec, then clear LCD
// Input: none
// Output: none
// uses mswait and TCNT timer
void swait(void){			
  PORTB ^= 0x04;     // toggle LEDB
  mwait(2000);     // 2 sec wait
  check(LCD_Clear());
}

//---------------------blank---------------------
// move cursor to second half of LCD display
// 32 spaces from address 08 to 40
// Input: none
// Output: none
void blank(void){
  check(LCD_OutString("                        "));
}

void main(void) {
 // unsigned char cont;
  PLL_Init();       // set E clock to 24 MHz
  TimerInit();      // enable Port T 
  check(LCD_Open(LCDINC+LCDNOSHIFT,
    LCDNOCURSOR+LCDNOBLINK,
    LCDNOSCROLL+LCDLEFT,
    LCD2LINE+LCD7DOT));
  check(LCD_OutString("9S12C32         ")); blank();
  check(LCD_OutString("Valvano         ")); swait();
 //asm cli   // allows debugger to run
  for(;;) {
 /*  if ( (PTT & 0x02) != 0){
      if (cont<15){
         cont++;   
          mwait(50); ;
    } else if (cont>16){
              cont = 0;
              mwait(50);
    }
   }
    
   if ( (PTT & 0x04) != 0){
      if (cont>0){
         cont--;   
         mwait(50);
    } else if (cont<0){
              cont = 15;
              mwait(50);
    }
   }   */
    //check(LCD_OutString("ABCDEFGH  "));
    //short(LCD_OutDec(unsigned char *cont)); 
    //blank();
    check(LCD_OutString("IJKLMNOP 1 1 1 1")); swait();
    check(LCD_OutString("01234567 2 2 2 2")); blank();
    check(LCD_OutString("890,./<> 3 3 3 3")); swait();
    check(LCD_OutString("abcdefgh 4 4 4 4")); blank();
    check(LCD_OutString("ijklmnop 5 5 5 5")); swait();
    check(LCD_OutString("!@#$%^&* 6 6 6 6")); blank();
    check(LCD_OutString("()_+-=[] 7 7 7 7")); swait();
  } 
}
