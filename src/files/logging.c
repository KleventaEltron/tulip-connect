#include <xc.h>
#include <string.h>
#include <stdbool.h>                   
#include "definitions.h"
#include "configuration.h"

#include "logging.h"
#include "modbus/heatpump_parameters.h"
#include "i2c/mac.h"
#include "alarms.h"
#include "eeprom.h"
#include "ntc.h"
#include "../config/default/user.h"
#include "../app_sd_card_tasks.h"
#include "sdcard.h"

#include "third_party/../time.h"
#include "modbus/display.h"
#include "osal/osal.h"
#include "tcpip/tcpip.h"
#include "credentials.h"
//#include "delay.h"

/* Used in sending and receiving data to and from the server */
#define TRUE true
#define FALSE false
#define UNDEFINED_UINT16_T -32768
#define UNDEFINED_INT16_T 32767 
#define UNDEFINED_STRING "UNDEFINED"
#define UNDEFINED_CHAR "U"

//extern const TCPIP_NETWORK_CONFIG __attribute__((unused))  TCPIP_HOSTS_CONFIGURATION[];
extern const TCPIP_STACK_MODULE_CONFIG TCPIP_STACK_MODULE_CONFIG_TBL [];
//extern const size_t TCPIP_HOSTS_CONFIGURATION_SIZE;
extern const size_t TCPIP_STACK_MODULE_CONFIG_TBL_SIZE;
extern APP_HEATING_AND_HOT_WATER_DATA app_Data;

NET_PRES_SKT_HANDLE_T   socket;
IP_MULTI_ADDRESS        address;

DEVICE_TYPE DEVICE_TYPE_REQUESTED = -1; 
char NTP_TIME_BUFFER[40];

uint8_t TcpIPStackResetCount = 0;

OSAL_MUTEX_DECLARE(loggingMutex);


void initializeMutex() {
    //SYS_CONSOLE_PRINT("**** APP STATE BEFORE ANYTHING >> %i\r\n", app_Data.appState);
    if (OSAL_MUTEX_Create(&loggingMutex) != OSAL_RESULT_TRUE) {}
}



bool setLoggingLock() {
    if (OSAL_MUTEX_Lock(&loggingMutex, OSAL_WAIT_FOREVER ) == OSAL_RESULT_TRUE) {
        //SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "***** Locked *****\r\n");
        return true;
    } 
    return false;
}



bool releaseLoggingLock() {
    if (OSAL_MUTEX_Unlock(&loggingMutex) == OSAL_RESULT_TRUE) {
        //SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "***** unlock *****\r\n");
        return true;
    } 
    return false;
}



bool setLoggingLockNoPrint() {
    if (OSAL_MUTEX_Lock(&loggingMutex, OSAL_WAIT_FOREVER ) == OSAL_RESULT_TRUE) {
        return true;
    } 
    return false;
}



bool releaseLoggingLockNoPrint() {
    if (OSAL_MUTEX_Unlock(&loggingMutex) == OSAL_RESULT_TRUE) {
        return true;
    } 
    return false;
}



bool isEthernetCableConnected(TCPIP_NET_HANDLE netH) {
    if (netH != NULL && !TCPIP_STACK_NetIsLinked(netH)){
        return false;
    }
    return true;
}

void increaseTcpIpResetCounter() {
    TcpIPStackResetCount += 1;
}

uint8_t getTcpIpResetCounter() {
    return TcpIPStackResetCount;
}



char * getLoggingStateToString(APP_LOGGING_TASKS_STATES logState) {
    switch(logState){
        case(APP_LOGGING_TASKS_STATE_INIT): return "0, Init"; break;        
        case(APP_LOGGING_TASKS_IDLE): return "1, Idle"; break;   
        case(APP_LOGGING_TASKS_WAIT_FOR_TCPIP_INIT): return "2, TCPIP Init"; break;    
        case(APP_LOGGING_TASKS_WAIT_FOR_IP): return "3, Wait IP"; break;    
        case(APP_LOGGING_TASKS_WAIT_FOR_SNTP): return "4, Wait SNTP"; break;    
        case(APP_LOGGING_TASKS_PARSE_STRING_TO_IP): return "5, Parse IP"; break;      
        case(APP_LOGGING_TASKS_WAIT_ON_DNS):  return "6, Wait DNS"; break;    
        case(APP_LOGGING_TASKS_START_CONNECTION): return "7, Start Conn"; break;             
        case(APP_LOGGING_TASKS_WAIT_FOR_CONNECTION): return "8, Wait Conn"; break;         
        case(APP_LOGGING_TASKS_WAIT_FOR_SSL_CONNECT): return "9, Wait SSL"; break;       
        case(APP_LOGGING_TASKS_SEND_REQUEST_SSL): return "10, Send Request"; break;    
        case(APP_LOGGING_TASKS_WAIT_FOR_RESPONSE_SSL): return "11, Wait Response"; break;           
        case(APP_LOGGING_TASKS_CLOSE_CONNECTION): return "12, Close Conn"; break;     
        case(APP_LOGGING_TASKS_WAIT_FOR_LOGGING_UNLOCK): return "13, Wait Unlock"; break;    
        default: return "14, Unkown"; break;
    }
}


