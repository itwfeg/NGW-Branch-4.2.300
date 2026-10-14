@echo off

set FEATURE=%1%
set FEATURESIG=%1%.signed

set KEY=HobartCodeSigningCertKey.pem
set CRT=HobartCodeSigningCert.crt

:: -----------------------------------------------------------------------------

if EXIST %FEATURE% (
	echo signing %FEATURE%...
) ELSE (
	echo target %FEATURE% does not exist!
	goto done
)

.\bin\sigcerttool %FEATURE% %CRT% %KEY% authentichobart

if EXIST %FEATURESIG% (
	echo.
) ELSE (
	echo failed to sign feature!
	goto done
)

:done
