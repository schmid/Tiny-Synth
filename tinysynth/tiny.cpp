#include <cmath>
#include "wav_file.hpp"

using namespace std;

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

	Wav_file<Sample> wav_file(1, 44100);
	wav_file.save("output.wav", output, sample_count);

	return 0;
}
