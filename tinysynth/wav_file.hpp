#pragma once
#include <string>

template <typename Sample_type> class Wav_file {
public:
	Wav_file(unsigned int channels, unsigned int sample_rate);
	void save(const std::string & filename, Sample_type *output, unsigned int sample_count);

private:
	Wav_file(const Wav_file & other) {} // hide copy ctor

	unsigned int channels_;
	unsigned int sample_rate_;
	unsigned char *fmt;
	unsigned char *data;
	unsigned char *riff;
};
