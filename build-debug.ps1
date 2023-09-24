Push-Location $(Split-Path -Parent -Path $MyInvocation.MyCommand.Definition)

try {
    ./run-build.ps1 build/debug Debug -DCZ_BUILD_TESTS=1
    if (!$?) { exit 1 }

    ./build/debug/cz-test.exe --colour-mode=none
    if (!$?) { exit 1 }
} finally {
    Pop-Location
}
