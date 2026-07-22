module;

export module ErrorHandler;

import std.compat;
import String;

namespace{
  constexpr string hexcode(const size_t& code){
    int r = (code >> 16) & 0xFF;
    int g = (code >> 8) & 0xFF;
    int b = code & 0xFF;
    return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
  }
  const string RESET  = "\033[0m";
  const string RED    = hexcode(0xff5a5a);
  const string BLUE   = hexcode(0x000080);
  const string GREEN  = hexcode(0x008000);
  const string CYAN   = hexcode(0x2FCFDF);
  const string YELLOW = hexcode(0xD8DE2A);

  struct ColoredText {
    string color;
    string text;
  };

  // Overload operator<< for your custom struct
  inline std::ostream& operator<<(std::ostream& os, const ColoredText& ct) {
    os << ct.color << ct.text << RESET;
    return os;
  }

  // Your helper function returning the struct
  inline ColoredText color_text(const string& color, const string& text) {
      return ColoredText{color, text};
  }
}

export class Error{
  public:

    string m_message;
    string m_context;

    size_t m_row , m_col;

    string m_origin;
  
    Error(string message , string context = ""  , std::source_location loc = std::source_location::current())
    : m_message(message) , m_context(context) ,  m_row(loc.line()) , m_col(loc.column()) , m_origin(loc.file_name())
    {}
};

export class Runtime_Error{
  public:
    string m_message;
    string m_context;
    
    size_t m_row , m_col;

    string m_origin;

    Runtime_Error(string message , string context = "" , std::source_location loc = std::source_location::current())
    : m_message(message) , m_context(context) , m_row(loc.line()) , m_col(loc.column()) , m_origin(loc.file_name());
    {}
};

export class Warning{};

export class Note{
  public:
    string m_message;
    string m_context;
    
    size_t m_row , m_col;

    string m_origin;

    Note(string message , string context = "" , std::source_location loc = std::source_location::current())
    : m_message(message) , m_context(context) , m_row(loc.line()) , m_col(loc.column()) , m_origin(loc.file_name())
    {}

    Note(string message , string context = "" , size_t line , size_t column , string file_name)
    : m_message(message) , m_context(context) , m_row(line) , m_col(column) , m_origin(file_name)
    {}
};

// -----------------------------------------------------------------------------------------------

export void show(const Note& note) noexcept {
  string file = note.m_origin + ":" + std::to_string(note.m_row) + ":" +  std::to_string(note.m_col) + ": ";
  std::cerr << color_text(YELLOW , file);
  std::cerr << color_text(CYAN ,"note: ") << err.m_message << '\n';   
}

export void show(const Warning& warn) noexcept {
  string file = warn.m_origin + ":" + std::to_string(warn.m_row) + ":" +  std::to_string(warn.m_col) + ": ";
  std::cerr << color_text(YELLOW , file);
  std::cerr << color_text(hexcode(0x793bd1) ,"warning: ") << warn.m_message << '\n';   
}

export void show(const Error& err) noexcept {
  string file = err.m_origin + ":" + std::to_string(err.m_row) + ":" +  std::to_string(err.m_col) + ": "; 
  std::cerr << color_text(YELLOW , file);
  std::cerr << color_text(RED ,"error: ") << err.m_message << '\n';   
}

export void show(const Runtime_Error& run_err , const Note& note = {""}) noexcept {
  string file = run_err.m_origin + ":" + std::to_string(run_err.m_row) + ":" +  std::to_string(run_err.m_col) + ": ";
  std::cerr << color_text(YELLOW , file);
  std::cerr << color_text(RED ,"runtime_error: ") << run_err.m_message << '\n';
  if(run_err.m_context.empty()){
    std::cerr << "  " << std::to_string(run_err.m_row) << " |   " << "no context" << '\n';
    if(!note.m_message.empty()) show(note);
  }else{
    std::cerr << "  " << std::to_string(run_err.m_row) << " |   " << run_err.m_context << '\n';   
  }
}
