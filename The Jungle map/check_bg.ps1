Add-Type -AssemblyName System.Drawing

$imgDir = 'C:\Users\mcu\Downloads\Chronos\The snek game\UI game picture (Jungle)'
$keyPath = Join-Path $imgDir 'Key.png'
$doorPath = Join-Path $imgDir 'Door.png'

$keyBmp = [System.Drawing.Bitmap]::FromFile($keyPath)
$bgColor = $keyBmp.GetPixel(0,0)
Write-Output "Key background color: $bgColor"
$keyBmp.Dispose()

$doorBmp = [System.Drawing.Bitmap]::FromFile($doorPath)
$bgColor2 = $doorBmp.GetPixel(0,0)
Write-Output "Door background color: $bgColor2"
$doorBmp.Dispose()
