Add-Type -AssemblyName System.Drawing

function Make-Transparent {
    param([string]$filepath)
    if (Test-Path $filepath) {
        $img = [System.Drawing.Image]::FromFile($filepath)
        $bmp = New-Object System.Drawing.Bitmap($img)
        $img.Dispose()
        for ($x = 0; $x -lt $bmp.Width; $x++) {
            for ($y = 0; $y -lt $bmp.Height; $y++) {
                $pixel = $bmp.GetPixel($x, $y)
                if ($pixel.R -gt 240 -and $pixel.G -gt 240 -and $pixel.B -gt 240) {
                    $bmp.SetPixel($x, $y, [System.Drawing.Color]::FromArgb(0, 255, 255, 255))
                }
            }
        }
        $bmp.Save($filepath, [System.Drawing.Imaging.ImageFormat]::Png)
        $bmp.Dispose()
        Write-Host "Processed $filepath"
    } else {
        Write-Host "File not found: $filepath"
    }
}

Make-Transparent "images\Eel head.png"
Make-Transparent "images\Eel body.png"
Make-Transparent "images\Eel tail.png"
