$NUCLEI_TOOL_ROOT="C:\Nuclei"

# Create your setup_config.ps1
# and define $NUCLEI_TOOL_ROOT like below
# set $NUCLEI_TOOL_ROOT="D:\Software\Nuclei"

$SETUP_CONFIG="setup_config.ps1"

if (Test-Path $SETUP_CONFIG) {
    . .\$SETUP_CONFIG
}

Write-Output "Setup Nuclei SDK Tool Environment"
Write-Output "NUCLEI_TOOL_ROOT=$NUCLEI_TOOL_ROOT"

$env:Path="$NUCLEI_TOOL_ROOT\gcc\bin;$NUCLEI_TOOL_ROOT\openocd\bin;$NUCLEI_TOOL_ROOT\build-tools\bin;"+$env:Path
