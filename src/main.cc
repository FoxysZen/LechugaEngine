#include <Application.h>

int main()
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