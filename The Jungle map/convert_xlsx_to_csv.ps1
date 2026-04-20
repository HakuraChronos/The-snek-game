$excel = New-Object -ComObject Excel.Application
$excel.Visible = $false
$infile = "c:\Users\mcu\Downloads\Chronos\The snek game\The_Jungle_Map_V7.xlsx"
$outfile = "c:\Users\mcu\Downloads\Chronos\The snek game\Snake-Game-main\Snake-Game-main\The-Jungle-Map.csv"

Write-Host "Converting 40x40 grid from $infile to $outfile..."

$workbook = $excel.Workbooks.Open($infile)
$worksheet = $workbook.Sheets.Item(1)

$stream = New-Object System.IO.StreamWriter($outfile, $false, (New-Object System.Text.UTF8Encoding($false)))

for ($r = 1; $r -le 40; $r++) {
    $rowData = @()
    for ($c = 1; $c -le 40; $c++) {
        $val = $worksheet.Cells.Item($r, $c).Value2
        if ($val -eq $null -or $val -eq "") {
            $rowData += "0"
        } else {
            $rowData += [string]$val
        }
    }
    $stream.WriteLine(($rowData -join ","))
}

$stream.Close()
$workbook.Close($false)
$excel.Quit()
Write-Host "Done."
