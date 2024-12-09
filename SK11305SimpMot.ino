/* v01

   

       20 FEB 2023: Carried over from BUTD
				
*/

#include <AccelStepper.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>


//*******************************************************
//**** AVAILABLE IO ON THE ARDUINO LCD KEYPAD SHIELD ****
//*******************************************************
// analog input pins - can be used are digital io as well
#define A1			A1
#define A2			A2
#define A3			A3
#define	A4			A4
#define A5			A5
// digital io pins
#define D0			0			// RX		
#define D1			1			// TX		
#define D2			2
#define D3			3
#define D11			11
#define D12			12
#define D13			13
//*******************************************************
//****			DEBUG / TESTING ONLY				 ****
//*******************************************************
//#define TEST_PIN			D2												// use to confirm timing or debug - does not affect the application
//#define TEST_PIN_HIGH		digitalWrite(TEST_PIN,HIGH)
//#define TEST_PIN_LOW		digitalWrite(TEST_PIN,LOW)
//#define TEST_PIN_TOG		digitalWrite(TEST_PIN,!(digitalRead(TEST_PIN)))
//*******************************************************


#define STEP_PIN	D11			// Step pin
#define DIR_PIN		D12			// Direction pin
#define EN_PIN		D13			// Enable pin (not used currently but is wired)

#define STEPS_PER_REV_DEF	50		// this value needs to correspond to the  switch setting on the DM1182 myStepperDrive drive 
#define MAX_SPEED_DEF		8		// revolutions per second
#define ACCELERATION_DEF	1600	// steps per sec

#define DIR_UP				-1		// "UP" movements are CCW shaft rotations or negative step values
#define DIR_DOWN			1		// "DOWN" movements are CW shaft rotations or positive step values

#define INVALID_DATA		0xFFFF

//  ******************************************************
//	****  LCD Display Screens 16x2 	**********************
//  ******************************************************
//	The strings will be stored in prog memory to save SRAM
//  ** see Arduino Reference on PROGMEM for details...
//						        "0123456789012345"
const char display0[] PROGMEM = "SK11305SimpMot  ";
const char display1[] PROGMEM = "Menu <any key>  ";
const char display2[] PROGMEM = "Stepper Drv Cfg ";
const char display3[] PROGMEM = "Cycle Test Cfg  ";
const char display4[] PROGMEM = "Limit Sensor Cfg";
const char display5[] PROGMEM = "Motor Control   ";
const char display6[] PROGMEM = "Max Speed: 12rps";
const char display7[] PROGMEM = "Accel:6789stps/s";
const char display8[] PROGMEM = "Steps/Rev: 12345";
const char display9[] PROGMEM = "Duty Cycle: 234%";
//						         "0123456789012345"
const char display10[] PROGMEM = "Lmt Sensors YES ";
const char display11[] PROGMEM = "Inv Up Lmt: YES ";
const char display12[] PROGMEM = "Inv Dn Lmt: NO  ";
const char display13[] PROGMEM = "# of Cycles:    ";
const char display14[] PROGMEM = "Btn Hld (ms):   ";
const char display15[] PROGMEM = "Dwell (secs):   ";
const char display16[] PROGMEM = "Start Cycle     ";
const char display17[] PROGMEM = "     of         ";
const char display18[] PROGMEM = "Cycle Active    ";
const char display19[] PROGMEM = "Cycle Complete  ";
//						         "0123456789012345"
const char display20[] PROGMEM = "Cycle Paused    ";
const char display21[] PROGMEM = "Reset Cycle Tst?";
const char display22[] PROGMEM = ">RESET<  RESUME ";
const char display23[] PROGMEM = " RESET  >RESUME<";
const char display24[] PROGMEM = "Terminate Test? ";
const char display25[] PROGMEM = "  >YES<    NO   ";
const char display26[] PROGMEM = "   YES    >NO<  ";
const char display27[] PROGMEM = "Set TOP?        ";
const char display28[] PROGMEM = "Set BOT?        ";
const char display29[] PROGMEM = "Timeout (s):    ";
//						         "0123456789012345"
const char display30[] PROGMEM = "Cy Err-TimeOut  ";
const char display31[] PROGMEM = "Cy Err-Stuck UEL";
const char display32[] PROGMEM = "Cy Err-Stuck DEL";
const char display33[] PROGMEM = "RunTime (s):    ";
const char display34[] PROGMEM = "Duty Cycle:   % ";
const char display35[] PROGMEM = "Total Test Time ";
const char display36[] PROGMEM = "  d,  h,  m,  s ";
const char display37[] PROGMEM = "secs:           ";
const char display38[] PROGMEM = "Edit Parameter  ";
const char display39[] PROGMEM = "Move to ?       ";
const char display40[] PROGMEM = "Mtr Ctrl Active ";
//						         "0123456789012345"
const char display41[] PROGMEM = "Rev/Sec: 90     ";


