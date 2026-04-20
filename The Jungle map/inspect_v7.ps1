$excel = New-Object -ComObject Excel.Application
$excel.Visible = $false
$wb = $excel.Workbooks.Open('C:\Users\mcu\Downloads\Chronos\The snek game\The_Jungle_Map_V7.xlsx')
$ws = $wb.Sheets.Item(1)

# Scan using .Text to detect actual map bounds
$maxRow = 0
$maxCol = 0
for ($r = 1; $r -le 60; $r++) {
    for ($c = 1; $c -le 60; $c++) {
        $v = $ws.Cells.Item($r, $c).Text.Trim()
        if ($v -ne "" -and $v -match '^\d+$') {
            if ($r -gt $maxRow) { $maxRow = $r }
            if ($c -gt $maxCol) { $maxCol = $c }
        }
    }
}

Write-Host "Map bounds: rows=$maxRow cols=$maxCol"

# Show first 3 rows
for ($r = 1; $r -le 3; $r++) {
    $rowStr = "Row " + $r + ": "
    for ($c = 1; $c -le [Math]::Min($maxCol, 15); $c++) {
        $v = $ws.Cells.Item($r, $c).Text.Trim()
        if ($v -eq "") { $v = "0" }
        $rowStr += $v + " "
    }
    Write-Host $rowStr
}

$wb.Close($false)
$excel.Quit()
