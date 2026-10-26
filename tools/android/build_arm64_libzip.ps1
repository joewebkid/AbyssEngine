param(
    [Parameter(Mandatory = $true)][string]$NdkRoot,
    [Parameter(Mandatory = $true)][string]$BuildDir
)

$ErrorActionPreference = 'Stop'
$version = 'libzip-0.9.3'
$url = "https://libzip.org/download/$version.tar.gz"
$expectedSha256 = '0e282dfed9e3da7d52223a0ccaee7c950782f10ed73f9b778243128a03c8cb1c'
$archive = Join-Path $BuildDir "$version.tar.gz"
$source = Join-Path $BuildDir "$version\lib"
$objects = Join-Path $BuildDir 'obj'
$toolchain = Join-Path $NdkRoot 'toolchains\llvm\prebuilt\windows-x86_64\bin'
$clang = Join-Path $toolchain 'clang.exe'
$ar = Join-Path $toolchain 'llvm-ar.exe'
$output = Join-Path $BuildDir 'libzip_gof2_arm64.a'

New-Item -ItemType Directory -Force -Path $BuildDir, $objects | Out-Null
if (-not (Test-Path -LiteralPath $archive)) {
    & curl.exe -fL --retry 2 --connect-timeout 20 -o $archive $url
    if ($LASTEXITCODE -ne 0) { throw "Could not download $url" }
}
$actualSha256 = (Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant()
if ($actualSha256 -ne $expectedSha256) { throw "Unexpected libzip archive SHA-256: $actualSha256" }
if (-not (Test-Path -LiteralPath $source)) {
    & tar.exe -xzf $archive -C $BuildDir
    if ($LASTEXITCODE -ne 0) { throw 'Could not extract libzip source' }
}

# The 0.9.3 source needs an autoconf config.h. These features are supplied by
# Android API 28; this port uses libc's mkstemp instead of the ARMv7 fallback.
@'
#define HAVE_DLFCN_H 1
#define HAVE_FSEEKO 1
#define HAVE_FTELLO 1
#define HAVE_INTTYPES_H 1
#define HAVE_LIBZ 1
#define HAVE_MEMORY_H 1
#define HAVE_MKSTEMP 1
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1
#define STDC_HEADERS 1
#define PACKAGE "libzip"
#define VERSION "0.9.3"
#define PACKAGE_VERSION "0.9.3"
'@ | Set-Content -LiteralPath (Join-Path $source 'config.h') -Encoding ascii

$built = @()
foreach ($file in Get-ChildItem -LiteralPath $source -Filter 'zip_*.c' | Sort-Object Name) {
    $object = Join-Path $objects ($file.BaseName + '.o')
    & $clang '--target=aarch64-linux-android28' '-O2' '-fPIC' '-Wno-everything' "-I$source" '-c' $file.FullName '-o' $object
    if ($LASTEXITCODE -ne 0) { throw "Compilation failed: $($file.Name)" }
    $built += $object
}
if (Test-Path -LiteralPath $output) { Remove-Item -LiteralPath $output }
& $ar rcs $output $built
if ($LASTEXITCODE -ne 0) { throw 'Could not create ARM64 libzip archive' }

$testSource = Join-Path $PSScriptRoot 'test_arm64_libzip.c'
$testOutput = Join-Path $BuildDir 'test_arm64_libzip'
& $clang '--target=aarch64-linux-android28' '-O2' '-fPIE' '-pie' "-I$source" $testSource $output '-lz' '-o' $testOutput
if ($LASTEXITCODE -ne 0) { throw 'Could not create ARM64 libzip test executable' }
Write-Output "ARM64 libzip: $output"
Write-Output "ARM64 archive reader: $testOutput"
