#ifndef LIGHTMODBUS_BASE_H
#define LIGHTMODBUS_BASE_H
 
#include <stdint.h>
 
#ifndef LIGHTMODBUS_WARN_UNUSED
#define LIGHTMODBUS_WARN_UNUSED __attribute__((warn_unused_result))
#endif
 
#ifndef LIGHTMODBUS_ALWAYS_INLINE
#define LIGHTMODBUS_ALWAYS_INLINE __attribute__((always_inline))
#endif
 
#define MODBUS_PDU_MIN 1   
#define MODBUS_PDU_MAX 253 
 
#define MODBUS_RTU_ADU_MIN     4   
#define MODBUS_RTU_ADU_MAX     256 
#define MODBUS_RTU_ADU_PADDING 3   
#define MODBUS_RTU_PDU_OFFSET  1   
 
#define MODBUS_TCP_ADU_MIN     8   
#define MODBUS_TCP_ADU_MAX     260 
#define MODBUS_TCP_ADU_PADDING 7   
#define MODBUS_TCP_PDU_OFFSET  7   
 
#define LIGHTMODBUS_RET_ERROR LIGHTMODBUS_WARN_UNUSED ModbusErrorInfo
 
#define MODBUS_ERROR_SOURCE_GENERAL  0u
 
#define MODBUS_ERROR_SOURCE_REQUEST  1u
 
#define MODBUS_ERROR_SOURCE_RESPONSE 2u
 
#define MODBUS_ERROR_SOURCE_RESERVED 3u
 
#define MODBUS_MAKE_ERROR(s, e) ((ModbusErrorInfo){.source = (s), .error = (e)})
 
#define MODBUS_NO_ERROR() MODBUS_MAKE_ERROR(MODBUS_ERROR_SOURCE_GENERAL, MODBUS_OK)
 
#define MODBUS_GENERAL_ERROR(e) MODBUS_MAKE_ERROR(MODBUS_ERROR_SOURCE_GENERAL, (MODBUS_ERROR_##e))
 
#define MODBUS_REQUEST_ERROR(e) MODBUS_MAKE_ERROR(MODBUS_ERROR_SOURCE_REQUEST, (MODBUS_ERROR_##e))
 
#define MODBUS_RESPONSE_ERROR(e) MODBUS_MAKE_ERROR(MODBUS_ERROR_SOURCE_RESPONSE, (MODBUS_ERROR_##e))
 
typedef struct ModbusErrorInfo
{
    unsigned int source : 2; 
    unsigned int error  : 6; 
} ModbusErrorInfo;
 
typedef enum ModbusError
{
    MODBUS_OK = 0,
 
    MODBUS_ERROR_OK = 0,
    
    MODBUS_ERROR_LENGTH,
    
    MODBUS_ERROR_ALLOC,
 
    MODBUS_ERROR_FUNCTION,
 
    MODBUS_ERROR_COUNT,
 
    MODBUS_ERROR_INDEX,
 
    MODBUS_ERROR_VALUE,
 
    MODBUS_ERROR_RANGE,
 
    MODBUS_ERROR_CRC,
 
    MODBUS_ERROR_BAD_PROTOCOL,
 
    MODBUS_ERROR_BAD_TRANSACTION,
 
    MODBUS_ERROR_ADDRESS,
 
    MODBUS_ERROR_OTHER,
} ModbusError;
 
typedef enum ModbusExceptionCode
{
    MODBUS_EXCEP_NONE = 0,
    MODBUS_EXCEP_ILLEGAL_FUNCTION = 1, 
    MODBUS_EXCEP_ILLEGAL_ADDRESS = 2,  
    MODBUS_EXCEP_ILLEGAL_VALUE = 3,    
    MODBUS_EXCEP_SLAVE_FAILURE = 4,    
    MODBUS_EXCEP_ACK = 5,              
    MODBUS_EXCEP_NACK = 7              
} ModbusExceptionCode;
 
typedef enum ModbusDataType
{
    MODBUS_HOLDING_REGISTER = 1, 
    MODBUS_INPUT_REGISTER = 2,   
    MODBUS_COIL = 4,             
    MODBUS_DISCRETE_INPUT = 8    
} ModbusDataType;
 
