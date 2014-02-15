#include <stdio.h>
#include <stdlib.h>

#include "portaudio/portaudio.h"

#pragma comment(lib, "libportaudio.2.dylib")

int main(int argc, char *argv[])
{
    Pa_Initialize();
    fprintf(stderr, "--------------------------------------------------\n");
    fprintf(stderr, "Input Devices\n");
    fprintf(stderr, "--------------------------------------------------\n");
    for (int i = 0; i < Pa_GetDeviceCount(); i++) {
        const PaDeviceInfo *deviceInfo;
        deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxInputChannels > 0) {
            const PaHostApiInfo *apiInfo;
            apiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
            fprintf(stderr, "%2d %s(%s)\n", i, deviceInfo->name, apiInfo->name);
        }
    }

    fprintf(stderr, "\n--------------------------------------------------\n");
    fprintf(stderr, "Output Devices\n");
    fprintf(stderr, "--------------------------------------------------\n");
    for (int i = 0; i < Pa_GetDeviceCount(); i++) {
        const PaDeviceInfo *deviceInfo;
        deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxOutputChannels > 0) {
            const PaHostApiInfo *apiInfo;
            apiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
            fprintf(stderr, "%2d %s(%s)\n", i, deviceInfo->name, apiInfo->name);
        }
    }
    Pa_Terminate();

    exit(0);
}
