/* By Jakob Schmid 2012. */

#include <cmath>
#include <vector>
#include <string>
#include "wav_file.hpp"
using namespace std;


float svin(float periods) {	return sin(periods * 6.283185307179586); }

float midi2freq(int note, int octave) {

    return 32.70319566257483 * pow(2., note / 12. + octave);
}

void vibrato_sine(int note, int octave, float * buffer, unsigned int sample_count, float seconds_per_sample) {

	int o = 0; // internal offset
	double ph = 0.0;
	double lfo_ph = 0.0;
	double env = 0.9;
	// relfreq is periods per sample = normal freq * seconds_per_sample
	double relfreq = midi2freq(note, octave) * seconds_per_sample /* p/s * s/smp */;
	double lfo_relfreq = 8 * seconds_per_sample;

	for(int n = 0; n < sample_count; n++) {

		env -= 0.00001;
		if(env < 0)
			env = 0;

		double out = svin(ph) * env;
		ph += relfreq + svin(lfo_ph) * 0.00006;
		lfo_ph += lfo_relfreq;

		buffer[o++] = out;
	}
	
}

int main() {

	typedef signed short Sample;

	unsigned int sr = 44100;
	float seconds_per_sample = 1.f / sr;
	unsigned int channels = 1;
	const unsigned int sample_count = sr * 4;

	float *samples = new float [sample_count]; // internal buffer
	Sample *output = new Sample[sample_count]; // output buffer

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
	notes.push_back(Note("c3", 0, 3));
	notes.push_back(Note("d3", 2, 3));
	notes.push_back(Note("eb3", 3, 3));
	notes.push_back(Note("f3", 5, 3));
	notes.push_back(Note("g3", 7, 3));
	notes.push_back(Note("ab3", 8, 3));
	notes.push_back(Note("bb3", 10, 3));
	notes.push_back(Note("c4", 0, 4));

	Wav_file<Sample> wav_file(1, 44100);

	for(auto i = notes.cbegin(); i != notes.cend(); ++i) {
		vibrato_sine(i->midi_note, i->midi_octave, samples, sample_count, seconds_per_sample);

		for(int s = 0; s < sample_count; ++s) {
			output[s] = (Sample) (samples[s] * (1<<15));
		}

		wav_file.save(i->note_name + ".wav", output, sample_count);

	}
	return 0;
}
