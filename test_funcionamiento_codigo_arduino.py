import serial
import time

# Inicializa la comunicación serial
ser = serial.Serial('COM5', 115200)  # Reemplaza 'COM5' con el puerto correcto

while True:
    ser.write(b'\x00')  # Envia un byte en 0
    print("Byte 0 enviado")
    time.sleep(0.5)     # Envia un byte cada 500ms
    if ser.in_waiting > 0:
        incoming_byte = ser.read()  # Lee el byte entrante
        print("Mensaje recibido")
        if incoming_byte == b'\x00':
            print("Byte 0 recibido, deteniendo el envío de bytes")
            break
        
ser.write(b'\x00') # Identificador
ser.write(b'\x00') # Usuarios Conectados

while True:
    if ser.in_waiting > 0:
        incoming_byte = ser.read()  # Lee el byte entrante
        print(f"Byte entrante: {incoming_byte}")
    
ser.close()  # Cierra la comunicación serial
