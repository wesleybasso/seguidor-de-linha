param(
    [string]$BuildDir = "build"
)

$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$Build = Join-Path $Root $BuildDir
New-Item -ItemType Directory -Force -Path $Build | Out-Null

$Cc = "arm-none-eabi-gcc"
$Objcopy = "arm-none-eabi-objcopy"
$Size = "arm-none-eabi-size"

$CFlags = @(
    "-std=c11",
    "-Wall",
    "-Wextra",
    "-Os",
    "-ffunction-sections",
    "-fdata-sections",
    "-mcpu=cortex-m4",
    "-mthumb",
    "-mfloat-abi=soft",
    "-DSTM32F411xE",
    "-DPEGASUS_TARGET_F411",
    "-ICore/Inc",
    "-IDrivers/CMSIS/Include",
    "-IDrivers/CMSIS/Device/ST/STM32F4xx/Include"
)

$LdFlags = @(
    "-mcpu=cortex-m4",
    "-mthumb",
    "-mfloat-abi=soft",
    "-TSTM32F411CEUX_FLASH.ld",
    "-Wl,--gc-sections",
    "-Wl,--no-warn-rwx-segments",
    "-Wl,-Map=$BuildDir/firmware.map"
)

$Objects = @()

Get-ChildItem -Path (Join-Path $Root "Core/Src") -Filter "*.c" | ForEach-Object {
    $Obj = Join-Path $Build ($_.BaseName + ".o")
    & $Cc @CFlags -c $_.FullName -o $Obj
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    $Objects += $Obj
}

$Startup = Join-Path $Root "Core/Startup/startup_stm32f411xe.s"
$StartupObj = Join-Path $Build "startup_stm32f411xe.o"
& $Cc @CFlags -c $Startup -o $StartupObj
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
$Objects += $StartupObj

Push-Location $Root
try {
    & $Cc @LdFlags @Objects -o "$BuildDir/firmware.elf"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & $Objcopy -O binary "$BuildDir/firmware.elf" "$BuildDir/firmware.bin"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & $Size "$BuildDir/firmware.elf"
} finally {
    Pop-Location
}
