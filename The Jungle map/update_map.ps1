$excel = New-Object -ComObject Excel.Application
$excel.Visible = $true
$workbook = $excel.Workbooks.Open("C:\Users\mcu\Downloads\Chronos\The Jungle Map.xlsx")
$worksheet = $workbook.Sheets.Item(1)

$imgDir = "C:\Users\mcu\Downloads\Chronos\UI game picture (Jungle)\"

$roads = @("road-1.png", "road-2.png", "road-3.png", "road-4.png")
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
        
        if ($val -eq "0") {
            $imgName = $roads | Get-Random
        } elseif ($val -eq "1") {
            if (($r -eq 1 -and $c -eq 1) -or ($r -eq 1 -and $c -eq 40) -or ($r -eq 40 -and $c -eq 1) -or ($r -eq 40 -and $c -eq 40)) {
                $imgName = "wall-corner.png"
            } elseif ($r -eq 1 -or $r -eq 40 -or $c -eq 1 -or $c -eq 40) {
                $imgName = "wall-3.png"
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
                $null = $worksheet.Shapes.AddPicture($imgPath, $false, $true, $left, $top, $width, $height)
            }
        }
    }
}
$workbook.SaveAs("C:\Users\mcu\Downloads\Chronos\The_Jungle_Map_Updated.xlsx")
Write-Output "Done"
