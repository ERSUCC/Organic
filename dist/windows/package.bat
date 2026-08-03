@echo off

set version=0.1.0

cmake -B build -DORGANIC_VERSION=%version% || exit /b 1
cmake --build build --config Release || exit /b 1
cmake --install build --config Release --prefix install || exit /b 1

for /f "tokens=* USEBACKQ" %%x in (`powershell -Command "[guid]::NewGuid()"`) do (
  set guid=%%x
)

wix build -src dist\windows\Organic.wxs -arch x64 -pdbtype none -bindpath install -out "install\Organic %version%.msi" ^
          -define name=Organic -define version=%version% -define guid=%guid% -define root="%cd%\install" || exit /b 1
