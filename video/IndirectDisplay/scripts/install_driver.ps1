# PI.ps1

$script_root = Split-Path -Parent -Path $MyInvocation.MyCommand.Definition

pnputil /add-driver $script_root\..\x64\Debug\IddSampleDriver\IddSampleDriver.inf /install
