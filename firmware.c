/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>

#include <ctype.h> 
#include "pico/stdlib.h"


#define UART_ID uart1
#define BAUDRATE 9600
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#define MAX_LENGTH 23

// prototype functions
void init(const uint led_used);
void decodeFrame(char reception[], int size, int sizeOfData);
void hexToASCII (char usefulData[]);
int main() {

    const uint LED_PIN = 25;
    char *ptr;
    char frameReceived[MAX_LENGTH];
    char betterArray[MAX_LENGTH];
    char single;
    int i_get, j =0, iteration = 0;
    stdio_init_all();
    uart_init(UART_ID, BAUDRATE);
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    printf("begining of program..");


    while(true){
        

       
        // while ((single = getchar()) != '\n' && i_get < MAX_LENGTH) {
        //     frameReceived[i_get] = single;
        //     i_get++;
        //     gpio_put(LED_PIN,1);
        //     uart_puts(UART_ID, "LED on\n");
        // }

       fgets(frameReceived, MAX_LENGTH, stdin);
        // if(single == '\n' || single == '\0'){
        //     i_get=0;
        // }

        // frameReceived[i_get] = '\0';
       // printf(" test %s", frameReceived);

        for(int i = 0; i<MAX_LENGTH; i++){
            if(frameReceived[i] != '\0' || frameReceived[i] != '\n'){
                printf(" %02X", frameReceived[i]); 
            }
        }
        printf("\n");
       // sleep_ms(1000);
       // decodeFrame(frameReceived, MAX_LENGTH, frameReceived[6]);

        gpio_put(LED_PIN,0);
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