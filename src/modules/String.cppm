module;

export module String;

import std.compat;

export class string_view; // forward declaration — full definition comes after `string` below,
                           // so `string`'s string_view constructors are declared here and
                           // defined out-of-line once string_view is complete.

export class string {
private:
    struct HeapData {
        char* data;
        size_t capacity;
    };

    union {
        HeapData heap_string;
        char stack_string[16];
    } m_value;

    size_t m_size;

    static size_t next_pow2(size_t n) {
        size_t capacity = 16;
        while (capacity < n) capacity <<= 1;
        return capacity;
    }

    static size_t c_strlen(const char* s) {
        size_t n = 0;
        while (s[n] != '\0') n++;
        return n;
    }

    bool is_heap() const { return m_size > 15; }

    char* raw() { return is_heap() ? m_value.heap_string.data : m_value.stack_string; }
    const char* raw() const { return is_heap() ? m_value.heap_string.data : m_value.stack_string; }

    // Frees the current heap buffer (if any) and makes *this hold `str` (length `len`),
    // allocating on the heap if needed. Safe to call from a freshly-constructed object
    // (m_size == 0, not heap) since no delete happens in that case.
    void assign(const char* str, size_t len) {
        if (is_heap()) {
            delete[] m_value.heap_string.data;
        }

        if (len > 15) {
            size_t cap = next_pow2(len);
            m_value.heap_string.data = new char[cap + 1];
            m_value.heap_string.capacity = cap;
            if (str) std::memcpy(m_value.heap_string.data, str, len);
            m_value.heap_string.data[len] = '\0';
        } else {
            if (str) std::memcpy(m_value.stack_string, str, len);
            m_value.stack_string[len] = '\0';
        }
        m_size = len;
    }

public:
    static constexpr size_t npos = static_cast<size_t>(-1);

    // ---------- constructors ----------

    string() : m_value{}, m_size(0) {
        m_value.stack_string[0] = '\0';
    }

    string(const char* raw_string) : m_value{}, m_size(0) {
        assign(raw_string, raw_string ? c_strlen(raw_string) : 0);
    }

    string(const char* raw_string, size_t len) : m_value{}, m_size(0) {
        assign(raw_string, len);
    }

    string(const std::string& raw_string) : m_value{}, m_size(0) {
        assign(raw_string.c_str(), raw_string.size());
    }

    string(const string& other) : m_value{}, m_size(0) {
        assign(other.raw(), other.m_size);
    }

    // Defined out-of-line below, after string_view is a complete type.
    string(std::string_view sv);
    string(string_view sv);

    string(string&& other) noexcept : m_value{}, m_size(other.m_size) {
        if (other.is_heap()) {
            m_value.heap_string = other.m_value.heap_string;
        } else {
            std::memcpy(m_value.stack_string, other.m_value.stack_string, other.m_size + 1);
        }
        other.m_size = 0;
        other.m_value.stack_string[0] = '\0';
    }

    // ---------- destructor ----------

    ~string() {
        if (is_heap()) {
            delete[] m_value.heap_string.data;
        }
    }

    // ---------- assignment ----------

    string& operator=(const string& other) {
        if (this == &other) return *this;
        assign(other.raw(), other.m_size);
        return *this;
    }

    string& operator=(string&& other) noexcept {
        if (this == &other) return *this;

        if (is_heap()) {
            delete[] m_value.heap_string.data;
        }

        m_size = other.m_size;
        if (other.is_heap()) {
            m_value.heap_string = other.m_value.heap_string;
        } else {
            std::memcpy(m_value.stack_string, other.m_value.stack_string, other.m_size + 1);
        }

        other.m_size = 0;
        other.m_value.stack_string[0] = '\0';

        return *this;
    }

    string& operator=(const char* str) {
        assign(str, str ? c_strlen(str) : 0);
        return *this;
    }

    string& operator=(const std::string& str) {
        assign(str.c_str(), str.size());
        return *this;
    }

    // Defined out-of-line below, after string_view is a complete type.
    string& operator=(std::string_view sv);
    string& operator=(string_view sv);

    // ---------- element access ----------

    char& operator[](size_t index) { return raw()[index]; }
    const char& operator[](size_t index) const { return raw()[index]; }

    char& at(size_t index) {
        if (index >= m_size) throw std::out_of_range("string::at");
        return raw()[index];
    }
    const char& at(size_t index) const {
        if (index >= m_size) throw std::out_of_range("string::at");
        return raw()[index];
    }

