#include <cstdio>
#include <cstdint>
#include <limits>

#include "wav_file.hpp"

using namespace std;


// explicit template instantiation.
template class Wav_file<signed short>;

template<typename Sample_type>
Wav_file<Sample_type>::Wav_file(unsigned int channels, unsigned int sample_rate)
	: channels_(channels),
	  sample_rate_(sample_rate),
	  riff(new unsigned char[12]),
	  fmt(new unsigned char[24]),
	  data(new unsigned char[8])
{
	unsigned int bits_per_smp = sizeof(Sample_type)*8;
	unsigned int block_align = sizeof(Sample_type) * channels_;
	unsigned int avg_bps = sample_rate_ * block_align;
	unsigned int o = 0;

	memcpy(fmt, "fmt ", 4);
    o = 4;
	fmt[o++] = 16;                 fmt[o++] = 0;                // chunk data size LS
	fmt[o++] = 0;                  fmt[o++] = 0;                // chunk data size MS
	fmt[o++] = 1;                  fmt[o++] = 0;                // 
	fmt[o++] = channels_;          fmt[o++] = 0;                // channels
	fmt[o++] = sample_rate_ & 255; fmt[o++] = sample_rate_ >> 8;          // sample rate
	fmt[o++] = 0;                  fmt[o++] = 0;                // sample rate
	fmt[o++] = avg_bps & 255;      fmt[o++] = (avg_bps >> 8) & 255; // avg bps
	fmt[o++] = avg_bps >> 16;      fmt[o++] = 0;                // avg bps
	fmt[o++] = block_align & 255;  fmt[o++] = block_align >> 8; // block align
	fmt[o++] = bits_per_smp;       fmt[o++] = 0;                // bits per smp

	memcpy(data, "data", 4);

	memcpy(riff, "RIFF\0\0\0\0WAVE", 12);
}

template<typename Sample_type>
Wav_file<Sample_type>::~Wav_file() {
	delete [] riff;
	delete [] fmt;
	delete [] data;
}

template <typename Sample_type>
void Wav_file<Sample_type>::save(const string & filename, const vector<Sample_type> &samples) {

	unsigned int o = 4;
	unsigned int sample_count = samples.size();
	data[o++] = sample_count & 255; data[o++] = (sample_count >> 8) & 255;
	data[o++] = (sample_count >> 16) & 255; data[o++] = sample_count >> 24;

	unsigned int length = 4 + sizeof(fmt) + sizeof(data) + sizeof(Sample_type) * sample_count; // compute full length
	riff[4] = length & 255; riff[5] = (length>>8) & 255;    
	riff[6] = (length >> 16)&255; riff[7] = (length>>24);

	FILE* f = fopen(filename.c_str(), "wb");
	fwrite(riff, 1, 12, f);
	fwrite(fmt, 1, 24, f);
	fwrite(data, 1, 8, f);
	fwrite(samples.data(), sizeof(Sample_type), sample_count, f);

	fclose(f);
}

template<typename Sample_type>
void Wav_file<Sample_type>::convert(const vector<float> &samples, vector<Sample_type> &output_samples) {

	unsigned int bits = numeric_limits<Sample_type>::digits;
	unsigned int sample_count = samples.size();

	output_samples.resize(sample_count, 0);

	for(int s = 0; s < sample_count; ++s) {
		output_samples[s] = static_cast<Sample_type>(samples[s] * (1 << bits));
	}
}

template <typename Sample_type>
void Wav_file<Sample_type>::save(const string & filename, const vector<float> &buffer) {
	vector<Sample_type> output;
	convert(buffer, output);
	save(filename, output);
}
