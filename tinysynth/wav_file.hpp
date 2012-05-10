#pragma once
#include <string>
#include <vector>

template <typename Sample_type> class Wav_file {
public:
	Wav_file(unsigned int channels, unsigned int sample_rate);
	~Wav_file();
	void save(const std::string & filename, const std::vector<Sample_type> &samples);
	void save(const std::string & filename, const std::vector<float> &internal_samples);

private:
	Wav_file(const Wav_file & other) {} // hide copy ctor

	void convert(const std::vector<float> &samples, std::vector<Sample_type> &output_samples);

	unsigned int channels_;
	unsigned int sample_rate_;
	unsigned char *fmt;
	unsigned char *data;
	unsigned char *riff;
};