const char* const myDISPLAY[] PROGMEM = { display0, display1, display2, display3, display4, display5,
											display6, display7, display8, display9, display10, display11,
											display12, display13, display14, display15, display16, display17,
											display18, display19, display20, display21, display22, display23,
											display24, display25, display26, display27, display28, display29,
											display30, display31, display32, display33, display34, display35,
											display36, display37, display38, display39, display40, display41 };

#define INDEX_PROG_VER		0
#define INDEX_MENU			1
#define INDEX_CFG_STEPPER	2
#define INDEX_CFG_CYCLE		3
#define INDEX_CFG_LIMITS	4
#define INDEX_MOTOR_CONTROL	5
#define INDEX_MAX_SPEED		6
#define INDEX_ACCEL			7
#define INDEX_STEPS_PER_REV	8
//#define INDEX_DUTY_CYCLE	9
#define INDEX_USE_LMT_SENSORS	10
#define INDEX_INV_UP		11
#define INDEX_INV_DOWN		12
#define INDEX_NUM_OF_CYCLES	13
#define INDEX_BTN_HOLD_TM	14
#define INDEX_DWELL_TIME	15
#define INDEX_CYCLE_START	16
#define INDEX_CYCLE_STATUS	17
#define INDEX_CYCLE_ACTIVE	18
#define INDEX_CYCLE_COMPLETE	19
#define INDEX_CYCLE_PAUSED	20
#define INDEX_RESET_CYCLE	21
#define INDEX_RESET			22
#define INDEX_RESUME		23
#define INDEX_TERMINATE		24
#define INDEX_YES			25
#define INDEX_NO			26
#define INDEX_SET_TOP		27
#define INDEX_SET_BOTTOM	28
#define INDEX_MOVE_TIMEOUT	29
#define INDEX_CY_ERROR_TMOUT	30
#define INDEX_CY_ERROR_UEL	31
#define INDEX_CY_ERROR_DEL	32
#define INDEX_MOVE_TIME		33
#define INDEX_DUTY_CYCLE	34
#define INDEX_TOTAL_TIME_1	35
#define INDEX_TOTAL_TIME_2	36
#define INDEX_TOTAL_TIME_3	37
#define INDEX_EDIT_PARA		38
#define INDEX_MOVE_TO_POS	39
#define INDEX_MOTOR_ACTIVE	40
#define INDEX_SET_POSITION	41

#define MAX_MAIN_MENU		5
#define MAX_SUB_MENU		5
											                       	//MAIN MENU      CYCLE MENUs     MTR CTRL MENUs    Limit MENUs     
uint8_t menuLCD[MAX_MAIN_MENU][MAX_SUB_MENU] = { {1,3,5,0,0}, {13,15,29,34,0}, {39,16,0,0,0}, {10,11,12,0,0} };

#define BUFFER_SIZE		20
char dispBuffer[BUFFER_SIZE];		// dispBuffer will be used to pull the strings out of program memory and placed in RAM...

//  ******************************************************
//	****  Application Keypad Definitions  	**************
//  ******************************************************

// button definitions
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define MODE_KEY_IDLE		10
#define MODE_KEY_PRESSED	20
#define MODE_KEY_HELD		30
#define MODE_KEY_RELEASED	40

#define TIME_HELD_KEY_MSECS	1000

#define UP_PIN 2
#define DOWN_PIN 3

struct btnPress {
	uint8_t active_key;
	uint8_t last_key;
	uint8_t mode;
	uint16_t held_time;
};
struct btnPress myKeys;

//  ******************************************************
//	**************    EEPROM ADDRESSES  	**************
//  ******************************************************

#define EE_ADDR_BASE			0	// starting location for our stored settings
#define EE_ADDR_DRIVE			EE_ADDR_BASE
#define EE_ADDR_CYCLE_DATA		EE_ADDR_DRIVE

#define EE_CHKSUM_VALID		0x55

//  ******************************************************

