# PI.ps1

pnputil /enum-drivers |
Select-String -Pattern IddSampleDriver -Context 1,6 |
ForEach-Object {
	$str = $_.ToString()
	Write-Output "$str"
}
