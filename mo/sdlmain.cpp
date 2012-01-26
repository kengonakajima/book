
#include "SDL.h"

#include "app.h"

App *g_app;
int main( int argc, char **argv )
{
    g_app = new App( argc, argv );
    g_app->execute();
    delete g_app;
    return 0;
}

