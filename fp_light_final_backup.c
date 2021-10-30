/*
 * File:   fp_combine_alarm_ring.c
 * Author: drjui
 *
 * Created on December 23, 2017, 5:41 PM
 */

#include <xc.h>
// CONFIG1H
#pragma config OSC = INTIO67    // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
// CONFIG3H
#pragma config PBADEN = ON     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
// CONFIG4L
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)

// CONFIG1H

#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = ON       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H

#pragma config WDTPS = 1        // Watchdog Timer Postscale Select bits (1:1)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)

#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)


// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)

#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define ZERO 0x03  // 1111 1100
#define ONE  0x9F  // 0110 0000
#define TWO  0x25  // 1101 1010
#define THREE  0x0D  // 1111 0010
#define FOUR  0x99  // 0110 0110
#define FIVE  0x49  // 1011 0110
#define SIX  0x41  // 1011 1110
#define SEVEN  0x1F  // 1110 0000
#define EIGHT  0x01  // 1111 1110
#define NINE  0x09  // 1111 0110

// choose LATC as the a~h of the displayer
#define DIS_NUM_OUT LATD
#define DIS_NUM_OUT_TRIS TRISD

// 250kHz low node
#define DO_T 58
#define DO_DC 119
#define RE_T 52
#define RE_DC 106
#define MI_T 46
#define MI_DC 94
#define FA_T 44
#define FA_DC 90
#define SO_T 39
#define SO_DC 80
#define LA_T 34
#define LA_DC 71
#define SI_T 31
#define SI_DC 63


int sec = 57*4; // for set
int min = 59; // for set
int hour = 9; // for set
int t1_flag = 0;
int but_flag = 0;
int ir_flag = 0;
//int but_flag_start = 0;

int his_min[10] = {0};
int his_hour[10] = {0};
int his_count = 0;

int his_dis_poi = 100;    
// history display pointer, 
//100 : display current time, 
//0 : display first history,
//1 : display second history, etc.

int last_upd_hdp = 0;   // value of sec when last updating his_dis_poi

int di = 0;

int std_count = 1;  // for set (real num, not start from 0)
int std_hour[10] = {10};  // for set
int std_min[10] = {00};  // for set
int std_check = 0;
int ring_flag = 0;

    int T[76] =  {MI_T , 0, MI_T , 0, RE_T , 0, MI_T , 0, RE_T , 0, MI_T , 0, MI_T , 0, MI_T , 0, RE_T , 0, MI_T , 0, SO_T , 0, RE_T , 0,
				  RE_T , 0, DO_T , 0, RE_T , 0, RE_T , 0, RE_T , 0, RE_T , 0, MI_T , 0, FA_T , 0, MI_T , 0,
				  MI_T , 0, MI_T , 0, RE_T , 0, DO_T , 0, MI_T , 0, MI_T , 0, MI_T , 0, RE_T , 0, MI_T , 0, FA_T , 0, MI_T , 0, DO_T + 12, 0,
				  FA_T , 0, FA_T , 0, MI_T , 0, RE_T , 0, DO_T , 0}; // PR2 value
	int dc[76] = {MI_DC , 0, MI_DC , 0, RE_DC , 0, MI_DC , 0, RE_DC , 0, MI_DC , 0, MI_DC, 0, MI_DC , 0, RE_DC , 0, MI_DC , 0, SO_DC , 0, RE_DC , 0,
				  RE_DC , 0, DO_DC , 0, RE_DC , 0, RE_DC , 0, RE_DC , 0, RE_DC , 0, MI_DC , 0, FA_DC , 0, MI_DC , 0,
				  MI_DC , 0, MI_DC , 0, RE_DC , 0, DO_DC , 0, MI_DC , 0, MI_DC , 0, MI_DC , 0, RE_DC , 0, MI_DC , 0, FA_DC , 0, MI_DC , 0, DO_DC + 8 , 0,
				  FA_DC , 0, FA_DC , 0, MI_DC , 0, RE_DC , 0, DO_DC , 0}; // PR2 value
	int beat_length[76] = {4, 1, 2, 1, 1, 1, 2, 1, 1, 1, 4, 1, 4, 1, 2, 1, 1, 1, 2, 1, 4, 1, 7, 0,
	                       2, 1, 1, 1, 7, 3, 4, 1, 2, 1, 1, 1, 2, 1, 4, 1, 6, 1,
						   4, 1, 3, 1, 1, 1, 1, 1, 6, 1, 4, 1, 2, 1, 1, 1, 1, 1, 4, 1, 4, 1, 10, 0,
						   1, 1, 2, 1, 4, 1, 4, 1, 10, 1};

