#include "FFT.h"

#define M_PI	3.141592654
#define POW2(X)	(1<<(X))
#define rint(X)	((int)(floor(X)+0.5))

void FFT::hanning (double* w)
{
	double part = 2.0 * M_PI / (double)fft_num;
	for (int i = 0; i < fft_num; i++) {
		*w++ = 0.5 - 0.5 * cos (part * (double)i);
	}
}

void FFT::fft (double* dr, double* di, double* xr, double* xi, char flag)
{
	sum_d (bit_num, dr, di, xr, xi, flag);
	if (flag) {
		for (int i = 0; i < fft_num; i++) {
			xr[i] /= (double)fft_num;
			xi[i] /= (double)fft_num;
		}
	}
}

void FFT::table1_d ()
{
	w1[0] = 0;
	w1[1] = 1;
	int n = 2;
	for (int i = 2; i <= bit_num; i++) {
		int m = n;
		n += n;
		for (int j = 0; j < m; j++) {
			w1[j] *= 2;
			w1[n-j-1] = n - 1 - w1[j];
		}
	}
}

void FFT::table2_d (char flag)
{
	int p1 = POW2 (bit_num - 2) - 1;
	int p2 = p1 * 2;
	double pt = (!flag ? -M_PI : M_PI) / (double)POW2 (bit_num - 1);

	for (int i = 0; i < p1; i++) {
		double pc = cos (pt * (double)(i + 1));
		double ps = sin (pt * (double)(i + 1));

		if (!flag) {
			w12r[i] = pc;
			w12i[i] = ps;
			w12r[p2-i] = -pc;
			w12i[p2-i] = ps;
		}
		else {
			w22r[i] = pc;
			w22i[i] = ps;
			w22r[p2-i] = -pc;
			w22i[p2-i] = ps;
		}
	}
	if (!flag) {
		w12r[p1] = 0.0;
		w12i[p1] = sin ((!flag ? -M_PI : M_PI) / 2.0);
	}
	else {
		w22r[p1] = 0.0;
		w22i[p1] = sin ((!flag ? -M_PI : M_PI) / 2.0);
	}
}

void FFT::sum_d (int bit_num, double* dr, double* di, double* xr, double* xi, char flag)
{
	int n1 = POW2 (bit_num);
	int n2 = n1 / 2;

	double *w2r = (!flag) ? w12r : w22r;
	double *w2i = (!flag) ? w12i : w22i;

	for (int i = 0; i < n1; i++) {
		xr[i] = dr[w1[i]];
		xi[i] = di[w1[i]];
	}
	for (int i = 1; i <= bit_num; i++) {
		int i0 = POW2 (i);
		int i1 = i0 / 2;
		int i2 = POW2 (bit_num - i);
		for (int j = 0; j <= i2 - 1; j++) {
			for (int k = i0 * j; k < i0 * j + i1; k++) {
				int ind = (i2 * k) % n2;
				double b1r = xr[k];
				double b1i = xi[k];
				double b2r = xr[k+i1];
				double b2i = xi[k+i1];
				if (!ind) {
					xr[k] = b1r + b2r;
					xi[k] = b1i + b2i;
					xr[k+i1] = b1r - b2r;
					xi[k+i1] = b1i - b2i;
				}
				else {
					double buf1 = w2r[ind-1] * b2r - w2i[ind-1] * b2i;
					double buf2 = w2r[ind-1] * b2i + w2i[ind-1] * b2r;

					xr[k] = b1r + buf1;
					xi[k] = b1i + buf2;
					xr[k+i1] = b1r - buf1;
					xi[k+i1] = b1i - buf2;
				}
			}
		}
	}
}

