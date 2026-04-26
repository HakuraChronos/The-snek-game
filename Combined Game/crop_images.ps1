Add-Type -AssemblyName System.Drawing

function Crop-TransparentEdges {
    param([string]$filepath)
    if (Test-Path $filepath) {
        $img = [System.Drawing.Image]::FromFile($filepath)
        $bmp = New-Object System.Drawing.Bitmap($img)
        $img.Dispose()
        
        $minX = $bmp.Width; $minY = $bmp.Height; $maxX = 0; $maxY = 0
        for ($x = 0; $x -lt $bmp.Width; $x++) {
            for ($y = 0; $y -lt $bmp.Height; $y++) {
                if ($bmp.GetPixel($x, $y).A -gt 10) {
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
            Write-Host "No visible pixels found in $filepath"
        }
    } else {
        Write-Host "File not found: $filepath"
    }
}

Crop-TransparentEdges "images\Sand1.png"
Crop-TransparentEdges "images\Sand2.png"
Crop-TransparentEdges "images\Sand3.png"
Crop-TransparentEdges "images\Eel head.png"
Crop-TransparentEdges "images\Eel body.png"
Crop-TransparentEdges "images\Eel tail.png"
