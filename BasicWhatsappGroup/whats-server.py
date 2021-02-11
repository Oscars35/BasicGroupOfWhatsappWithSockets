import optparse
import sys
import socket

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
    whats_socket=socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
    whats_socket.bind((host, port))
    return whats_socket

if __name__ == "__main__":
    parser()
    whats_socket = get_new_socket()
    while True:
        message, address = whats_socket.recvfrom(BUFFER_SIZE)
        message = message.decode("utf-8")
        print (message)
        print (address)
        if message == '1':
            message = str.encode(message)
            whats_socket.sendto(message, address)
            clients.append(address)
        else:
            for address_client in clients:
                if address_client != address:
                    message_to_send = str.encode(message)
                    whats_socket.sendto(message_to_send, address_client)