struct CycleTester {
	uint16_t cycle_count;			// the number of cycles completed (1 cycle = 1 up and 1 down movement)
	uint16_t dwell_time;			// the amount of time (sec) to wait between movements
	uint16_t num_cycles;			// the number of cycles for this test period
	uint32_t total_test_time;		// the amount of time (secs) the cycle test has been running
	uint16_t move_time;				// variable to keep track of the amount of time (sec) when moving from limit to limit.
	uint16_t move_timeout;			// movement timeout in secs - only used when sensors is enabled
	uint16_t duty_cycle;			// the target duty cycle
	uint8_t index;					// the step index for the cycle test
	bool limit_sensors;				// the enable for using limit sensors with the movements
	bool inv_up_lmt;				// flag to invert the normal sensor reading for up
	bool inv_dn_lmt;				// flag to invert the normal sensor reading for down
	bool uel_reached;				// flag for reaching the upper end limit
	bool del_reached;				// flag for reaching the down end limit
	byte chksum;
};
struct CycleTester myCycleTest;

struct theLCD {
	uint8_t row1_index;
	uint8_t row2_index;
	uint8_t top_menu_index;
	uint8_t sub_menu_index;
};
struct theLCD myLCD;

struct theEdit {
	uint16_t max_Value;				// max value for the current setting - initialized before editing begins
	uint16_t min_Value;				// min value for the current setting - initialized before editing begins
	uint16_t cur_Value;				// the current displayed value of the setting
	uint16_t stepSize;				// the smallest amount of value change when editing - initialized before editing begins
	uint16_t xMultipler;
};
struct theEdit myEdit;


byte times10 = 0;		// this will be used for a 100 msec timer
byte times100 = 0;		// this will be used for a 1000 msec or 1 sec timer
uint32_t sTime = 0;		// used for keeping track of relative time, 1 tick = 10 msecs

bool frComplete = false;
String inStr = "";
String inCmd = "";


// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


uint32_t Time100ms;
uint32_t Time1000ms;

void setup() {
	
	//pinMode(TEST_PIN,OUTPUT);
  pinMode(UP_PIN, OUTPUT);
  pinMode(DOWN_PIN,OUTPUT);

  digitalWrite(DOWN_PIN, HIGH);
  digitalWrite(UP_PIN, HIGH);
 
	//TEST_PIN_LOW;
	
	lcd.begin(16, 2);						// start the LCD library for a 16x2 display
	
	updateDisplayProgStart();
	
	


	init_myKeys();							// initialize the key button structure
	init_myCycleTest();						// initialize the Cycle Test data 

  Time100ms = millis();
  Time1000ms=millis();
	
	Serial.begin(57600);					// init serial
	Serial.println("BUTD");
}

void loop() {
  uint32_t currentTime = 0;

  if (frComplete) {            // this will get checked every time through

    frComplete = false;
    inStr = "";
    inCmd = "";
  }
  else {
    currentTime = millis();
    if ((currentTime-Time100ms) >= 100) {        // ***** 100 msec loop ******
      Time100ms = currentTime;          // **************************
      monitor_ui(); 
      switch (myLCD.row1_index) {
        case INDEX_CYCLE_ACTIVE:
          monitorCycleTest();
          break;
      }
    }                 
    if ((currentTime-Time1000ms) >= 1000) {        // ***** 1000 msec loop *****
      Time1000ms = currentTime;
      if (myLCD.row1_index == INDEX_CYCLE_ACTIVE) myCycleTest.total_test_time += 1;
      //TEST_PIN_TOG;
    }
 }
  
}


void init_myKeys() {
	myKeys.active_key = btnNONE;
	myKeys.last_key = btnNONE;
	myKeys.mode = MODE_KEY_RELEASED;
	myKeys.held_time = 0;
}

void init_myCycleTest(void) {
	EEPROM.get(EE_ADDR_CYCLE_DATA,myCycleTest);

	if (myCycleTest.chksum != EE_CHKSUM_VALID) {
		myCycleTest.cycle_count = 0;
		myCycleTest.num_cycles = 4000;
		myCycleTest.dwell_time = 30;
		myCycleTest.total_test_time = 0;
		myCycleTest.move_time = 0;
		myCycleTest.duty_cycle = 50;
		myCycleTest.move_timeout = 120;
		myCycleTest.index = 0;
		myCycleTest.limit_sensors = true;
		myCycleTest.inv_up_lmt = true;
		myCycleTest.inv_dn_lmt = false;
		myCycleTest.uel_reached = false;
		myCycleTest.del_reached = false;
	}	
}

void saveCycleData(void) {
	myCycleTest.chksum = EE_CHKSUM_VALID;
	EEPROM.put(EE_ADDR_CYCLE_DATA,myCycleTest);
}

