#define gas_sensor 32
#define avg_reading_num 5
#define gas_alarm_threshold 400 //lpg alarm threshold ppm value

#define CLK 18
#define DATA 19
#define DISP_BRIGHTNESS 3				//Between 0 to 7

#define PB_ACCEPT 21
#define PB_TEST 16
#define HOOTER_1 22
#define HOOTER_2 23

#include <esp_now.h>
#include <WiFi.h>
#include <TM1637Display.h>
#include <esp_wifi.h>

//Variable to check communication and generate alarms
int comm_check = 0, comm_alm = 0, lpg_alm = 0, tank_alm = 0;
int gas_ppm = 0;
int accept_pressed = 0;

// Structure example to receive data
// Must match the sender structure
bool tank_full = 0;

// Create a display object of type TM1637Display
TM1637Display display = TM1637Display(CLK, DATA);

// Create an array that turns all segments ON
const uint8_t allON[] = { 0xff, 0xff, 0xff, 0xff };

// Create an array that turns all segments OFF
const uint8_t allOFF[] = { 0x00, 0x00, 0x00, 0x00 };

//Array for displaying Conn
const uint8_t conn[] = {
  SEG_A | SEG_F | SEG_D | SEG_E,          			// C
  SEG_G | SEG_C | SEG_D | SEG_E,  							// o
  SEG_G | SEG_C | SEG_E,                    		// n
  SEG_G | SEG_C | SEG_E           							// n
};

//Array for displaying FULL
const uint8_t full[] = {
  SEG_A | SEG_F | SEG_E | SEG_G,          			// F
  SEG_F | SEG_E | SEG_C | SEG_D | SEG_B,  			// U
  SEG_F | SEG_E | SEG_D,                        // L
  SEG_F | SEG_E | SEG_D           							// L
};

//Array for displaying LPG
const uint8_t lpg[] = {
  SEG_F | SEG_E | SEG_D,          							// L
  SEG_A | SEG_B | SEG_G | SEG_F | SEG_E,  			// P
  SEG_A | SEG_F | SEG_E | SEG_D | SEG_C,				// G
  SEG_D           															// _
};

//Array for displaying Indu
const uint8_t indu[] = {
  SEG_B | SEG_C,          										// I
  SEG_G | SEG_C | SEG_E,  										// n
  SEG_B | SEG_G | SEG_E | SEG_D | SEG_C,			// d 
  SEG_D | SEG_C | SEG_E           						// u
};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&tank_full, incomingData, sizeof(tank_full));
  comm_check = 0;
}
 
void setup() {
  //Pull up the Push Button Input
  pinMode(PB_ACCEPT, INPUT_PULLUP);
  pinMode(PB_TEST, INPUT_PULLUP);
  pinMode(HOOTER_1, OUTPUT);
  pinMode(HOOTER_2, OUTPUT);
  
  //Initialise led display
  display.setBrightness(DISP_BRIGHTNESS);
  display.setSegments(indu);
  delay(2000);
  display.clear();

  // Initialize Serial Monitor
  Serial.begin(9600);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
	esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
	esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // register for recv CB to get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}
 
void loop() {

  comm_check++;
  
  int total = 0;
  // Take multiple readings and sum them up
  for (int i = 0; i < avg_reading_num; i++) {
    total += map(analogRead(gas_sensor), 0, 4095, 0, 1000);
    delay(100);  // small gap between samples (100ms)
  }
  gas_ppm = total / avg_reading_num;

	// Setting of alarm variables
	if ((gas_ppm > gas_alarm_threshold) & (millis() > 86400000)) {	//Since MQ-6 has some warm up time, hence alarm only activated after 1 day of continuous operation
		lpg_alm = 1;
	}
	else {
		lpg_alm = 0;
	}
	if (tank_full == 1) {
		tank_alm = 1;
	}
	else {
		tank_alm = 0;
	}
	if (comm_check > 8) {
		comm_alm = 1;
	}
	else {
		comm_alm = 0;
	}
	
	// Setting of led display message
	if (lpg_alm) {
		display.setSegments(lpg);
	}
	else if (comm_alm) {
		display.setSegments(conn);
	}
	else if (tank_alm) {
		display.setSegments(full);
	}
	else if (digitalRead(PB_TEST) == LOW) {
		display.setSegments(allON);
	}
	else {
		display.setSegments(allOFF);
	}
	
	//Setting of hooter logic
	if ((tank_alm | lpg_alm | comm_alm) & ~(accept_pressed)) {
		digitalWrite(HOOTER_1, HIGH);
		digitalWrite(HOOTER_2, HIGH);
		if(digitalRead(PB_ACCEPT) == LOW) {
			accept_pressed = 1;
		}
		else {
			accept_pressed = accept_pressed;
		}
	}
	else if (tank_alm | lpg_alm | comm_alm) {
		digitalWrite(HOOTER_1, LOW);
		digitalWrite(HOOTER_2, LOW);
	}
	else if (digitalRead(PB_TEST) == LOW) {
		digitalWrite(HOOTER_1, HIGH);
		digitalWrite(HOOTER_2, HIGH);
	}
	else {
		digitalWrite(HOOTER_1, LOW);
		digitalWrite(HOOTER_2, LOW);
		accept_pressed = 0;
	}
	Serial.print("Tank Level: ");
	Serial.println(tank_full);
	Serial.print("Gas ppm: ");
	Serial.println(gas_ppm);
	Serial.print("Comm Alarm: ");
	Serial.println(comm_alm);
}