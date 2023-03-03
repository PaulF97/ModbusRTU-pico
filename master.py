from pymodbus.client import ModbusSerialClient
from pymodbus.transaction import ModbusRtuFramer
import time
import threading
import logging
import serial


FORMAT = ('%(message)-15s')
logging.basicConfig(format=FORMAT)
log = logging.getLogger()
log.setLevel(logging.DEBUG)

master = ModbusSerialClient(framer=ModbusRtuFramer, port = '/dev/ttyACM0', stopbits=1, bytesize=8, parity='N', baudrate=115200)
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)



connexion = master.connect()
print(master)
print("\033[92m-------------------client connected----------------------\033[0m")

print("test serial connexion")

#  ------------------ VARIOUS TESTS ------------------

#  ------------------ excecution time of write and read function ------------------
startWrite = time.time()
receive = master.write_registers(5, "bonjour".encode(), 0x01)
endWrite = time.time()
print(endWrite - startWrite)

startRead = time.time()
receive = master.read_holding_registers(address=5, count=2, slave=0x01)
endRead = time.time()
print(endRead - startRead)
print(receive.__dict__)


#  ------------------ serial ------------------
# ser.write(b'bonjour')
# ser.close()
# receive = master.read_holding_registers(address=5, count=2, slave=0x01)


