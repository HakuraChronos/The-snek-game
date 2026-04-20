$csharp = @"
using System;
using System.Drawing;
using System.Drawing.Imaging;

public class ImageCropper {
    public static Rectangle GetCropBounds(Bitmap img) {
        int minX = img.Width;
        int minY = img.Height;
        int maxX = 0;
        int maxY = 0;
        bool hasVisible = false;

        BitmapData data = img.LockBits(new Rectangle(0, 0, img.Width, img.Height), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
        int stride = data.Stride;
        int bytes = Math.Abs(stride) * img.Height;
        byte[] rgbValues = new byte[bytes];
        System.Runtime.InteropServices.Marshal.Copy(data.Scan0, rgbValues, 0, bytes);
        img.UnlockBits(data);

        for (int y = 0; y < img.Height; y++) {
            for (int x = 0; x < img.Width; x++) {
                int index = y * stride + x * 4;
                byte alpha = rgbValues[index + 3];
                if (alpha > 10) {
                    if (x < minX) minX = x;
                    if (x > maxX) maxX = x;
                    if (y < minY) minY = y;
                    if (y > maxY) maxY = y;
                    hasVisible = true;
                }
            }
        }
        
        if (hasVisible) {
            return new Rectangle(minX, minY, maxX - minX + 1, maxY - minY + 1);
        }
        return Rectangle.Empty;
    }
}
"@
Add-Type -TypeDefinition $csharp -ReferencedAssemblies System.Drawing

$dir = "C:\Users\mcu\Downloads\Chronos\The snek game\UI game picture (Jungle)"
$filesToCrop = @("trap-1.png", "spider.png", "trap-3.png")

foreach ($fileName in $filesToCrop) {
    $filePath = Join-Path $dir $fileName
    if (Test-Path $filePath) {
        Write-Output "Processing $fileName..."
        $img = [System.Drawing.Bitmap]::FromFile($filePath)
        
        $cropRect = [ImageCropper]::GetCropBounds($img)
        
        if ($cropRect.Width -gt 0 -and ($cropRect.Width -ne $img.Width -or $cropRect.Height -ne $img.Height)) {
            Write-Output "Cropping $fileName from $($img.Width)x$($img.Height) to $($cropRect.Width)x$($cropRect.Height)"
            $croppedImg = $img.Clone($cropRect, $img.PixelFormat)
            $img.Dispose()
            
            $croppedImg.Save($filePath, [System.Drawing.Imaging.ImageFormat]::Png)
            $croppedImg.Dispose()
        } else {
            Write-Output "No cropping needed for $fileName"
            $img.Dispose()
        }
    } else {
        Write-Output "File not found: $fileName"
    }
}
Write-Output "Done cropping traps."
