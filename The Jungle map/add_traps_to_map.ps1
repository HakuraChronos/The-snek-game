Add-Type -AssemblyName System.Drawing
$excel = New-Object -ComObject Excel.Application
$excel.Visible = $true
$excel.DisplayAlerts = $false
$workbook = $excel.Workbooks.Open("C:\Users\mcu\Downloads\Chronos\The snek game\The_Jungle_Map_V6.xlsx")
$worksheet = $workbook.Sheets.Item(1)

$imgDir = "C:\Users\mcu\Downloads\Chronos\The snek game\UI game picture (Jungle)\"

Write-Output "Scanning for cells with 9..."
$nines = @()
for ($r = 1; $r -le 40; $r++) {
    for ($c = 1; $c -le 40; $c++) {
        $cell = $worksheet.Cells.Item($r, $c)
        if ($cell.Text.Trim() -eq "9") {
            $nines += @{ Row = $r; Col = $c; Visited = $false; Cell = $cell }
        }
    }
}

Write-Output "Found $($nines.Count) cells with 9"

if ($nines.Count -gt 0) {
    # Group into connected components (BFS)
    $components = @()
    foreach ($nine in $nines) {
        if (-not $nine.Visited) {
            $comp = @()
            $queue = New-Object System.Collections.Queue
            $queue.Enqueue($nine)
            $nine.Visited = $true
            
            while ($queue.Count -gt 0) {
                $curr = $queue.Dequeue()
                $comp += $curr
                
                # Find neighbors (4-way)
                foreach ($other in $nines) {
                    if (-not $other.Visited) {
                        $rowDiff = [Math]::Abs($curr.Row - $other.Row)
                        $colDiff = [Math]::Abs($curr.Col - $other.Col)
                        if (($rowDiff -eq 1 -and $colDiff -eq 0) -or ($rowDiff -eq 0 -and $colDiff -eq 1)) {
                            $other.Visited = $true
                            $queue.Enqueue($other)
                        }
                    }
                }
            }
            $components += ,$comp
        }
    }

    Write-Output "Found $($components.Count) trap areas"

    # Apply traps
    foreach ($comp in $components) {
        # 0 = Plant + Thorny Bush, 1 = Spider + Thorny Bush
        $theme = Get-Random -Minimum 0 -Maximum 2
        Write-Output "Component with $($comp.Count) cells -> Theme $theme"
        
        foreach ($item in $comp) {
            $cell = $item.Cell
            $r = $item.Row
            $c = $item.Col
            
            $imgName = ""
            if ($theme -eq 0) {
                $imgName = @("trap-1.png", "trap-3.png") | Get-Random
            } else {
                $imgName = @("spider.png", "trap-3.png") | Get-Random
            }
            
            $cell.Value2 = ""
            $imgPath = Join-Path $imgDir $imgName
            if (Test-Path $imgPath) {
                $left = $cell.Left
                $top = $cell.Top
                $width = $cell.Width
                $height = $cell.Height
                
                $pic = $worksheet.Shapes.AddPicture($imgPath, $false, $true, $left, $top, $width, $height)
                $pic.LockAspectRatio = 0
                
                # Set a background color just in case (optional, we can set SaddleBrown)
                # Actually, wait, trap might look better on grass or dirt. We use SaddleBrown for consistency.
                $cell.Interior.Color = [System.Drawing.ColorTranslator]::ToOle([System.Drawing.Color]::SaddleBrown)

                $pic.Width = $width
                $pic.Height = $height
            }
        }
    }
}

$workbook.SaveAs("C:\Users\mcu\Downloads\Chronos\The snek game\The_Jungle_Map_V7.xlsx", 51)
Write-Output "Done creating V7"