bool setupNewTcpipStack() {
    TCPIP_STACK_INIT    tcpipInit;
    
    char macToString[40];
    
    uint8_t firstMacByte = eui64[2];
    firstMacByte |= 0x02;  // Set "locally administered" bit
    firstMacByte &= ~0x01; 
    
    sprintf(macToString, "%02X:%02X:%02X:%02X:%02X:%02X", 
            firstMacByte,eui64[3],eui64[4],eui64[5],eui64[6],eui64[7]);
    
    //SYS_CONSOLE_PRINT("!!! MAC ADDRESS SET TO %s !!!\r\n", macToString);        
    
    TCPIP_NETWORK_CONFIG __attribute__((unused))  TCPIP_HOSTS_CONFIGURATION_CUSTOM[] =
    {
        /*** Network Configuration Index 0 ***/
        {
            .interface = TCPIP_NETWORK_DEFAULT_INTERFACE_NAME_IDX0,
            .hostName = TCPIP_NETWORK_DEFAULT_HOST_NAME_IDX0,
            .macAddr = macToString, // Change default MAC to MAC obtained from IC
            .ipAddr = TCPIP_NETWORK_DEFAULT_IP_ADDRESS_IDX0,
            .ipMask = TCPIP_NETWORK_DEFAULT_IP_MASK_IDX0,
            .gateway = TCPIP_NETWORK_DEFAULT_GATEWAY_IDX0,
            .priDNS = TCPIP_NETWORK_DEFAULT_DNS_IDX0,
            .secondDNS = TCPIP_NETWORK_DEFAULT_SECOND_DNS_IDX0,
            .powerMode = TCPIP_NETWORK_DEFAULT_POWER_MODE_IDX0,
            .startFlags = TCPIP_NETWORK_DEFAULT_INTERFACE_FLAGS_IDX0,
            .pMacObject = &TCPIP_NETWORK_DEFAULT_MAC_DRIVER_IDX0,
        },
    };
    size_t TCPIP_HOSTS_CONFIGURATION_SIZE_CUSTOM = sizeof (TCPIP_HOSTS_CONFIGURATION_CUSTOM) / sizeof (*TCPIP_HOSTS_CONFIGURATION_CUSTOM);
    
    tcpipInit.pNetConf = TCPIP_HOSTS_CONFIGURATION_CUSTOM;
    tcpipInit.nNets = TCPIP_HOSTS_CONFIGURATION_SIZE_CUSTOM;
    tcpipInit.pModConfig = TCPIP_STACK_MODULE_CONFIG_TBL;
    tcpipInit.nModules = TCPIP_STACK_MODULE_CONFIG_TBL_SIZE;
    tcpipInit.initCback = 0;

    sysObj.tcpip = TCPIP_STACK_Initialize(0, &tcpipInit.moduleInit);
    if (sysObj.tcpip == SYS_MODULE_OBJ_INVALID) {
        SYS_CONSOLE_PRINT("TCPIP_STACK_Init Failed!\n");
        return false;
    }    
    return true;
}



void setHardwareId( char device[], char* hardwareId, int modbusIndex ) {
    switch (DEVICE_TYPE_REQUESTED){
        case TULIP_PRINT:{
            sprintf(hardwareId, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                    eui64[0], eui64[1], eui64[2], eui64[3], eui64[4], eui64[5], eui64[6], eui64[7]);       
            break;
        }
        
        case HEATPUMP:{
            sprintf(hardwareId, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X-W-%i",
                    eui64[0], eui64[1], eui64[2], eui64[3], eui64[4], eui64[5], eui64[6], eui64[7], modbusIndex);
            break;
        }
        
        case BATTERY:{
            sprintf(hardwareId, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X-B-%i",
                    eui64[0], eui64[1], eui64[2], eui64[3], eui64[4], eui64[5], eui64[6], eui64[7], modbusIndex);            
            break;
        }
        
        case SMART_METER:{
            sprintf(hardwareId, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X-SM-%i",
                    eui64[0], eui64[1], eui64[2], eui64[3], eui64[4], eui64[5], eui64[6], eui64[7], modbusIndex);            
            break;
        }
        
        case INVERTER:{
            sprintf(hardwareId, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X-I-%i",
                    eui64[0], eui64[1], eui64[2], eui64[3], eui64[4], eui64[5], eui64[6], eui64[7], modbusIndex);            
            break;
        }
        
        case HEATPUMP_BOILER:{
            sprintf(hardwareId, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X-HB-%i",
                    eui64[0], eui64[1], eui64[2], eui64[3], eui64[4], eui64[5], eui64[6], eui64[7], modbusIndex);            
            break;
        }
        
        /* ZOU NOOIT GECALLED MOETEN WORDEN */
        default:{
            break;
        }
    }
    return;
}



