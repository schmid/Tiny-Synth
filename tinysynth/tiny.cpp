#include <cstdio>
#include <cstdint>
#include <cmath>
#include <string>
using namespace std;

class Wav_file {
public:
	template <typename Sample> void save(const string & filename, Sample *output, unsigned int sample_count);
};

template <typename Sample> void Wav_file::save(const string & filename, Sample *output, unsigned int sample_count) {
	unsigned char riff[]= "RIFF\0\0\0\0WAVE";
	unsigned char fmt[24] = "fmt ";
	typedef  signed short Sample;
	unsigned int sr = 44100;
	unsigned int channels = 1;
	unsigned int bits_per_smp = sizeof(Sample)*8;
	unsigned int block_align = sizeof(Sample) * channels;
	unsigned int avg_bps = sr * block_align;
	unsigned int o = 4;
	fmt[o++] = 16;                fmt[o++] = 0;                // chunk data size LS
	fmt[o++] = 0;                 fmt[o++] = 0;                // chunk data size MS
	fmt[o++] = 1;                 fmt[o++] = 0;                // WAVE_FORMAT_IEEE_FLOAT
	fmt[o++] = 1;                 fmt[o++] = 0;                // channels
	fmt[o++] = sr & 255;          fmt[o++] = sr >> 8;          // sample rate
	fmt[o++] = 0;                 fmt[o++] = 0;                // sample rate
	fmt[o++] = avg_bps & 255;     fmt[o++] = (avg_bps >> 8) & 255; // avg bps
    fmt[o++] = avg_bps >> 16;     fmt[o++] = 0;                // avg bps
	fmt[o++] = block_align & 255; fmt[o++] = block_align >> 8; // block align
	fmt[o++] = bits_per_smp;      fmt[o++] = 0;                // bits per smp
	unsigned char data[8] = "data";
	data[4] = sample_count & 255; data[5] = (sample_count >> 8) & 255;
	data[6] = (sample_count >> 16) & 255; data[7] = sample_count >> 24;

	unsigned int length = 4 + sizeof(fmt) + sizeof(data) + sizeof(Sample) * sample_count; // compute full length
	riff[4] = length & 255; riff[5] = (length>>8) & 255;    
	riff[6] = (length >> 16)&255; riff[7] = (length>>24);

	FILE* f = fopen(filename.c_str(), "wb");
	fwrite(riff, 1, 12, f);
	fwrite(fmt, 1, sizeof(fmt), f);
	fwrite(data, 1, sizeof(data), f);
   	fwrite(output, sizeof(Sample), sample_count, f);

	fclose(f);
}

int main() {
	const unsigned int sample_count = 44100 * 4;
	typedef  signed short Sample;
	unsigned int sr = 44100;
	unsigned int channels = 1;

	auto output = new Sample[sample_count]; // output buffer
	auto samples = new float [sample_count]; // internal buffer
	int o = 0; // internal offset
	unsigned int output_offset = 0; // output buffer offset
	double ph = 0.0;
	double lfo_ph = 0.0;
	double env = 1;
	double freq = 0.1;
	for(int n = 0; n < sample_count; n++) {

		env -= 0.00002;
		if(env < 0)
			env = 0;
		double out = sin(ph) * env;
		ph += freq + sin(lfo_ph) * 0.001; lfo_ph += 0.0005;

		samples[o++] = out;
		output[output_offset++] = (Sample) (out * (1<<15));
	}

	Wav_file wav_file;
	wav_file.save("C:\\tmp\\output.wav", output, sample_count);

	return 0;
}
