#ifndef LIGHTMODBUS_SLAVE_H
#define LIGHTMODBUS_SLAVE_H
 
#include <stdint.h>
#include <stddef.h>
#include "base.h"
 
typedef struct ModbusSlave ModbusSlave;
 
typedef ModbusErrorInfo (*ModbusRequestParsingFunction)(
    ModbusSlave *status,
    uint8_t function,
    const uint8_t *requestPDU,
    uint8_t requestLength);
 
typedef struct ModbusSlaveFunctionHandler
{
    uint8_t id;
    ModbusRequestParsingFunction ptr;
} ModbusSlaveFunctionHandler;
 
typedef enum ModbusRegisterQuery
{
    MODBUS_REGQ_R_CHECK, 
    MODBUS_REGQ_W_CHECK, 
    MODBUS_REGQ_R,       
    MODBUS_REGQ_W        
} ModbusRegisterQuery;
 
typedef struct ModbusRegisterCallbackArgs
{
    ModbusDataType type;        
    ModbusRegisterQuery query;  
    uint16_t index;             
    uint16_t value;             
    uint8_t function;           
} ModbusRegisterCallbackArgs;
 
typedef struct ModbusRegisterCallbackResult
{
    ModbusExceptionCode exceptionCode; 
    uint16_t value;                    
} ModbusRegisterCallbackResult;
 
typedef ModbusError (*ModbusRegisterCallback)(
    const ModbusSlave *status,
    const ModbusRegisterCallbackArgs *args,
    ModbusRegisterCallbackResult *out);
 
typedef ModbusError (*ModbusSlaveExceptionCallback)(
    const ModbusSlave *status,
    uint8_t function,
    ModbusExceptionCode code);
 
struct ModbusSlave
{
    ModbusRegisterCallback registerCallback;        
    ModbusSlaveExceptionCallback exceptionCallback; 
    const ModbusSlaveFunctionHandler *functions;    
    uint8_t functionCount;                          
    
    ModbusBuffer response;
 
    void *context; 
};
 
 
LIGHTMODBUS_RET_ERROR modbusSlaveInit(
    ModbusSlave *status,
    ModbusRegisterCallback registerCallback,
    ModbusSlaveExceptionCallback exceptionCallback,
    ModbusAllocator allocator,
    const ModbusSlaveFunctionHandler *functions,
    uint8_t functionCount);
 
void modbusSlaveDestroy(ModbusSlave *status);
 
LIGHTMODBUS_RET_ERROR modbusBuildException(
    ModbusSlave *status,
    uint8_t function,
    ModbusExceptionCode code);
 
LIGHTMODBUS_RET_ERROR modbusBuildExceptionPDU(
    ModbusSlave *status,
    uint8_t function,
    ModbusExceptionCode code);
 
LIGHTMODBUS_RET_ERROR modbusBuildExceptionRTU(
    ModbusSlave *status,
    uint8_t address,
    uint8_t function,
    ModbusExceptionCode code);
 
LIGHTMODBUS_RET_ERROR modbusBuildExceptionTCP(
    ModbusSlave *status,
    uint16_t transactionID,
    uint8_t unitID,
    uint8_t function,
    ModbusExceptionCode code);
 
LIGHTMODBUS_RET_ERROR modbusParseRequest(ModbusSlave *status, const uint8_t *request, uint8_t requestLength);
LIGHTMODBUS_RET_ERROR modbusParseRequestPDU(ModbusSlave *status, const uint8_t *request, uint8_t requestLength);
LIGHTMODBUS_RET_ERROR modbusParseRequestRTU(ModbusSlave *status, uint8_t slaveAddress, const uint8_t *request, uint16_t requestLength);
LIGHTMODBUS_RET_ERROR modbusParseRequestTCP(ModbusSlave *status, const uint8_t *request, uint16_t requestLength);
 
LIGHTMODBUS_WARN_UNUSED static inline const uint8_t *modbusSlaveGetResponse(const ModbusSlave *status)
{
    return status->response.data;
}
 
LIGHTMODBUS_WARN_UNUSED static inline uint16_t modbusSlaveGetResponseLength(const ModbusSlave *status)
{
    return status->response.length;
}
 
static inline void modbusSlaveSetUserPointer(ModbusSlave *status, void *ptr)
{
    status->context = ptr;
}
 
static inline void *modbusSlaveGetUserPointer(const ModbusSlave *status)
{
    return status->context;
}
 
LIGHTMODBUS_WARN_UNUSED static inline ModbusError modbusSlaveAllocateResponse(ModbusSlave *status, uint16_t pduSize)
{
    return modbusBufferAllocateADU(&status->response, pduSize, modbusSlaveGetUserPointer(status));
}
 
static inline void modbusSlaveFreeResponse(ModbusSlave *status)
{
    modbusBufferFree(&status->response, modbusSlaveGetUserPointer(status));
}
 
extern ModbusSlaveFunctionHandler modbusSlaveDefaultFunctions[];
extern const uint8_t modbusSlaveDefaultFunctionCount;
 
#endif