bool getCurrentUtcTimestamp ( void ) {
    uint32_t utcSeconds = TCPIP_SNTP_UTCSecondsGet();
    if(utcSeconds > 0) {
        utcSeconds += 7200; // +2 uur voor converteren tussen tijdzones
        time_t utc_time_t = utcSeconds;
        struct tm *utc_time = gmtime(&utc_time_t);
        strftime(NTP_TIME_BUFFER, sizeof(NTP_TIME_BUFFER), "%Y-%m-%d %H:%M:%S", utc_time);
        //SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "LOG TIMESTAMP %s\r\n", NTP_TIME_BUFFER);
        
        if (sizeof (NTP_TIME_BUFFER) > 0) {
            return true;
        }
        return false;
    }
    return false;
}



char parameter[100];
void setLogValue_STRING( char* requestBuilder, char settingName[], char value[]) {
    sprintf(parameter, "\"%s\": \"%s\",", settingName, value);
    strcat(requestBuilder, parameter);
    memset(parameter, 0, sizeof (parameter));
    return;
}



void setLogValue_BOOLEAN( char* requestBuilder, char settingName[], bool value) {
    sprintf(parameter, "\"%s\": %d,", settingName, value);
    strcat(requestBuilder, parameter);
    memset(parameter, 0, sizeof (parameter));
    return;
}



void setLogValue_NUMBER( char* requestBuilder, char settingName[], int value ) {
    sprintf(parameter, "\"%s\": %i,", settingName, value);
    strcat(requestBuilder, parameter);
    memset(parameter, 0, sizeof (parameter));
    return;
}





