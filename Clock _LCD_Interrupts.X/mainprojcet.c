/*
 * File:   mainprojcet.c
 * Author: hp
 *
 * Created on April 16, 2023, 3:07 PM
 */


#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "lcd_x8.h"
#include "my_adc.h"
#include "my_ser.h"
#include <string.h>
#include <proc/pic18f4620.h>

#define _XTAL_FREQ   4000000UL     // needed for the delays, set to 4 MH= your crystal frequency
// CONFIG1H
#pragma config OSC = XT         // Oscillator Selection bits (XT oscillator)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = ON         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

#define STARTVALUE  3036
void reloadTimer0( void );
void reloadTimer0( void )
{
    
    TMR0H = (unsigned char) ((STARTVALUE >>  8) & 0x00FF);
    TMR0L =  (unsigned char)(STARTVALUE & 0x00FF );
     
}


void reloadTimer3(void)
{  
    TMR3H = (unsigned char)((STARTVALUE >>  8) & 0x00FF);
    TMR3L = (unsigned char)(STARTVALUE & 0x00FF );   
}

void initTimers01(void) {
    T0CON = 0;
    
    INTCONbits.T0IF = 0;
    T0CONbits.T0PS0 = 1; 
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS2 = 0;
    TMR0H = (unsigned char) ((STARTVALUE >> 8) & 0x00FF);
    TMR0L = (unsigned char) (STARTVALUE & 0x00FF);
    T1CONbits.TMR1CS = 1; 
    T1CONbits.T1CKPS1 = 0;
    T1CONbits.T1CKPS0 = 0;
 
    INTCONbits.GIE = 1;
    INTCONbits.T0IE = 1;
    T1CONbits.TMR1ON = 1;
    T0CONbits.TMR0ON = 1;

}



void initialization(void) {
    INTCONbits.GIEH   = 1;    // Enable global interrupt bits
    INTCONbits.GIEL   = 1;    // Enable global interrupt bits
    INTCON2 = 0;
    INTCON3 = 0;
    INTCON2bits.INTEDG0 = 1;  // Interrupt 0 on rising edge
    INTCON2bits.INTEDG1 = 1;  // Interrupt 1 on rising edge
    INTCON2bits.INTEDG2 = 1;  // Interrupt 2 on rising edge
    INTCON3bits.INT1IE  = 1;  // Enable external interrupt 1
    INTCON3bits.INT2IE  = 1;  // Enable external interrupt 2
    RCONbits.IPEN = 0;        // Disable Interrupt priority , All are high
    INTCONbits.INT0IE   = 1;  // Enable external interrupt 0
    PORTCbits.RC5 = 0; // Turn Off Heater
    T3CONbits.TMR3ON = 1; 
    T0CON = 0;
    T0CONbits.T0PS0  = 1; // 16 Pre_Scalar
    T0CONbits.T0PS1  = 1;
    T0CONbits.T0PS2  = 0;
    T0CONbits.TMR0ON = 1;
    INTCONbits.TMR0IE = 1;    // Enable Timer0 Interrupt
    T0CONbits.TMR0ON  = 1;
}

void setupPorts(void) {
    ADCON0 = 0;
    ADCON1 = 0b00001100; 
    TRISB = 0xFF; 
    TRISC = 0x80; 
    TRISA = 0xFF; 
    TRISD = 0x00;
    TRISE = 0x00;
  
}
////////////////////////////////////////////////////
 int seconds = 0;
 int minutes = 0;
 int hours   = 0;
 float a0,a1,temp;
 unsigned char modes = 0; 
 unsigned char modeschange = 0; 
 signed short time = 0;
 int heater=0;
 int cooler=0;
 float p1;
 float voltage,t,sp;
 float Percent_Heat_Counter=0;
 int H=0,timer=0;






void modeschange1(void)
{
      INTCONbits.INT0IF = 0;
    if (modeschange == 0) modeschange = 1;
    else if (modeschange == 1) modeschange = 2;
    else if (modeschange == 2) modeschange = 3;
    else if (modeschange == 3) modeschange = 4;
    else if (modeschange == 4) modeschange = 5;
    else if (modeschange == 5) modeschange = 6;
    else if (modeschange == 6) modeschange = 0;


}

void Timer3_int(void){

if(timer==20){
        timer=0;
    }
    else if(timer <= (Percent_Heat_Counter/10) && Percent_Heat_Counter!=0){
        INTCON3bits.INT1IE=1;
        PORTCbits.RC5=1;   
        delay_ms(40);
        timer+=1;
    }
    else{
      INTCON3bits.INT1IE=0;
      PORTCbits.RC5=0;
      delay_ms(40);  
      timer+=1;
    }
    reloadTimer3();
}




void incrementClock(void) {

if(modeschange == 0){
        time++;
    }else if(modeschange == 1){
        time +=60;
    }else if(modeschange == 2){
        time +=3600;
    }
    
    if(time > 36000){
        time = 36000;
    }
     hours = time / 3600;
     minutes = (time - hours*3600)/60;
     seconds = time - hours*3600 - minutes*60;
        
}

void decrementClock(void) {
   if(modeschange == 0){
        time--;
    }else if(modeschange == 1){
        time -=60;
    }else if(modeschange == 2){
        time -=3600;
    }
    if(time < 0){
        time = 0;
    }
     hours = time / 3600;
     minutes = (time - hours*3600)/60;
     seconds = time - hours*3600 - minutes*60;
        
}




///////////////////////////////////



void  __interrupt(high_priority) highIsr(void)//new syntax
{
   if(INTCONbits.INT0IF) { modeschange1();} 
   else if(PIR2bits.TMR3IF){Timer3_int();PIR2bits.TMR3IF=0;}; 
   
}


void main(void) {
    char Buffer[32];       
    initTimers01();
    setupPorts();
    lcd_init();
    init_adc_no_lib() ;
    initialization();
  
  
   while(1) {
      CLRWDT();
      
      
      p1 = read_adc_voltage(0);
      voltage = read_adc_voltage(2);
      t = 300 * voltage;
      sp=t/5;
    
      lcd_gotoxy(1, 3);  
      sprintf(Buffer, "SP:%4.1fC ",sp);
      lcd_puts(Buffer);
      lcd_gotoxy(1, 2);
      sprintf(Buffer, "CT:%4.1fC ", t);
      lcd_puts(Buffer);
      
      if(PORTBbits.RB3 == 0&& modeschange==6){
          if(Percent_Heat_Counter==10)
              Percent_Heat_Counter=10;
          else
        Percent_Heat_Counter++;
        
      }
      if(PORTBbits.RB4 == 0 && modeschange==6){
          if(Percent_Heat_Counter==0)
              Percent_Heat_Counter=0;
          else  
        Percent_Heat_Counter--;
      }
      lcd_gotoxy(1, 4);
      sprintf(Buffer, "MD:%s", modeschange==0?"S  ":modeschange==1?"10S  ":modeschange==2?"M  ":modeschange==3?"10M  ":modeschange==4?"HR  ":modeschange==5?"HY  ":"HT  ");
      lcd_puts(Buffer);
      lcd_gotoxy(13, 4);
      sprintf(Buffer, "H:%d",Percent_Heat_Counter);
      lcd_puts(Buffer);
      

      
      
      
      
      
      
      
      
   }
}
