param(
    [Parameter(Mandatory = $true)][string]$NdkRoot,
    [Parameter(Mandatory = $true)][string]$BuildDir,
    [Parameter(Mandatory = $true)][string]$Perl,
    [Parameter(Mandatory = $true)][string]$Make
)

$ErrorActionPreference = 'Stop'
$version = 'openssl-1.0.2u'
$url = "https://www.openssl.org/source/old/1.0.2/$version.tar.gz"
$expectedSha256 = 'ecd0c6ffb493dd06707d38b14bb4d8c2288bb7033735606569d8f90f89669d16'
$archive = Join-Path $BuildDir "$version.tar.gz"
$source = Join-Path $BuildDir $version
$objects = Join-Path $BuildDir 'obj'
$toolchain = Join-Path $NdkRoot 'toolchains\llvm\prebuilt\windows-x86_64\bin'
$clang = Join-Path $toolchain 'clang.exe'
$ar = Join-Path $toolchain 'llvm-ar.exe'
$output = Join-Path $BuildDir 'libcrypto_gof2_arm64.a'

New-Item -ItemType Directory -Force -Path $BuildDir, $objects | Out-Null
if (-not (Test-Path -LiteralPath $archive)) {
    & curl.exe -fL --retry 2 --connect-timeout 20 -o $archive $url
    if ($LASTEXITCODE -ne 0) { throw "Could not download $url" }
}
$actualSha256 = (Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant()
if ($actualSha256 -ne $expectedSha256) { throw "Unexpected OpenSSL archive SHA-256: $actualSha256" }
if (-not (Test-Path -LiteralPath $source)) {
    & tar.exe -xzf $archive -C $BuildDir
    if ($LASTEXITCODE -ne 0) { throw 'Could not extract OpenSSL source' }
}

# Generate 1.0.2u's own configuration and public header links. The old
# Configure script may try an optional `make depend`; `links` is run below.
$savedPath = $env:PATH
$env:PATH = (Split-Path -Parent $Perl) + ';' + (Split-Path -Parent $Make) + ';' + $savedPath
Push-Location $source
try {
    & $Perl Configure android64-aarch64 no-asm no-shared *> (Join-Path $BuildDir 'configure.log')
    if ($LASTEXITCODE -ne 0) { throw 'OpenSSL Configure failed' }
    & $Make links *> (Join-Path $BuildDir 'links.log')
    if ($LASTEXITCODE -ne 0) { throw 'OpenSSL header links failed' }
}
finally {
    Pop-Location
    $env:PATH = $savedPath
}
if (-not (Test-Path -LiteralPath (Join-Path $source 'include\openssl\sha.h'))) {
    throw 'OpenSSL SHA header was not generated'
}

$includes = @("-I$(Join-Path $source 'include')", "-I$(Join-Path $source 'crypto')")
$compile = @(
    @('crypto\sha\sha256.c', 'sha256.o'),
    @('crypto\mem_clr.c', 'mem_clr.o')
)
$built = @()
foreach ($item in $compile) {
    $object = Join-Path $objects $item[1]
    & $clang '--target=aarch64-linux-android28' '-O2' '-fPIC' '-Wno-deprecated-non-prototype' $includes '-c' (Join-Path $source $item[0]) '-o' $object
    if ($LASTEXITCODE -ne 0) { throw "Compilation failed: $($item[0])" }
    $built += $object
}
$memcmp = Join-Path $PSScriptRoot '..\..\third_party\openssl\crypto_memcmp.c'
$memcmpObject = Join-Path $objects 'crypto_memcmp.o'
& $clang '--target=aarch64-linux-android28' '-O2' '-fPIC' '-c' $memcmp '-o' $memcmpObject
if ($LASTEXITCODE -ne 0) { throw 'CRYPTO_memcmp compilation failed' }
$built += $memcmpObject

if (Test-Path -LiteralPath $output) { Remove-Item -LiteralPath $output }
& $ar rcs $output $built
if ($LASTEXITCODE -ne 0) { throw 'Could not create ARM64 crypto archive' }

$testSource = Join-Path $PSScriptRoot 'test_arm64_sha256.c'
$testOutput = Join-Path $BuildDir 'test_arm64_sha256'
& $clang '--target=aarch64-linux-android28' '-std=c11' '-O2' '-fPIE' '-pie' $includes $testSource $output '-o' $testOutput
if ($LASTEXITCODE -ne 0) { throw 'Could not create ARM64 SHA-256 test executable' }
Write-Output "ARM64 crypto: $output"
Write-Output "ARM64 SHA-256 test: $testOutput"
