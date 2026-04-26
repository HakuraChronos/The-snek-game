Add-Type -AssemblyName System.Drawing

function Process-Image {
    param([string]$filepath)
    if (Test-Path $filepath) {
        $img = [System.Drawing.Image]::FromFile($filepath)
        $bmp = New-Object System.Drawing.Bitmap($img)
        $img.Dispose()
        
        $minX = $bmp.Width; $minY = $bmp.Height; $maxX = 0; $maxY = 0
        for ($x = 0; $x -lt $bmp.Width; $x++) {
            for ($y = 0; $y -lt $bmp.Height; $y++) {
                $pixel = $bmp.GetPixel($x, $y)
                if ($pixel.R -gt 240 -and $pixel.G -gt 240 -and $pixel.B -gt 240) {
                    $bmp.SetPixel($x, $y, [System.Drawing.Color]::FromArgb(0, 255, 255, 255))
                    $pixel = $bmp.GetPixel($x, $y)
                }
                
                if ($pixel.A -gt 10) {
                    if ($x -lt $minX) { $minX = $x }
                    if ($y -lt $minY) { $minY = $y }
                    if ($x -gt $maxX) { $maxX = $x }
                    if ($y -gt $maxY) { $maxY = $y }
                }
            }
        }
        
        if ($minX -le $maxX -and $minY -le $maxY) {
            $rect = New-Object System.Drawing.Rectangle($minX, $minY, ($maxX - $minX + 1), ($maxY - $minY + 1))
            $cropped = $bmp.Clone($rect, $bmp.PixelFormat)
            $bmp.Dispose()
            $cropped.Save($filepath, [System.Drawing.Imaging.ImageFormat]::Png)
            $cropped.Dispose()
            Write-Host "Processed and Cropped $filepath"
        } else {
            $bmp.Dispose()
            Write-Host "No visible pixels found in $filepath"
        }
    } else {
        Write-Host "File not found: $filepath"
    }
}

Process-Image "images\bamboo coral.png"
Process-Image "images\brown coral.png"
Process-Image "images\flower coral.png"
Process-Image "images\green looking coral.png"
Process-Image "images\no idea coral.png"
Process-Image "images\red coral.png"
Process-Image "images\vegetable coral.png"
Process-Image "images\yellow coral.png"
