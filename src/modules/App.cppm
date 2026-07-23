module; 

export module App;

import ErrorHandler;
import std.compat;
using string = std::string;

export class App{
  private:
    string m_title;
  public:
    App(string title = "title")
    : m_title(title)
    {}    
    void run(){};
};