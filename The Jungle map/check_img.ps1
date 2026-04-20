Add-Type -AssemblyName System.Drawing

$files = Get-ChildItem "C:\Users\mcu\Downloads\Chronos\The snek game\UI game picture (Jungle)\*.png"

foreach ($file in $files) {
    if ($file.Name -in "road-1.png", "wall-3.png", "bush-2.png") {
        $img = [System.Drawing.Bitmap]::FromFile($file.FullName)
        Write-Output ("File: " + $file.Name + " (" + $img.Width + "x" + $img.Height + ")")
        
        # Check first 5 pixels of row 0
        $str = ""
        for($x=0;$x-lt 5;$x++){
            $p = $img.GetPixel($x, 0)
            if ($p.A -eq 0) { $str += "TR " } else { $str += "SO " }
        }
        Write-Output "Top-Left: $str"
        
        $img.Dispose()
    }
}
