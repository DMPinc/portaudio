#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>
//#include "fftw/fftw3.h"
#include "portaudio/portaudio.h"

#pragma comment(lib, "libportaudio.2.dylib")

int fft_num;
fftw_plan p;
fftw_complex *fft_in, *fft_out;

int CallBack(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
    short *in = (short *)input;
    short *out = (short *)output;

    for (int i = 0; i < frameCount; i++) {
        *out++ = *in++;
//        printf("%i", in[i]);
    }

    fft_in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fft_num);
    fft_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fft_num);
    
    for (int i = 0; i < fft_num; i++) {
        if (i < (int)frameCount) {
            fft_in[i][0] = (double)in[i];
        } else {
            fft_in[i][0] = 0.0;
        }
        fft_in[i][1] = 0.0;
    }

    p = fftw_plan_dft_1d(fft_num, fft_in, fft_out, FFTW_FORWARD, FFTW_ESTIMATE); 
    fftw_execute(p);

    double max_power = 0.0;
    int max_idx = 0;
    for (int i = 1; i < fft_num / 2; i++) {
        double s = fft_out[i][0] * fft_out[i][0] + fft_out[i][1] * fft_out[i][1];
        if (s > max_power) {
            max_power = s;
            max_idx = i;
        }
    }

    double freq = (44100 / (double)fft_num) * max_idx;

//    double freq = max_idx * (48000 / (double)fft_num);
//    double freq = max_idx / (double)fft_num * 48000;

    printf("%.0lf, ", freq);
//    printf("0");
    
    return (int)paContinue;
}

void displayOption()
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
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        displayOption();
        exit(0);
    }

    int InDeviceId = atoi(argv[1]);
    int OutDeviceId = atoi(argv[2]);

    const int SamplingRate = 44100;
    const int BufLength = SamplingRate / 50;

    for (double i = 1.0; i < 1000; i = i + 1.0) {
        fft_num = pow(2.0, i);
        if (fft_num > BufLength * 2) {
            break;
        }
    }

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

    fftw_destroy_plan(p);
    fftw_free(fft_in); 
    fftw_free(fft_out);
}