int read_LCD_buttons() {
	int adc_key_in = 0;
	
	adc_key_in = analogRead(0);      // read the value from the sensor
	// my buttons when read are centered at these valies: 0, 144, 329, 504, 741
	// we add approx 50 to those values and check to see if we are close
	if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
	// For V1.0 comment the other threshold and use the one below:
	if (adc_key_in < 50)   return btnRIGHT;
	if (adc_key_in < 195)  return btnUP;
	if (adc_key_in < 380)  return btnDOWN;
	if (adc_key_in < 555)  return btnLEFT;
	if (adc_key_in < 790)  return btnSELECT;

	return btnNONE;  // when all others fail, return this...
}

void monitor_ui(void) {
	// this routine is called once every 100 msec or 10 times per second
	myKeys.active_key = read_LCD_buttons();
	if (myKeys.active_key == btnNONE) {					// no keys are pressed, so check the last key variable
		if (myKeys.last_key == btnNONE) {
			myKeys.mode = MODE_KEY_IDLE;
		}
		else {											// else, key was just released...
			myKeys.mode = MODE_KEY_RELEASED;
			act_on_key_released();
		}
		myKeys.held_time = 0;
		myKeys.last_key = btnNONE;
	}
	else if (myKeys.active_key != myKeys.last_key) {	// else, a key is pressed so check this active key with the last key to see if it is a new key
		myKeys.last_key = myKeys.active_key;			// it is a new key, so set the last key equal to the active key
		myKeys.held_time = 100;							// increment the held_time variable
		myKeys.mode = MODE_KEY_PRESSED;					// set the mode to KEY PRESSED
		act_on_key_press();								// check what should be done with this key press
	}
	else {												// the active key is the same as the last key so
		myKeys.held_time += 100;						// increment the held_time variable
		if (myKeys.held_time >= TIME_HELD_KEY_MSECS) {	// check if the held_time variable is greater than or equal to the HELD_KEY threshold
			myKeys.mode = MODE_KEY_HELD;				// the key has been held past the HELD KEY threshold so set the mode to KEY_HELD
			act_on_key_held();							// check what should be done with the held key
		}
	}
}

void act_on_key_press() {
	switch(myLCD.row1_index) {
		case INDEX_PROG_VER:
		case INDEX_CFG_CYCLE:
		case INDEX_CFG_LIMITS:
		case INDEX_MOTOR_CONTROL:
			navigate_menu();
			break;
		case INDEX_EDIT_PARA:
			edit_menu();
			break;
		case INDEX_CYCLE_ACTIVE:
			pauseCycleTest();
			break;
		case INDEX_CYCLE_PAUSED:
			updateDisplay(0,INDEX_TERMINATE);
			updateDisplay(1,INDEX_NO);
			break;
		case INDEX_TERMINATE:
			terminate_choice();
			break;
		case INDEX_RESET_CYCLE:
			reset_choice();
			break;
	}
}

void act_on_key_held() {
	switch(myLCD.row1_index) {
		case INDEX_PROG_VER:
		case INDEX_CFG_CYCLE:
		case INDEX_CFG_LIMITS:
		case INDEX_MOTOR_CONTROL:
		case INDEX_EDIT_PARA:
			break;
		case INDEX_MOTOR_ACTIVE:
			move_menu();
			break;
	}
}

void act_on_key_released() {
	
}

void updateEditVariables(void) {
	switch (menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index]) {
		case INDEX_NUM_OF_CYCLES:
			myEdit.max_Value = 9900;
			myEdit.min_Value = 50;
			myEdit.stepSize = 50;
			myEdit.xMultipler = 10;
			myEdit.cur_Value = myCycleTest.num_cycles;
			break;
		case INDEX_DWELL_TIME:
			myEdit.max_Value = 900;
			myEdit.min_Value = 5;
			myEdit.stepSize = 5;
			myEdit.xMultipler = 10;
			myEdit.cur_Value = myCycleTest.dwell_time;
			break;
		case INDEX_MOVE_TIMEOUT:
			myEdit.max_Value = 900;
			myEdit.min_Value = 10;
			myEdit.stepSize = 10;
			myEdit.xMultipler = 10;
			myEdit.cur_Value = myCycleTest.move_timeout;
			break;
		case INDEX_DUTY_CYCLE:
			myEdit.max_Value = 100;
			myEdit.min_Value = 10;
			myEdit.stepSize = 5;
			myEdit.xMultipler = 2;
			myEdit.cur_Value = myCycleTest.duty_cycle;
			break;

		case INDEX_USE_LMT_SENSORS:
			myEdit.max_Value = 1;
			myEdit.min_Value = 0;
			myEdit.stepSize = 1;
			myEdit.xMultipler = 1;
			if (myCycleTest.limit_sensors == true) myEdit.cur_Value = 1;
			else myEdit.cur_Value = 0;
			break;
		case INDEX_INV_UP:
			myEdit.max_Value = 1;
			myEdit.min_Value = 0;
			myEdit.stepSize = 1;
			myEdit.xMultipler = 1;
			if (myCycleTest.limit_sensors == true) myEdit.cur_Value = 1;
			else myEdit.cur_Value = 0;
			break;
		case INDEX_INV_DOWN:
			myEdit.max_Value = 1;
			myEdit.min_Value = 0;
			myEdit.stepSize = 1;
			myEdit.xMultipler = 1;
			if (myCycleTest.limit_sensors == true) myEdit.cur_Value = 1;
			else myEdit.cur_Value = 0;
			break;
	}
	if (myEdit.cur_Value > myEdit.max_Value)
		myEdit.cur_Value = myEdit.max_Value;
	else if (myEdit.cur_Value < myEdit.min_Value)
		myEdit.cur_Value = myEdit.min_Value;
	updateDisp_SetupEdits(myEdit.cur_Value);
}

