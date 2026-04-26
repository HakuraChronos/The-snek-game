Add-Type -AssemblyName System.Drawing
function Check-Alpha([string]$filepath) {
    if (Test-Path $filepath) {
        $img = [System.Drawing.Image]::FromFile($filepath)
        $bmp = New-Object System.Drawing.Bitmap($img)
        $img.Dispose()
        $maxA = 0
        for ($x = 0; $x -lt $bmp.Width; $x++) {
            for ($y = 0; $y -lt $bmp.Height; $y++) {
                $pixel = $bmp.GetPixel($x, $y)
                if ($pixel.A -gt $maxA) { $maxA = $pixel.A }
            }
        }
        $bmp.Dispose()
        Write-Host "$filepath Max Alpha: $maxA"
    }
}
Check-Alpha "images\Eel head.png"
Check-Alpha "images\Eel body.png"
Check-Alpha "images\Eel tail.png"
