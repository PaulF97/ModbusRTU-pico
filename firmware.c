/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


// LIBRARY
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "tusb_config.h"



// MACROS
#define LIGHTMODBUS_SLAVE_FULL
#define LIGHTMODBUS_DEBUG
#define LIGHTMODBUS_IMPL


// LIGTHMODBUS LIB
#include "liblightmodbus/include/lightmodbus/lightmodbus.h"


#ifndef REG_COUNT
#define REG_COUNT 16
#endif


// CONSTANTS
#define UART_ID uart1
#define BAUDRATE 9600
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#define MAX_LENGTH_W 9

#define debug(...) uart_puts(UART_ID, __VA_ARGS__);

// uint8_t coils[REG_COUNT / 8];
// uint16_t regs[REG_COUNT];


static uint16_t registers[REG_COUNT];
static uint16_t inputRegisters[REG_COUNT];
static uint8_t coils[REG_COUNT / 8];
static uint8_t discreteInputs[REG_COUNT / 8];



// prototype functions
void init(const uint led_used);
void decodeFrame(char reception[], int size, int sizeOfData);
void hexToASCII (char usefulData[]);
void printErrorInfo(ModbusErrorInfo err);
void printAndSendFrameResponse(ModbusErrorInfo err, const ModbusSlave *slave);

// callback prototypes
ModbusError registerCallback(const ModbusSlave *slaveID,const ModbusRegisterCallbackArgs *args,ModbusRegisterCallbackResult *result);
ModbusError exceptionCallback(const ModbusSlave *slave,  uint8_t function, ModbusExceptionCode code);

int main() {

    registers[0] = 0xDEAD;
    registers[1] = 0xBEEF;

    const uint LED_PIN = 25;
    char receiveBuffer[255];
    char betterArray[MAX_LENGTH_W];
    char single;
    int i_get = 0;
    ModbusSlave slave;
    ModbusErrorInfo error;

    stdio_init_all();
    uart_init(UART_ID, BAUDRATE);
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    debug("begining of program..\r\n");

   

    error = modbusSlaveInit(&slave, registerCallback, exceptionCallback, modbusDefaultAllocator, modbusSlaveDefaultFunctions, modbusSlaveDefaultFunctionCount);
    if(!modbusIsOk(error))
        debug("modbusSlaveInit() failed!");

    //uint8_t *ptr = receiveBuffer;

    fflush(stdin);

    while(1){
        
        i_get=0;
        fgets(receiveBuffer, MAX_LENGTH_W, stdin);
        char str[50];

        debug("Reading hex data from stdin...\n")

        sprintf(str, "data=%2X\n\r", receiveBuffer);
        debug(str);
        //*ptr++ = fgetc(stdin);
       // sleep_ms(1000);

        error = modbusParseRequestRTU(&slave, 0x01, receiveBuffer, i_get);
        printErrorInfo(error);
        if(modbusIsOk(error))
        {
            printAndSendFrameResponse(error, &slave);
        }
    }
      return 0;
}



void init(const uint led_used){
    //tusb_init();
    stdio_init_all();
    uart_init(UART_ID, BAUDRATE);
    gpio_init(led_used);
    gpio_set_dir(led_used, GPIO_OUT);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

}


void printAndSendFrameResponse(ModbusErrorInfo err , const ModbusSlave *slave){
    char *data;
    int length;
	//for (int i = 0; i < modbusSlaveGetResponseLength(slave); i++){
        //printf("%02x", modbusSlaveGetResponse(slave)[i]);
        // data[i] = modbusSlaveGetResponse(slave)[i];
    //}
    data = modbusSlaveGetResponse(slave);
    length = modbusSlaveGetResponseLength(slave);
    char str[50];
    sprintf(str, "data=%.6X\n\rlength=%d\n\r", data, length);
    debug(str);
    // length = modbusSlaveGetResponseLength(slave); // get the length of frame
    // printf(data, "%d", length);
}