void setLoggingDataPerDeviceType ( char* requestBuilder, char device[]) {
    char hardwareId[50];
   
    /* SET THE DEVICE TYPE IN THE HTTP HEADER */
    sprintf(parameter, "{\"type\": \"%s\",", device);
    strcat(requestBuilder, parameter);
    memset(parameter, 0, sizeof (parameter));
    
    /* SET THE HARDWARE ID IN THE HTTP HEADER */
    setHardwareId( device, hardwareId, 1 );
    sprintf(parameter, "\"hardware_id\": \"%s\",", hardwareId);
    strcat(requestBuilder, parameter);
    memset(parameter, 0, sizeof (parameter));
        
    /* ADD THE REQUEST BODY TO THE HTTP */
    strcat(requestBuilder, "\"values\": {");
    switch (DEVICE_TYPE_REQUESTED){
        
        case TULIP_PRINT:{
            setLogValue_STRING(requestBuilder, HardwareID, hardwareId);          
            setLogValue_STRING(requestBuilder, KlantID, UNDEFINED_STRING);
            
            /* Software versie */
            sprintf(parameter, "\"%s\": \"%d-%d-%d\",", SoftwareVersie, (int)((THIS_FIRMWARE_VERSION / 1000000)), (int)((THIS_FIRMWARE_VERSION / 1000) % 1000), (int)(THIS_FIRMWARE_VERSION % 1000));
            strcat(requestBuilder, parameter);
            memset(parameter, 0, sizeof (parameter));
            
            /* Bootloader software versie */
            uint32_t BOOTLOADER_FIRMWARE_VERSION = ReadSmartEeprom32(BOOTLOADER_SOFTWARE_VERSION);
            sprintf(parameter, "\"%s\": \"%d-%d-%d\",", BootloaderSoftwareVersie, (int)((BOOTLOADER_FIRMWARE_VERSION / 1000000)), (int)((BOOTLOADER_FIRMWARE_VERSION / 1000) % 1000), (int)(BOOTLOADER_FIRMWARE_VERSION % 1000));
            strcat(requestBuilder, parameter);
            memset(parameter, 0, sizeof (parameter));
                
            //setLogValue_STRING(requestBuilder, BootloaderSoftwareVersie, UNDEFINED_STRING);
            setLogValue_STRING(requestBuilder, RTCCDatumtijd, NTP_TIME_BUFFER);    
            setLogValue_STRING(requestBuilder, NTPDatumtijd, NTP_TIME_BUFFER);   
            setLogValue_NUMBER(requestBuilder, Lognummer, UNDEFINED_INT16_T);
            
            setLogValue_NUMBER(requestBuilder, ConnectTemp1, GetNtcTemperature(NTC_HEATING_BUFFER));
            setLogValue_NUMBER(requestBuilder, ConnectTemp2, GetNtcTemperature(NTC_HOT_WATER_BUFFER));
            setLogValue_NUMBER(requestBuilder, ConnectTemp3, GetNtcTemperature(NTC_RESERVED_1));
            setLogValue_NUMBER(requestBuilder, ConnectTemp4, GetNtcTemperature(NTC_RESERVED_2));
            
            setLogValue_BOOLEAN(requestBuilder, ConnectInput1, GetThermostatContact());
            setLogValue_BOOLEAN(requestBuilder, ConnectInput2, GetDigitalInput2());
            setLogValue_BOOLEAN(requestBuilder, ConnectInput3, GetDigitalInput3());
            
            setLogValue_BOOLEAN(requestBuilder, ConnectRelay1, RelayPotfree1_Get());
            setLogValue_BOOLEAN(requestBuilder, ConnectRelay2, RelayPotfree2_Get());
            setLogValue_BOOLEAN(requestBuilder, ConnectRelay3, RelayPotfree3_Get());
            setLogValue_BOOLEAN(requestBuilder, BufferElekElement, getStatusHeatingElementHeatingBuffer());
            setLogValue_BOOLEAN(requestBuilder, TapwaterElekElement, getStatusHeatingElementHotWaterBuffer());
            
            setLogValue_BOOLEAN(requestBuilder, Reserved230vRelay, getReserved230VRelay());
            setLogValue_BOOLEAN(requestBuilder, ThreeWayValveRelay, getStatus3WayValve());
            
            setLogValue_BOOLEAN(requestBuilder, CirculatorPump, getStatusCirculationPump());
            setLogValue_BOOLEAN(requestBuilder, SwitchInput, getInputSlideSwitch());
            
            setLogValue_BOOLEAN(requestBuilder, VerbondenMetServer, TRUE);
            setLogValue_BOOLEAN(requestBuilder, DIPswitch1, GetDip1());
            setLogValue_BOOLEAN(requestBuilder, DIPswitch2, GetDip2());
            setLogValue_BOOLEAN(requestBuilder, DIPswitch3, GetDip3());
            setLogValue_BOOLEAN(requestBuilder, DIPswitch4, GetDip4());
            
            //setLogValue_BOOLEAN(requestBuilder, ConfigModusActief, FALSE);
            //setLogValue_STRING(requestBuilder, WifiStatus, UNDEFINED_CHAR);   
            //setLogValue_NUMBER(requestBuilder, WifiInfo, UNDEFINED_INT16_T);
            
            setLogValue_BOOLEAN(requestBuilder, SDcardAanwezig, SD_CARD_MOUNT_FLAG);
            setLogValue_NUMBER(requestBuilder, SDstatus, getSdCardState());          
            
            //setLogValue_STRING(requestBuilder, RS485commMetDisplayOK, UNDEFINED_CHAR);
            //setLogValue_STRING(requestBuilder, RS485ommMetWarmtepompOK, UNDEFINED_CHAR);
            //setLogValue_NUMBER(requestBuilder, EthernetStatus, UNDEFINED_INT16_T);  
            //setLogValue_NUMBER(requestBuilder, USBstatus, UNDEFINED_INT16_T);  
            
            setLogValue_BOOLEAN(requestBuilder, PowerFailStatus, getPowerFailStatus());
            setLogValue_BOOLEAN(requestBuilder, SupercapacitorFaultStatus, getSupercapFaultStatus());
            setLogValue_BOOLEAN(requestBuilder, Systemgoodindicator, getSystemGoodIndicator());
            setLogValue_BOOLEAN(requestBuilder, SupercapacitorPowerGoodIndicator, getSupercapacitorPowerGoodIndicator());
            
            setLogValue_NUMBER(requestBuilder, Alarmbytes, GetAlarm(0)); 
            
            requestBuilder[strlen(requestBuilder)-1] = '\0';
            break;
        }
        
        
        
        case HEATPUMP:{
            setLogValue_NUMBER(requestBuilder, "WL1", getDataFromMemoryCallable(ADDRESS_RUNNING_STATUS_1));
            setLogValue_NUMBER(requestBuilder, "WL2", getDataFromMemoryCallable(ADDRESS_RUNNING_STATUS_2));
            setLogValue_NUMBER(requestBuilder, "WL3", getDataFromMemoryCallable(ADDRESS_FAULT_STATE_1));
            setLogValue_NUMBER(requestBuilder, "WL4", getDataFromMemoryCallable(ADDRESS_FAULT_STATE_2));
            setLogValue_NUMBER(requestBuilder, "WL5", getDataFromMemoryCallable(ADDRESS_FAULT_STATE_3));
            setLogValue_NUMBER(requestBuilder, "WL6", getDataFromMemoryCallable(ADDRESS_SYSTEM_1_FAULT_STATE_1));
            setLogValue_NUMBER(requestBuilder, "WL7", getDataFromMemoryCallable(ADDRESS_SYSTEM_1_FAULT_STATE_2));
            setLogValue_NUMBER(requestBuilder, "WL8", getDataFromMemoryCallable(ADDRESS_SYSTEM_1_DRIVE_FAULT_STATE_1));
            setLogValue_NUMBER(requestBuilder, "WL9", getDataFromMemoryCallable(ADDRESS_SYSTEM_1_DRIVE_FAULT_STATE_2));
            setLogValue_NUMBER(requestBuilder, "WL10", getDataFromMemoryCallable(ADDRESS_SYSTEM_1_DRIVE_FAULT_STATE_3));
            
            setLogValue_NUMBER(requestBuilder, "WL11", getDataFromMemoryCallable(ADDRESS_RELAY_OUTPUT_STATUS_1));
            setLogValue_NUMBER(requestBuilder, "WL12", getDataFromMemoryCallable(ADDRESS_RELAY_OUTPUT_STATUS_2));
            setLogValue_NUMBER(requestBuilder, "WL13", getDataFromMemoryCallable(ADDRESS_RELAY_OUTPUT_STATUS_3));
            setLogValue_NUMBER(requestBuilder, "WL14", getDataFromMemoryCallable(ADDRESS_RELAY_OUTPUT_STATUS_4));
            setLogValue_NUMBER(requestBuilder, "WL15", getDataFromMemoryCallable(ADDRESS_SWITCH_PORT_STATE_1));
            setLogValue_NUMBER(requestBuilder, "WL16", getDataFromMemoryCallable(ADDRESS_SWITCH_PORT_STATE_2));
            setLogValue_NUMBER(requestBuilder, "WL17", getDataFromMemoryCallable(ADDRESS_SWITCH_PORT_STATE_3));
            setLogValue_NUMBER(requestBuilder, "WL18", getDataFromMemoryCallable(ADDRESS_SWITCH_PORT_STATE_4));
            setLogValue_NUMBER(requestBuilder, "WL19", getDataFromMemoryCallable(ADDRESS_CURRENT_UNIT_TOOLING_NO));
            setLogValue_NUMBER(requestBuilder, "WL20", (int16_t)getDataFromMemoryCallable(ADDRESS_COMPRESSOR_1_TARGET_FREQUENCY));
            
            setLogValue_NUMBER(requestBuilder, "WL21", (int16_t)getDataFromMemoryCallable(ADDRESS_COMPRESSOR_OPERATING_FREQUENCY));
            setLogValue_NUMBER(requestBuilder, "WL22", (int16_t)getDataFromMemoryCallable(ADDRESS_FAN_OPERATING_FREQUENCY_ROTATIONAL_SPEED));
            setLogValue_NUMBER(requestBuilder, "WL23", (int16_t)getDataFromMemoryCallable(ADDRESS_ELECTRONIC_EXPANSION_VALVE_STEPS_COUNT));
            setLogValue_NUMBER(requestBuilder, "WL24", (int16_t)getDataFromMemoryCallable(ADDRESS_NUMBER_OF_EVI_VALVE_STEPS));
            setLogValue_NUMBER(requestBuilder, "WL25", (int16_t)getDataFromMemoryCallable(ADDRESS_AC_INPUT_VOLTAGE));
            setLogValue_NUMBER(requestBuilder, "WL26", (int16_t)getDataFromMemoryCallable(ADDRESS_AC_INPUT_CURRENT));
            setLogValue_NUMBER(requestBuilder, "WL27", (int16_t)getDataFromMemoryCallable(ADDRESS_COMPRESSOR_PHASE_CURRENT));
            setLogValue_NUMBER(requestBuilder, "WL28", (int16_t)getDataFromMemoryCallable(ADDRESS_COMPRESSOR_IPM_TEMPERATURE));
            setLogValue_NUMBER(requestBuilder, "WL29", (int16_t)getDataFromMemoryCallable(ADDRESS_HIGH_PRESSURE_SATURATION_TEMPERATURE));
            setLogValue_NUMBER(requestBuilder, "WL30", (int16_t)getDataFromMemoryCallable(ADDRESS_LOW_PRESSURE_SATURATION_TEMPERATURE));
            
            setLogValue_NUMBER(requestBuilder, "WL31", (int16_t)getDataFromMemoryCallable(ADDRESS_EXTERNAL_AMBIENT_TEMPERATURE_T1));
            setLogValue_NUMBER(requestBuilder, "WL32", (int16_t)getDataFromMemoryCallable(ADDRESS_EXTERNAL_COIL_TUBE_FIN_T2));
            setLogValue_NUMBER(requestBuilder, "WL33", (int16_t)getDataFromMemoryCallable(ADDRESS_INTERNAL_COIL_TUBE_PLATE_REPLACEMENT_T3));
            setLogValue_NUMBER(requestBuilder, "WL34", (int16_t)getDataFromMemoryCallable(ADDRESS_RETURN_AIR_TEMPERATURE_T4));
            setLogValue_NUMBER(requestBuilder, "WL35", (int16_t)getDataFromMemoryCallable(ADDRESS_EXHAUST_TEMPERATURE_T5));
            setLogValue_NUMBER(requestBuilder, "WL36", (int16_t)getDataFromMemoryCallable(ADDRESS_RETURN_WATER_TEMPERATURE_T6));
            setLogValue_NUMBER(requestBuilder, "WL37", (int16_t)getDataFromMemoryCallable(ADDRESS_WATER_OUTLET_TEMPERATURE_T7));
            setLogValue_NUMBER(requestBuilder, "WL38", (int16_t)getDataFromMemoryCallable(ADDRESS_ECONOMIZER_INLET_TUBE_T8));
            setLogValue_NUMBER(requestBuilder, "WL39", (int16_t)getDataFromMemoryCallable(ADDRESS_ECONOMIZER_OUTLET_TUBE_T9));
            setLogValue_NUMBER(requestBuilder, "WL40", (int16_t)getDataFromMemoryCallable(ADDRESS_WATER_TANK_TEMPERATURE));
            
            setLogValue_NUMBER(requestBuilder, "WL41", (int16_t)getDataFromMemoryCallable(ADDRESS_FLUORINE_OUTLET_TEMPERATURE_OF_PLATE_HEAT_EXCHANGER));
            setLogValue_NUMBER(requestBuilder, "WL42", (int16_t)getDataFromMemoryCallable(ADDRESS_DRIVE_MANUFACTURER));
            setLogValue_NUMBER(requestBuilder, "WL43", (int16_t)getDataFromMemoryCallable(ADDRESS_PUMP_SPEED_PWM));
            setLogValue_NUMBER(requestBuilder, "WL44", (int16_t)getDataFromMemoryCallable(ADDRESS_WATER_FLOW));
            setLogValue_NUMBER(requestBuilder, "WL45", (int16_t)getDataFromMemoryCallable(ADDRESS_USER_RETURN_WATER_TEMPERATURE));
            setLogValue_NUMBER(requestBuilder, "WL46", (int16_t)getDataFromMemoryCallable(ADDRESS_DEVICE_INPUT_VOLTAGE));
            setLogValue_NUMBER(requestBuilder, "WL47", (int16_t)getDataFromMemoryCallable(ADDRESS_DEVICE_INPUT_CURRENT));
            setLogValue_NUMBER(requestBuilder, "WL48", (int16_t)getDataFromMemoryCallable(ADDRESS_DEVICE_INPUT_POWER_KW));
            setLogValue_NUMBER(requestBuilder, "WL49", (int16_t)getDataFromMemoryCallable(ADDRESS_TOTAL_UNIT_ELECTRICITY_CONSUMPTION_KWH));
            setLogValue_NUMBER(requestBuilder, "WL50", (int16_t)getDataFromMemoryCallable(ADDRESS_AUXILIARY_HEATING_SOURCE_HOT_WATER_TEMPERATURE_VALUE));
            
            setLogValue_NUMBER(requestBuilder, "WL51", (int16_t)getDataFromMemoryCallable(ADDRESS_AUXILIARY_HEATING_SOURCE_HEATING_TEMPERATURE_VALUE));
            setLogValue_NUMBER(requestBuilder, "WL52", (int16_t)getDataFromMemoryCallable(ADDRESS_BUFFER_TANK_FOR_HEATING_TEMPERATURE_VALUE));
            setLogValue_NUMBER(requestBuilder, "WL53", (int16_t)getDataFromMemoryCallable(ADDRESS_MAIN_OUTLET_WATER_TEMPERATURE_VALUE));
            setLogValue_NUMBER(requestBuilder, "WL54", (int16_t)getDataFromMemoryCallable(ADDRESS_WATER_INLET_TEMPERATURE));
            setLogValue_NUMBER(requestBuilder, "WL55", (int16_t)getDataFromMemoryCallable(ADDRESS_WATER_OUTLET_TEMPERATURE));
            setLogValue_NUMBER(requestBuilder, "WL56", (int16_t)getDataFromMemoryCallable(ADDRESS_EXTERNAL_ENVIRONMENT_TEMPERATURE));
            setLogValue_NUMBER(requestBuilder, "WL57", (int16_t)getDataFromMemoryCallable(ADDRESS_WATER_TANK_TEMPERATURE_2));
            
            setLogValue_NUMBER(requestBuilder, "WL58", (int16_t)app_Data.appState);
            setLogValue_NUMBER(requestBuilder, "WL59", UserParameters[ADDRESS_HEATING_SET_TEMPERATURE - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP]);
            setLogValue_NUMBER(requestBuilder, "WL60", ReadSmartEeprom16(SEEP_ADDR_HEATING_SETPOINT));
            setLogValue_NUMBER(requestBuilder, "WL61", ReadSmartEeprom16(SEEP_ADDR_HOT_WATER_SETPOINT));
            setLogValue_NUMBER(requestBuilder, "WL62", UnitSystemParameterL[ADDRESS_STERILIZATION_TEMPERATURE_SETTING - START_ADDRESS_UNIT_SYSTEM_PARAMETER_L][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP]);
            setLogValue_NUMBER(requestBuilder, "WL63", UserParameters[ADDRESS_COOLING_SET_TEMPERATURE - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP]);    
            
            
            /*
             * Vervangen waardes
            setLogValue_NUMBER(requestBuilder, "WL58", (int16_t)getDataFromMemoryCallable(ADDRESS_SET_THE_UPPER_LIMIT_OF_FLOOR_HEATING_HEATING_HEATING_TEMPERATURE));
            setLogValue_NUMBER(requestBuilder, "WL59", (int16_t)getDataFromMemoryCallable(ADDRESS_SET_THE_LOWER_LIMIT_OF_FLOOR_HEATING_HEATING_HEATING_TEMPERATURE));
            setLogValue_NUMBER(requestBuilder, "WL60", (int16_t)getDataFromMemoryCallable(ADDRESS_SET_THE_UPPER_LIMIT_OF_HOT_WATER_TEMPERATURE));
            setLogValue_NUMBER(requestBuilder, "WL61", (int16_t)getDataFromMemoryCallable(ADDRESS_SET_THE_LOWER_LIMIT_OF_HOT_WATER_TEMPERATURE));
            setLogValue_NUMBER(requestBuilder, "WL62", (int16_t)getDataFromMemoryCallable(ADDRESS_SET_THE_UPPER_LIMIT_OF_REFRIGERATING_TEMPERATURE));
            setLogValue_NUMBER(requestBuilder, "WL63", (int16_t)getDataFromMemoryCallable(ADDRESS_SET_THE_LOWER_LIMIT_OF_REFRIGERATING_TEMPERATURE));       
            */     
            requestBuilder[strlen(requestBuilder)-1] = '\0';
            break;
        }
        
        
        case BATTERY:{
                       
            break;
        }
        
        case SMART_METER:{
                     
            break;
        }
        
        case INVERTER:{
                       
            break;
        }
        
        case HEATPUMP_BOILER:{
                      
            break;
        }
        
        /* ZOU NOOIT GECALLED MOETEN WORDEN */
        default:{
            break;
        }
    }
    strcat(requestBuilder, "}}");
    return;
}



