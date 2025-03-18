# Tulip Connect

V1-0-1:
- Bug fix: Circulation pump now also goes on when heating buffer is on temperature (and not only when heating this buffer)

V1-0-2:
- At startup the Tulip Connect sends the next settings to the heatpump:
	1. 255 to address 0x01B7 (password is 255)
	2. 0   to address 0x0174 (Set unit temperature control method to: Return water)
	3. 0   to address 0x0130 (The tank temperature probe is disabled for hot water)
	
V1-0-3:
- Added back and forth traffic for unknown parameters of display and heat pump (probably all settings e.g., min/max values)

V1-0-4:
- Changed minimum NTC temperature value to -400 instead of -450 

V1-0-5:
- Changed SmartEeprom setting: SEEP_ADDR_PUMP_MAXIMUM_OFF_TIME_SEC from 7200 to 86400 seconds (2 hours to 24 hours).
- Changed SmartEeprom setting: SEEP_ADDR_PUMP_ON_TIME_AFTER_OFF_TIME_REACHED_SEC from 600 to 10 seconds (10 minutes to 10 seconds).
- Changed SmartEeprom setting: SEEP_ADDR_PUMP_LAG_TIME_AFTER_THERMOSTAT_CONTACT_DISCONNECTED_SEC from 120 to 60 seconds.
- Bug fix in smartEeprom read and write 16 and 32 bits addresses.

V1-0-6:
- The heat pump gave an alarm when the 3-way valve was switching while the water pump was running. 
  This is solved by switching off the heat pump first, waiting for the pump to stop, and only then switching on the 3-way valve.  
- Setpoint can now be changed when in heating of hot water mode.
- At startup the Tulip Connect sends the next settings extra to the heatpump:
	1. 5   to address 0x011A (Air conditioner return difference)
	2. 1   to address 0x0125 (Device reaching target temperature and shutdown mode)
	
V1-0-7:
- Added hardcoded firmware version
- Firmware version can be seen in debug output
- Added CONSOLE, DEBUG, and COMMAND in Harmony for debug output
- Redesigned the Heating and hot water statemachine and states. 
- Improvement of sending settings to the heatpump
- Settings are now directly adjusted in the display
- Hardcoded that the circulation pump can only be on in the heating states
- Timer counters all on 1 timer
- The Connect now looks at startup which NTC's are connected, and adjusts the mode setting on the screen
- 3-way valve goes back to heating circuit when in Idle mode
- In hot water mode, the setpoint is increased with 5 degrees because the heatpump stopped to early sometimes, even before the setpoint was reached in the buffer. 

V1-0-8:
- Hardcoded firmware version set to 1-0-8
- At startup address 0x011A (Air conditioner return difference) is no longer set to 5.
- Address 0x0801 (High temperature sterilization function) is not set to 0 at startup. 
- Application start address changed 0 to 0x40000, because of the bootloader functionality. 

V1-0-9:
- Hardcoded firmware version set to 1-0-9
- Updated the debug output.
- Added Sterilization functionality.
	1. Keeps track of time and days (in smart eeprom).
	2. Added some Smart Eeprom settings.
	3. Does the sterilization mode every set moment (e.g. every 7 days at 14 pm). 
	4. When it can't reach the sterilization temperature in time, the heating element turns on.
- Added defrosting functionality.
	1. Added some Smart Eeprom settings.
	2. When in hot water or sterilization mode, keeps track of the temperature and turns on the element if the temperature falls to much.
