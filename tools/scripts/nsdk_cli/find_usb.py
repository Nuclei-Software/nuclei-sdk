#!/bin/env python3
import os
import sys
import tempfile
import pprint
import serial
import serial.tools.list_ports
import json
from nsdk_utils import *
#from prettytable import PrettyTable

def parse_usbinfo(origin):
    bus_splits = origin["bus"].split()
    bus, device = bus_splits[1], bus_splits[3].strip(":")
    vid = origin["idvendor"].split()[1]
    pid = origin["idproduct"].split()[1]
    manufact = " ".join(origin["imanufacturer"].split()[2:])
    product = " ".join(origin["iproduct"].split()[2:])
    if len(origin["iserial"].split()) > 2:
        serial = origin["iserial"].split()[2]
    else:
        serial = ""
    return {"bus": bus, "device": device, "pid": pid, "vid": vid, "serial": serial, "manufacturer": manufact, "product": product}

def find_usbinfo():
    cmdlog = tempfile.mktemp()
    # lsusb may need sudo privilege to get iserial info
    os.system("lsusb -v 2>/dev/null | grep -i -E \"^bus |iserial|idvendor|idproduct|imanufacturer|iproduct\" > %s" % (cmdlog))
    usb_devices = []
    INFOCNT = 6
    with open(cmdlog, "r") as cf:
        lines = cf.readlines()
        for i in range(0, int(len(lines) / INFOCNT)):
            bus = lines[i*INFOCNT]
            if "Linux Foundation" in bus:
                continue
            if "VMware, Inc" in bus:
                continue
            if "VirtualBox" in bus:
                continue
            bus = bus.lower()
            if "future technology" not in bus and "xilinx" not in bus:
                continue
            try:
                origin = {"bus": lines[i*INFOCNT].strip(), "idvendor": lines[i*INFOCNT + 1].strip(), \
                          "idproduct": lines[i*INFOCNT + 2].strip(), "iserial": lines[i*INFOCNT + 5].strip(), \
                          "imanufacturer": lines[i*INFOCNT + 3], "iproduct": lines[i*INFOCNT + 4]}
                parsed = parse_usbinfo(origin)
                device = {"origin": origin, "parsed": parsed}
            except:
                continue
            usb_devices.append(device)

    os.remove(cmdlog)
    return usb_devices

def find_usb_serport(ser):
    comports = serial.tools.list_ports.comports()
    serports = []
    for port in comports:
        cur_serno = port.serial_number
        if cur_serno is None:
            continue
        if cur_serno == ser:
            serports.append(port.device)
    return serports

def parse_usb_devices():
    usb_devices = find_usbinfo()
    fpga_devices = find_fpgas()
    usb_infos = dict()
    fpga_runners = dict()
    all_ftdiser = ""
    for usb in usb_devices:
        parsed = usb["parsed"]
        serports = find_usb_serport(parsed["serial"])
        parsed["serports"] = serports
        loc = "%s:%s" % (parsed["bus"], parsed["device"])
        usb_infos[loc] = {"serial": parsed["serial"], \
                          "serports": serports, \
                          "pid": parsed["pid"], \
                          "vid": parsed["vid"], \
                          "manufacturer": parsed["manufacturer"], \
                          "product": parsed["product"], \
                          "info": usb["origin"]["bus"]}
        is_fpga = False
        for fpga in fpga_devices:
            if parsed["serial"] in fpga:
                usb_infos[loc]["fpga_serial"] = fpga
                usb_infos[loc]["fpga_device"] = fpga_devices[fpga]
                is_fpga = True
        if is_fpga == False:
            if all_ftdiser != "":
                all_ftdiser = "%s,%s" % (all_ftdiser, parsed["serial"])
            else:
                all_ftdiser = parsed["serial"]
    # construct fpga runner
    index = 0
    for fpga in fpga_devices:
        fpga_device = fpga_devices[fpga]
        if "xc7a200t" in fpga_device:
            board = "ddr200t"
        elif "xcku060" in fpga_device:
            board = "ku060"
        elif "vu9p" in fpga_device:
            board = "vcu118"
        else:
            board = fpga_device
        key = "%s_%s" % (index, board)
        index = index + 1
        fpga_runners[key] = {"board_type": board, "fpga_serial": fpga, \
                             "ftdi_serial": all_ftdiser, "serial_port": None}
    fpga_runner_dict = {"fpga_runners": fpga_runners}
    return usb_infos, fpga_runner_dict

if __name__ == '__main__':
    usb_infos, fpga_runner_dict = parse_usb_devices()
    if len(usb_infos) == 0:
        print("No usb found, please check!")
        sys.exit(1)
    # save as json
    pprint.pprint(usb_infos)
    with open("usb_info.json", "w") as uf:
        json.dump(usb_infos, uf, indent=4)
        print("Save usb information into usb_info.json")
    if len(fpga_runner_dict["fpga_runners"]) == 0:
        print("No fpga devices found, please check!")
    else:
        save_yaml("fpga_runner.yaml", fpga_runner_dict)
        print("Save fpga runner information into fpga_runners.yaml")
