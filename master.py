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
# startWrite = time.perf_counter()
# receive = master.write_registers(5, "bonjour".encode(), 0x01)
# endWrite = time.perf_counter()
# print("time write process", endWrite - startWrite)
# print("write transaction per seconds", 1/float((endWrite - startWrite)))
# startRead = time.perf_counter()
# receive = master.read_holding_registers(address=5, count=2, slave=0x01)
# endRead = time.perf_counter()
# print("time read process", endRead - startRead)
# print("read transaction per seconds", 1/float((endRead - startRead)))
# print(receive.__dict__)


#  ------------------ SERIAL ------------------
# ser.write(b'bonjour')
# ser.close()
# receive = master.read_holding_registers(address=5, count=2, slave=0x01)


# ------------------ SEND ZERO DATA BEFORE NORMAL FRAMES ------------------
# send = master.write_registers(5,None, 0x01)
# time.sleep(1)
# while True:
#     receive = master.write_registers(5, "bonjour".encode(), 0x01)
#     time.sleep(1)


# ------------------ SEND TO ILLEGAL ADDRESS BEFORE NORMAL FRAMES ------------------
send = master.write_registers(5000,"test illegal address".encode(), 0x01)
while True:
    receive = master.write_registers(5, "bonjour".encode(), 0x01)
    time.sleep(1)