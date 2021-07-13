#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "KurveMConfig.h"

#include "AppMain.h"

int main (int a_argc, char* a_argv[])
{
    printf("KurveM: %d.%d \n", KURVEM_VERSION_MAJOR, KURVEM_VERSION_MINOR);

    Application* app = new AppMain();

    app->Run();

    delete app;

    return 0;
}