void parseDeviceType (DEVICE_TYPE dvt, char* device) {
    DEVICE_TYPE_REQUESTED = dvt;
    switch (dvt) {
        case TULIP_PRINT:{
            strcpy(device, "tulip_print");
            break;
        }
        
        case HEATPUMP:{
            strcpy(device, "heatpump");
            break;
        }
        
        case BATTERY:{
            break;
        }
        
        case SMART_METER:{
            break;
        }
        
        case INVERTER:{
            break;
        }
        
        case HEATPUMP_BOILER:{
            break;
        }
        
        /* ZOU NOOIT GECALLED MOETEN WORDEN */
        default:{
            break;
        }
    }    
    return;
}



void getRealTimeDataStatussen ( char requestBuilder[] ) {   
    char dateTimeSet[150];
    char device[20];
              
    sprintf(dateTimeSet, "{\"date_time_utc\": \"%s\",", NTP_TIME_BUFFER);
    strcat(requestBuilder, dateTimeSet);  
    strcat(requestBuilder, "\"checksum\": \"xxxyyy\",");
    strcat(requestBuilder, "\"devices\": [");
    
    /* LOG TULIP PRINT DATA */
    parseDeviceType(TULIP_PRINT, device);  
    setLoggingDataPerDeviceType(requestBuilder, device);
    memset(device, 0, sizeof (device));   
    
    /* ALS GEEN COMMUNICATIE ERROR MET DE WARMTE POMP LOG OOK DEZE WAARDES */
    if (!GetAlarm(ALARM_HEATPUMP_COMMUNICATION)) {
    //if (GetAlarm(ALARM_HEATPUMP_COMMUNICATION)) {
        strcat(requestBuilder, ",");
        parseDeviceType(HEATPUMP, device);
        setLoggingDataPerDeviceType(requestBuilder, device);
    }
    
    strcat(requestBuilder, "]}");
    return;  
}



