# piglatin

## Building & Running

On Linux & macOS, run `cmake` then `make` or `ninja` as usual.

On Windows, I found I had to specify a 32-bit build, and build Release, to match the library:
```shell script
cmake <path> -G "Visual Studio 16 2019" -A Win32
cmake --build . --config Release
```
