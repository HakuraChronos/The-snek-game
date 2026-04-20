param($path)
$excel = New-Object -ComObject Excel.Application
$excel.Visible = $false
$wb = $excel.Workbooks.Open($path)
$ws = $wb.Sheets.Item(1)
$found = $false
for ($r = 1; $r -le 40; $r++) {
    for ($c = 1; $c -le 40; $c++) {
        $v = $ws.Cells.Item($r, $c).Value2
        if ($v -ne $null -and $v -ne 0 -and $v -ne "") {
            Write-Host "Found value '$v' at row $r, column $c"
            $found = $true
        }
    }
}
if (-not $found) {
    Write-Host "No non-zero values found in the 40x40 grid of $path."
}
$wb.Close($false)
$excel.Quit()
