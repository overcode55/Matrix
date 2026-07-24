import std.compat;
import InfoHandler;
import App;

int main(){
    App app("title");
    show(Error("test runtime err" , {"first context" , "second cotext"}));
    return 0;
}