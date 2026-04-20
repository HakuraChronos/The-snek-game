Add-Type -AssemblyName System.Drawing

$dir = "C:\Users\mcu\Downloads\Chronos\The snek game\UI game picture (Jungle)"
$files = Get-ChildItem "$dir\*.png"

foreach ($file in $files) {
    Write-Output "Processing $($file.Name)..."
    $img = [System.Drawing.Bitmap]::FromFile($file.FullName)
    
    $minX = $img.Width
    $minY = $img.Height
    $maxX = 0
    $maxY = 0
    
    $hasVisible = $false
    
    for ($y = 0; $y -lt $img.Height; $y++) {
        for ($x = 0; $x -lt $img.Width; $x++) {
            $p = $img.GetPixel($x, $y)
            if ($p.A -gt 10) { # visible pixel
                if ($x -lt $minX) { $minX = $x }
                if ($x -gt $maxX) { $maxX = $x }
                if ($y -lt $minY) { $minY = $y }
                if ($y -gt $maxY) { $maxY = $y }
                $hasVisible = $true
            }
        }
    }
    
    if ($hasVisible) {
        $cropRect = New-Object System.Drawing.Rectangle($minX, $minY, ($maxX - $minX + 1), ($maxY - $minY + 1))
        
        # Only crop if it actually has transparent borders
        if ($cropRect.Width -ne $img.Width -or $cropRect.Height -ne $img.Height) {
            Write-Output "Cropping $($file.Name) from $($img.Width)x$($img.Height) to $($cropRect.Width)x$($cropRect.Height)"
            $croppedImg = $img.Clone($cropRect, $img.PixelFormat)
            $img.Dispose()
            
            # Save the cropped image
            $croppedImg.Save($file.FullName, [System.Drawing.Imaging.ImageFormat]::Png)
            $croppedImg.Dispose()
        } else {
            Write-Output "No cropping needed for $($file.Name)"
            $img.Dispose()
        }
    } else {
        $img.Dispose()
    }
}
Write-Output "Done cropping."