// Forward declaration for ModbusBuffer
struct ModbusBuffer;
 
typedef ModbusError (*ModbusAllocator)(
    struct ModbusBuffer *buffer,
    uint16_t size,
    void *context);
 
typedef struct ModbusBuffer
{
    ModbusAllocator allocator;
 
    uint8_t *data;      
    uint8_t *pdu;       
    uint16_t length;    
 
    uint8_t padding;    
    uint8_t pduOffset;  
} ModbusBuffer;
 
LIGHTMODBUS_WARN_UNUSED ModbusError modbusDefaultAllocator(
    ModbusBuffer *buffer,
    uint16_t size,
    void *context);
 
LIGHTMODBUS_RET_ERROR modbusBufferInit(ModbusBuffer *buffer, ModbusAllocator allocator);
void modbusBufferDestroy(ModbusBuffer *buffer, void *context);
LIGHTMODBUS_WARN_UNUSED ModbusError modbusBufferAllocateADU(ModbusBuffer *buffer, uint16_t pduSize, void *context);
void modbusBufferFree(ModbusBuffer *buffer, void *context);
 
uint16_t modbusCRC(const uint8_t *data, uint16_t length);
 
static inline void modbusBufferModePDU(ModbusBuffer *buffer)
{
    buffer->padding = 0;
    buffer->pduOffset = 0;
}
 
static inline void modbusBufferModeRTU(ModbusBuffer *buffer)
{
    buffer->padding = MODBUS_RTU_ADU_PADDING;
    buffer->pduOffset = MODBUS_RTU_PDU_OFFSET;
}
 
static inline void modbusBufferModeTCP(ModbusBuffer *buffer)
{
    buffer->padding = MODBUS_TCP_ADU_PADDING;
    buffer->pduOffset = MODBUS_TCP_PDU_OFFSET;
}
 
LIGHTMODBUS_WARN_UNUSED static inline uint8_t modbusMaskRead(const uint8_t *mask, uint16_t n)
{
    return (mask[n >> 3] & (1 << (n & 7))) != 0;
}
 
static inline void modbusMaskWrite(uint8_t *mask, uint16_t n, uint8_t value)
{
    if (value)
        mask[n >> 3] |= (1 << (n & 7));
    else
        mask[n >> 3] &= ~(1 << (n & 7));
}
 
LIGHTMODBUS_WARN_UNUSED static inline uint16_t modbusBitsToBytes(uint16_t n)
{
    return (n + 7) >> 3;
}
 
LIGHTMODBUS_WARN_UNUSED static inline uint16_t modbusRLE(const uint8_t *p)
{
    uint8_t lo = *p;
    uint8_t hi = *(p + 1);
    return (uint16_t) lo | ((uint16_t) hi << 8);
}
 
static inline uint16_t modbusWLE(uint8_t *p, uint16_t val)
{
    *p = val & 0xff;
    *(p + 1) = val >> 8;
    return val;
}
 
LIGHTMODBUS_WARN_UNUSED static inline uint16_t modbusRBE(const uint8_t *p)
{
    uint8_t lo = *(p + 1);
    uint8_t hi = *p;
    return (uint16_t) lo | ((uint16_t) hi << 8);
}
 
static inline uint16_t modbusWBE(uint8_t *p, uint16_t val)
{
    *p = val >> 8;
    *(p + 1) = val & 0xff;
    return val;
}
 
static inline uint8_t modbusCheckRangeU16(uint16_t index, uint16_t count)
{
    // return (uint16_t)(index + count - 1) < index;
    return index > UINT16_MAX - count + 1;
}
 
LIGHTMODBUS_WARN_UNUSED static inline uint8_t modbusGetErrorSource(ModbusErrorInfo err)
{
    return err.source;
}
 
LIGHTMODBUS_WARN_UNUSED static inline ModbusError modbusGetErrorCode(ModbusErrorInfo err)
{
    return (ModbusError) err.error;
}
 
LIGHTMODBUS_WARN_UNUSED static inline uint8_t modbusIsOk(ModbusErrorInfo err)
{
    return modbusGetErrorSource(err) == MODBUS_ERROR_SOURCE_GENERAL && modbusGetErrorCode(err) == MODBUS_OK;
}
 
