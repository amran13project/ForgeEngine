$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$Root = $PSScriptRoot
$ToolchainDir = Join-Path $Root 'Toolchain'
$CacheDir = Join-Path $Root 'Cache'
$BinDir = Join-Path $Root 'bin'
$SourceDir = Join-Path $Root 'src'
New-Item -ItemType Directory -Force -Path $ToolchainDir,$CacheDir,$BinDir | Out-Null

function Find-Clang {
    [array]$candidates = @(Get-ChildItem -Path $ToolchainDir -Filter 'clang++.exe' -File -Recurse -ErrorAction SilentlyContinue)
    foreach ($candidate in $candidates) {
        $candidatePath = [string]$candidate.FullName
        $candidateDir = [string]$candidate.Directory.FullName
        $parentDir = [string](Split-Path $candidateDir -Parent)
        $rootHeaderA = Join-Path $parentDir 'include\c++\v1'
        $rootHeaderB = Join-Path (Split-Path $parentDir -Parent) 'include\c++\v1'
        if ((Test-Path $rootHeaderA) -or (Test-Path $rootHeaderB)) {
            return $candidatePath
        }
    }
    return $null
}

$clang = Find-Clang
if (-not $clang) {
    $archiveName = 'llvm-mingw-20260922-ucrt-x86_64.zip'
    $archive = Join-Path $CacheDir $archiveName
    $url = "https://github.com/mstorsjo/llvm-mingw/releases/download/20260922/$archiveName"
    Write-Host "Downloading native Windows compiler: $archiveName"
    if (-not (Test-Path $archive) -or (Get-Item $archive).Length -lt 150MB) {
        if (Test-Path $archive) { Remove-Item $archive -Force }
        $partial = "$archive.part"
        if (Test-Path $partial) { Remove-Item $partial -Force }
        & curl.exe -L --fail --retry 4 --retry-delay 2 -o $partial $url
        if ($LASTEXITCODE -ne 0) { throw "Compiler download failed with exit code $LASTEXITCODE" }
        Move-Item $partial $archive -Force
    } else {
        Write-Host 'Compiler archive present. Reusing cached download.'
    }

    Write-Host 'Extracting native Windows compiler...'
    $tempExtract = Join-Path $ToolchainDir '_extract'
    if (Test-Path $tempExtract) { Remove-Item $tempExtract -Recurse -Force }
    New-Item -ItemType Directory -Force -Path $tempExtract | Out-Null
    & tar.exe -xf $archive -C $tempExtract
    if ($LASTEXITCODE -ne 0) { throw "Compiler extraction failed with exit code $LASTEXITCODE" }
    [array]$rootCandidates = @(Get-ChildItem $tempExtract -Directory -ErrorAction SilentlyContinue)
    $rootCandidate = $null
    foreach ($candidateDir in $rootCandidates) {
        $candidateClang = Join-Path $candidateDir.FullName 'bin\clang++.exe'
        if (Test-Path $candidateClang) {
            $rootCandidate = $candidateDir
            break
        }
    }
    if ($null -eq $rootCandidate -and $rootCandidates.Count -gt 0) { $rootCandidate = $rootCandidates[0] }
    if ($null -eq $rootCandidate) { throw 'Compiler archive did not expose a root directory.' }
    $installRoot = Join-Path $ToolchainDir $rootCandidate.Name
    if (Test-Path $installRoot) { Remove-Item $installRoot -Recurse -Force }
    Move-Item $rootCandidate.FullName $installRoot
    Remove-Item $tempExtract -Recurse -Force
    $clang = Find-Clang
}

if (-not $clang) { throw 'clang++.exe was not found in the portable toolchain.' }
$clangDir = Split-Path $clang -Parent
$compilerRoot = Split-Path $clangDir -Parent
$headers = Join-Path $compilerRoot 'include\c++\v1'
if (-not (Test-Path $headers -PathType Container)) {
    $headers = Join-Path (Split-Path $compilerRoot -Parent) 'include\c++\v1'
}
if (-not (Test-Path $headers -PathType Container)) { throw 'C++ standard library headers were not found.' }
if (-not (Test-Path (Join-Path $headers 'algorithm')) -or -not (Test-Path (Join-Path $headers 'filesystem'))) {
    throw "C++ standard library headers are incomplete: $headers"
}

