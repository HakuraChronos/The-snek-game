Add-Type -AssemblyName System.IO.Compression.FileSystem

$filePath = "C:\Users\mcu\Downloads\Chronos\The snek game\Snake-Game-main\Snake-Game-main\The-Jungle-Map.xlsx"
$zip = [System.IO.Compression.ZipFile]::OpenRead($filePath)

$entry = $zip.GetEntry("xl/worksheets/sheet1.xml")
$reader = New-Object System.IO.StreamReader($entry.Open())
$xml = $reader.ReadToEnd()
$reader.Close()

$ssEntry = $zip.GetEntry("xl/sharedStrings.xml")
$sharedStrings = @()
if ($ssEntry -ne $null) {
    $ssReader = New-Object System.IO.StreamReader($ssEntry.Open())
    $ssXml = [xml]$ssReader.ReadToEnd()
    $ssReader.Close()
    foreach ($si in $ssXml.sst.si) {
        $t = $si.t
        if ($t -eq $null) { $t = ($si.r | ForEach-Object { $_.t }) -join "" }
        $sharedStrings += $t
    }
}
$zip.Dispose()

$sheetXml = [xml]$xml
$data = @{}
$maxR = 0
$maxC = 0

foreach ($row in $sheetXml.worksheet.sheetData.row) {
    foreach ($c in $row.c) {
        $ref = $c.r
        if ($ref -match "^([A-Z]+)(\d+)$") {
            $colStr = $Matches[1]
            $rowNum = [int]$Matches[2]
            $colNum = 0
            foreach ($ch in $colStr.ToCharArray()) {
                $colNum = $colNum * 26 + ([int][char]$ch - [int][char]'A' + 1)
            }
            $val = $c.v
            if ($c.t -eq "s" -and $val -ne $null) { $val = $sharedStrings[[int]$val] }
            if ($val -eq $null) { $val = "" }
            $data["$rowNum,$colNum"] = $val.Trim()
            if ($rowNum -gt $maxR) { $maxR = $rowNum }
            if ($colNum -gt $maxC) { $maxC = $colNum }
        }
    }
}

Write-Host "The-Jungle-Map.xlsx: Total cells=$($data.Count), Rows=$maxR, Cols=$maxC"

Write-Host "=== First 3 rows ==="
for ($r = 1; $r -le 3; $r++) {
    $rowStr = "R$r`: "
    for ($c = 1; $c -le 15; $c++) {
        $key = "$r,$c"
        $v = if ($data.ContainsKey($key)) { $data[$key] } else { "." }
        $rowStr += $v.PadLeft(2) + " "
    }
    Write-Host $rowStr
}

Write-Host "=== Value summary ==="
$valueCounts = @{}
foreach ($kv in $data.GetEnumerator()) {
    if ($kv.Key -match "^(\d+),(\d+)$") {
        $r = [int]$Matches[1]; $c = [int]$Matches[2]
        if ($r -ge 1 -and $r -le $maxR -and $c -ge 1 -and $c -le $maxC) {
            $v = $kv.Value
            if (-not $valueCounts.ContainsKey($v)) { $valueCounts[$v] = 0 }
            $valueCounts[$v]++
        }
    }
}
$valueCounts.GetEnumerator() | Sort-Object Name | ForEach-Object {
    Write-Host "  '$($_.Name)': $($_.Value) cells"
}
