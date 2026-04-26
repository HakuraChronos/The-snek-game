Add-Type -AssemblyName System.Drawing

function Remove-BlackBG {
    param([string]$filepath)
    if (Test-Path $filepath) {
        $img = [System.Drawing.Image]::FromFile($filepath)
        $bmp = New-Object System.Drawing.Bitmap($img)
        $img.Dispose()
        
        for ($x = 0; $x -lt $bmp.Width; $x++) {
            for ($y = 0; $y -lt $bmp.Height; $y++) {
                $pixel = $bmp.GetPixel($x, $y)
                # If pixel is pure black or extremely close to black, make it transparent
                if ($pixel.R -le 15 -and $pixel.G -le 15 -and $pixel.B -le 15) {
                    $bmp.SetPixel($x, $y, [System.Drawing.Color]::FromArgb(0, 0, 0, 0))
                }
            }
        }
        
        # Crop transparent borders again just in case
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
            Write-Host "Wiped Black BG and Cropped $filepath"
        } else {
            $bmp.Dispose()
            Write-Host "No visible pixels left in $filepath"
        }
    }
}

Remove-BlackBG "images\Eel head.png"
Remove-BlackBG "images\Eel body.png"
Remove-BlackBG "images\Eel tail.png"
