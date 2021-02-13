import threading
import optparse
import sys
import socket
import signal

#Variables globals per a que quedi més bonic
DEFAULT_PORT = 1234
DEFAULT_VERBOSE = False
DEFAULT_HOST = '127.0.0.1'
BUFFER_SIZE = 1024

#List of clients
clients = []
host = DEFAULT_HOST
port = DEFAULT_PORT
verbose = False

def signal_handler(sig, frame):
    decision = input ("\nAre you sure you want to close the server?. y|n: ")
    while decision != 'y' and decision != 'n' :
        decision = input ("Bad output. Are you sure you want to close the server?. y|n: ")
    if decision == 'y':
        print('Okey!, Closing server!...')
        sys.exit()

    else:
        signal.signal(signal.SIGINT, signal_handler)


def parser():
    global verbose, port, host  #es com el this per a la funció i detectar que agafem les de #listofclients
    parser = optparse.OptionParser(formatter=optparse.TitledHelpFormatter())
    parser.add_option ('-v', '--verbose', action='store_true', default=DEFAULT_VERBOSE, help='verbose output')
    parser.add_option ('--host', action='store', type='string', default=DEFAULT_HOST, help='Host, default LocalHost')
    parser.add_option ('-p', '--port', action='store', type='int', default=DEFAULT_PORT, help='Listening port, default 1234')
    (options, args) = parser.parse_args()
    if len(args) > 0:
        parser.error('bad args, use --help for help')
    if options.port is not None:
        port = options.port
    if options.verbose is not False:
        verbose = True
    if options.host is not None:
        host = options.host

def get_new_socket():
    whats_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    whats_socket.bind((host, port))
    return whats_socket

def read_from_socket(socket_client, clients, address):
    while True:
        try:
            print(f"reading message {address}")
            message = socket_client.recv(BUFFER_SIZE)
            message = message.decode("utf-8")
            if len(message) > 0:
                final_message = str(address) + "--> " + message
                for i in clients:
                    if i != socket_client:
                        i.send(str.encode(final_message))
            else:
                clients.remove(socket_client)
                socket_client.close()
                print("process exit")
                sys.exit() 

        except socket.error as msg:
            print("Socket Error: %s",msg)
            sys.exit() 

if __name__ == "__main__":
    parser()
    whats_socket = get_new_socket()
    whats_socket.listen(1)
    signal.signal(signal.SIGINT, signal_handler)
    print("Waiting for client connections.....")
    while True:
        client_socket, address = whats_socket.accept()
        print(f"{address} connected!")

        clients.append(client_socket)
        client_socket.send(str.encode("Connected to the server!"))

        tn = threading.Thread(target=read_from_socket, args=(client_socket, clients, address), daemon=True)
        tn.start()
