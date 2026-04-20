$excel = New-Object -ComObject Excel.Application
$excel.Visible = $true
$workbook = $excel.Workbooks.Open("C:\Users\mcu\Downloads\Chronos\The snek game\The Jungle Map.xlsx")
$worksheet = $workbook.Sheets.Item(1)

$excel.ActiveWindow.DisplayGridlines = $false

$imgDir = "C:\Users\mcu\Downloads\Chronos\The snek game\UI game picture (Jungle)\"

$roads = @("road-1.png", "road-2.png")
$bushes = @("bush-2.png", "bush-3.png")
$apples = @("apple.png", "greenapple.png")

for ($c=1; $c -le 40; $c++) {
    $worksheet.Columns.Item($c).ColumnWidth = 3
}
for ($r=1; $r -le 40; $r++) {
    $worksheet.Rows.Item($r).RowHeight = 20
}

for ($r = 1; $r -le 40; $r++) {
    for ($c = 1; $c -le 40; $c++) {
        $cell = $worksheet.Cells.Item($r, $c)
        $val = $cell.Text.Trim()
        
        $imgName = $null
        $rotation = 0
        
        if ($val -eq "0") {
            if (($r + $c) % 2 -eq 0) {
                $imgName = "road-1.png"
            } else {
                $imgName = "road-2.png"
            }
        } elseif ($val -eq "1") {
            if (($r -eq 1 -and $c -eq 1) -or ($r -eq 1 -and $c -eq 40) -or ($r -eq 40 -and $c -eq 1) -or ($r -eq 40 -and $c -eq 40)) {
                $imgName = "wall-corner.png"
                if ($r -eq 1 -and $c -eq 40) { $rotation = 90 }
                if ($r -eq 40 -and $c -eq 40) { $rotation = 180 }
                if ($r -eq 40 -and $c -eq 1) { $rotation = 270 }
            } elseif ($r -eq 1 -or $r -eq 40 -or $c -eq 1 -or $c -eq 40) {
                $imgName = "wall-3.png"
                if ($c -eq 40) { $rotation = 90 }
                if ($r -eq 40) { $rotation = 180 }
                if ($c -eq 1) { $rotation = 270 }
            } else {
                $imgName = $bushes | Get-Random
            }
        } elseif ($val -eq "2") {
            $imgName = "Worm head.png"
        } elseif ($val -eq "3") {
            $imgName = $apples | Get-Random
        } elseif ($val -eq "5") {
            $imgName = "Key.png"
        } elseif ($val -eq "6") {
            $imgName = "Door.png"
        }
        
        if ($imgName -ne $null) {
            $imgPath = $imgDir + $imgName
            if (Test-Path $imgPath) {
                $cell.Value2 = ""
                $left = $cell.Left
                $top = $cell.Top
                $width = $cell.Width
                $height = $cell.Height
                $pic = $worksheet.Shapes.AddPicture($imgPath, $false, $true, $left, $top, $width, $height)
                $pic.LockAspectRatio = 0
                $pic.Width = $width + 1.5
                $pic.Height = $height + 1.5
                if ($rotation -ne 0) {
                    $pic.Rotation = $rotation
                }
            }
        }
    }
}
$workbook.SaveAs("C:\Users\mcu\Downloads\Chronos\The snek game\The_Jungle_Map_Fixed.xlsx", 51)
Write-Output "Done"