void change_time(void);
void display_round(int dis_min, int dis_hour);
int diff(void);
void alarm_ring(void);
void PWM2_Duty(unsigned int duty);
void interrupt tc_int(void);

void MyadcInit();
void MyadcRead();
unsigned int MyadcValue;


void main(void) {
    
    int i, j;
	
	MyadcInit(); 
    
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;   // the above four bits is the common anode pins of the displayer
    TRISAbits.TRISA4 = 0;   // the anode of the single displayer
    
    TRISAbits.TRISA7 = 1;   // the button input

    DIS_NUM_OUT_TRIS = 0;
    DIS_NUM_OUT = 0;

    
    RCONbits.IPEN = 0x01;   // Enable priority levels on interrupts
    INTCONbits.GIE = 1;     // When IPEN = 1, Enables all high priority interrupts
	
	INTCON3bits.INT1IE = 1;	// INT1 External Interrupt Enable bit
	INTCON3bits.INT2IE = 1;
	INTCON3bits.INT1IP = 1;	// INT1 External Interrupt Priority bit
	INTCON3bits.INT2IP = 1;
	INTCON3bits.INT1IF = 0;	// INT1 External Interrupt Flag bit
	INTCON3bits.INT2IF = 0;
	INTCON2bits.INTEDG1 = 0; // button : trigger on falling edge
	INTCON2bits.INTEDG2 = 1; // IR sensor : trigger on rising edge
	
	TRISBbits.TRISB1 = 1; // input
	TRISBbits.TRISB2 = 1; // input
	
    
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 0;
    OSCCONbits.IRCF0 = 0;  // Internal Oscillator Frequency Select : 1m_Hz
    
    T1CON = 0x85;  // 10000101  bit 5-4 : 10 = 1:1 Prescale value
    PIE1bits.TMR1IE = 1;
    IPR1bits.TMR1IP = 1;
    PIR1bits.TMR1IF = 0;
    
    TRISCbits.TRISC2 = 1; // sensor input signal
    TRISCbits.TRISC0 = 0; // testing output
    TRISBbits.TRISB0 = 0; // testing output
    //TRISBbits.TRISB1 = 0; // testing output
    TRISCbits.TRISC3 = 0; // output : for the reminding LED string
    //TRISCbits.TRISC1 = 0; // set RC1 output  i.e. CCP2
    
    CCP2CONbits.CCP2M3 = 1;  //set CCP2 to PWM mode
	CCP2CONbits.CCP2M2 = 1;
	CCP2CONbits.CCP2M1 = 0;
	CCP2CONbits.CCP2M0 = 0;
    
    T2CON=0x00;             // off, postscale : 1, prescale : 1
    PIE1bits.TMR2IE=0;       // timer2 interrupt off   NOT SURE
    LATCbits.LATC3 = 0;     // set reminding LED string off
    
    if(std_count > 0) {
        for(i = std_count-1; i >= 0; --i) {
            if(std_hour[i] > hour ||(std_hour[i] == hour && std_min[i] > min))
                std_check = i;
        }
    }
    
    t1_flag = 0;
    but_flag = 0;
	ir_flag = 0;
    ring_flag = 0;
    
    LATCbits.LATC0 = 0;
    LATBbits.LATB0 = 0;
    LATBbits.LATB1 = 0;
    
    while(1) {
        //-------------------info.-collecting functions(ISR-similar)-------
        //if(PORTAbits.RA7 == 0) {   
        //        but_flag = 1;
        //}
        
        if(std_count > 0 && hour == std_hour[std_check] && min == std_min[std_check]) {
            if(his_count < std_check+1)
                ring_flag = 1;
            ++std_check;
            if(std_check == std_count)
                std_check = 0;
        }

        //-----------------------responding functions-----------------------
        if(t1_flag == 1) {
            change_time(); 
			MyadcRead();
        }
        if(but_flag == 1) { // may updates displayer
            but_flag = 0;
            if(!(sec >= last_upd_hdp && sec - last_upd_hdp <= 1) && his_count > 0) {
                if(his_dis_poi == 100 )
                    his_dis_poi = 0;
                else if(his_dis_poi == his_count-1)
                    his_dis_poi = 100;
                else
                    ++his_dis_poi;
                last_upd_hdp = sec;
            }
        }
        
        // do the 4-number display
        if(his_dis_poi != 100) {  
            display_round(his_min[his_dis_poi], his_hour[his_dis_poi]);
        } else {
            //for(i = 0; i < 10; ++i)  // brighten
                display_round(min, hour);
        }
        
        // infrared sensor
		if(his_count > 0)
			di = diff();
		else 
			di = 1;
        if(ir_flag == 1 && di >= 1  )
        {   
            ir_flag = 0;
            LATCbits.LATC0 = 1;
            his_hour[his_count] = hour;
            his_min[his_count] = min;
            his_count++;
        }
		
		if(MyadcValue < 0x10 && ring_flag == 0)
			LATCbits.LATC3 = 1;
		else
			LATCbits.LATC3 = 0;
        
        // reminding response
        if(ring_flag == 1) {
            alarm_ring();
        }
		

    }
    return;
}

