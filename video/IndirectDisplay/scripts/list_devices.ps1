# PI.ps1

pnputil /enum-devices |
Select-String -Pattern IddSampleDriver -Context 0,5 |
ForEach-Object {
	$str = $_.ToString()
	Write-Output "$str"
}
