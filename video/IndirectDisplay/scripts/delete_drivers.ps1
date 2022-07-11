# PI.ps1

# Find idd sample devices and remove them
pnputil /enum-devices |
Select-String -Pattern SWD\\IddSampleDriver -Context 0,4 |
ForEach-Object {
	$str = $_.ToString() # Select-String returns an Object Array, convert it to a string

	$positionID = $str.IndexOf("SWD\IddSampleDriver") # Find beginning of substring you want
	$result_start = $str.Substring($positionID)

	$result_end = $result_start.IndexOf("`n") # Find end of substring you want
	$result = $result_start.Substring(0, $result_end).Trim()
	Write-Output "Removing device $result"
	pnputil /remove-device $result
}

# Find idd sample drivers and remove them
pnputil /enum-drivers  |
Select-String -Pattern iddsampledriver -Context 1,0 |
ForEach-Object {
	$str = $_.ToString() # Select-String returns an Object Array, convert it to a string

	$positionOEM = $str.IndexOf("oem") # Find beginning of substring you want
	$result_start = $str.Substring($positionOEM)

	$result_end = $result_start.IndexOf(".inf") # Find end of substring you want
	$result = $result_start.Substring(0, $result_end)
	Write-Output "Deleting $result.inf driver"

	$inf_file = -join($result, ".inf")

	pnputil -d $inf_file
}
