Add-Type -AssemblyName System.Drawing
$filepath = "images\blue door.png"
if (Test-Path $filepath) {
    $img = [System.Drawing.Image]::FromFile($filepath)
    $bmp = New-Object System.Drawing.Bitmap($img)
    $img.Dispose()
    $color = $bmp.GetPixel(0,0)
    $bmp.Dispose()
    Write-Host "$filepath (0,0): R=$($color.R) G=$($color.G) B=$($color.B) A=$($color.A)"
}