void saveEdit(void) {
	bool end_edit = false;
	
	switch (menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index]) {
		case INDEX_NUM_OF_CYCLES:
			myCycleTest.num_cycles = myEdit.cur_Value;
			saveCycleData();
			end_edit = true;
			break;
		case INDEX_DWELL_TIME:
			myCycleTest.dwell_time = myEdit.cur_Value;
			saveCycleData();
			end_edit = true;
			break;
		case INDEX_MOVE_TIMEOUT:
			myCycleTest.move_timeout = myEdit.cur_Value;
			saveCycleData();
			end_edit = true;
			break;
		case INDEX_DUTY_CYCLE:
			myCycleTest.duty_cycle = myEdit.cur_Value;
			saveCycleData();
			end_edit = true;
			break;
		case INDEX_USE_LMT_SENSORS:
			myCycleTest.limit_sensors = (myEdit.cur_Value == 1);
			saveCycleData();
			end_edit = true;
			break; 
		case INDEX_INV_UP:
			myCycleTest.inv_up_lmt = (myEdit.cur_Value == 1);
			saveCycleData();
			end_edit = true;
			break;
		case INDEX_INV_DOWN:
			myCycleTest.inv_dn_lmt = (myEdit.cur_Value == 1);
			saveCycleData();
			end_edit = true;
			break;			
	}
	if (end_edit == true) {
		updateDisplay(0,menuLCD[0][myLCD.top_menu_index]);			// set row1 back 
	}
}

void decrement_submenu (void) {
	if (myLCD.sub_menu_index == 0) 
		myLCD.sub_menu_index = MAX_SUB_MENU - 1;
	else
		myLCD.sub_menu_index -= 1;
	// make sure the value of the menu index is not zero
	while (menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index] == 0) {
		myLCD.sub_menu_index -= 1;
	}
}

void increment_submenu (void) {
	if (myLCD.sub_menu_index == (MAX_SUB_MENU - 1)) 
		myLCD.sub_menu_index = 0;
	else
		myLCD.sub_menu_index += 1;
	// make sure the value of the menu index is not zero...
	if (menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index] == 0) 
		myLCD.sub_menu_index = 0;
}

void enter_menu(void) {
	// enter one of the menu choices...
	switch (myLCD.row1_index) {
		case INDEX_PROG_VER:
			updateDisplay(0,menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index]);	// move the sub menu to row1
			myLCD.top_menu_index = myLCD.sub_menu_index;							// set the top menu index to the sub menu index and display the first parameter for that sub menu
			myLCD.sub_menu_index = 0;
			updateDisplay(1,menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index]);	// update row2 of the display
			updateDisp_SetupEdits(INVALID_DATA);
			break;

		case INDEX_CFG_CYCLE:
		case INDEX_CFG_LIMITS:
			updateDisplay(0,INDEX_EDIT_PARA);										// enter edit mode...
			updateEditVariables();													// set the edit variables to the parameter...
			break;
		case INDEX_MOTOR_CONTROL:
			if (menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index] == INDEX_CYCLE_START) {
				if (myCycleTest.cycle_count != 0) {
					if (myCycleTest.cycle_count >= myCycleTest.num_cycles) {
						reset_myCycleTestVariables();
						startCycleTest();
					}
					updateDisplay(0,INDEX_RESET_CYCLE);
					updateDisplay(1,INDEX_RESET);
				}
				else startCycleTest();
			}
			else updateDisplay(0,INDEX_MOTOR_ACTIVE);
			break;		
	}	
}

