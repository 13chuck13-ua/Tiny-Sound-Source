/* ============================================================================
 *   _______             ____                  __  ____
 *  /_  __(_)__  __ __  / __/__  __ _____  ___/ / / __/__  __ _____________
 *   / / / / _ \/ // / _\ \/ _ \/ // / _ \/ _  / _\ \/ _ \/ // / __/ __/ -_)
 *  /_/ /_/_//_/\_, / /___/\___/\_,_/_//_/\_,_/ /___/\___/\_,_/_/  \__/\__/
 *             /___/
 *  Tiny Sound Source
 *  Disney Sound Source replica project with Covox Speech Thing compatible mode
 *
 *  ver: 1.0
 *  author: 13chuck13
 *  date: 09.10.2026
 * ----------------------------------------------------------------------------
 *  ATtiny-2313 pinout:
 *                  -----
 *                -|o    |- VCC (+5V)
 *      (LED) PD0 -|     |- PB7 -
 *                -|     |- PB6  |
 *          XTAL2 -|     |- PB5  |
 *          XTAL1 -|     |- PB4  |- (LPT data)
 *  (STROBE) INT0 -|     |- PB3  |
 *  (SELECT) INT1 -|     |- PB2  |
 *  (DSS cap) PD4 -|     |- PB1  |
 *     (PWM) OC0B -|     |- PB0 -
 *            GND -|     |- PD6 (FIFO-FULL)
 *                  -----
 *
 *  Description:
 *    PB0-PB7:      8 bit data input from LPT
 *    OC0B:         Sound output (need external RC filter)
 *    INT0:         STROBE input from LPT
 *    INT1:         SELECT input from RC low-pass filter
 *    PD0:          Mode-LED
 *    PD4:          Additional capacitor for low-pass filtering in DSS mode
 *    PD6:          FIFO-FULL output
 *    XTAL1/XTAL2:  External crystal oscillator (16 MHz)
 *
 *
 *  Clock:   external oscillator 16 MHz
 *
 *  Fuses:
 *           LFUSE = 0xDF
 *           HFUSE = 0xD1
 *           EFUSE = 0xFF
 *           LOCK = 0xFF
 *
   ---------------------------------------------------------------------------- */


#include <avr/io.h>
#include <avr/interrupt.h>


typedef unsigned char byte;


#define F_CPU 16000000UL
#define FIFO_SIZE  32               /* ОБОВ'ЯЗКОВО двійка в ступені */
#define FIFO_MASK  (FIFO_SIZE - 1)
#define DAC_MID 128                 /* значення idle-state для ШІМ */
#define MODE_COVOX 0
#define MODE_DSS 1


volatile byte fifo[FIFO_SIZE];
volatile byte index_wr = 0;
volatile byte index_rd = 0;
volatile byte fifo_counter = 0;
volatile byte mode = MODE_COVOX;
volatile byte value_prev = DAC_MID;


void init(void);
inline void fifo_push(byte data);
inline byte fifo_pop(void);


/* ------------------------------------------------------- */


ISR(INT0_vect)  /* запис вхідних даних у DSS FIFO */
{
    byte data;

    data = PINB;
    fifo_push(data);
}


ISR(INT1_vect)  /* зміна режиму Covox/DSS */
{
    if(mode == MODE_COVOX)
    {
        mode = MODE_DSS;
        MCUCR &= ~(1 << ISC10);     /* INT1 по задньому фронту сигналу */
        MCUCR |=  (1 << ISC11);
        index_wr = 0;               /* скидаємо значення FIFO */
        index_rd = 0;
        fifo_counter = 0;
        PORTD &= ~(1 << PD6);       /* FIFO-FULL off */
        TCNT1 = 0;                  /* скидаємо таймер */
        TIMSK |= (1 << OCIE1A);     /* вмикаємо Timer1 */
        GIMSK |= (1 << INT0);       /* вмикаємо переривання по STROBE */
        PORTD &= ~(1 << PD0);       /* вмикаємо додатковий low-pass на виході PWM */
        DDRD |= (1 << PD4);
        PORTD |= (1 << PD0);        /* LED on */
    }
    else    /* mode == MODE_DSS */
    {
        mode = MODE_COVOX;
        MCUCR |= (1 << ISC11) | (1 << ISC10);   /* INT1 по передньому фронту сигналу */
        TIMSK &= ~(1 << OCIE1A);                /* вимикаємо Timer1 */
        PORTD &= ~(1 << PD6);                   /* FIFO-FULL off */
        GIMSK &= ~(1 << INT0);                  /* вимикаємо переривання по STROBE */
        DDRD &= ~(1 << PD4);                    /* вимикаємо додатковий low-pass на виході PWM */
        PORTD &= ~(1 << PD0);                   /* LED off */
    }
}


