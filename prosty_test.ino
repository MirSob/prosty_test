
#include "RedkeaESP8266.h"

//char ssid[] = "SIC4NET";
//char pass[] = "karmenw0parach";
char ssid[] = "MiLenovoK6";
char pass[] = "mojewifi";
char deviceID[] = "-L1X4g96PT-TZHWB2ZnK";
//#define _INTIMER0
#define _INTIMER1
#define _REDKEA

RedkeaESP8266 redkea;
#include "pins_arduino.h"
volatile bool pin_state = false;
volatile bool offserial = false;
volatile unsigned long next;
volatile unsigned long interruptCounter;

void inline offSerial(void);
void inline blinkISR(void);
void inline blinkISR_D8(void);

void setup()
{
  
	Serial.begin(115200);
	pinMode(D6, OUTPUT);
	pinMode(D8, OUTPUT);
	/* add setup code here */
	pinMode(D3, INPUT_PULLUP);
	 attachInterrupt(digitalPinToInterrupt(D3), offSerial, FALLING);
	 noInterrupts();

#ifdef _INTIMER0   
	 timer0_isr_init();
	 timer0_attachInterrupt(blinkISR);
   next = ESP.getCycleCount() + 1000;
	 timer0_write(next);
#endif
#ifdef _INTIMER1
	 timer1_isr_init();
	 timer1_attachInterrupt(blinkISR_D8);
	 timer1_enable(TIM_DIV265, TIM_EDGE, TIM_LOOP);
	 timer1_write(3906);
#endif   
#ifdef _REDKEA
   redkea.begin(ssid, pass, deviceID);
#endif   
	 interrupts();
	 //Set up ESP watchdog
	 ESP.wdtDisable();
	 ESP.wdtEnable(WDTO_8S);

}

void inline offSerial(void) {
noInterrupts();
offserial != offserial;
interruptCounter++;
interrupts();
}

void inline blinkISR(void) {
	noInterrupts();

	digitalWrite(D6, !digitalRead(D6));
	//analogWrite(ledPin5, sl5Value);
	if (digitalRead(D6))
		Serial.write("|");
	else
		Serial.write("*");
	
	next = ESP.getCycleCount() + 999450;
	ESP.wdtFeed();
	timer0_write(next);
	interrupts();
}

void inline blinkISR_D8(void) {
	noInterrupts();
		digitalWrite(D8, !digitalRead(D8));
		//analogWrite(ledPin5, sl5Value);
		if(digitalRead(D8)) {
			if (!offserial) Serial.write("#");
		} else
			if(!offserial) Serial.write("r");
		
	ESP.wdtFeed();
	interrupts();
}

int raz = 0;
int numberOfInterrupts = 0;
void loop()
{
#ifdef _REDKEA  
  redkea.loop();
#endif  
  yield();


if (!raz) {

	digitalWrite(D8, 0);
	delay(10);
	digitalWrite(D8, 1);
	delay(10);
	digitalWrite(D8, 0);
	raz = 1;
}

if (interruptCounter>0) {

	interruptCounter--;
	numberOfInterrupts++;

	Serial.print("An interrupt has occurred. Total: ");
	Serial.println(numberOfInterrupts);
}

	yield();
}
