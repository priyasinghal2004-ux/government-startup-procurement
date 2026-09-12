#include <drogon/drogon.h>

/**
 * INTERVIEW DEFENSE NOTE:
 * Drogon event loop initialization:
 * drogon::app().loadConfigFile("config.json") loads listener ports, thread pools,
 * and the PostgreSQL DbClient connection pool.
 * drogon::app().run() starts the non-blocking event-driven epoll/kqueue runtime.
 */
int main()
{
    drogon::app().loadConfigFile("config.json");
    drogon::app().run();
    return 0;
}