ISR(TIMER1_COMPA_vect)  /* відтворення звуку DSS */
{
    byte value;

    value = fifo_pop();
    if(value != value_prev)
    {
        OCR0B = value;      /* пишемо чергове значення в PWM */
        value_prev = value;
    }
}


int main(void)
{
    byte sample_1;
    byte sample_2;

    init();
    sei();  /* глобальний дозвіл переривань */

    for (;;)
    {
        while(mode == MODE_COVOX)
        {
            sample_1 = PINB;
            sample_2 = PINB;
            while(sample_1 == sample_2)
            {
                sample_2 = PINB;
            }
            __asm__ __volatile__("nop");    /* чекаємо стабільного стану на шині */
            __asm__ __volatile__("nop");
            __asm__ __volatile__("nop");
            __asm__ __volatile__("nop");
            __asm__ __volatile__("nop");
            OCR0B = PINB;
        }
    }
}


void init(void)
{
    /* GPIO */
    DDRB = 0x00;            /* всі піни PB як вхід */
    DDRD &= ~(1 << PD2);    /* PD2(INT0) як вхід */
    DDRD &= ~(1 << PD3);    /* PD3(INT1) як вхід */
    DDRD &= ~(1 << PD4);    /* PD4(DSS cap) як вхід */
    DDRD |= (1 << PD0);     /* PD0(MODE-LED) як вихід */
    DDRD |= (1 << PD5);     /* PD5(OC0B/PWM) як вихід */
    DDRD |= (1 << PD6);     /* PD6(FIFO-FULL) як вихід */
    PORTD &= ~(1 << PD0);   /* PD0(MODE-LED) = OFF */
    PORTD &= ~(1 << PD6);   /* PD6(FIFO-FULL) = LOW */
    PORTB = 0xFF;           /* увімкнути підтягуючі резистори на PB */
    PORTD |= (1 << PD2);    /* увімкнути підтягуючий резистор на INT0 */
    PORTD |= (1 << PD3);    /* увімкнути підтягуючий резистор на INT1 */

    /* INT */
    MCUCR &= ~(1 << ISC00);                 /* INT0 по задньому фронту сигналу */
    MCUCR |=  (1 << ISC01);
    MCUCR |= (1 << ISC11) | (1 << ISC10);   /* INT1 по передньому фронту сигналу */
    GIMSK &= ~(1 << INT0);                  /* заборона зовнішнього переривання INT0 */
    GIMSK |= (1 << INT1);                   /* дозвіл зовнішнього переривання INT1 */

    /* Timer */
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS10);
    /* Wingardium leviosa! (чарівна константа в коді :) */
    OCR1A = 1142;                           /* 14 KHz */
    TIMSK &= ~(1 << OCIE1A);                /* вимикаємо Timer1 */

    /* PWM */
    TCCR0A =                /* 62 KHz 256 відліків */
        (1 << WGM00) |
        (1 << WGM01) |
        (1 << COM0B1);
    TCCR0B =
        (1 << CS00);
    OCR0B = DAC_MID;
}


inline void fifo_push(byte data)
{
    if (fifo_counter > FIFO_SIZE - 2)   /* нема двох вільних байтів */
    {
        /* Ми не мали тут опинитись, але ось ми тут */
        return;
    }

    if (fifo_counter == 0)
    {
        fifo[index_wr] = (DAC_MID + data) / 2;
    }
    else
    {
        fifo[index_wr] = (fifo[(index_wr - 1) & FIFO_MASK] + data) / 2;
    }

    fifo[(index_wr + 1) & FIFO_MASK] = data;

    index_wr = (index_wr + 2) & FIFO_MASK;
    fifo_counter += 2;

    if (fifo_counter > FIFO_SIZE - 2)
    {
        PORTD |= (1 << PD6);   /* високий рвіень на FIFO-FULL */
    }
}


inline byte fifo_pop(void)
{
    byte data;

    if (fifo_counter == 0)  /* буфер порожній */
    {
        return DAC_MID;
    }

    data = fifo[index_rd];
    index_rd = (index_rd + 1) & FIFO_MASK;
    fifo_counter--;

    if(fifo_counter == (FIFO_SIZE - 2))
    {
        PORTD &= ~(1 << PD6);    /* низький рівень на FIFO-FULL */
    }

    return data;
}