void back_out_of_menu (void) {
	if (myLCD.row1_index != INDEX_PROG_VER) {
		myLCD.sub_menu_index = myLCD.top_menu_index;
		myLCD.top_menu_index = 0;
		updateDisplay(0, INDEX_PROG_VER);										// this is the main menu top row default - prog ver
		updateDisplay(1,menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index]);	// update row2 of the display		
	}
}

void navigate_menu() {
	bool update_the_display = false;		// local variable just for this routine
	
	switch (myKeys.active_key) {
		case btnUP:
			decrement_submenu();
			update_the_display = true;
			break;
		case btnDOWN:
			increment_submenu();
			update_the_display = true;
			break;
			break;
		case btnRIGHT:
		case btnSELECT:
			if (menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index] != INDEX_MENU) {	// special case - nothing to enter into with this index...
				enter_menu();
			}
			else {
				increment_submenu();
				update_the_display = true;
			}
			break;
		case btnLEFT:
			if (menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index] != INDEX_MENU) {	// special case - nothing to enter into with this index...
				back_out_of_menu();
			}
			else {
				increment_submenu();
				update_the_display = true;
			}
			break;
	}
	if (update_the_display) {
		updateDisplay(1,menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index]);
		updateDisp_SetupEdits(INVALID_DATA);
	}
}

void edit_menu() {
	uint16_t xMultipler = 1;

	switch (myKeys.active_key) {
		case btnRIGHT:
			xMultipler = myEdit.xMultipler;
		case btnUP:
			switch (menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index]) {		// look for special cases...
				case INDEX_USE_LMT_SENSORS:
				case INDEX_INV_UP:
				case INDEX_INV_DOWN:
					if (myEdit.cur_Value == myEdit.max_Value) myEdit.cur_Value = myEdit.min_Value;
					else myEdit.cur_Value = myEdit.max_Value;
					break;	
				default:
					myEdit.cur_Value += (myEdit.stepSize * xMultipler);
					break;
			}
			if (myEdit.cur_Value > myEdit.max_Value) myEdit.cur_Value = myEdit.max_Value;
			updateDisp_SetupEdits(myEdit.cur_Value);
			break;
		case btnLEFT:
			xMultipler = myEdit.xMultipler;
		case btnDOWN:
			switch (menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index]) {
				case INDEX_USE_LMT_SENSORS:
				case INDEX_INV_UP:
				case INDEX_INV_DOWN:
					if (myEdit.cur_Value == myEdit.max_Value) myEdit.cur_Value = myEdit.min_Value;
					else myEdit.cur_Value = myEdit.max_Value;
					break;
				default:
					myEdit.cur_Value -= (myEdit.stepSize * xMultipler);
					break;
			}
			if ((myEdit.cur_Value < myEdit.min_Value) || (myEdit.cur_Value > myEdit.max_Value)) myEdit.cur_Value = myEdit.min_Value;
			updateDisp_SetupEdits(myEdit.cur_Value);
			break;
		case btnSELECT:
			saveEdit();
			break;
		case btnNONE:
			break;
	}
}

void move_menu(void) {
	switch(myKeys.active_key) {
		case btnUP:
			switch (menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index]) {

				case INDEX_MOVE_TO_POS:
					
					break;
			}
			break;
		case btnDOWN:
			switch (menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index]) {

				case INDEX_MOVE_TO_POS:
					
					break;
			}
			break;
		case btnSELECT:
			switch (menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index]) {

			}
			break;
		case btnRIGHT:
		case btnLEFT:
			updateDisplay(0,menuLCD[0][myLCD.top_menu_index]);			// set row1 back
			updateDisplay(1,menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index]);
			break;
	}
}

void reset_choice(void) {
	switch (myKeys.active_key) {
		case btnLEFT:
		case btnRIGHT:
		case btnUP:
		case btnDOWN:
			toggleDisplay_ResetResume();
			break;
		case btnSELECT:
			switch (myLCD.row2_index) {
				case INDEX_RESET:
					reset_myCycleTestVariables();
					startCycleTest();
					break;
				case INDEX_RESUME:
					startCycleTest();
					break;
			}
			break;
	}
}

void terminate_choice(void) {
	switch (myKeys.active_key) {
		case btnLEFT:
		case btnRIGHT:
		case btnUP:
		case btnDOWN:
			toggleDisplay_YesNo();
			break;
		case btnSELECT:
			switch (myLCD.row2_index) {
				case INDEX_YES:
					updateDisplayProgStart();
					break;
				case INDEX_NO:
					startCycleTest();
					break;
			}
			break;
	}
}

