#include <Application.h>

int main(int argc, char *argv[])
{
    Application application;

    if (!application.init())
    {
        return -1;
    }

    application.run();
    application.shutdown();

    return 0;
}