#include <FreeRTOS_AVR.h>
// First set
const uint8_t RED_PIN = 9;
const uint8_t YEL_PIN = 8;
const uint8_t GRN_PIN = 7;
//
const uint8_t RED2_PIN = 5;
const uint8_t YEL2_PIN = 3;
const uint8_t GRN2_PIN = 6;
// delays
const uint32_t orangeWait = 500;
const uint32_t greenWait = 2000;
const uint32_t yellowWait = 1000;

bool is1going = false;
bool is2going = false;
int count = 0;

SemaphoreHandle_t sem;

static void Thread1(void* arg) {
  while (1) {
    // Wait for signal.
    xSemaphoreTake(sem, portMAX_DELAY);

    is1going = true;
  	Serial.println("1: Start");
    // Turn LED off.
	red(2);
	vTaskDelay(1000);
	orange(2);
	vTaskDelay(600);
	green(2);
	vTaskDelay(1500);
	yellow(2);
	vTaskDelay(600);
	red(2);
	is1going = false;
	if ( is1going ) {
		// Send signal.
   		xSemaphoreGive(sem);
	}

	Serial.println("1: End");
  }
}

static void Thread2(void* arg) {
  while(1) {

    // Send signal.
    xSemaphoreGive(sem);

    is2going = true;
  	Serial.println("2: Start");

    // Turn LED on.
    red(1);
	vTaskDelay(1000);
	orange(1);
	vTaskDelay(600);
	green(1);
	vTaskDelay(1500);
	yellow(1);
	vTaskDelay(600);
	red(1);

	is2going = false;
    Serial.println("2: End");
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(YEL_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  pinMode(RED2_PIN, OUTPUT);
  pinMode(YEL2_PIN, OUTPUT);
  pinMode(GRN2_PIN, OUTPUT);
  red(1);
  red(2);
  portBASE_TYPE s1, s2;
  // initialize semaphore
  sem = xSemaphoreCreateCounting(1, 0);
  // create task at priority two
  s1 = xTaskCreate(Thread1, NULL, configMINIMAL_STACK_SIZE, NULL, 2, NULL);
  // create task at priority one
  s2 = xTaskCreate(Thread2, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  // check for creation errors
  if (sem== NULL || s1 != pdPASS || s2 != pdPASS ) {
    Serial.println(F("Creation problem"));
    while(1);
  }
  // start scheduler
  vTaskStartScheduler();
  Serial.println(F("Insufficient RAM"));
  while(1);
}

void loop() {
	buttonState = digitalRead(2);
}

void red(int i) {
  if ( i == 1 ) {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(YEL_PIN, LOW);
    digitalWrite(GRN_PIN, LOW);
  } else if ( i == 2 ) {
    digitalWrite(RED2_PIN, HIGH);
    digitalWrite(YEL2_PIN, LOW);
    digitalWrite(GRN2_PIN, LOW);
  }
}

void yellow(int i) {
  if ( i == 1 ) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(YEL_PIN, HIGH);
    digitalWrite(GRN_PIN, LOW);
  } else if ( i == 2 ) {
    digitalWrite(RED2_PIN, LOW);
    digitalWrite(YEL2_PIN, HIGH);
    digitalWrite(GRN2_PIN, LOW);
  }
}

void green(int i) {
  if ( i == 1 ) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(YEL_PIN, LOW);
    digitalWrite(GRN_PIN, HIGH);
  } else if ( i == 2 ) {
    digitalWrite(RED2_PIN, LOW);
    digitalWrite(YEL2_PIN, LOW);
    digitalWrite(GRN2_PIN, HIGH);
  }
}

void orange(int i) {
  if ( i == 1 ) {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(YEL_PIN, HIGH);
    digitalWrite(GRN_PIN, LOW);
  } else if ( i == 2 ) {
    digitalWrite(RED2_PIN, HIGH);
    digitalWrite(YEL2_PIN, HIGH);
    digitalWrite(GRN2_PIN, LOW);
  }
}