void toggleDisplay_ResetResume(void) {
	if (myLCD.row2_index == INDEX_RESET) myLCD.row2_index = INDEX_RESUME;
	else if (myLCD.row2_index == INDEX_RESUME) myLCD.row2_index = INDEX_RESET;
	updateDisplay(1,myLCD.row2_index);
}

void toggleDisplay_YesNo(void) {
	if (myLCD.row2_index == INDEX_YES) myLCD.row2_index = INDEX_NO;
	else if (myLCD.row2_index == INDEX_NO) myLCD.row2_index = INDEX_YES;
	updateDisplay(1,myLCD.row2_index);
}

void updateDisplay(uint8_t row, uint8_t theIndex) {
	strcpy_P(dispBuffer,(char*)pgm_read_word(&(myDISPLAY[theIndex])));
	lcd.setCursor(0,row);
	lcd.print(dispBuffer);
	if (row == 0) myLCD.row1_index = theIndex;
	else if (row == 1) myLCD.row2_index = theIndex;
}

void updateDisplayProgStart(void) {
	updateDisplay(0,INDEX_PROG_VER);
	updateDisplay(1,INDEX_MENU);
	myLCD.top_menu_index = 0;
	myLCD.sub_menu_index = 0;
}


void updateDisplayMoveTime() {
	uint16_t dutycy = 0;
	
	dutycy = (myCycleTest.move_time * 100) / (myCycleTest.move_time + myCycleTest.dwell_time);
	updateDisplay(0,INDEX_MOVE_TIME);
	lcd.setCursor(13,0); lcd.print(myCycleTest.move_time);
	updateDisplay(1,INDEX_DUTY_CYCLE);
	lcd.setCursor(12,1); lcd.print(dutycy);
}

void updateDisp_SetupEdits(uint16_t theData) {
	// editing based on the current submenu
	switch (menuLCD[myLCD.top_menu_index][myLCD.sub_menu_index]) {
		case INDEX_NUM_OF_CYCLES:
			if (theData == INVALID_DATA) theData = myCycleTest.num_cycles;
			lcd.setCursor(12,1); lcd.print("    ");
			lcd.setCursor(12,1); lcd.print(theData);
			break;
		case INDEX_DWELL_TIME:
			if (theData == INVALID_DATA) theData = myCycleTest.dwell_time;
			lcd.setCursor(13,1); lcd.print("   ");
			lcd.setCursor(13,1); lcd.print(theData);
			break;			
		case INDEX_MOVE_TIMEOUT:
			if (theData == INVALID_DATA) theData = myCycleTest.move_timeout;
			lcd.setCursor(13,1); lcd.print("   ");
			lcd.setCursor(13,1); lcd.print(theData);
			break;
		case INDEX_DUTY_CYCLE:
			if (theData == INVALID_DATA) theData = myCycleTest.duty_cycle;
			lcd.setCursor(11,1); lcd.print("   ");
			if (theData == 100) lcd.setCursor(11,1);
			else lcd.setCursor(12,1);
			lcd.print(theData);
			break;
		case INDEX_USE_LMT_SENSORS:
			if (theData == INVALID_DATA) theData = (uint16_t)(myCycleTest.limit_sensors);
			lcd.setCursor(12,1); lcd.print("   ");
			lcd.setCursor(12,1);
			if (theData == 1) lcd.print(F("YES"));
			else lcd.print(F("NO"));
			break;
		case INDEX_INV_UP:
			if (theData == INVALID_DATA) theData = (uint16_t)(myCycleTest.inv_up_lmt);
			lcd.setCursor(12,1); lcd.print("   ");
			lcd.setCursor(12,1);
			if (theData == 1) lcd.print(F("YES"));
			else lcd.print(F("NO"));
			break;
		case INDEX_INV_DOWN:
			if (theData == INVALID_DATA) theData = (uint16_t)(myCycleTest.inv_dn_lmt);
			lcd.setCursor(12,1); lcd.print("   ");
			lcd.setCursor(12,1); 
			if (theData == 1) lcd.print(F("YES"));
			else lcd.print(F("NO"));
			break;	
	}
}

void clearDwellTimer() {
	lcd.setCursor(13,1); lcd.print("   ");
}

void updateDwellTimer(uint16_t timer) {
	uint16_t dtimer = 0;
	
	// display the dwell timer as a count down timer (timer variable is counting up...)
	dtimer = myCycleTest.dwell_time - timer;
	
	// clear the dwell timer data field
	clearDwellTimer();
	
	// set the cursor position based on the size of the data
	if (dtimer < 10) lcd.setCursor(15,1);
	else if (dtimer < 100) lcd.setCursor(14,1);
	else lcd.setCursor(13,1);
	
	lcd.print(dtimer);
}

