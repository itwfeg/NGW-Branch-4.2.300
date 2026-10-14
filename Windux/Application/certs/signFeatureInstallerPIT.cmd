@echo off

set FEATURE=%1%
set FEATURESIG=%1%.signed

set KEY=ProductIntegrationSigningCertKey.pem
set CRT=ProductIntegrationSigningCert.crt

:: -----------------------------------------------------------------------------

if EXIST %FEATURE% (
	echo signing %FEATURE%...
) ELSE (
	echo target %FEATURE% does not exist!
	goto done
)

.\bin\sigcerttool %FEATURE% %CRT% %KEY% authenticpit

if EXIST %FEATURESIG% (
	echo.
) ELSE (
	echo failed to sign feature!
	goto done
)

:done