void FFT::init_bpf_filter (int ch, int num, int sample_freq, int freq_low, int freq_high)
{
	static char init_flag = 0;

	if (init_flag) {
		for (int i = 0; i < channels; i++) {
			free (prev_data[i]);
		}
		free (prev_data);
		for (int i = 0; i < channels; i++) {
			free (prev_fft_data[i]);
		}
		free (prev_fft_data);
		free (out_data);
		free (window);
		free (w1);
		free (dr);
		free (di);
		free (xr);
		free (xi);
		free (w12r);
		free (w12i);
		free (w22r);
		free (w22i);
	}
	init_flag = 1;

	channels = ch;
	frame_size = num;
	for (double i = 1.0; i < 1000; i = i + 1.0) {
		fft_num = pow(2.0, i);
		if (fft_num > frame_size * 2) {
			break;
		}
	}
	bit_num = rint (log ((double)fft_num) / log (2.0));

	prev_data = (short **)malloc (channels * sizeof (short *));
	for (int i = 0; i < channels; i++) {
		prev_data[i] = (short *)calloc (frame_size, sizeof (short));
	}
	prev_fft_data = (short **)malloc (channels * sizeof (short *));
	for (int i = 0; i < channels; i++) {
		prev_fft_data[i] = (short *)calloc (fft_num, sizeof (short));
	}
	fft_data = (short *)malloc (fft_num * sizeof (short));
	out_data = (short *)malloc (fft_num * sizeof (short));
	window = (double *)malloc (2 * frame_size * sizeof (double));
	w1 = (int *)malloc (fft_num * sizeof (int));
	dr = (double *)malloc (fft_num * sizeof (double));
	di = (double *)malloc (fft_num * sizeof (double));
	xr = (double *)malloc (fft_num * sizeof (double));
	xi = (double *)malloc (fft_num * sizeof (double));
	w12r = (double *)malloc ((fft_num / 2 + 1) * sizeof (double));
	w12i = (double *)malloc ((fft_num / 2 + 1) * sizeof (double));
	w22r = (double *)malloc ((fft_num / 2 + 1) * sizeof (double));
	w22i = (double *)malloc ((fft_num / 2 + 1) * sizeof (double));

	hanning (window);

	table1_d ();
	table2_d (0);
	table2_d (1);

	freq_low_idx = (int)floor ((double)freq_low / ((double)sample_freq / (double)fft_num));
	freq_high_idx = (int)ceil((double)freq_high / ((double)sample_freq / (double)fft_num));
}

void FFT::bpf_filter (int ch, short *data)
{
	double *xr1;
	double *xi1;
	double *xr2;
	double *xi2;

	int n2 = fft_num / 2;

	for (int i = 0; i < frame_size; i++) {
		fft_data[i] = prev_data[ch][i] * window[i];
		prev_data[ch][i] = data[i];
	}
	for (int i = 0; i < frame_size; i++) {
		fft_data[frame_size + i] = data[i] * window[frame_size + i];
	}
	for (int i = 2 * frame_size; i < fft_num; i++) {
		fft_data[i] = 0;
	}
	for (int i = 0; i < fft_num; i++) {
		dr[i] = (double)fft_data[i];
	}
	memset (di, 0, fft_num * sizeof (double));

	fft (dr, di, xr, xi, 0);
	xr[0] = xi[0] = xr[n2] = xi[n2] = 0.0;

	xr1 = &xr[1];
	xi1 = &xi[1];
	xr2 = &xr[fft_num-1];
	xi2 = &xi[fft_num-1];
	for (int i = 0; i < freq_low_idx; i++) {
		*xr1++ = 0.0;
		*xi1++ = 0.0;
		*xr2-- = 0.0;
		*xi2-- = 0.0;
	}
	xr1 = &xr[n2-1];
	xi1 = &xi[n2-1];
	xr2 = &xr[n2+1];
	xi2 = &xi[n2+1];
	for (int i = freq_high_idx; i < n2; i++) {
		*xr1-- = 0.0;
		*xi1-- = 0.0;
		*xr2++ = 0.0;
		*xi2++ = 0.0;
	}
	fft (xr, xi, dr, di, 1);

	for (int i = 0; i < frame_size; i++) {
		out_data[i] = prev_fft_data[ch][frame_size + i] + (short)dr[i];
	}
	for (int i = 0; i < fft_num; i++) {
		prev_fft_data[ch][i] = (short)dr[i];
	}

	for (int i = 0; i < frame_size; i++) {
		data[i] = out_data[i];
	}
}
