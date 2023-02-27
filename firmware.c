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



uint8_t coils[REG_COUNT / 8];
uint16_t regs[REG_COUNT];

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

    const uint LED_PIN = 25;
    char *ptr;
    char receiveBuffer[MAX_LENGTH_W];
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

    printf("begining of program..");



    error = modbusSlaveInit(&slave, registerCallback, exceptionCallback, modbusDefaultAllocator, modbusSlaveDefaultFunctions, modbusSlaveDefaultFunctionCount);
    assert(modbusIsOk(error) && "modbusSlaveInit() failed!");


    while(1){

        //memset(receiveBuffer, i_get, sizeof(receiveBuffer));
        i_get=0;
        // while ((single = getchar()) != '\n' && i_get<MAX_LENGTH_W) {
        //     receiveBuffer[i_get] = single;
        //     i_get++;
        //     gpio_put(LED_PIN,1);
        //     uart_puts(UART_ID, "LED on\n");
        // }

        // uart_puts(UART_ID, "outside while\n");
        // receiveBuffer[i_get] = '\0';

        fgets(receiveBuffer, MAX_LENGTH_W, stdin);
       // sleep_ms(1000);
        uart_puts(UART_ID, "RTU frame before treatment by library ..");
        for(int i = 0; i<MAX_LENGTH_W; i++){
            uart_puts(UART_ID, "printing the frame\n");
           // if((receiveBuffer[i_get] != '\0' || receiveBuffer[i_get] != '\n')){
                printf(" %02X", receiveBuffer[i]);
         //   }
        }

        printf("\n"); 
        gpio_put(LED_PIN,0);

        error = modbusParseRequestRTU(&slave, 0x01, receiveBuffer, MAX_LENGTH_W);
        printErrorInfo(error);
        uart_puts(UART_ID, "RTU response from lib: ");
        printAndSendFrameResponse(error, &slave);
        printf("\n");
    }
      return 0;
}



void init(const uint led_used){
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
	for (int i = 0; i < modbusSlaveGetResponseLength(slave); i++){
        printf("0x%02x ", modbusSlaveGetResponse(slave)[i]);
        // data[i] = modbusSlaveGetResponse(slave)[i];
    }
    // length = modbusSlaveGetResponseLength(slave); // get the length of frame
    // printf(data, "%d", length);

}

/*
* goes in this callback when a frame is received
*/
ModbusError registerCallback(const ModbusSlave *slaveID,const ModbusRegisterCallbackArgs *args,ModbusRegisterCallbackResult *result){
	printf(
		"Register query:\n"
		"\tquery: %s\n"
		"\t type: %s\n"
		"\t   id: %d\n"
		"\tvalue: %c\n"
		"\t  fun: %d\n",
		modbusRegisterQueryStr(args->query),
		modbusDataTypeStr(args->type),
		args->index,
		args->value,
		args->function
	);

    // switch (args->query){

	// 	// Pretend to allow all access
	// 	case MODBUS_REGQ_R_CHECK:
	// 	case MODBUS_REGQ_W_CHECK:
	// 		result->exceptionCode = MODBUS_EXCEP_NONE;
	// 		break;

	// 	// Return 7 when reading
	// 	case MODBUS_REGQ_R:
	// 		result->value = 7;
	// 		break;
		
	// 	default: break;
	// }

    uart_puts(UART_ID, "inside callback");

	switch (args->query){
    uart_puts(UART_ID, "inside switch");

		case MODBUS_REGQ_R_CHECK:
        uart_puts(UART_ID, "MODBUS_REGQ_R_CHECK");
			if (args->index < REG_COUNT)
				result->exceptionCode = MODBUS_EXCEP_NONE;
			else	
				result->exceptionCode = MODBUS_EXCEP_ILLEGAL_ADDRESS;
			break;
		case MODBUS_REGQ_W_CHECK:
            uart_puts(UART_ID, "MODBUS_REGQ_W_CHECK");
			if (args->index < REG_COUNT - 2)
				result->exceptionCode = MODBUS_EXCEP_NONE;
			else	
				result->exceptionCode = MODBUS_EXCEP_SLAVE_FAILURE;
			break;

		case MODBUS_REGQ_R:
        uart_puts(UART_ID, "MODBUS_REGQ_REGQ_R");
           // result->value=7;
			switch (args->type){
				case MODBUS_HOLDING_REGISTER: result->value = regs[args->index]; break;
				case MODBUS_INPUT_REGISTER: result->value = regs[args->index]; break;
				case MODBUS_COIL: result->value = modbusMaskRead(coils, args->index); break;
				case MODBUS_DISCRETE_INPUT: result->value = modbusMaskRead(coils, args->index); break;
			}
			break;


		case MODBUS_REGQ_W:
            uart_puts(UART_ID, "MODBUS_REGQ_REGQ_W");
			switch (args->type)
			{
				case MODBUS_HOLDING_REGISTER: regs[args->index] = args->value; break;
				case MODBUS_COIL: modbusMaskWrite(coils, args->index, args->value); break;
				default: abort(); break;
			}
			break;
		default: break;
	}
    
    return MODBUS_OK;

}
	
ModbusError exceptionCallback(const ModbusSlave *slave,  uint8_t function, ModbusExceptionCode code){
    uart_puts(UART_ID, "in error register callback");
	printf("Slave exception %s (function %d)\n", modbusExceptionCodeStr(code), function);
	return MODBUS_OK;
}


/*
* check if modbus is initialized
*/
void printErrorInfo(ModbusErrorInfo err)
{
	if (modbusIsOk(err)){
		uart_puts(UART_ID, "FRAME IS CORRECT\n");
  
    }else{
        uart_puts(UART_ID, "THERE IS A PROBLEM WITH THE FRAME\n");
		printf("%s: it comes from the following element : %s",
			modbusErrorSourceStr(modbusGetErrorSource(err)),
			modbusErrorStr(modbusGetErrorCode(err)));
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
        printf(" %02X", data[j]);
        j++;
    }

   // hexToASCII(data);

    printf("\n");
    printf("frame format : \n");
    printf (" slave address -- function code -- starting address -- quantity -- byte count -- data -- CRC \n");
    printf ("The value of slave ID is %02X \n the value of function code is %02X \n the value of starting address is %02X \n the value of quantity is %02X \n  the value of byte count is %02X \n the CRC bytes are %02X and %02X \n", slaveID, functionCode, addressStart, sizeOfData, byteCount, checksum[0], checksum[1]);
    sleep_ms(2000);
}


void hexToASCII(char usefulData[]){
    char dataInASCII[14];

    for(int i =0; i< strlen(usefulData); i+=2){
        sscanf(&usefulData[i], "%2x", &dataInASCII[i/2]);
    }

    printf("data in interpretable String : ");
    for(int i = 0; i< sizeof(dataInASCII); i++){
        printf("%s", dataInASCII[i]);
    }
    printf("\n");


}