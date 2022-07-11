# PI.ps1

$script_root = Split-Path -Parent -Path $MyInvocation.MyCommand.Definition

powershell $script_root\delete_drivers.ps1
powershell $script_root\install_driver.ps1
