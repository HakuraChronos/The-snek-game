$excel = New-Object -ComObject Excel.Application
$workbook = $excel.Workbooks.Open("C:\Users\mcu\Downloads\Chronos\The snek game\Snake-Game-main\Snake-Game-main\The-Jungle-Map.xlsx")
$worksheet = $workbook.Sheets.Item(1)
Write-Output ("Cell(1,1) text is: '" + $worksheet.Cells.Item(1,1).Text + "'")
$workbook.Close($false)
$excel.Quit()
