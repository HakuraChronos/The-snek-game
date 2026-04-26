Add-Type -AssemblyName System.Drawing
$filepath = "images\blue door.png"
if (Test-Path $filepath) {
    $img = [System.Drawing.Image]::FromFile($filepath)
    $bmp = New-Object System.Drawing.Bitmap($img)
    $img.Dispose()
    
    for ($x = 0; $x -lt $bmp.Width; $x++) {
        for ($y = 0; $y -lt $bmp.Height; $y++) {
            $pixel = $bmp.GetPixel($x, $y)
            if ($pixel.R -ge 245 -and $pixel.G -ge 245 -and $pixel.B -ge 245) {
                $bmp.SetPixel($x, $y, [System.Drawing.Color]::FromArgb(0, 0, 0, 0))
            }
        }
    }
    
    # Crop the image
    $minX = $bmp.Width; $minY = $bmp.Height; $maxX = 0; $maxY = 0
    for ($x = 0; $x -lt $bmp.Width; $x++) {
        for ($y = 0; $y -lt $bmp.Height; $y++) {
            $pixel = $bmp.GetPixel($x, $y)
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
    }
}