void parseRequestType (REQUEST_TYPE rqt, char* request) {
    switch(rqt) {
        case GET:{
            strcpy(request, "GET");
            break;
        }    
        case POST:{
            strcpy(request, "POST");
            break;
        }
        case PUT:{
            strcpy(request, "PUT");
            break;
        } 
        case PATCH:{
            strcpy(request, "PATCH");
            break;
        }  
        case DELETE:{
            strcpy(request, "DELETE");
            break;
        }
        default:{
            break;
        }        
    }
    return;
}



void parsePath (char* path, REQUEST_TYPE rqt) {
    if (rqt == GET) { sprintf(path, "%s", GET_PATH); }
    else if (rqt == POST) { sprintf(path, "%s", POST_PATH); }
    return;
}



bool TULIP_REQUEST_TESTER ( char request[], char path[]) {
    char networkBuffer[4096];
    char requestBody[2048];

    getRealTimeDataStatussen(requestBody);           
    
    sprintf(networkBuffer, "%s /%s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "Accept: application/json\r\n"
                    "Content-Type: application/json\r\n"
                    "Authorization: Bearer %s\r\n"
                    "Connection: close\r\n"
                    "Content-Length: %i\r\n\r\n"
                    "%s\r\n",
                    request, path, HOST, TOKEN, strlen(requestBody), requestBody);     
 
    uint16_t bytesSend = NET_PRES_SocketWrite(socket, (uint8_t*) networkBuffer, strlen(networkBuffer));
    
    /* Er was een probleem tijdens het versturen van het pakket */
    if (bytesSend <= 0) {
        SYS_CONSOLE_PRINT("***** COULD NOT COMPLETE REQUEST, CHECK IF NETWORK TX BUFFER SIZE IS BIG ENOUGH TO SEND THE REQUEST! SIZE NOW >> %d *****\r\n", strlen(networkBuffer));  
        return false;
    } else {
        //SYS_CONSOLE_PRINT("%s", networkBuffer);
        SYS_CONSOLE_PRINT("*** REQUEST SEND WITH FOLLOWING SETTINGS: %s, %s, %s\r\n", request, path, NTP_TIME_BUFFER);
    }
    DEVICE_TYPE_REQUESTED = -1;  
    return true;
}     



