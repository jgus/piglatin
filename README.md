# piglatin

## Building

On Linux & macOS, run `cmake` then `make` or `ninja` as usual.

On Windows, I found I had to specify a 32-bit build, and build Release, to match the library:
```shell script
cmake <path> -G "Visual Studio 16 2019" -A Win32
cmake --build . --config Release
```

## Design Notes

The problem description specified "Once processing is complete, the result is logged to a file." However, I decided against any explicit file operations in the code, instead opting for the typical Unix design and the standard streams: `in` for input, `out` for results, and `err` for non-result communication with the user. This avoids unnecessary complication in the code (including handing platform-specific nuances in file and path handling, configuration, etc.) while still trivially allowing the user to log to a file via piping.

The problem description also refers to requesting input from the user; I interpreted this to mean a prompt on `err` for each input, though arguably that's rather verbose, and a little odd given the asynchronous nature of the output.

Allowing for the possibility of input streams that are piped rather than interactive, I opted to omit the prompt when the input is not interactive, as well as treating the various "quit" keywords as standard input (since end-of-stream is a more meaningful end-of-input marker in non-interactive sessions.)

Most of the implementation is fairly trivial, with the most interesting part being the implementation of the `async_converter` class, which encapsulates an instance of `PigLatinConverter` along with a worker thread, and all the logic needed to interact with that worker thread. A description of the threading logic in that class can be found at the top of `async_converter.cc`.

The documentation for `PigLatinConverter` states that it doesn't handle punctuation well (you have a typo there, BTW) and so potentially it might be appropriate to filter input before giving it to `PigLatinConverter`, and/or handle any errors it throws. However, the documentation doesn't give any information on errors, and in my testing it didn't throw on any inputs I gave it, so I don't see any errors to handle. As for filtering, while output was a little weird in the presence of punctuation, numbers, etc., it seemed like a reasonable best-effort, so I opted to err on the side of simplicity, and not to filter any user input, apart from the quit keywords. (Also, arguably, to _really_ do any input filtering right, one would have to get into locale-correct string handling, which would really add complexity.)

I did find a defect in the API design of `PigLatinConverter`, which is that there's no way for a caller to know whether the result returned from `PigLatinConverter::convert()` is a valid result (from a successful operation), or whether it's garbage (from an interrupted one.) (Ok, in practice, the result returned was _always_ correct in my testing, as though the method performed the operation immediately, then added an interruptible delay after that - but in theory, the result returned from an interrupted operation _should_ be garbage and not a correct result.) I worked around this by having `async_converter` explicitly remember when it has canceled the converter, and check that knowledge for every value returned to the worker thread. However, the design would be cleaner (and probably less error-prone) if `PigLatinConverter::convert()` were to throw some appropriate exception, return some special value, or otherwise specifically indicate that it's returning as a result of cancellation.
