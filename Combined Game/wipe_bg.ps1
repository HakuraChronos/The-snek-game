Add-Type -AssemblyName System.Drawing

function Remove-Background {
    param([string]$filepath, [int]$minR, [int]$maxR, [int]$minG, [int]$maxG, [int]$minB, [int]$maxB)
    if (Test-Path $filepath) {
        $img = [System.Drawing.Image]::FromFile($filepath)
        $bmp = New-Object System.Drawing.Bitmap($img)
        $img.Dispose()
        
        for ($x = 0; $x -lt $bmp.Width; $x++) {
            for ($y = 0; $y -lt $bmp.Height; $y++) {
                $pixel = $bmp.GetPixel($x, $y)
                if ($pixel.R -ge $minR -and $pixel.R -le $maxR -and
                    $pixel.G -ge $minG -and $pixel.G -le $maxG -and
                    $pixel.B -ge $minB -and $pixel.B -le $maxB) {
                    $bmp.SetPixel($x, $y, [System.Drawing.Color]::FromArgb(0, 0, 0, 0))
                }
            }
        }
        
        # Also crop the image
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
            Write-Host "No visible pixels remaining in $filepath"
        }
    }
}

# Seaweed: R=183, G=184, B=186
Remove-Background "images\seaweed.png" 175 190 175 190 180 195

# Shark mouth: R=185, G=219, B=254
Remove-Background "images\shark mouth.png" 175 195 210 225 245 255
