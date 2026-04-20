$csvPath = "c:\Users\mcu\Downloads\Chronos\The snek game\Snake-Game-main\Snake-Game-main\The-Jungle-Map.csv"
$lines = Get-Content $csvPath
$lines37 = $lines[36].Split(',')

# Swap or move door/key
# User says Door (6) is where Key (5) was. 
# My CSV has 6 at Col 5 (index 4).
# In design, Key (5) is at Col 5, Door (6) is at Col 33 (index 32).

$lines37[4] = "5"   # Set Col 5 to Key
$lines37[32] = "6"  # Set Col 33 to Door

$lines[36] = $lines37 -join ","
$lines | Out-File $csvPath -Encoding utf8 -Force
Write-Host "Updated Door/Key positions in CSV."