//void loggingRequestBuilder ( REQUEST_TYPE rqt, DEVICE_TYPE dvt) {
bool loggingRequestBuilder ( REQUEST_TYPE rqt ) {
    char request[7];
    char path[20];
    
    getCurrentUtcTimestamp();
    parseRequestType(rqt, request);
    parsePath(path, rqt);
                
    return TULIP_REQUEST_TESTER(request, path);
}



/*
 * Read out the server response from the network buffer.
 */
void readNetworkBufferSslResponse ( void ) {
    char networkBuffer[2048];
    uint16_t res = NET_PRES_SocketRead(socket, (uint8_t*) networkBuffer, sizeof (networkBuffer));
            
    SYS_CONSOLE_PRINT("Received %d bytes from the server\r\n", res);
    
    //UNCOMMENT VOOR HELE RESPONSE
    for(int i = 0; i < res; i++) {
        SYS_CONSOLE_PRINT("%c", networkBuffer[i]);
    }
    
    return;
}



/*
 * Starts the TCP connection the the server on a given PORT
 */
bool startSocketConnection ( void ) {
    //SYS_CONSOLE_PRINT("Starting TCP/IPv4 Connection to : %d.%d.%d.%d port  '%d'\r\n",
    //    address.v4Add.v[0], address.v4Add.v[1], address.v4Add.v[2], address.v4Add.v[3], PORT);
                
    NET_PRES_SKT_ERROR_T *error = NULL;
    socket = NET_PRES_SocketOpen(0, NET_PRES_SKT_UNENCRYPTED_STREAM_CLIENT, IP_ADDRESS_TYPE_IPV4, PORT, (NET_PRES_ADDRESS *)&address, error);                    
    NET_PRES_SocketWasReset(socket);
            
    if (socket == INVALID_SOCKET) {
        SYS_CONSOLE_MESSAGE("Could not create socket - aborting\r\n");
        return false;
    }
    return true;
}



