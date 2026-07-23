import std.compat;
import ErrorHandler;

int main(){
    show<Runtime_Error>(Runtime_Error("test runtime err"))
    return 0;
}