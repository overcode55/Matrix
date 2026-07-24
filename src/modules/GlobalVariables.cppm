module;

export module GlobalVariables;

import std.compat;
import String;


export inline thread_local char g_current_noexcept_state[256] =
    "Default state\n";

export inline thread_local char g_last_noexcept_state[256] =
    "Default state\n";

export inline thread_local char g_current_noexcept_possible_fail_reason[256] =
    "No reason\n";

export inline thread_local char g_last_noexcept_possible_fail_reason[256] =
    "No reason\n";

export inline void set(char (&var)[256], const char* value) noexcept {
    if (value == nullptr) {
        var[0] = '\0';
        return;
    }

    std::snprintf(var, 256, "%s", value);
}

export inline void set(char (&var)[256], const char (&value)[256]) noexcept {
    std::memcpy(var, value, 256);
    var[255] = '\0';
}