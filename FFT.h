#pragma once

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

class FFT
{
private:
	int channels;
	int frame_size;
	int fft_num;
	int bit_num;
	short **prev_data;
	short **prev_fft_data;
	short *fft_data;
	short *out_data;
	double *window;
	int *w1;
	double *dr;
	double *di;
	double *xr;
	double *xi;
	double *w12r;
	double *w12i;
	double *w22r;
	double *w22i;

	int freq_low_idx;
	int freq_high_idx;

	void hanning(double* w);
	void fft(double* dr, double* di, double* xr, double* xi, char flag);
	void table1_d();
	void table2_d(char flag);
	void sum_d (int bit_num, double* dr, double* di, double* xr, double* xi, char flag);

public:
	void init_bpf_filter(int channels, int num, int sample_freq, int freq_low, int freq_high);
	void bpf_filter(int channel, short *data);
};
