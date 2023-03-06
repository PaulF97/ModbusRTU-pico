# ModbusRTU-pico

The repository will manage communication between a PC and the PICO microcontroler threw MODBUS protocole.

The goel of this project is to control various GPIO pins of the PICO using the Modbus protocole.

The modbus protocole is a communication protocole used to send data between electronic devises. Usually between a PC and a device.

In our case, the PC will send modbus commands to the PICO respecting RTU frame. RTU is the serial method to communicat in modbus.

We will use the Pymodbus library contaning various functions usefull to communicat between devices or serial ports such as opening initializing clients, servers, write or read into registers. Modbus has 4 types of registers.

Coils registers : address from 0001 to 09999 access read and write. 1 bit space Discrete Input : address from 10001 to 19999 access read only. 1 bit space Input Register : address from 30001 to 39999 access read only. 16 bits space Holding register : address from 40001 to 49999 access read and write. 16 bits space

During this project, we used various libraries and implementations : 

In the PC (master), we use a python library called pyModbus witch will send modbus write and read requests to are PICO using USB serial connexion.
In the PICO (slave), we integrated a library called ligthmodbus witch will reconize the modbus Frames the PC has send to the PICO. this lirbary will parse the modbus frame and reconize the type of frame (read or write requests) and what are the registers we want to access. The library can also manage the various potential errors we can have on the modbus protocole.

The USB link will only be used to send the modbus frame. Therefore, we have configured a UART line that will bypass the USB link and send various messages to the user on the PC.

The uart data will be visible on the port /dev/ttyUSB0 and the modbus data on /dev/ttyACM0.

We have validated each parts (read and write) seperatly but the first version can send continousely write and read requests.

To launch a test, you have to use the branch dev_lib_write_read where a tag v1.0.0 has been created.

On a minicom, use the following command to see the data passing threw the UART : sudo minicom -D /dev/ttyUSB0 -b 115200

When the firmware is in the PICO, you should have the following message : 

![image](https://user-images.githubusercontent.com/65350546/222456566-87e59030-0c43-472d-81d6-a41d8d1cfed5.png)

then launch the python script that will send various requests in a infinit loop using python3 master.py.

On the UART debug window, you should see data passing threw the UART.

To stop the test, just interrupt the python script. The python is the master, therefore, it will be the part managing the send of data.


# build and run project

mkdir build

cd build

cmake ..

make

cp file_name.uf2 /media/<user>/RP2_RPI_boot
