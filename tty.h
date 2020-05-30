#pragma once

// Returns true iff the input stream is an interactive console (vs e.g. a piped stream or file)
bool input_is_tty();
