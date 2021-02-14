import threading, optparse, sys, socket 
from queue import Queue
from FlagsParser import Parser
from MessageFormater import *


BUFFER_SIZE = 1024


# Closing menu
def show_close_menu(clients, whats_socket, color_queue):
    decision = input("\nAre you sure you want to close the server?. y|n: ")
    while decision != 'y' and decision != 'n' :
        decision = input("Bad output. Are you sure you want to close the server?. y|n: ")
    if decision == 'y':
        print('Okey!, Closing server!...')
        sys.exit()
    connect_clients(clients, whats_socket, color_queue)



# returns a color queue
def get_color_queue():
    queue = Queue()
    for color in ColorList:
        queue.put(color)
    return queue



# Returns a TCP socket
def get_socket(host, port):
    whats_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    whats_socket.bind((host, port))
    return whats_socket



# Reads the message from the client. If the client has closed the socket, send to the other clients that the 
# client has left the group and close his thread and socket
def receive_message(socket_client, clients, address, color_queue, color):
    message = socket_client.recv(BUFFER_SIZE)
    if message:
        final_message = message.decode("utf-8")
        final_message = Formater.message_send(color, final_message, str(address))
        send_message_other_clients(final_message, socket_client, clients)
    else:
        send_message_other_clients(Formater.left_chat(address), socket_client, clients)
        clients.remove(socket_client)
        color_queue.put(color)
        socket_client.close()
        sys.exit() 



# Sends the message to the receivers
def send_message_other_clients(message, socket_sender, clients):
    for current_socket in clients:
        if current_socket != socket_sender:
            current_socket.send(str.encode(message))



# Reads a message from a socket
def read_from_socket(socket_client, clients, address, color_queue, color):
    while True:
        try:
            receive_message(socket_client, clients, address, color_queue, color)
        except socket.error as msg:
            print("socket error: %s",msg)
            sys.exit() 



# Gets the connected client, it assigns him a color and reads from his socket
def connect_client(clients, whats_socket, color_queue):
        while True:
            client_socket, address = whats_socket.accept()
    
            # Tell to everybody that the client joined the server
            client_socket.send(str.encode(Formater.connection_stablished_client()))
            send_message_other_clients(Formater.joined(address), client_socket, clients)
            print(Formater.connection_stablished_server(address))
    
            # Get color for this user
            color = color_queue.get()
    
            clients.append(client_socket)
            tn = threading.Thread(target=read_from_socket, args=(client_socket, clients, address, color_queue, color), daemon=True)
            tn.start()



# Tries to get the clients. If the user does ctrl+c shows the menu for closing the server
def connect_clients(clients, whats_socket, color_queue):
    try:
        connect_client(clients, whats_socket, color_queue)
    except KeyboardInterrupt:
        show_close_menu(clients, whats_socket, color_queue)


# Main
if __name__ == "__main__":
    host, port, verbose = Parser.parse()
    color_queue = get_color_queue()
    whats_socket = get_socket(host, port)
    whats_socket.listen(1)
    print("Waiting for client connections.....")
    connect_clients([], whats_socket, color_queue)
 
