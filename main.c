#include <avr/io.h>
#include <avr/interrupt.h>

// Define LM35 sensor pin
#define LM35_PIN A0

// Define onboard LED pin
#define LED_PIN 13

// Threshold temperature in degrees Celsius
#define THRESHOLD_TEMPERATURE 30

// Blink intervals in milliseconds
#define BLINK_INTERVAL_LOW 250
#define BLINK_INTERVAL_HIGH 500

// Function prototypes
void initADC();
void initTimer();
void initLED();
void startBlinkTimer(int interval);
void stopBlinkTimer();

// Variable to store temperature
volatile uint8_t temperature;

int main(void) {
    // Initialize ADC, Timer, and LED
    initADC();
    initTimer();
    initLED();

    // Enable global interrupts
    sei();

    while (1) {
        // Main loop - do nothing
    }
}

// ADC Conversion Complete ISR
ISR(ADC_vect) {
    // Read the ADC value
    temperature = ADCH;

    // Check temperature and control LED accordingly
    if (temperature < THRESHOLD_TEMPERATURE) {
        startBlinkTimer(BLINK_INTERVAL_LOW);
    } else {
        startBlinkTimer(BLINK_INTERVAL_HIGH);
    }
}

// Timer1 Compare Match A ISR
ISR(TIMER1_COMPA_vect) {
    // Toggle the LED state
    PORTB ^= (1 << LED_PIN);
}

void initADC() {
    // Set AREF to AVcc, enable ADC, and set prescaler to 128
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // Enable ADC conversion complete interrupt
    ADCSRA |= (1 << ADIE);

    // Start initial conversion
    ADCSRA |= (1 << ADSC);
}

void initTimer() {
    // Set Timer1 in CTC mode with prescaler 1024
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);

    // Set compare value for desired interval (250 or 500 ms)
    OCR1A = F_CPU / 1024 - 1;
}

void initLED() {
    // Set LED pin as an output
    DDRB |= (1 << LED_PIN);
}

void startBlinkTimer(int interval) {
    // Stop Timer1
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));

    // Reset Timer1 counter
    TCNT1 = 0;

    // Set compare value for the desired interval
    OCR1A = interval * (F_CPU / 1024 / 1000) - 1;

    // Start Timer1
    TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);
}

void stopBlinkTimer() {
    // Stop Timer1
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));

    // Turn off LED
    PORTB &= ~(1 << LED_PIN);
}