    char& front() { return raw()[0]; }
    const char& front() const { return raw()[0]; }
    char& back() { return raw()[m_size - 1]; }
    const char& back() const { return raw()[m_size - 1]; }

    const char* c_str() const { return raw(); }
    const char* data() const { return raw(); }
    char* data() { return raw(); }

    // ---------- capacity ----------

    size_t size() const { return m_size; }
    size_t length() const { return m_size; }
    bool empty() const { return m_size == 0; }

    void clear() {
        if (is_heap()) delete[] m_value.heap_string.data;
        m_size = 0;
        m_value.stack_string[0] = '\0';
    }

    // ---------- modifiers ----------

    string& append(const char* str, size_t len) {
        size_t new_size = m_size + len;

        if (new_size <= 15 && !is_heap()) {
            // still fits in the existing SSO buffer
            if (str) std::memcpy(m_value.stack_string + m_size, str, len);
            m_size = new_size;
            m_value.stack_string[m_size] = '\0';
            return *this;
        }

        size_t cap_needed = next_pow2(new_size);
        char* new_data = new char[cap_needed + 1];

        std::memcpy(new_data, raw(), m_size);
        if (str) std::memcpy(new_data + m_size, str, len);
        new_data[new_size] = '\0';

        if (is_heap()) delete[] m_value.heap_string.data;

        m_value.heap_string.data = new_data;
        m_value.heap_string.capacity = cap_needed;
        m_size = new_size;

        return *this;
    }

    string& operator+=(const string& other) { return append(other.raw(), other.m_size); }
    string& operator+=(const std::string& other) { return append(other.c_str(), other.size()); }
    string& operator+=(const char* str) { return append(str, str ? c_strlen(str) : 0); }
    string& operator+=(char c) { return append(&c, 1); }

    // Defined out-of-line below, after string_view is a complete type.
    string& operator+=(std::string_view sv);
    string& operator+=(string_view sv);

    // ---------- comparisons ----------

    bool operator==(const string& other) const {
        return m_size == other.m_size && std::memcmp(raw(), other.raw(), m_size) == 0;
    }
    bool operator!=(const string& other) const { return !(*this == other); }

    bool operator==(const char* str) const {
        if (!str) return m_size == 0;
        return std::strcmp(raw(), str) == 0;
    }
    bool operator!=(const char* str) const { return !(*this == str); }

    bool operator<(const string& other) const { return std::strcmp(raw(), other.raw()) < 0; }
    bool operator>(const string& other) const { return std::strcmp(raw(), other.raw()) > 0; }
    bool operator<=(const string& other) const { return !(*this > other); }
    bool operator>=(const string& other) const { return !(*this < other); }

    // ---------- iterators ----------

    char* begin() { return raw(); }
    char* end() { return raw() + m_size; }
    const char* begin() const { return raw(); }
    const char* end() const { return raw() + m_size; }

    // ---------- substr / find ----------

    string substr(size_t pos, size_t len = npos) const {
        if (pos > m_size) throw std::out_of_range("string::substr");
        size_t actual_len = std::min(len, m_size - pos);
        return string(raw() + pos, actual_len);
    }

    size_t find(const char* needle, size_t start = 0) const {
        if (start > m_size) return npos;
        const char* found = std::strstr(raw() + start, needle);
        if (!found) return npos;
        return static_cast<size_t>(found - raw());
    }

    size_t find(const string& needle, size_t start = 0) const {
        return find(needle.c_str(), start);
    }

    // ---------- interop ----------

    std::string std_str() const { return std::string(raw(), m_size); }

    // ---------- non-members ----------

    friend string operator+(const string& lhs, const string& rhs) {
        string result(lhs);
        result += rhs;
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const string& s) {
        os.write(s.c_str(), s.size());
        return os;
    }
};

// ---------------------------------------------------------------------
// string_view: a non-owning (pointer + length) view into character data.
// Never allocates, never copies bytes, never owns what it points to —
// the caller is responsible for making sure the underlying data outlives
// the view (same contract as std::string_view).
// ---------------------------------------------------------------------

export class string_view {
private:
    const char* m_data;
    size_t m_size;

    static size_t c_strlen(const char* s) {
        size_t n = 0;
        while (s[n] != '\0') n++;
        return n;
    }

public:
    static constexpr size_t npos = static_cast<size_t>(-1);

    // ---------- constructors ----------

    constexpr string_view() noexcept : m_data(nullptr), m_size(0) {}

    string_view(const char* str) : m_data(str), m_size(str ? c_strlen(str) : 0) {}