void interrupt tc_int(void)     // High priority interrupt
{
    if(TMR1IE && TMR1IF){
        TMR1IF=0;
        t1_flag = 1;
        TMR1H = 0x0B;
        TMR1L = 0xDB;
    }
	if(INT1IE && INT1IF) { // button input
		INT1IF = 0;
		but_flag = 1;
	}
	if(INT2IE && INT2IF) { // IR sensor input
		INT2IF = 0;
		ir_flag = 1;
	}
}

void change_time(void) {
	int i = 0;
    t1_flag = 0;
    sec++;
            
    if(sec == 240) {
        sec = 0;
        min++;
    }
    if(min == 60) {
        min = 0;
        hour++;
    }
    if(hour == 24)
        hour = 0;
	if(sec == 0 && min == 0 && hour == 0) {
		for(i = 0; i < 10; ++i) {
			his_min[i] = 0;
			his_hour[i] = 0;
		}
		his_count = 0;
	}
}


void display_round(int dis_min, int dis_hour) {
    static int pre_min = 0;
    static int pre_hour = 0;
    static int dis_num_0 = 0;
    static int dis_num_1 = 0;
    static int dis_num_2 = 0;
    static int dis_num_3 = 0;
    int tmp;
    int tmp_com_ano_out = 0;
    int tmp_dis_num_out = 0;
    int i, j;
    
    LATBbits.LATB1 = 1;
    
    for(i = 0; i < 5; i++) {
    for(j = 4; j >= 0; j--) {
        if(i == 0) {
            if(dis_min != pre_min)
                dis_num_0 = dis_min % 10;
            tmp = dis_num_0;
        }
        else if(i == 1) {
            if(dis_min != pre_min)
                dis_num_1 = dis_min / 10;
            tmp = dis_num_1;
        }
        else if(i == 2) {
            if(dis_hour != pre_hour)
                dis_num_2 = dis_hour % 10; 
            tmp = dis_num_2;
        }
        else if(i == 3) {
            if(dis_hour != pre_hour)
                 dis_num_3 = dis_hour / 10;
            tmp = dis_num_3;
        } else
            tmp = his_count;
        DIS_NUM_OUT = 0xFF;  // remove the 'twilight'
        switch(tmp) {
            case 0: tmp_dis_num_out = ZERO; break;
            case 1: tmp_dis_num_out = ONE; break;
            case 2: tmp_dis_num_out = TWO; break;
            case 3: tmp_dis_num_out = THREE; break;
            case 4: tmp_dis_num_out = FOUR; break;
            case 5: tmp_dis_num_out = FIVE; break;
            case 6: tmp_dis_num_out = SIX; break;
            case 7: tmp_dis_num_out = SEVEN; break;
            case 8: tmp_dis_num_out = EIGHT; break;
            case 9: tmp_dis_num_out = NINE; break;
        }
        
        if(i == 2)
            --tmp_dis_num_out;

        LATAbits.LATA0 = 0;
        LATAbits.LATA1 = 0;
        LATAbits.LATA2 = 0;
        LATAbits.LATA3 = 0;
        LATAbits.LATA4 = 0;
        switch(i) {
            case 0: LATAbits.LATA0 = 1; break;
            case 1: LATAbits.LATA1 = 1; break;
            case 2: LATAbits.LATA2 = 1; break;
            case 3: LATAbits.LATA3 = 1; break;
            case 4: LATAbits.LATA4 = 1; break;
        }
        
        DIS_NUM_OUT = tmp_dis_num_out;
    }
    }
    
    pre_min = dis_min;
    pre_hour = dis_hour;
    
    LATBbits.LATB1 = 0;
}

