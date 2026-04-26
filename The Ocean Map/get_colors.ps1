Add-Type -AssemblyName System.Drawing
function Get-PixelColor([string]$filepath) {
    if (Test-Path $filepath) {
        $img = [System.Drawing.Image]::FromFile($filepath)
        $bmp = New-Object System.Drawing.Bitmap($img)
        $img.Dispose()
        $color = $bmp.GetPixel(0,0)
        $bmp.Dispose()
        Write-Host "$filepath (0,0): R=$($color.R) G=$($color.G) B=$($color.B)"
    }
}
Get-PixelColor "images\seaweed.png"
Get-PixelColor "images\shark mouth.png"