Write-Host "Compiler root: $compilerRoot"
Write-Host "C++ headers:   $headers"
Write-Host 'Validating native Windows C++ toolchain...'

$probe = Join-Path $CacheDir 'toolchain_probe.cpp'
$probeExe = Join-Path $CacheDir 'toolchain_probe.exe'
@'
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <iostream>
int main(){ std::cout << std::clamp(7,0,5) << " " << sizeof(std::uint64_t) << " " << std::filesystem::path(".").string(); return 0; }
'@ | Set-Content -Path $probe -Encoding ASCII
$probeArgs = @(
    '-std=c++20','-O0',
    ('-I' + $headers),
    $probe,
    '-o',$probeExe,
    '-mconsole',
    '-Wl,--subsystem,console',
    '-Wl,--entry=mainCRTStartup',
    '-lkernel32','-luser32'
)
& $clang @probeArgs
if ($LASTEXITCODE -ne 0 -or -not (Test-Path $probeExe)) { throw 'Portable compiler self-test failed.' }

function Compile-Target([string]$name, [string]$mainFile, [string[]]$extraSources) {
    $out = Join-Path $BinDir ($name + '.exe')
    $sources = @((Join-Path $SourceDir $mainFile))
    foreach ($s in $extraSources) { $sources += Join-Path $SourceDir $s }
    Write-Host "Compiling $name..."
    $args = @('-std=c++20','-O2','-DNOMINMAX','-DWIN32_LEAN_AND_MEAN',('-I' + $SourceDir),('-I' + $headers))
    $args += $sources
    $args += @('-o',$out,'-mconsole','-Wl,--subsystem,console','-Wl,--entry=mainCRTStartup','-luser32','-lgdi32','-lshell32','-lole32','-lws2_32','-lkernel32')
    & $clang @args
    if ($LASTEXITCODE -ne 0 -or -not (Test-Path $out)) { throw "$name compilation failed with exit code $LASTEXITCODE" }
    return $out
}

$core = @(
'core\ForgeSystem.cpp','project\ProjectManager.cpp','scene\Scene.cpp','assets\AssetDatabase.cpp',
'physics\Physics.cpp','ai\AI.cpp','scripting\VisualScript.cpp','audio\Audio.cpp','network\NetworkLab.cpp',
'profiler\Profiler.cpp','build\BuildCenter.cpp','doctor\ProjectDoctor.cpp','recovery\Recovery.cpp',
'plugins\PluginManager.cpp','testing\TestingCenter.cpp','world\WorldGenerator.cpp',
'account\AccountService.cpp','ai_platform\ForgeAI.cpp','publish\PublishCenter.cpp',
'compliance\ComplianceChecker.cpp','localization\LocalizationManager.cpp',
'accessibility\AccessibilitySettings.cpp','security\SecurityCenter.cpp',
'release\ReleaseManager.cpp','workspace\WorkspaceProfile.cpp',
'platform\NativeWindow.cpp','renderer\SoftwareRenderer.cpp'
)
$editor = @($core + @('editor\ForgeEditor.cpp'))
$runtime = @($core + @('runtime\RuntimeApp.cpp'))

[void](Compile-Target 'ForgeEngine' 'main.cpp' $editor)
[void](Compile-Target 'ForgeRuntime' 'runtime_main.cpp' $runtime)

function Bundle-RuntimeDlls {
    $runtimeDir = Join-Path $compilerRoot 'bin'
    $dlls = @('libc++.dll','libc++abi.dll','libunwind.dll','libwinpthread-1.dll','libgcc_s_seh-1.dll','libatomic-1.dll','libstdc++-6.dll')
    $copied = 0
    foreach ($dll in $dlls) {
        $src = Join-Path $runtimeDir $dll
        if (Test-Path $src -PathType Leaf) { Copy-Item $src (Join-Path $BinDir $dll) -Force; $copied++ }
    }
    Write-Host "Bundled $copied portable runtime DLL(s)."
}

Bundle-RuntimeDlls

Remove-Item $probe,$probeExe -Force -ErrorAction SilentlyContinue
Write-Host ''
Write-Host 'Forge Engine Professional build succeeded.'
Write-Host "Editor : $BinDir\ForgeEngine.exe"
Write-Host "Runtime: $BinDir\ForgeRuntime.exe"
