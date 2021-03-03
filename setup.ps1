$NUCLEI_TOOL_ROOT="C:\Nuclei"

# Create your setup_config.ps1 before run .\setup.ps1
# and define $NUCLEI_TOOL_ROOT like below
# $NUCLEI_TOOL_ROOT="D:\Software\Nuclei"
# Start Windows PowerShell with the "Run as Administrator" option
# Run the following command below to enable running unsigned scripts:
# set-executionpolicy remotesigned

$SETUP_CONFIG="setup_config.ps1"

if (Test-Path $SETUP_CONFIG) {
    . .\$SETUP_CONFIG
}

Write-Output "Setup Nuclei SDK Tool Environment"
Write-Output "NUCLEI_TOOL_ROOT=$NUCLEI_TOOL_ROOT"

$env:Path="$NUCLEI_TOOL_ROOT\gcc\bin;$NUCLEI_TOOL_ROOT\openocd\bin;$NUCLEI_TOOL_ROOT\build-tools\bin;"+$env:Path
