import optparse

DEFAULT_PORT = 1234
DEFAULT_VERBOSE = False
DEFAULT_HOST = '127.0.0.1'

class Parser:

    @staticmethod
    def parse():
            parser = optparse.OptionParser(formatter=optparse.TitledHelpFormatter())
            parser.add_option ('-v', '--verbose', action='store_true', default=DEFAULT_VERBOSE, help='verbose output')
            parser.add_option ('--host', action='store', type='string', default=DEFAULT_HOST, help='Host, default LocalHost')
            parser.add_option ('-p', '--port', action='store', type='int', default=DEFAULT_PORT, help='Listening port, default 1234')
            (options, args) = parser.parse_args()
            if len(args) > 0:
                parser.error('bad args, use --help for help')
            if options.port is not None:
                port = options.port
            if options.host is not None:
                host = options.host
            return host, port, options.verbose