/*
 * Wait for the connection to either be established successfully or fail.
 * In case of failing the process is aborted.
 */
SSL_NEGOTIATION_STATES waitForConnection ( void ) {
    if (!NET_PRES_SocketIsConnected(socket)) {
        return SOCKET_NOT_CONNECTED;
    }
            
    SYS_CONSOLE_MESSAGE("Connection Opened: Starting SSL Negotiation\r\n");
    if (!NET_PRES_SocketEncryptSocket(socket)) {
        SYS_CONSOLE_MESSAGE("SSL Create Connection Failed - Aborting the process!\r\n");
        return SSL_CREATE_CONNECTION_FAILED;
    } else {
        return SSL_NEGOTIATION_SUCCES;
    }    
}



/*
 * Parse the DNS for the server
 */
bool waitOnDNS ( void ) {
    TCPIP_DNS_RESULT result = TCPIP_DNS_IsResolved(HOST, &address, IP_ADDRESS_TYPE_IPV4);
    if (result != TCPIP_DNS_RES_OK) {
        return false;
    }
    
    //SYS_CONSOLE_PRINT("DNS Resolved IPv4 Address: %d.%d.%d.%d for host '%s'\r\n",
    //    address.v4Add.v[0], address.v4Add.v[1], address.v4Add.v[2], address.v4Add.v[3], HOST);
    return true;
}



/*
 * Wait for the SSL negotiation process to be completed.
 * If it fails we abort the process. 
 */
SSL_NEGOTIATION_STATES waitForSslConnection ( void ) {
    if (NET_PRES_SocketIsNegotiatingEncryption(socket)) {
        return SSL_BUSY_NEGOTIATING;
    }
            
    if (!NET_PRES_SocketIsSecure(socket)) {
        SYS_CONSOLE_MESSAGE("SSL Connection Negotiation Failed - Aborting\r\n");
        return SSL_SOCKET_NOT_SECURE;
    }

    SYS_CONSOLE_MESSAGE("SSL Connection Opened: Starting Clear Text Communication\r\n");
    return SSL_NEGOTIATION_SUCCES;
}



/*
 * Wait for the socket to be available for requests.
 */
SSL_SOCKET_STATES socketReady( void ) {
    if (NET_PRES_SocketReadIsReady(socket) == 0) {
        if (NET_PRES_SocketWasReset(socket) || NET_PRES_SocketWasDisconnected(socket)) {
            return SSL_SOCKET_WAS_DISCONNECTED;
        }
        return SSL_SOCKET_NOT_READY;
    }    
    return SSL_SOCKET_READY;
}



/*
 * Close an open socket
 */
void closeSocket ( void ) {
    NET_PRES_SocketClose(socket);
}