    constexpr string_view(const char* str, size_t len) noexcept : m_data(str), m_size(len) {}

    string_view(const std::string& str) noexcept : m_data(str.data()), m_size(str.size()) {}

    string_view(const string& str) noexcept : m_data(str.data()), m_size(str.size()) {}

    string_view(std::string_view sv) noexcept : m_data(sv.data()), m_size(sv.size()) {}

    string_view(const string_view&) noexcept = default;
    string_view& operator=(const string_view&) noexcept = default;

    // ---------- element access ----------

    const char& operator[](size_t index) const { return m_data[index]; }

    const char& at(size_t index) const {
        if (index >= m_size) throw std::out_of_range("string_view::at");
        return m_data[index];
    }

    const char& front() const { return m_data[0]; }
    const char& back() const { return m_data[m_size - 1]; }

    const char* data() const { return m_data; }

    // ---------- capacity ----------

    size_t size() const { return m_size; }
    size_t length() const { return m_size; }
    bool empty() const { return m_size == 0; }

    // ---------- modifiers (these just move the view, never touch memory) ----------

    void remove_prefix(size_t n) {
        m_data += n;
        m_size -= n;
    }

    void remove_suffix(size_t n) {
        m_size -= n;
    }

    // ---------- iterators ----------

    const char* begin() const { return m_data; }
    const char* end() const { return m_data + m_size; }

    // ---------- substr / find ----------

    string_view substr(size_t pos, size_t len = npos) const {
        if (pos > m_size) throw std::out_of_range("string_view::substr");
        size_t actual_len = std::min(len, m_size - pos);
        return string_view(m_data + pos, actual_len);
    }

    size_t find(char c, size_t start = 0) const {
        for (size_t i = start; i < m_size; i++) {
            if (m_data[i] == c) return i;
        }
        return npos;
    }

    size_t find(string_view needle, size_t start = 0) const {
        if (needle.m_size == 0) return start <= m_size ? start : npos;
        if (start >= m_size) return npos;

        for (size_t i = start; i + needle.m_size <= m_size; i++) {
            if (std::memcmp(m_data + i, needle.m_data, needle.m_size) == 0) {
                return i;
            }
        }
        return npos;
    }

    bool starts_with(string_view prefix) const {
        if (prefix.m_size > m_size) return false;
        return std::memcmp(m_data, prefix.m_data, prefix.m_size) == 0;
    }

    bool ends_with(string_view suffix) const {
        if (suffix.m_size > m_size) return false;
        return std::memcmp(m_data + (m_size - suffix.m_size), suffix.m_data, suffix.m_size) == 0;
    }

    bool contains(string_view needle) const {
        return find(needle) != npos;
    }

    // ---------- comparisons ----------

    int compare(string_view other) const {
        size_t min_len = std::min(m_size, other.m_size);
        int result = min_len ? std::memcmp(m_data, other.m_data, min_len) : 0;
        if (result != 0) return result;
        if (m_size < other.m_size) return -1;
        if (m_size > other.m_size) return 1;
        return 0;
    }

    bool operator==(string_view other) const {
        return m_size == other.m_size && compare(other) == 0;
    }
    bool operator!=(string_view other) const { return !(*this == other); }
    bool operator<(string_view other) const { return compare(other) < 0; }
    bool operator>(string_view other) const { return compare(other) > 0; }
    bool operator<=(string_view other) const { return !(*this > other); }
    bool operator>=(string_view other) const { return !(*this < other); }

    // ---------- interop ----------

    std::string std_str() const { return std::string(m_data, m_size); }
    string to_string() const { return string(m_data, m_size); }

    friend std::ostream& operator<<(std::ostream& os, string_view sv) {
        os.write(sv.data(), sv.size());
        return os;
    }
};

// ---------------------------------------------------------------------
// string's string_view-taking members — defined here because they need
// string_view to be a complete type (its .data()/.size() calls below).
// ---------------------------------------------------------------------

string::string(std::string_view sv) : m_value{}, m_size(0) {
    assign(sv.data(), sv.size());
}

string::string(string_view sv) : m_value{}, m_size(0) {
    assign(sv.data(), sv.size());
}

string& string::operator=(std::string_view sv) {
    assign(sv.data(), sv.size());
    return *this;
}

string& string::operator=(string_view sv) {
    assign(sv.data(), sv.size());
    return *this;
}

string& string::operator+=(std::string_view sv) {
    return append(sv.data(), sv.size());
}

string& string::operator+=(string_view sv) {
    return append(sv.data(), sv.size());
}
