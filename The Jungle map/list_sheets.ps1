$excel = New-Object -ComObject Excel.Application
$excel.Visible = $false
$wb = $excel.Workbooks.Open('C:\Users\mcu\Downloads\Chronos\The snek game\The_Jungle_Map_V7.xlsx')
foreach ($ws in $wb.Sheets) {
    Write-Host "Sheet: $($ws.Name)"
}
$wb.Close($false)
$excel.Quit()
