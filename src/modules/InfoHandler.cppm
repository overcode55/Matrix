module;

export module ErrorHandler;

import std.compat;
using string = std::string;

namespace /* Color */ { 
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
}/* Color */

namespace /* Class definitions */{

enum class Importance{
  HIGH,
  MEDIUM,
  LOW
};  

export class Error{
  public:

    string m_message;
    std::vector<string> m_context;

    size_t m_row , m_col;

    string m_origin;
    
    Importance m_importance;

    Error(string message , std::vector<string> context = {}  , std::source_location loc = std::source_location::current())
    : m_message(message) , m_context(context) ,  m_row(loc.line()) , m_col(loc.column()) , m_origin(loc.file_name()) , m_importance(Importance::HIGH)
    {}
};

export class Runtime_Error{
  public:
    string m_message;
    std::vector<string> m_context;
    
    size_t m_row , m_col;

    string m_origin;
    
    Importance m_importance;

    Runtime_Error(string message , size_t line , size_t column , string file_name , std::vector<string> context = {})
    : m_message(message) , m_context(context) , m_row(line) , m_col(column) , m_origin(file_name) , m_importance(Importance::HIGH)
    {}
};

export class Warning{
   public:
    string m_message;
    std::vector<string> m_context;
    
    size_t m_row , m_col;

    string m_origin;
    
    Importance m_importance;

    Warning(string message , std::vector<string> context = {} , std::source_location loc = std::source_location::current())
    : m_message(message) , m_context(context) , m_row(loc.line()) , m_col(loc.column()) , m_origin(loc.file_name()) , m_importance(Importance::MEDIUM)
    {}
};

export class Note{
  public:
    string m_message;
    std::vector<string> m_context;
    
    size_t m_row , m_col;

    string m_origin;

    Importance m_importance;

    Note(string message , std::vector<string> context = {} , std::source_location loc = std::source_location::current())
    : m_message(message) , m_context(context) , m_row(loc.line()) , m_col(loc.column()) , m_origin(loc.file_name()) , m_importance(Importance::LOW)
    {}
};
}/* Class definitions */

namespace /* Show implementation */{
template <typename T>
concept Infomation = requires(T obj) {
  { obj.m_message }   -> std::convertible_to<string>;
  { obj.m_context }   -> std::convertible_to<std::vector<string>>;
  { obj.m_row }       -> std::convertible_to<size_t>;
  { obj.m_col }       -> std::convertible_to<size_t>;
  { obj.m_origin }    -> std::convertible_to<string>;
  { obj.m_importance} -> std::convertible_to<Importance>;
};

std::pair<string , string> convert_importance(Importance impt){
  switch (impt)
  {
  case Importance::HIGH:
    return std::pair<string , string>{RED , "error: "};
    break;
  case Importance::MEDIUM:
    return std::pair<string , string>{YELLOW , "warning: "};
    break;
  case Importance::LOW:
    return std::pair<string , string>{CYAN , "note: "};
    break;
  default:
    return std::pair<string , string>{hexcode(0xFFFFFF) , "message: "};
    break;
  }
}

template <Infomation T>
export show(T info , bool buffer = false){
  std::ostream& os = (buffer ? std::cout : std::cerr);
  string file = note.m_origin + ":" + std::to_string(note.m_row) + ":" +  std::to_string(note.m_col) + ": ";
  os << color_text(YELLOW , file);
  std::pair<string , string> what = convert_importance(info.m_importance);
  os << color_text(what.first , what.second) << info.m_message << '\n';
  if(!info.m_context.empty()){
    for(const string& context : info.m_context){
      os << context << "\n";
    }
  }else{
    os << "no extra context" << '\n';
  }
}

export void show(const Note& note , bool buffer = false) noexcept {
  std::ostream& os = (buffer ? std::cout : std::cerr);
  string file = note.m_origin + ":" + std::to_string(note.m_row) + ":" +  std::to_string(note.m_col) + ": ";
  os << color_text(YELLOW , file);
  os << color_text(CYAN ,"note: ") << note.m_message << '\n';   
  if(note.m_context.empty()){
    os << "  " << std::to_string(note.m_row) << " |   " << "no context" << '\n';
  }else{
    os << "  " << std::to_string(note.m_row) << " |   " << note.m_context << '\n';   
  }   
}

export void show(const Warning& warn , std::optional<Note> note = std::nullopt) noexcept {
  string file = warn.m_origin + ":" + std::to_string(warn.m_row) + ":" +  std::to_string(warn.m_col) + ": ";
  std::cerr << color_text(YELLOW , file);
  std::cerr << color_text(hexcode(0x793bd1) ,"warning: ") << warn.m_message << '\n';
  if(warn.m_context.empty()){
    std::cerr << "  " << std::to_string(warn.m_row) << " |   " << "no context" << '\n';
    if(note.has_value() &&!note->m_message.empty()) show(*note);
  }else{
    std::cerr << "  " << std::to_string(warn.m_row) << " |   " << warn.m_context << '\n';   
    if(note.has_value() &&!note->m_message.empty()) show(*note);
  }   
}

export void show(const Error& err , std::optional<Note> note = std::nullopt) noexcept {
  string file = err.m_origin + ":" + std::to_string(err.m_row) + ":" +  std::to_string(err.m_col) + ": "; 
  std::cerr << color_text(YELLOW , file);
  std::cerr << color_text(RED ,"error: ") << err.m_message << '\n';
  if(err.m_context.empty()){
    std::cerr << "  " << std::to_string(err.m_row) << " |   " << "no context" << '\n';   
    if(note.has_value() &&!note->m_message.empty()) show(*note);
  }else{
    std::cerr << "  " << std::to_string(err.m_row) << " |   " << err.m_context << '\n';   
    if(note.has_value() &&!note->m_message.empty()) show(*note);   
  }   
}

export void show(const Runtime_Error& run_err , std::optional<Note> note = std::nullopt) noexcept {
  string file = run_err.m_origin + ":" + std::to_string(run_err.m_row) + ":" +  std::to_string(run_err.m_col) + ": ";
  std::cerr << color_text(YELLOW , file);
  std::cerr << color_text(RED ,"runtime_error: ") << run_err.m_message << '\n';
  if(run_err.m_context.empty()){
    std::cerr << "  " << std::to_string(run_err.m_row) << " |   " << "no context" << '\n';   
    if(note.has_value() &&!note->m_message.empty()) show(*note);
  }else{
    std::cerr << "  " << std::to_string(run_err.m_row) << " |   " << run_err.m_context << '\n';    
    if(note.has_value() &&!note->m_message.empty()) show(*note);  
  }
}
}/* Show implementation */
