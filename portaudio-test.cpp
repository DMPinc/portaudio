#include <stdio.h>
#include <stdlib.h>

#include "portaudio/portaudio.h"

#pragma comment(lib, "libportaudio.2.dylib")

int CallBack(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
    short *in = (short *)input;
    short *out = (short *)output;

    for (int i = 0; i < frameCount; i++) {
        *out++ = *in++;
        printf("%i", *in);
    }

    return (int)paContinue;
}

int main(int argc, char *argv[])
{
/*
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
*/


    int InDeviceId = atoi(argv[1]);
    int OutDeviceId = atoi(argv[2]);


    const int SamplingRate = 48000;
    const int BufLength = SamplingRate / 100;

    Pa_Initialize();

    PaStreamParameters in_param;
    in_param.channelCount = 1;
    in_param.device = InDeviceId;
    in_param.hostApiSpecificStreamInfo = NULL;
    in_param.sampleFormat = paInt16;    
    in_param.suggestedLatency = Pa_GetDeviceInfo(in_param.device)->defaultLowInputLatency;

    PaStreamParameters out_param;
    out_param.channelCount = 1;
    out_param.device = OutDeviceId;
    out_param.hostApiSpecificStreamInfo = NULL;
    out_param.sampleFormat = paInt16;
    out_param.suggestedLatency = Pa_GetDeviceInfo(out_param.device)->defaultLowInputLatency;

    PaStream *Stream;
    Pa_OpenStream(&Stream, &in_param, &out_param, SamplingRate, BufLength, NULL, CallBack, NULL);
    const PaStreamInfo *info = Pa_GetStreamInfo(Stream);

    fprintf(stderr, "----------Start----------\n");
    Pa_StartStream(Stream);

    getchar();
    Pa_CloseStream(Stream);
    Pa_Terminate();

}
