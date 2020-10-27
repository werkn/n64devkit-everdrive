#install chocolatey
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope LocalMachine
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

#install vscode
choco install vscode

#add remote container plugin
code --install-extension ms-vscode-remote.remote-containers
code --install-extension disk0.win-classic-theme

#install docker-desktop
choco install docker-desktop

#install github-desktop
choco install github-desktop