/*
* goes in this callback when a frame is received
*/
ModbusError registerCallback(const ModbusSlave *slaveID,const ModbusRegisterCallbackArgs *args,ModbusRegisterCallbackResult *result){
	//printf(
		// "Register query:\r\n"
		// "\tquery: %s\r\n"
		// "\t type: %s\r\n"
		// "\t   id: %d\r\n"
		// "\tvalue: %c\r\n"
		// "\t  fun: %d\r\n",
	//	modbusRegisterQueryStr(args->query),
	//	modbusDataTypeStr(args->type),    // Always return MODBUS_OK
	//	args->index,
	//	args->value,
	//	args->function
	//);


    debug("inside callback");
    switch (args->query)
    {
        // R/W access check
        case MODBUS_REGQ_R_CHECK:
        case MODBUS_REGQ_W_CHECK:
            result->exceptionCode = args->index < REG_COUNT ? MODBUS_EXCEP_NONE : MODBUS_EXCEP_ILLEGAL_ADDRESS;
            break;
 
        // Read register        
        case MODBUS_REGQ_R:
            switch (args->type)
            {
                case MODBUS_HOLDING_REGISTER: result->value = registers[args->index]; break;
                case MODBUS_INPUT_REGISTER:   result->value = inputRegisters[args->index]; break;
                case MODBUS_COIL:             result->value = modbusMaskRead(coils, args->index); break;
                case MODBUS_DISCRETE_INPUT:   result->value = modbusMaskRead(discreteInputs, args->index); break;
            }
            break;
 
        // Write register
        case MODBUS_REGQ_W:
            switch (args->type)
            {
                case MODBUS_HOLDING_REGISTER: registers[args->index] = args->value; break;
                case MODBUS_COIL:             modbusMaskWrite(coils, args->index, args->value); break;
                default:                      break;
            }
            break;
    }
 
    return MODBUS_OK;

}
	
ModbusError exceptionCallback(const ModbusSlave *slave,  uint8_t function, ModbusExceptionCode code){
    debug("in error register callback\r\n");
	//printf("Slave exception %s (function %d)\r\n", modbusExceptionCodeStr(code), function);
	return MODBUS_OK;
}


/*
* check if modbus is initialized
*/
void printErrorInfo(ModbusErrorInfo err)
{
	if (modbusIsOk(err)){
		debug("FRAME IS CORRECT\r\n");
  
    }else{
        debug("THERE IS A PROBLEM WITH THE FRAME\r\n");
		//printf("%s: it comes from the following element : %s",
		debug(modbusErrorSourceStr(modbusGetErrorSource(err)));
		debug(modbusErrorStr(modbusGetErrorCode(err)));
    }
}

/*
* https://rapidscada.net/modbus/
*/
void decodeFrame(char reception[], int size, int sizeOfDataFrame){

    char slaveID;
    char functionCode;
    char addressStart;
    char sizeOfData;
    char byteCount;
    char data[sizeOfDataFrame];
    char checksum[2];
    int j = 0;
    
    slaveID = reception[0];
    functionCode = reception[1];
    addressStart = reception[3];
    sizeOfData = reception[5];
    byteCount = reception[6];
    checksum[0]= reception[size - 2];
    checksum[1] = reception[size - 1];

    for(int i = 7; i < byteCount + 7; i++){
        data[j] = reception[i];
        //printf(" %02X", data[j]);
        j++;
    }

   // hexToASCII(data);

    //printf("\r\n");
    //printf("frame format : \r\n");
    //printf (" slave address -- function code -- starting address -- quantity -- byte count -- data -- CRC \r\n");
    //printf ("The value of slave ID is %02X \r\n the value of function code is %02X \r\n the value of starting address is %02X \r\n the value of quantity is %02X \r\n  the value of byte count is %02X \r\n the CRC bytes are %02X and %02X \r\n", slaveID, functionCode, addressStart, sizeOfData, byteCount, checksum[0], checksum[1]);
    sleep_ms(2000);
}


void hexToASCII(char usefulData[]){
    char dataInASCII[14];

    for(int i =0; i< strlen(usefulData); i+=2){
        sscanf(&usefulData[i], "%2x", &dataInASCII[i/2]);
    }

    //printf("data in interpretable String : ");
    for(int i = 0; i< sizeof(dataInASCII); i++){
        //printf("%s", dataInASCII[i]);
    }
    //printf("\r\n");


}