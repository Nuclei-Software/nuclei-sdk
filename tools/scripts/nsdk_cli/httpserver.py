#!/bin/env python3

import os
import sys
import argparse
import random
import http.server
import socket

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Open http server on selected directory")
    parser.add_argument('--directory', '-d', default=os.getcwd(),
                                    help='specify alternate directory (default: current directory)')
    parser.add_argument('--port', help="port to listen to, default random port")

    args = parser.parse_args()

    lisport = random.randint(10000, 65000)
    httpdirectory = args.directory

    if os.path.isdir(args.directory) == False:
        print("ERROR: Directory %s not exist, please check!" % (args.directory))
        sys.exit(1)

    print("HTTP Served for directory %s" % (httpdirectory))
    if args.port:
        lisport = int(args.port)
        print("Listen to port %d(specified by --port)" % (lisport))
    else:
        print("Listen to port %d(random generated)" % (lisport))


    # changed based on https://docs.python.org/3/library/http.server.html

    class myHandler(http.server.SimpleHTTPRequestHandler):
        # https://gist.github.com/scottj/a510d6bd96941901fb99554566ee226d
        extensions_map = {
            '': 'application/octet-stream',
            '.manifest': 'text/cache-manifest',
            '.html': 'text/html',
            '.txt': 'text/plain',
            '.text': 'text/plain',
            '.log': 'text/plain',
            '.csv': 'text/csv',
            '.png': 'image/png',
            '.jpg': 'image/jpg',
            '.svg': 'image/svg+xml',
            '.css': 'text/css',
            '.pdf': 'application/pdf',
            '.js':'application/x-javascript',
            '.wasm': 'application/wasm',
            '.json': 'application/json',
            '.xml': 'application/xml',
            '.xls': 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet',
            '.xlsx': 'application/vnd.ms-excel'
        }
        def __init__(self, *args, **kwargs):
            super().__init__(*args, directory=httpdirectory, **kwargs)

    httpd = http.server.HTTPServer(("", lisport), myHandler)

    try:
        print("Please open http://%s:%d" % (socket.gethostname(), lisport))
        httpd.serve_forever()
    except:
        print("http server closed!")
        pass
    sys.exit(0)
