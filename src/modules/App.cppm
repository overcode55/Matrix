module; 

#include <cstdio>

export module App;

import InfoHandler;
import std.compat;
import String;
import GlobalVariables;

void NoexceptErrorHandler() noexcept {

    char message1[100] = "\033[38;2;255;90;90mError: \033[0mA noexcept function has thrown an exction! Possible reasons are:\n";
    char space[10]    = "|      ";
    char message2[60] = "The current and last info state were:\n";
    char reason[15] = "REASON: ";
    char last_reason[15] = "LAST REASON: ";
    char state[15] = "STATE: ";
    char last_state[15] = "LAST STATE: ";
    
    std::fputs(message1, stderr);
    std::fputs(space, stderr);
    std::fputs(space, stderr);
    std::fputs(reason, stderr);
    std::fputs(g_current_noexcept_possible_fail_reason, stderr);
    std::fputs(space, stderr);
    std::fputs(space, stderr);
    std::fputs(last_reason, stderr);
    std::fputs(g_last_noexcept_possible_fail_reason, stderr);

    std::fputs(space, stderr);
    std::fputs(message2, stderr);
    std::fputs(space, stderr);
    std::fputs(space, stderr);
    std::fputs(state, stderr);
    std::fputs(g_current_noexcept_state, stderr);
    std::fputs(space, stderr);
    std::fputs(space, stderr);
    std::fputs(last_state, stderr);
    std::fputs(g_last_noexcept_state, stderr);

    std::abort();
}

export class App{
  private:
    
    string m_title;
    

    void init()
    {
      std::set_terminate(NoexceptErrorHandler);
    }
  public:
    App(string title = "title")
    : m_title(title)
    {
      init();
    }    
    void run(){};
};