int diff(void)
{
    int difference;
    if(hour == his_hour[his_count-1])
    {
        difference = min - his_min[his_count-1];
    }else
    {
        difference = (hour - his_hour[his_count-1])*60 + min -his_min[his_count-1];
    }
    return difference;
}

void alarm_ring(void) {
    static int x = 0;

    // 1mHz low
    //int T[14] =  {DO_T , 0, DO_T , 0, SO_T , 0, SO_T , 0, LA_T , 0, LA_T , 0, SO_T , 0}; // PR2 value
	//int dc[14] = {DO_DC, 0, DO_DC, 0, SO_DC, 0, SO_DC, 0, LA_DC, 0, LA_DC, 0, SO_DC, 0};  // 10-bit value    
	//int beat_length[14] = {4, 1, 4, 1, 4, 1, 4, 1, 4, 1, 4, 1, 8, 1};

	long long int duration = 2*4;	// greater then (the song) slower
    
	static long int cycount = 0;
	
	if(cycount == 0) {	// time to change the note
		if(x == 0) {	// PWM starting config.
			T2CON=0x7e;             // on, postscale : 16, prescale : 16
			PIE1bits.TMR2IE=0;       // set timer2 interrupt on   NOT SURE
            TRISCbits.TRISC1 = 0; // set RC1 output  i.e. CCP2
		}
        PWM2_Duty(dc[x]);	// corresponding to dc
		PR2 = T[x];	// corresponding to T
	}
    if(PR2 == 0)            // setting reminding LED string
        LATCbits.LATC3 = 0;
    else
        LATCbits.LATC3 = 1;
    if(cycount > beat_length[x]*duration) {
        cycount = -1;
        ++x;
        if(x == 76) {
            x = 0;
            T2CON=0x00;             // off, postscale : 1, prescale : 1
            PIE1bits.TMR2IE=0;       // timer2 interrupt off   NOT SURE
            LATCbits.LATC3 = 0;     // set reminding LED string off
            TRISCbits.TRISC1 = 1; // off RC1  i.e. CCP2
            cycount = -1;
            ring_flag = 0;
        }
    }
    ++cycount;
}

void PWM2_Duty(unsigned int duty)
{
//set duty to CCPR1L , CCP1X(CCP1CON:5) and CCP1Y(CCP1CON:4)
    // i.e. translate and store duty to the ten-bit storage
    CCPR2L = duty/4;
    CCP2CONbits.DC2B1 = (duty / 2) % 2;
    CCP2CONbits.DC2B0 = duty % 2;
}


void MyadcInit()
{
    //TODO
    ADCON1bits.VCFG1 = 0 ;  //setting vref-     //refer datasheet p224
    ADCON1bits.VCFG0 = 0 ;  //setting vref+
	
	
    ADCON1bits.PCFG  = 7 ;  //Setting A/D Port Configuration Control    //refer datasheet p224
	
    ADCON0bits.CHS = 7 ;    //setting input channel  AN1
    TRISEbits.TRISE2 = 1 ;             //setting port as input 
	
    ADCON2bits.ADFM =  1;    //setting left justified	//both left and right is OK, NOT SURE
    
     
    //setting acquisition time (ADCON2) ACQT 2:0
    //setting conversion time (ADCON2))
    ADCON2bits.ACQT = 1;                                   
	// Tacq = 2 Tad (in this lab, >= 2.4us)(refer lab pdf p14)
    ADCON2bits.ADCS = 0;    // Tad = FOSC/2
	
    ADCON0bits.ADON = 1;    //turn on ad module

    //INTCONbits.GIE = 1;
    PIR1bits.ADIF = 0;
    //PIE1bits.ADIE = 1;
 
    return ;
}

void MyadcRead()
{
    
    ADCON0bits.GO_NOT_DONE = 1;
    while(ADCON0bits.GO_NOT_DONE) {
        ;
    }
    MyadcValue = ADRESH*256 + ADRESL;
    return ;
}
