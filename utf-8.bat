@echo off
chcp 65001 >nul
echo ========================================================
echo 执行 [转化文件格式为 UTF-8]
echo ========================================================

powershell -NoProfile -ExecutionPolicy Bypass -Command "$utf8NoBom = New-Object System.Text.UTF8Encoding $false; Get-ChildItem -Path . -Recurse | Where-Object { $_.Extension -match '^\.(cpp|h|c|hpp|cc)$' } | ForEach-Object { $path = $_.FullName; $bytes = [System.IO.File]::ReadAllBytes($path); if ($bytes.Length -ge 3 -and $bytes[0] -eq 239 -and $bytes[1] -eq 187 -and $bytes[2] -eq 191) { $text = [System.IO.File]::ReadAllText($path); [System.IO.File]::WriteAllText($path, $text, $utf8NoBom); Write-Output \"✂️ [成功剥离BOM]: $path\" } elseif ([System.Text.Encoding]::UTF8.GetString($bytes).Contains([char]65533)) { $text = [System.IO.File]::ReadAllText($path, [System.Text.Encoding]::Default); [System.IO.File]::WriteAllText($path, $text, $utf8NoBom); Write-Output \"🔄 [GBK转UTF8]: $path\" } else { Write-Output \"✅ [UTF-8转化完成]: $path\" } }"

echo.
echo ========================================================
echo [转化文件格式为 UTF-8] 执行完毕
echo ========================================================
pause
