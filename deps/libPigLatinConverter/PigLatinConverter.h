#pragma once


// Includes --------------------------------------------------------------------
#include <string>
#include <memory>


// Types -----------------------------------------------------------------------
class PigLatinConverter
{
public:
    PigLatinConverter();
    ~PigLatinConverter();

    // Converts a string to pig latin. This is a blocking function, and may
    // take 0-10 seconds to complete. It assumes the string is in English and
    // space-delimited (it does not handle puncuation well).
    std::string convert(const std::string& text);

    // Cancels an in-progress conversion (if one exists). Should be called
    // by a different thread than convert(...).
    void cancel();

private:
    struct Private;
    std::unique_ptr<Private> _impl;
};
