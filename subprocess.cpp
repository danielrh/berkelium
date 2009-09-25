#include <berkelium/Berkelium.hpp>

#ifdef _WIN32
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    int argc = 0;
    char **argv = NULL
#else
int main(int argc, char**argv)
{
#endif

    Berkelium::forkedProcessHook(argc, argv);

    return 0;
}
