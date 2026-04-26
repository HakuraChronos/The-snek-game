Add-Type -AssemblyName System.Drawing
try {
    $img = [System.Drawing.Image]::FromFile("images\Eel head.png")
    Write-Host "Eel head valid. Width: $($img.Width) Height: $($img.Height)"
    $img.Dispose()
    $img = [System.Drawing.Image]::FromFile("images\Eel body.png")
    Write-Host "Eel body valid. Width: $($img.Width) Height: $($img.Height)"
    $img.Dispose()
    $img = [System.Drawing.Image]::FromFile("images\Eel tail.png")
    Write-Host "Eel tail valid. Width: $($img.Width) Height: $($img.Height)"
    $img.Dispose()
} catch {
    Write-Host "Error loading image: $(.Exception.Message)"
}