void updateCycleStatus(void) {
	// clear the changing data field
	lcd.setCursor(0,1); lcd.print("    ");
	
	// set the cursor based on the number being displayed
	if (myCycleTest.cycle_count < 10) lcd.setCursor(3,1);
	else if (myCycleTest.cycle_count < 100) lcd.setCursor(2,1);
	else if (myCycleTest.cycle_count < 1000) lcd.setCursor(1,1);
	else lcd.setCursor(0,1);
	
	lcd.print(myCycleTest.cycle_count);
	lcd.setCursor(8,1); lcd.print(myCycleTest.num_cycles);
	lcd.setCursor(14,1); lcd.print("  ");
}

void monitorCycleTest(void) {
	// this function will be called 10 times per second...
	#define MAX_RETRIES		5
	
	static uint8_t num_of_tries = 0;
	uint32_t time_ms = 0;		// variable that converts the ticks variable into milliseconds... 1 tick = 10msec
	
	time_ms = millis()-sTime;
	if (myLCD.row1_index == INDEX_CYCLE_ACTIVE) {
		switch (myCycleTest.index) {
			case 0:				// wait for start of cycle - this case should not happen but just in case...
			case 1:       // moves bottom down, assumes both in top position
        botDown();
        sTime = millis();
        myCycleTest.index = 2;
        break;
      case 2:       // wait
        if (time_ms >= (uint32_t(myCycleTest.dwell_time) * 1000)) {
          sTime = millis();
          myCycleTest.index = 3;
          clearDwellTimer();
        }
        else updateDwellTimer(time_ms/1000);  // update the display with the dwell timer
        break;
      case 3:       // move bottom up
        botUp();
        sTime = millis();
        myCycleTest.index = 4;
        break;     
      case 4:       // wait
        if (time_ms >= (uint32_t(myCycleTest.dwell_time) * 1000)) {
          sTime = millis();
          myCycleTest.index = 5;
          clearDwellTimer();
        }
        else updateDwellTimer(time_ms/1000);  // update the display with the dwell timer
        break;
      case 5:       // wait for the motor to reach the bottom position   Added by AP
        myCycleTest.index = 1;
        myCycleTest.cycle_count += 1;
        updateCycleStatus();
        sTime = millis();
        if (myCycleTest.cycle_count == myCycleTest.num_cycles) {
          updateDisplay(0,INDEX_CYCLE_COMPLETE);
        }
        saveCycleData();
       break;
		}
	}
}

void startCycleTest(void) {
	updateDisplay(0,INDEX_CYCLE_ACTIVE);
	updateDisplay(1,INDEX_CYCLE_STATUS);
	updateCycleStatus();
	if (myCycleTest.limit_sensors) {
		lcd.setCursor(12,0); lcd.print("*");
	}
}

void pauseCycleTest(void) {
	saveCycleData();
	updateDisplay(0,INDEX_CYCLE_PAUSED);
}

void reset_myCycleTestVariables(void) {
	myCycleTest.index = 1;
	myCycleTest.cycle_count = 0;
	myCycleTest.move_time = 0;
	myCycleTest.total_test_time = 0;
	saveCycleData();
}

void serialEvent() {
	while (Serial.available()) {
		byte frCH = (byte)Serial.read();							// look at each character - one at a time
		
		if (frComplete == false) {
			if ((frCH == 13) || (frCH == 10)) {						// look for the carriage return or line feed char to complete the frame
				frComplete = true;
			}
			else if ((frCH >= 0x20) && (frCH <= 0x7F)) {			// only add ASCII readable characters to the string (or buffer)
				inStr += (char)frCH;
			}
		}
	}
}



void botUp()  {
  digitalWrite(UP_PIN, LOW);
  delay(250);
  digitalWrite(UP_PIN, HIGH);
}

void botDown()  {
  digitalWrite(DOWN_PIN, LOW);
  delay(250);
  digitalWrite(DOWN_PIN, HIGH);
}

void midUp()  {
  digitalWrite(UP_PIN, LOW);
  delay(500);
  digitalWrite(UP_PIN, HIGH);
  delay(10);
  digitalWrite(UP_PIN, LOW);
  delay(500);
  digitalWrite(UP_PIN, HIGH);
}

void midDown()  {
  digitalWrite(DOWN_PIN, LOW);
  delay(500);
  digitalWrite(DOWN_PIN, HIGH);
  delay(10);
  digitalWrite(DOWN_PIN, LOW);
  delay(500);
  digitalWrite(DOWN_PIN, HIGH);
}