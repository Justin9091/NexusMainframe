$serviceName = "NexusCore"

Write-Host "Stopping $serviceName service..." -ForegroundColor Yellow
Stop-Service -Name $serviceName -ErrorAction SilentlyContinue

Write-Host "Removing $serviceName service..." -ForegroundColor Yellow
sc.exe delete $serviceName

Write-Host "$serviceName service uninstalled successfully." -ForegroundColor Green