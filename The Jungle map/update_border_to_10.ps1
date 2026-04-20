$excel = New-Object -ComObject Excel.Application
$excel.Visible = $false
$filePath = "c:\Users\mcu\Downloads\Chronos\The snek game\The_Jungle_Map_V7.xlsx"
$workbook = $excel.Workbooks.Open($filePath)
$worksheet = $workbook.Sheets.Item(1)

# Using 40x40 based on standard map size
$rows = 40
$cols = 40

Write-Host "Updating border cells in $filePath to 10..."

for ($r = 1; $r -le $rows; $r++) {
    for ($c = 1; $c -le $cols; $c++) {
        # Check if it's a border cell
        if ($r -eq 1 -or $r -eq $rows -or $c -eq 1 -or $c -eq $cols) {
            $cell = $worksheet.Cells.Item($r, $c)
            # Only change it if it's currently a wall (1) or a bush (1)
            # Or if the user wants ALL border cells to be 10, we'll just set it.
            # The request: "chỉnh toàn bộ số cho bức tường ngoài cùng của map thành 10"
            $cell.Value2 = 10
        }
    }
}

$workbook.Save()
$workbook.Close()
$excel.Quit()
Write-Host "Done."
