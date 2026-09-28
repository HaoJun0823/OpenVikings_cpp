@echo off
setlocal

cd /d "%~dp0"

powershell -NoProfile -Command ^
"$cppFiles = Get-ChildItem -Path . -Recurse -File -Filter *.cpp; ^
$hFiles = Get-ChildItem -Path . -Recurse -File -Filter *.h; ^
$cppLines = 0; ^
$hLines = 0; ^
foreach ($file in $cppFiles) { ^
    $cppLines += (Get-Content -LiteralPath $file.FullName ^| Measure-Object -Line).Lines; ^
}; ^
foreach ($file in $hFiles) { ^
    $hLines += (Get-Content -LiteralPath $file.FullName ^| Measure-Object -Line).Lines; ^
}; ^
$totalFiles = $cppFiles.Count + $hFiles.Count; ^
$totalLines = $cppLines + $hLines; ^
$output = @(); ^
$output += ('CPP files:        {0}' -f $cppFiles.Count); ^
$output += ('CPP lines:        {0}' -f $cppLines); ^
$output += ''; ^
$output += ('H files:          {0}' -f $hFiles.Count); ^
$output += ('H lines:          {0}' -f $hLines); ^
$output += ''; ^
$output += '--------------------'; ^
$output += ('Total files:      {0}' -f $totalFiles); ^
$output += ('Total lines:      {0}' -f $totalLines); ^
$output ^| Set-Content -LiteralPath 'cound.txt' -Encoding UTF8"

echo Done. Results written to cound.txt
pause