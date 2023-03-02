from pymodbus.client import ModbusSerialClient
from pymodbus.transaction import ModbusRtuFramer
import time
import logging

FORMAT = ('%(message)-15s')
logging.basicConfig(format=FORMAT)
log = logging.getLogger()
log.setLevel(logging.DEBUG)

master = ModbusSerialClient(framer=ModbusRtuFramer, port = '/dev/ttyACM0', stopbits=1, bytesize=8, parity='N', baudrate=115200)

connexion = master.connect()
print(master)
print("\033[92m-------------------client connected----------------------\033[0m")



while True:
    print("before write")
    sending = master.write_registers(5, "bonjour".encode(), 0x01)
    sending = master.write_registers(20, "paul".encode(), 0x01)
    receive = master.read_holding_registers(address=5, count=2, slave=0x01)
    receive2 = master.read_holding_registers(address=10, count=7, slave=0x01)
    print(receive.__dict__)