LIGHTMODBUS_WARN_UNUSED static inline ModbusError modbusGetGeneralError(ModbusErrorInfo err)
{
    return err.source == MODBUS_ERROR_SOURCE_GENERAL ? modbusGetErrorCode(err) : MODBUS_OK;
}
 
LIGHTMODBUS_WARN_UNUSED static inline ModbusError modbusGetRequestError(ModbusErrorInfo err)
{
    return err.source == MODBUS_ERROR_SOURCE_REQUEST ? modbusGetErrorCode(err) : MODBUS_OK;
}
 
LIGHTMODBUS_WARN_UNUSED static inline ModbusError modbusGetResponseError(ModbusErrorInfo err)
{
    return err.source == MODBUS_ERROR_SOURCE_RESPONSE ? modbusGetErrorCode(err) : MODBUS_OK;
}
 
LIGHTMODBUS_WARN_UNUSED LIGHTMODBUS_ALWAYS_INLINE static inline ModbusError modbusUnpackRTU(
    const uint8_t *frame,
    uint16_t length,
    uint8_t checkCRC,
    const uint8_t **pdu,
    uint16_t *pduLength,
    uint8_t *address)
{
    // Check length
    if (length < MODBUS_RTU_ADU_MIN || length > MODBUS_RTU_ADU_MAX)
        return MODBUS_ERROR_LENGTH;
 
    // Extract address
    *address = frame[0];
 
    // Check CRC
    if (checkCRC && modbusCRC(frame, length - 2) != modbusRLE(frame + length - 2))
        return MODBUS_ERROR_CRC;
 
    *pdu = frame + MODBUS_RTU_PDU_OFFSET;
    *pduLength = length - MODBUS_RTU_ADU_PADDING;
 
    return MODBUS_OK;
}
 
LIGHTMODBUS_WARN_UNUSED LIGHTMODBUS_ALWAYS_INLINE static inline ModbusError modbusPackRTU(
    uint8_t *frame,
    uint16_t length,
    uint8_t address)
{
    // Check length
    if (length < MODBUS_RTU_ADU_MIN || length > MODBUS_RTU_ADU_MAX)
        return MODBUS_ERROR_LENGTH;
 
    // Write address
    frame[0] = address;
 
    // Compute and write CRC
    modbusWLE(&frame[length - 2], modbusCRC(frame, length - 2));
    
    return MODBUS_OK;
}
 
LIGHTMODBUS_WARN_UNUSED LIGHTMODBUS_ALWAYS_INLINE static inline ModbusError modbusUnpackTCP(
    const uint8_t *frame,
    uint16_t length,
    const uint8_t **pdu,
    uint16_t *pduLength,
    uint16_t *transactionID,
    uint8_t *unitID)
{
    // Check length
    if (length < MODBUS_TCP_ADU_MIN || length > MODBUS_TCP_ADU_MAX)
        return MODBUS_ERROR_LENGTH;
 
    // Discard non-Modbus messages
    uint16_t protocolID = modbusRBE(&frame[2]);
    if (protocolID != 0)
        return MODBUS_ERROR_BAD_PROTOCOL;
 
    // Length mismatch
    uint16_t messageLength = modbusRBE(&frame[4]);
    if (messageLength != length - 6)
        return MODBUS_ERROR_LENGTH;
 
    *transactionID = modbusRBE(&frame[0]);
    *unitID = frame[6];
    *pdu = frame + MODBUS_TCP_PDU_OFFSET;
    *pduLength = length - MODBUS_TCP_ADU_PADDING;
 
    return MODBUS_OK;
}
 
LIGHTMODBUS_WARN_UNUSED LIGHTMODBUS_ALWAYS_INLINE static inline ModbusError modbusPackTCP(
    uint8_t *frame,
    uint16_t length,
    uint16_t transactionID,
    uint8_t unitID)
{
    // Check length
    if (length < MODBUS_TCP_ADU_MIN || length > MODBUS_TCP_ADU_MAX)
        return MODBUS_ERROR_LENGTH;
 
    modbusWBE(&frame[0], transactionID);
    modbusWBE(&frame[2], 0);
    modbusWBE(&frame[4], length - 6);
    frame[6] = unitID;
    return MODBUS_OK;
}
 
#endif