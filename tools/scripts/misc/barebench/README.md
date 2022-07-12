# How to run barebench

## Preparation

1. Connect all the fpga board required.
2. Install Nuclei Studio and Vivado Lab and setup **PATH**.
3. Modify the `*.yaml` files located in this folder.
   You can use `python3 tools/scripts/nsdk_cli/find_usb.py` to
   find the fpga board in generated `fpga_runner.yaml`.
   Change the `fpga_runners` section to match your own `fpga_runner.yaml`


## Download fpga bitstreams

Modify `download_fpga.sh` to download latest bitstream.
The latest bitstream can be found in http://doc/cpu_signoff/

If the script is modified, you can execute the script to download bitstream:

`bash download_fpga.sh`

## Execute benchmark

Execute benchmark for `n900` and `ux900`.

CONFIG="n900,ux900" bash run.sh

Sample output:

~~~shell
INFO: Generate zip file /home/xl_ci/workspace/nuclei-sdk/tools/scripts/misc/barebench/barebench_0.3.8-28-g8790a05748_20220711T084520.zip using /home/xl_ci/workspace/nuclei-sdk/tools/scripts/misc/barebench/gen/0.3.8-28-g8790a05748/barebench
Copy to your local machine using command below
scp xl_ci@whssvm1:/home/xl_ci/workspace/nuclei-sdk/tools/scripts/misc/barebench/barebench_0.3.8-28-g8790a05748_20220711T084520.zip .
~~~
