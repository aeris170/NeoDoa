# Enable detailed error output and logging
$VerbosePreference = "Continue"
$DebugPreference = "Continue"
$ErrorActionPreference = "Stop"  # Keep stop behavior to catch errors

# Check if clonemode argument is provided
if (-not $args[0]) {
    Write-Host "No clonemode supplied, defaulting to ssh" -ForegroundColor yellow -BackgroundColor black
    $clonemode = "ssh"
} else {
    $clonemode = $args[0]
}

# Check if vcpkg path argument is provided
if (-not $args[1]) {
    Write-Host "No vcpkg path supplied, defaulting to ./vcpkg" -ForegroundColor yellow -BackgroundColor black
    $path = "./vcpkg"
} else {
    $path = $args[1]
}

Write-Host "Clonemode: $clonemode" -ForegroundColor white -BackgroundColor black
Write-Host "vcpkg path: $path" -ForegroundColor white -BackgroundColor black

# Create the directory if it doesn't exist
if (-not (Test-Path -Path $path)) {
    Write-Host "Creating directory: $path" -ForegroundColor white -BackgroundColor black
    New-Item -Path $path -ItemType Directory | Out-Null
} else {
    Write-Host "Directory already exists: $path" -ForegroundColor yellow -BackgroundColor black
}

# Change directory to the vcpkg path
Write-Host "Changing to directory: $path" -ForegroundColor white -BackgroundColor black
Set-Location -Path $path

# Clone vcpkg using the provided clonemode
Write-Host "Cloning vcpkg..." -ForegroundColor white -BackgroundColor black
if ($clonemode -eq "ssh") {
    Start-Process -FilePath "git" -ArgumentList "clone git@github.com:microsoft/vcpkg.git ." `
        -NoNewWindow -Wait -PassThru | Tee-Object -Variable cloneOutput
} elseif ($clonemode -eq "https") {
    Start-Process -FilePath "git" -ArgumentList "clone https://github.com/Microsoft/vcpkg.git ." `
        -NoNewWindow -Wait -PassThru | Tee-Object -Variable cloneOutput
} else {
    Write-Host "Incorrect clonemode! Expected https or ssh, got something else" -ForegroundColor red -BackgroundColor black
	Set-Location -Path ..
    exit 1
}

# Check if clone was successful
if ($cloneOutput.ExitCode -ne 0) {
    Write-Host "Error during git clone: $($cloneOutput.StandardError)" -ForegroundColor red -BackgroundColor black
	Set-Location -Path ..
    exit 1
}

# Pull the latest changes
Write-Host "Pulling latest vcpkg changes..." -ForegroundColor white -BackgroundColor black
$pullOutput = Start-Process -FilePath "git" -ArgumentList "pull" -NoNewWindow -Wait -PassThru
if ($pullOutput.ExitCode -ne 0) {
    Write-Host "Error during git pull: $($pullOutput.StandardError)" -ForegroundColor red -BackgroundColor black
	Set-Location -Path ..
    exit 1
}

# Run the bootstrap script
if (Test-Path -Path "./bootstrap-vcpkg.bat") {
    Write-Host "Running bootstrap-vcpkg.bat..." -ForegroundColor white -BackgroundColor black
    $bootstrapOutput = Start-Process -FilePath "./bootstrap-vcpkg.bat" -NoNewWindow -Wait -PassThru
    if ($bootstrapOutput.ExitCode -ne 0) {
        Write-Host "Error during bootstrap: $($bootstrapOutput.StandardError)" -ForegroundColor red -BackgroundColor black
		Set-Location -Path ..
        exit 1
    }
} else {
    Write-Host "Bootstrap script not found!" -ForegroundColor red -BackgroundColor black
	Set-Location -Path ..
    exit 1
}

# Install required packages
Write-Host "Installing required vcpkg packages..." -ForegroundColor white -BackgroundColor black
$installOutput = Start-Process -FilePath "./vcpkg.exe" -ArgumentList @(
    'install',
    'angelscript[addons]',
    'argparse',
    'assimp',
    'cppzmq',
    'entt',
    'eventpp',
    'glew',
    'glfw3',
    'glm',
    'icu',
    'imgui[core,docking-experimental,glfw-binding,sdl2-binding,opengl3-binding,vulkan-binding]',
    'imguizmo',
    'lunasvg',
    'stb',
    'tinyxml2',
    '--recurse'
) -NoNewWindow -Wait -PassThru
if ($installOutput.ExitCode -ne 0) {
    Write-Host "Error during vcpkg install: $($installOutput.StandardError)" -ForegroundColor red -BackgroundColor black
	Set-Location -Path ..
    exit 1
}

# Integrate vcpkg
Write-Host "Integrating vcpkg..." -ForegroundColor white -BackgroundColor black
$integrateOutput = Start-Process -FilePath "./vcpkg.exe" -ArgumentList 'integrate', 'install' -NoNewWindow -Wait -PassThru
if ($integrateOutput.ExitCode -ne 0) {
    Write-Host "Error during vcpkg integrate: $($integrateOutput.StandardError)" -ForegroundColor red -BackgroundColor black
	Set-Location -Path ..
    exit 1
}

# Notify completion
Write-Host "vcpkg setup complete!" -ForegroundColor green -BackgroundColor black