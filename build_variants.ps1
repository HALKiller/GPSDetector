#Requires -Version 5.1
<#
.SYNOPSIS
Build all PCB / GPS baud / Release-Test combinations using MPLAB X makefiles.
.EXAMPLE
.\build_variants.ps1
.EXAMPLE
.\build_variants.ps1 -DryRun
.EXAMPLE
.\build_variants.ps1 -Configuration PIC16F1936_xc8_1_38_no_BL -Pcb 69 -Baud 9600 -Mode T
#>
[CmdletBinding()]
param(
    [string]$Configuration,
    [ValidateSet(66,67,69)][int[]]$Pcb = @(66,67,69),
    [ValidateSet(9600,115200)][int[]]$Baud = @(9600,115200),
    [ValidateSet('R','T')][string[]]$Mode = @('R','T'),
    [string]$MakePath,
    [switch]$DryRun
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$projectRoot = $PSScriptRoot

# Byte-preserving encoding: edits only touch ASCII macro values.
$encoding = [System.Text.Encoding]::GetEncoding(28591)
$header = $encoding.GetString([IO.File]::ReadAllBytes((Join-Path $projectRoot 'include/Global.h')))
function Set-Define([string]$Text, [string]$Name, [string]$Value) {
    $pattern = '(?m)^(\s*#\s*define[ \t]+' + [regex]::Escape($Name) + '[ \t]+)\S+'
    $regex = [regex]::new($pattern)
    if ($regex.Matches($Text).Count -ne 1) { throw "Expected exactly one definition of $Name in Global.h." }
    return $regex.Replace($Text, [System.Text.RegularExpressions.MatchEvaluator]{ param($m) $m.Groups[1].Value + $Value })
}
$versionMatch = [regex]::Matches($header, '(?m)^\s*#define[ \t]+FW_VERSION_STR[ \t]+"([A-Za-z0-9._-]+)"')
if ($versionMatch.Count -ne 1) { throw 'Expected one filename-safe FW_VERSION_STR in Global.h.' }
$version = $versionMatch[0].Groups[1].Value
if (!$Configuration) {
    $impl = [IO.File]::ReadAllText((Join-Path $projectRoot 'nbproject/Makefile-impl.mk'))
    $match = [regex]::Match($impl, '(?m)^DEFAULTCONF=([^\r\n]+)')
    if (!$match.Success) { throw 'Cannot determine active configuration. Supply -Configuration.' }
    $Configuration = $match.Groups[1].Value.Trim()
}
if ($Configuration -notmatch '^[A-Za-z0-9_]+$') { throw 'Invalid configuration name.' }
if (!(Test-Path -LiteralPath (Join-Path $projectRoot "nbproject/Makefile-$Configuration.mk"))) {
    throw "No generated makefile for $Configuration. Open/build that configuration in MPLAB X first."
}
$variants = @(foreach ($p in ($Pcb | Select-Object -Unique)) {
    foreach ($b in ($Baud | Select-Object -Unique)) {
        foreach ($m in ($Mode | Select-Object -Unique)) {
            $m = $m.ToUpperInvariant()
            $suffix = if ($b -eq 9600) { '9k6' } else { '115k2' }
            $text = Set-Define $header 'PCB_VERSION' "$p"
            $text = Set-Define $text 'BAUD_SETTING' "$b"
            $text = Set-Define $text 'COMPILE_FOR_RELEASE' ([string][int]($m -eq 'R'))
            $text = Set-Define $text 'COMPILE_FOR_TEST' ([string][int]($m -eq 'T'))
            $text = Set-Define $text 'COMPILE_FOR_INTERNAL_TEST' '0'
            $text = Set-Define $text 'COMPILE_FOR_DEBUG' '0'
            [pscustomobject]@{ Name="v${version}${m}${p}_$suffix"; Pcb=$p; Baud=$b; Mode=$m; Header=$text }
        }
    }
})
Write-Host "Configuration: $Configuration"
$variants | Select-Object Name,Pcb,Baud,Mode | Format-Table | Out-Host
if ($DryRun) { return }

if (!$MakePath) {
    $localFile = Join-Path $projectRoot "nbproject/Makefile-local-$Configuration.mk"
    if (Test-Path -LiteralPath $localFile) {
        $local = [IO.File]::ReadAllText($localFile)
        $ide = [regex]::Match($local, '(?m)^PATH_TO_IDE_BIN=(.+?MPLABX[/\\][^/\\]+)')
        if ($ide.Success) { $MakePath = Join-Path $ide.Groups[1].Value 'gnuBins/GnuWin32/bin/make.exe' }
    }
    if (!$MakePath -or !(Test-Path -LiteralPath $MakePath)) {
        throw 'Cannot locate MPLAB make.exe. Supply -MakePath with its full path, or rebuild in MPLAB X to refresh local paths.'
    }
}
$MakePath = (Resolve-Path -LiteralPath $MakePath).Path
$runId = (Get-Date -Format 'yyyyMMdd-HHmmss') + '-' + [guid]::NewGuid().ToString('N').Substring(0,8)
$output = Join-Path $projectRoot "dist/variants/$Configuration/$runId"
$workRoot = Join-Path $projectRoot "build/variants/$runId"
New-Item -ItemType Directory -Path $output,$workRoot -Force | Out-Null
$results = @()
$oldPath = $env:PATH
try {
    $env:PATH = (Split-Path $MakePath) + ';' + $oldPath
    foreach ($variant in $variants) {
        Write-Host "Building $($variant.Name)..."
        $work = Join-Path $workRoot $variant.Name
        $log = Join-Path $output ($variant.Name + '.log')
        try {
            New-Item -ItemType Directory -Path $work | Out-Null
            foreach ($folder in @('include','src','nbproject')) {
                Copy-Item -LiteralPath (Join-Path $projectRoot $folder) -Destination $work -Recurse
            }
            Get-ChildItem -LiteralPath $projectRoot -File | Where-Object {
                $_.Name -eq 'Makefile' -or $_.Extension -in @('.h','.inc','.as','.c')
            } | Copy-Item -Destination $work
            [IO.File]::WriteAllBytes((Join-Path $work 'include/Global.h'), $encoding.GetBytes($variant.Header))
            # Each variant starts with no object files or old HEX files.
            Push-Location $work
            try {
                # Native compiler diagnostics go to the per-variant log, including stderr.
                $savedPreference = $ErrorActionPreference
                $ErrorActionPreference = 'Continue'
                try {
                    & $MakePath '-f' 'Makefile' "CONF=$Configuration" 'TYPE_IMAGE=PRODUCTION' 'build' *> $log
                    $buildExit = $LASTEXITCODE
                } finally { $ErrorActionPreference = $savedPreference }
            } finally { Pop-Location }
            if ($buildExit -ne 0) { throw "make exited with code $buildExit. See $log" }
            $hexFiles = @(Get-ChildItem -Path (Join-Path $work "dist/$Configuration/production/*.hex") -File)
            if ($hexFiles.Count -ne 1 -or $hexFiles[0].Length -eq 0) { throw 'Expected exactly one nonempty production HEX file.' }
            $destination = Join-Path $output ($variant.Name + '.hex')
            Copy-Item -LiteralPath $hexFiles[0].FullName -Destination $destination
            $results += [pscustomobject]@{ Name=$variant.Name; Status='OK'; SHA256=(Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash; Error='' }
        } catch {
            Write-Warning "$($variant.Name): $($_.Exception.Message)"
            $results += [pscustomobject]@{ Name=$variant.Name; Status='FAILED'; SHA256=''; Error=$_.Exception.Message }
        }
        $results | Export-Csv -LiteralPath (Join-Path $output 'summary.csv') -NoTypeInformation
    }
} finally { $env:PATH = $oldPath }
$results | Select-Object Name,Status | Format-Table | Out-Host
Write-Host "Firmware, logs and summary: $output"
Write-Host "Source snapshots and intermediate files: $workRoot"
if (@($results | Where-Object Status -eq 'FAILED').Count) { throw 'Some variants failed. See summary.csv and the build logs.' }
