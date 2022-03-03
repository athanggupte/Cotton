rem Run all tests
@echo off
set /A ret=0

setlocal EnableDelayedExpansion
for /f %%f in ('where /r bin tests*.exe') do (
    %%f || set /A ret=1
)

exit !ret!

endlocal