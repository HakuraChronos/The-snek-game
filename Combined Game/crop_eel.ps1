Add-Type -AssemblyName System.Drawing

function Crop-Transparent {
    param([string]$filepath)
    if (Test-Path $filepath) {
        $img = [System.Drawing.Image]::FromFile($filepath)
        $bmp = New-Object System.Drawing.Bitmap($img)
        $img.Dispose()
        
        $minX = $bmp.Width; $minY = $bmp.Height; $maxX = 0; $maxY = 0
        for ($x = 0; $x -lt $bmp.Width; $x++) {
            for ($y = 0; $y -lt $bmp.Height; $y++) {
                $pixel = $bmp.GetPixel($x, $y)
                # Check for transparency or pure black border
                if ($pixel.A -gt 10 -and ($pixel.R -gt 0 -or $pixel.G -gt 0 -or $pixel.B -gt 0)) {
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
            Write-Host "Cropped $filepath"
        } else {
            $bmp.Dispose()
            Write-Host "No visible pixels in $filepath"
        }
    }
}

Crop-Transparent "images\Eel head.png"
Crop-Transparent "images\Eel body.png"
Crop-Transparent "images\Eel tail.png"
