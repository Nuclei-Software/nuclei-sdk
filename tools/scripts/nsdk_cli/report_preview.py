#!/bin/env python3

import os
import sys
import argparse
import random

try:
    from grip import serve
except Exception as exc:
    print("Import Error: %s" % (exc))
    print("Please install requried package grip using: pip3 install grip")
    sys.exit(1)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Preview SDK CLI Report in web")
    parser.add_argument('--logdir', required=True, help="The root directory of cli report")
    parser.add_argument('--port', help="port to listen to")

    args = parser.parse_args()

    report_md = os.path.join(args.logdir, "report.md")

    if os.path.isfile(report_md) == False:
        print("ERROR: Can't find report.md in %s provided in --logdir option" %(args.logdir))
        sys.exit(1)

    lisport = random.randint(10000, 65000)
    if args.port:
        lisport = int(args.port)
        print("Listen to port %d(specified by --port)" % (lisport))
    else:
        print("Listen to port %d(random generated)" % (lisport))

    serve(path=report_md, host="0.0.0.0", port=lisport)
    sys.exit(0)
