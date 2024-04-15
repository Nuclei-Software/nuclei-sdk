#!/bin/env python3

import os
import re
import sys


def generate_binary_from_log(logfile):
    """
    Parses a log file to extract binary data sections and writes them to separate files.

    Args:
        logfile (str): Path to the log file to process.

    Returns:
        bool: True if processing was successful, False otherwise.
    """

    # Check if the log file exists
    if not os.path.isfile(logfile):
        print(f"{logfile} does not exist. Please check!")
        return False

    # Define regular expressions for data start/end patterns (consider making them more specific)
    datastart_pattern = r"Dump\s+.+?\s+start"
    dataend_pattern = r"Dump\s+.+?\s+finish"

    # Track processing state (0: idle, 1: collecting hex data, 2: generating binary file)
    state = 0

    # Initialize variables for storing hex data and generated filename
    hexstr = ""
    genfilename = ""

    # Open the log file in read mode
    with open(logfile, "r") as lf:
        for line in lf.readlines():
            # Remove leading/trailing whitespace
            line = line.strip()

            # Skip empty lines
            if not line:
                continue

            # Check for data start pattern
            if re.search(datastart_pattern, line):
                # Reset state for new data section
                state = 1
                hexstr = ""
                genfilename = ""
                continue

            # Check for data end pattern
            if re.search(dataend_pattern, line):
                # Reset state for idle
                state = 0
                continue

            # Check for "CREATE" line indicating filename
            if line.startswith("CREATE"):
                # Extract filename and reset state for collecting hex data
                state = 2
                genfilename = line.strip("CREATE:").strip()

            # Append hex data to hexstr while collecting data
            if state == 1:
                hexstr += line

            # Process extracted hex data and create binary file
            if state == 2 and genfilename:
                try:
                    # Attempt to convert hex string to bytes (handle potential conversion errors)
                    binarydata = bytes.fromhex(hexstr)
                    print(f"Generating {genfilename}")
                    # Open the binary file in write-binary mode
                    with open(genfilename, "wb") as wf:
                        wf.write(binarydata)
                except ValueError:
                    print(f"Error: Invalid hex data when creating : {genfilename}")

                # Reset variables for next data section
                hexstr = ""
                genfilename = ""
                state = 1

    return True


if __name__ == "__main__":
    if len(sys.argv) > 1:
        logfile = sys.argv[1]
        print(f"Parsing log file {logfile}")
        generate_binary_from_log(logfile)
    else:
        print(f"Help: {sys.argv[0]} logfile")
