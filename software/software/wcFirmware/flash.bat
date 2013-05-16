@echo off

if "%1"=="" goto noTarget

if NOT exist "%2.hex" goto noHexfile
echo on
avrdude.exe -p %1 -P usb -c usbtiny -B 1 -U flash:w:%2.hex
@echo off


if "%3"=="WE" goto skipedWritingEEPROM
if NOT exist "%2.eep" goto noEEPROMfile
echo on
avrdude.exe -p %1 -P usb -c usbtiny -B 1 -i 1000 -U eeprom:w:%2.eep:a
@echo off

goto ENDE




:noTarget
echo no target defined!
goto printUsage

:noHexfile
echo Hex file '%2.hex' does not exist!
goto printUsage

:noEEPROMfile
echo EEPROM file '%2.eep' does not exist!
goto printUsage

:skipedWritingEEPROM
echo Skipped wrting EEPROM!
goto ENDE

:printUsage
echo.
echo usage:   flash.bat  target  basefilename  [WE]
echo   target       - avrdude target string:  e.g. m88 or m168 
echo   basefilename - filename with path to hex and eep file without ending
echo                  e.g.:  for default\project1.[hex^|eep]: default\project1
echo   WE           - if specified the writing of eeprom will be omitted
echo.
echo.

goto ENDE

:ENDE  
pause