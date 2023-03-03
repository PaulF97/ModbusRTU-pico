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
    sending = master.write_coil(1, 1, 0x01)
    time.sleep(1)
    sending2 = master.write_coil(3, 1, 0x01)
    time.sleep(1)
    sending2 = master.write_coil(5, 1, 0x01)
    time.sleep(1)
    # receive = master.read_holding_registers(address=5, count=2, slave=0x01)
    # receive2 = master.read_holding_registers(address=10, count=7, slave=0x01)
    # print(receive.__dict__)    time.sleep(1)
