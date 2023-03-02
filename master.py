from pymodbus.client import ModbusSerialClient
from pymodbus.transaction import ModbusRtuFramer
import time
import logging
import serial
FORMAT = ('%(message)-15s')
logging.basicConfig(format=FORMAT)
log = logging.getLogger()
log.setLevel(logging.DEBUG)

master = ModbusSerialClient(framer=ModbusRtuFramer, port = '/dev/ttyACM0', stopbits=1, bytesize=8, parity='N', baudrate=115200)

master.connect()
print(master)
print("\033[92m-------------------client connected----------------------\033[0m")

receive = master.read_holding_registers(address=5, count=2, slave=0x01)
print(receive.__dict__)
time.sleep(1)
 