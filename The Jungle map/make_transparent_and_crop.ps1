Add-Type -AssemblyName System.Drawing

$imgDir = "C:\Users\mcu\Downloads\Chronos\The snek game\UI game picture (Jungle)"
$filesToProcess = @("Key.png", "Door.png")

foreach ($fileName in $filesToProcess) {
    $imgPath = Join-Path $imgDir $fileName
    $img = [System.Drawing.Bitmap]::FromFile($imgPath)
    
    # Get background color
    $bgColor = $img.GetPixel(0, 0)
    Write-Output "Processing $fileName with bg color: $bgColor"
    
    # Create a copy with transparency
    $bmp = new-object System.Drawing.Bitmap($img)
    $img.Dispose() # release original file lock
    
    $bmp.MakeTransparent($bgColor)
    
    # Find bounding box of non-transparent pixels
    $minX = $bmp.Width
    $minY = $bmp.Height
    $maxX = 0
    $maxY = 0
    
    for ($x = 0; $x -lt $bmp.Width; $x++) {
        for ($y = 0; $y -lt $bmp.Height; $y++) {
            $p = $bmp.GetPixel($x, $y)
            if ($p.A -gt 0) {
                if ($x -lt $minX) { $minX = $x }
                if ($x -gt $maxX) { $maxX = $x }
                if ($y -lt $minY) { $minY = $y }
                if ($y -gt $maxY) { $maxY = $y }
            }
        }
    }
    
    if ($minX -le $maxX -and $minY -le $maxY) {
        $cropRect = New-Object System.Drawing.Rectangle($minX, $minY, ($maxX - $minX + 1), ($maxY - $minY + 1))
        $croppedBmp = $bmp.Clone($cropRect, $bmp.PixelFormat)
        $croppedBmp.Save($imgPath, [System.Drawing.Imaging.ImageFormat]::Png)
        $croppedBmp.Dispose()
        Write-Output "Cropped and saved $fileName"
    } else {
        Write-Output "Image $fileName is empty after removing background!"
    }
    $bmp.Dispose()
}
