import socket
import threading

# Impostazioni porte
ARDUINO_PORT = 5001
PLC_PORT = 2001
SERVER_IP = '0.0.0.0'  # ascolta su tutte le interfacce

client_socket_arduino = None
addr_arduino = None
client_socket_PLC = None
addr_plc = None
arrived_to_target = 0

start_cart = 0

def handle_arduino(client_socket, addr, dispositivo):
    global start_cart, arrived_to_target
    print(f"[{dispositivo}] Connection from {addr}")
    try:
        while True:
                #if an objet was detected send command to arduino to start cart
                if start_cart:
                    print("Starting cart")
                    client_socket.sendall(bytes(1))
                    start_cart = 0       
                else:
                    data = client_socket.recv(6)      
                    #if received data from Arduino this means the cart is ready, so set arrived_to_target flag
                    if data.decode()=="Target" and not arrived_to_target:
                        print("Cart Ready!")
                        arrived_to_target = 1
    except Exception as e:
        print(f"[{dispositivo}] Errore: {e}")
    finally:
        client_socket.close()
        print(f"[{dispositivo}] Connection closed from{addr}")




def handle_plc(client_socket, addr, dispositivo):
    global start_cart
    print(f"[{dispositivo}] Connection from {addr}")
    try:
        while True:
                #if the cart arrived to target, send trigger to PLC to start the arm
                if arrived_to_target:
                    print("Starting arm")
                    client_socket.sendall(bytes(1))
                else:
                    data = client_socket.recv(1024)
                    if data and not start_cart:
                        #If received data from PLC, this means an object is detected, so set start_cart flag
                        print("Object detected")
                        start_cart=1
            

    except Exception as e:
        print(f"[{dispositivo}] Error: {e}")
    finally:
        client_socket.close()
        print(f"[{dispositivo}] Connection closed from {addr}")



def avvia_listener_arduino(porta, dispositivo):
    global client_socket_arduino, addr_arduino
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.bind((SERVER_IP, porta))
        server.listen()
        print(f"[{dispositivo}] Waiting on port {porta}")
        while True:
            client_socket_arduino, addr_arduino = server.accept()
            threading.Thread(target=handle_arduino, args=(client_socket_arduino, addr_arduino, dispositivo)).start()

def avvia_listener_plc(porta, dispositivo):
    global client_socket_plc, addr_plc
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.bind((SERVER_IP, porta))
        server.listen()
        print(f"[{dispositivo}] Waiting on port {porta}")
        while True:
            client_socket_plc, addr_plc = server.accept()
            threading.Thread(target=handle_plc, args=(client_socket_plc, addr_plc, dispositivo)).start()



def main():
    threading.Thread(target=avvia_listener_arduino, args=(ARDUINO_PORT, "ARDUINO")).start()
    threading.Thread(target=avvia_listener_plc, args=(PLC_PORT, "PLC")).start()

if __name__ == "__main__":
    main()
