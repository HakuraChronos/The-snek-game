Add-Type -AssemblyName System.Drawing

$excel = New-Object -ComObject Excel.Application
$excel.Visible = $false
$excel.DisplayAlerts = $false

$srcPath = "C:\Users\mcu\Downloads\Chronos\The snek game\The_Jungle_Map_V7.xlsx"
$workbook = $excel.Workbooks.Open($srcPath)
$worksheet = $workbook.Sheets.Item(1)

$saddle  = [System.Drawing.ColorTranslator]::ToOle([System.Drawing.Color]::SaddleBrown)
$ygColor = [System.Drawing.ColorTranslator]::ToOle([System.Drawing.Color]::YellowGreen)
$xlNone  = -4142

# Build shape info (size + position) per top-left cell
$shapeInfo = @{}
foreach ($shape in $worksheet.Shapes) {
    try {
        $tlCell = $shape.TopLeftCell
        $r = $tlCell.Row
        $c = $tlCell.Column
        $key = "$r,$c"
        if (-not $shapeInfo.ContainsKey($key)) {
            $shapeInfo[$key] = @{
                W    = $shape.Width
                H    = $shape.Height
                Name = $shape.Name
            }
        }
    } catch {}
}

# Reference cell size
$refCell = $worksheet.Cells.Item(5, 5)
$cellW = $refCell.Width   # ~20.4
$cellH = $refCell.Height  # ~19.8

Write-Host "Cell size: W=$cellW H=$cellH"
Write-Host "Assigning values..."

$stats = @{ road=0; bush=0; wall=0; apple=0; key=0; door=0; trap=0 }

for ($r = 1; $r -le 40; $r++) {
    for ($c = 1; $c -le 40; $c++) {
        $cell  = $worksheet.Cells.Item($r, $c)
        $key   = "$r,$c"
        $isBorder = ($r -eq 1 -or $r -eq 40 -or $c -eq 1 -or $c -eq 40)

        $bgColorIndex = $xlNone
        $bgColor      = 0
        try {
            $bgColorIndex = $cell.Interior.ColorIndex
            if ($bgColorIndex -ne $xlNone) { $bgColor = $cell.Interior.Color }
        } catch {}

        $hasShape = $shapeInfo.ContainsKey($key)
        $shapeW   = if ($hasShape) { $shapeInfo[$key].W } else { 0 }
        $shapeH   = if ($hasShape) { $shapeInfo[$key].H } else { 0 }

        $val = 0  # default: road

        if ($isBorder) {
            # Outer border -> outer wall
            $val = 10
            $stats.wall++
        } elseif ($bgColor -eq $ygColor) {
            # YellowGreen background
            # Door is larger than cell (>100% size), Apple/Key are smaller (~60%)
            if ($shapeW -gt ($cellW * 1.05)) {
                # Large shape on YellowGreen = Door
                $val = 6
                $stats.door++
            } elseif ($shapeW -lt ($cellW * 0.8)) {
                # Small shape on YellowGreen = Apple (most common) or Key
                # Key.png is the only key, and from the original map there's usually 1 key
                # We can't distinguish key from apple by size alone here
                # Default: Apple. Key will be manually set if needed.
                $val = 3
                $stats.apple++
            } else {
                # Normal size on YellowGreen - treat as apple
                $val = 3
                $stats.apple++
            }
        } elseif ($bgColor -eq $saddle) {
            # SaddleBrown background
            # Trap shapes are EXACTLY cell size (set to $width, $height - no +1.5)
            # Bush shapes are cell size + 1.5 (slightly larger)
            if ($shapeW -lt ($cellW + 1.0)) {
                # Exact cell size or smaller = Trap
                $val = 9
                $stats.trap++
            } else {
                # Slightly larger than cell = Bush
                $val = 1
                $stats.bush++
            }
        } elseif ($hasShape) {
            # No special color but has shape = Road
            $val = 0
            $stats.road++
        } else {
            # No shape, no color = Road (empty cell, no image)
            $val = 0
            $stats.road++
        }

        # Set value behind the image
        $cell.Value2 = $val
    }
}

Write-Host "=== Final Assignment ==="
Write-Host "  Road     (0):  $($stats.road)"
Write-Host "  Bush     (1):  $($stats.bush)"
Write-Host "  Apple    (3):  $($stats.apple)"
Write-Host "  Door     (6):  $($stats.door)"
Write-Host "  Trap     (9):  $($stats.trap)"
Write-Host "  OuterWall(10): $($stats.wall)"

$workbook.Save()
Write-Host "Saved: $srcPath"
$workbook.Close($false)
$excel.Quit()
Write-Host "Done."
