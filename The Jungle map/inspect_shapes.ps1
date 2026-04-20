Add-Type -AssemblyName System.Drawing

$excel = New-Object -ComObject Excel.Application
$excel.Visible = $false
$excel.DisplayAlerts = $false

$srcPath = "C:\Users\mcu\Downloads\Chronos\The snek game\The_Jungle_Map_V7.xlsx"
$workbook = $excel.Workbooks.Open($srcPath)
$worksheet = $workbook.Sheets.Item(1)

$saddle   = [System.Drawing.ColorTranslator]::ToOle([System.Drawing.Color]::SaddleBrown)
$ygColor  = [System.Drawing.ColorTranslator]::ToOle([System.Drawing.Color]::YellowGreen)
$xlNone   = -4142  # xlColorIndexNone

# Build shape top-left cell map AND collect shape sizes
# Key/Door images have special sizes set in fix_map.ps1:
#   Door:  width * 1.6, height * 1.6  (larger)
#   Apple: width * 0.6, height * 0.6  (smaller)
#   Others: width + 1.5, height + 1.5

# Cell size is 20 row height, col width 3 (approx 22.5 pts)
# Let's get actual cell width/height
$refCell = $worksheet.Cells.Item(5, 5)
$cellW = $refCell.Width
$cellH = $refCell.Height
Write-Host "Reference cell size: W=$cellW H=$cellH"

# Map shape by top-left cell
$shapeInfo = @{}
foreach ($shape in $worksheet.Shapes) {
    try {
        $tlCell = $shape.TopLeftCell
        $r = $tlCell.Row
        $c = $tlCell.Column
        $key = "$r,$c"
        $sw = $shape.Width
        $sh = $shape.Height
        if (-not $shapeInfo.ContainsKey($key)) {
            $shapeInfo[$key] = @{ W = $sw; H = $sh; Name = $shape.Name }
        }
    } catch {}
}

Write-Host "Total cells with shapes: $($shapeInfo.Count)"

# Find cells with very large shapes (door) or very small (apple)
Write-Host "`n=== Shape size distribution (sample non-border) ==="
$sizes = @{}
foreach ($kv in $shapeInfo.GetEnumerator()) {
    $key = $kv.Key
    if ($key -match "^(\d+),(\d+)$") {
        $r = [int]$Matches[1]; $c = [int]$Matches[2]
        if ($r -ge 2 -and $r -le 39 -and $c -ge 2 -and $c -le 39) {
            $w = [Math]::Round($kv.Value.W, 1)
            $h = [Math]::Round($kv.Value.H, 1)
            $sz = "$w x $h"
            if (-not $sizes.ContainsKey($sz)) { $sizes[$sz] = 0 }
            $sizes[$sz]++
        }
    }
}
$sizes.GetEnumerator() | Sort-Object { $_.Value } -Descending | Select-Object -First 20 | ForEach-Object {
    Write-Host "  $($_.Name): $($_.Value) shapes"
}

# Show cells with unusual colors (not saddle, not yg, not none)
Write-Host "`n=== Cells with YellowGreen or no-color + shape (non-border) ==="
for ($r = 2; $r -le 39; $r++) {
    for ($c = 2; $c -le 39; $c++) {
        $cell = $worksheet.Cells.Item($r, $c)
        $key = "$r,$c"
        try {
            $bg = $cell.Interior.Color
            $colorType = $cell.Interior.ColorIndex
            if ($colorType -eq $xlNone -and $shapeInfo.ContainsKey($key)) {
                $sw = [Math]::Round($shapeInfo[$key].W, 1)
                $sh = [Math]::Round($shapeInfo[$key].H, 1)
                Write-Host "  R$r C$c : no-color shape size=$sw x $sh"
            } elseif ($bg -eq $ygColor) {
                $sw = if ($shapeInfo.ContainsKey($key)) { [Math]::Round($shapeInfo[$key].W, 1) } else { "N/A" }
                $sh = if ($shapeInfo.ContainsKey($key)) { [Math]::Round($shapeInfo[$key].H, 1) } else { "N/A" }
                Write-Host "  R$r C$c : YellowGreen shape size=$sw x $sh"
            }
        } catch {}
    }
}

$workbook.Close($false)
$excel.Quit()
