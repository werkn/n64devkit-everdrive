$header = @'

------------------------------------------------------
 _   _   ____    ___  ______           _   ___ _   
| \ | | / ___|  /   | |  _  \         | | / (_) |  
|  \| |/ /___  / /| | | | | |_____   _| |/ / _| |_ 
| . ` || ___ \/ /_| | | | | / _ \ \ / /    \| | __|
| |\  || \_/ |\___  | | |/ /  __/\ V /| |\  \ | |_ 
\_| \_/\_____/    |_/ |___/ \___| \_/ \_| \_/_|\__|

https://www.werkncode.io 

N64DevKit is an amalgamation of Libdragon and
a build/setup script (this script to be precise) that 
automatically sets up a local development environment 
for creating N64 homebrew using Libdragon and Windows 10.

The hope is that by reducing the complexity of setting up 
the development environment more people will attempt to create 
homebrew N64...  and when more people attempt to create
homebrew N64 everyone wins :)

 
On a clean install of Windows 10 this script is the only
thing that needs to be run to have a 100% fully setup
development environment.  Excluding having installed VSCode and Docker-Desktop
------------------------------------------------------

'@

Write-Host -ForegroundColor Green $header
Write-Host -ForegroundColor Yellow "Press <ENTER> to setup N64 DevKit, <Ctrl+C> to quit."
Read-Host  
Write-Host ""                                                 

Write-Host -ForegroundColor Green "Configuring ExecutionPolicy for Powershell to RemoteSigned, allowing local unsigned execution."
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope LocalMachine

#check for Docker
try
{
	docker-compose --version | Out-Null
}
catch [System.Management.Automation.CommandNotFoundException]
{
    Write-Host -ForegroundColor Magenta "Docker-compose is required to continue..."  
	Write-Host -ForegroundColor Magenta "Install Docker-Desktop for Windows and then try running this script again."
	Break
}

#build / download docker image, stand up container, and share out ./homebrew
Write-Host -ForegroundColor Green "Setting up Docker development environment..."
Write-Host ""
Write-Host -ForegroundColor Yellow "If prompted to share ./homebrew folder, select yes/allow."
Write-Host ""

docker-compose up -d

Write-Host -ForegroundColor Green "Configuring paths..."

$binPath=Join-Path -Path $(Get-Location) -ChildPath "\bin"
$homebrewPath=Join-Path -Path $(Get-Location) -Childpath "\homebrew"
$everdrivePath=Join-Path -Path $binPath -ChildPath "\everdrive"

Write-Host ""

#create a new Profile.ps1 for the current user, all hosts
#this is essentially .bashrc for windows
if (!(Test-Path -Path $PROFILE.CurrentUserAllHosts)) {
	Write-Host -ForegroundColor Green "Creating profile.ps1 for current user..."
	New-Item -ItemType File -Path $PROFILE.CurrentUserAllHosts -Force
} else {
	Write-Host -ForegroundColor Magenta "profile.ps1 already exists... Not creating."
}

Write-Host -ForegroundColor Green "Configuring profile.ps1 and command aliases..."

#add ed64 as command aliases to make deploying easier 
$everdriveAlias="function ed64 { & ""$everdrivePath\usb64.exe"" @(""-rom=$homebrewPath\deploy.z64"", ""-start"") }" 

Add-Content $PROFILE.CurrentUserAllHosts $everdriveAlias

Add-Content $Profile.CurrentUserAllHosts "Write-Host -ForegroundColor Magenta ""N64 Devkit Aliases Loaded"" "
Add-Content $Profile.CurrentUserAllHosts "Write-Host -ForegroundColor Magenta ""Commands available:"" "
Add-Content $Profile.CurrentUserAllHosts "Write-Host -ForegroundColor Yellow "" - ed64: Run ed64 deployment using latest Libdragon Built ROM  (deploy.z64) "" "

Write-Host -ForegroundColor Green "Setup should now be complete."
Write-Host -ForegroundColor Yellow "Press <ENTER> to close this window..."
Read-Host