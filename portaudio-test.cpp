#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <fftw3.h>
//#include "fftw/fftw3.h"
#include "portaudio/portaudio.h"

#pragma comment(lib, "libportaudio.2.dylib")

int fft_num;
fftw_plan p;
fftw_complex *fft_in, *fft_out;
std::string midi_keys[128];
double midi_freqs[128];
std::string prev_midi_key;
int sound_length = 0;

int CallBack(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
    short *in = (short *)input;
    short *out = (short *)output;

    for (int i = 0; i < frameCount; i++) {
        *out++ = *in++;
    }

    fft_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fft_num);
    double double_in[fft_num];
    
    for (int i = 0; i < fft_num; i++) {
        if (i < (int)frameCount) {
            double_in[i] = (double)in[i];
        } else {
            double_in[i] = 0.0;
        }
    }

    p = fftw_plan_dft_r2c_1d(fft_num, double_in, fft_out, FFTW_ESTIMATE); 
    fftw_execute(p);

    double max_power = 0.0;
    int max_idx = 0;
    for (int i = 0; i < fft_num / 2; i++) {
        double s = fft_out[i][0] * fft_out[i][0] + fft_out[i][1] * fft_out[i][1];
        if (s > max_power) {
            max_power = s;
            max_idx = i;
        }
    }

    double freq = (44100 / (double)fft_num) * max_idx;

    double min_distance = 1000.0;
    int min_idx = 0;
    for (int i = 0; i < 128; i++) {
        double distance = (midi_freqs[i] - freq) * (midi_freqs[i] - freq);
        if (distance < min_distance) {
            min_distance = distance;
            min_idx = i;
        }
    }

    std::string current_midi_key = midi_keys[min_idx];

    printf("%.0lf:", freq);
    std::cout << current_midi_key << ", ";

    if (prev_midi_key != current_midi_key) {
        std::cout << std::endl << std::endl << current_midi_key << ":" << sound_length << "ms, " << std::endl;
        prev_midi_key = current_midi_key;
        sound_length = 0;
    } else {
        sound_length += 20;
    }
    
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

void loadMidiHzTxt() {
    char str[50];
    char *token;

    FILE *fp;
    fp = fopen("midi-Hz.txt", "r");
    int idx = 0;
    while (fgets(str, 50, fp) != NULL) {
        str[strlen(str) - 1] = '\0';
        token = strtok(str, " " );

        std::string midi_key = token;
        midi_keys[idx] = midi_key;
        std::cout << midi_key << std::endl;

        token = strtok( NULL, "," );
        double freq = atof(token);
        midi_freqs[idx] = freq;
        printf("%f\n", freq);

        idx++;
    }
    fclose(fp);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        displayOption();
        exit(0);
    }

    loadMidiHzTxt();

    int InDeviceId = atoi(argv[1]);
    int OutDeviceId = atoi(argv[2]);

    const int SamplingRate = 44100;
    const int BufLength = SamplingRate / 50;

    // FFT用に2のべき乗を計算する
    for (double i = 1.0; i < 1000; i = i + 1.0) {
        fft_num = pow(2.0, i);
        if (fft_num > BufLength * 2) {
            break;
        }
    }

    Pa_Initialize();

    PaStreamParameters in_param;
    in_param.channelCount = 2;
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
