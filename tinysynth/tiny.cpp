/* By Jakob Schmid 2012. */

#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include "wav_file.hpp"
using namespace std;


float svin(float periods) {	return sin(periods * 6.283185307179586); }

float midi2freq(int note, int octave) {

    return 32.70319566257483 * pow(2., note / 12. + octave);
}

void vibrato_sine(int note, int octave, vector<float> &buffer, float seconds_per_sample) {

	unsigned int sample_count = buffer.size();
	double ph = 0.0;
	double lfo_ph = 0.0;
	double env = 0.9;
	// relfreq is periods per sample = normal freq * seconds_per_sample
	double relfreq = midi2freq(note, octave) * seconds_per_sample /* p/s * s/smp */;
	double lfo_relfreq = 8 * seconds_per_sample;

	for(int n = 0; n < sample_count; n++) {

		env = 0.9 - 0.9 * 2 * (float)n / sample_count;
		if(env < 0)
			env = 0;

		double out = svin(ph) * env;
		ph += relfreq + svin(lfo_ph) * 0.00006;
		lfo_ph += lfo_relfreq;

		buffer[n] = out;
	}
	
}

int main() {

	typedef signed short Sample;

	unsigned int sr = 44100;
	float seconds_per_sample = 1.f / sr;
	unsigned int channels = 1;
	const unsigned int sample_count = sr * 4;

	vector<float> samples(sr * 1, 0); // internal buffer

	struct Note {
		Note(const string & note_name, int midi_note, int midi_octave)
			: note_name(note_name),
			  midi_note(midi_note),
			  midi_octave(midi_octave)
		{}
		string note_name;
		int midi_note;
		int midi_octave;
	};

	vector<Note> notes;
	ostringstream ss;
	for(int n = 0; n <= 12; ++n) {
		ss.str("");
		ss << "note" << n;
		notes.push_back(Note(ss.str(), n, 3));
	}

	Wav_file<Sample> wav_file(1, 44100);

	for(auto i = notes.cbegin(); i != notes.cend(); ++i) {
		vibrato_sine(i->midi_note, i->midi_octave, samples, seconds_per_sample);
		wav_file.save(i->note_name + ".wav", samples);
	}

	samples.resize(44100 * 8);
	vibrato_sine(0, 1, samples, seconds_per_sample);
	wav_file.save("c1_long.wav", samples);

	return 0;
}
