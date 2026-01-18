# Install Nexus Main Frame as Windows Service
# This script must be run as Administrator

$serviceName = "NexusService"
$displayName = "Nexus Main Frame Service"
$description = "Nexus Main Frame modular framework background service"

# Get the directory where this script is located
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$exePath = Join-Path $scriptDir "NexusMainFrame.exe"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Nexus Main Frame Service Installer" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if running as Administrator
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

if (-not $isAdmin) {
    Write-Host "ERROR: This script must be run as Administrator" -ForegroundColor Red
    Write-Host "Right-click PowerShell and select 'Run as Administrator'" -ForegroundColor Yellow
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

# Check if executable exists
if (-not (Test-Path $exePath)) {
    Write-Host "ERROR: NexusMainFrame.exe not found at: $exePath" -ForegroundColor Red
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

# Check if service already exists
$existingService = Get-Service -Name $serviceName -ErrorAction SilentlyContinue

if ($existingService) {
    Write-Host "Service '$serviceName' already exists." -ForegroundColor Yellow
    Write-Host ""

    # Stop the service if it's running
    if ($existingService.Status -eq 'Running') {
        Write-Host "Stopping existing service..." -ForegroundColor Yellow
        Stop-Service -Name $serviceName -Force -ErrorAction SilentlyContinue
        Start-Sleep -Seconds 2
    }

    # Remove the old service
    Write-Host "Removing old service..." -ForegroundColor Yellow
    sc.exe delete $serviceName | Out-Null
    Start-Sleep -Seconds 2
}

# Create the service
Write-Host "Installing service '$displayName'..." -ForegroundColor Green

try {
    New-Service -Name $serviceName `
                -BinaryPathName $exePath `
                -DisplayName $displayName `
                -Description $description `
                -StartupType Automatic `
                -ErrorAction Stop | Out-Null

    Write-Host "Service installed successfully!" -ForegroundColor Green
    Write-Host ""

    # Start the service
    Write-Host "Starting service..." -ForegroundColor Green
    Start-Service -Name $serviceName -ErrorAction Stop

    # Wait a moment for service to start
    Start-Sleep -Seconds 2

    # Check service status
    $service = Get-Service -Name $serviceName

    if ($service.Status -eq 'Running') {
        Write-Host "Service is now running!" -ForegroundColor Green
    } else {
        Write-Host "WARNING: Service installed but not running (Status: $($service.Status))" -ForegroundColor Yellow
        Write-Host "Check Windows Event Viewer for details" -ForegroundColor Yellow
    }

} catch {
    Write-Host "ERROR: Failed to install service" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Installation complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Service Name: $serviceName" -ForegroundColor White
Write-Host "Display Name: $displayName" -ForegroundColor White
Write-Host "Status: Running" -ForegroundColor Green
Write-Host ""
Write-Host "Useful commands:" -ForegroundColor Yellow
Write-Host "  View status:  Get-Service $serviceName" -ForegroundColor Gray
Write-Host "  Stop service: Stop-Service $serviceName" -ForegroundColor Gray
Write-Host "  Start service: Start-Service $serviceName" -ForegroundColor Gray
Write-Host "  View logs: Get-EventLog -LogName Application -Source NexusService -Newest 10" -ForegroundColor Gray
Write-Host ""
Read-Host "Press Enter to exit"