/* By Jakob Schmid 2012. */

#include <cmath>
#include <sstream>
#include <vector>
#include "wav_file.hpp"
using namespace std;

float svin(float periods) {	return sin(periods * 6.283185307179586); }
float saw(float periods) { return periods - floor(periods); }
float midi2freq(int note, int octave) {
    return 32.70319566257483 * pow(2., note / 12. + octave);
}

class Buffer {
public:
    typedef signed short Sample;

    Buffer(unsigned int smp_count, unsigned int smp_p_s = 44100, unsigned int channels = 1)
        : smp_p_s(smp_p_s), s_p_smp(1.0/smp_p_s), samples(smp_count)
    {}

    ~Buffer() {}

    void unit_impulse(unsigned int sample_index) {
        samples[sample_index] = 1;
    }

    void repeator() {
        double ph = 0.0;
        float env = 0.9;
        double freq = midi2freq(0, 1); // p/s
        double time = 0.0;
        for(int n = 0; n < smp_count(); n++) {

            time = saw(n * s_p_smp * 2.0);

            env = max(1.0 - time * 4.0, 0.0);
            ph  = time * freq;

            double out = svin(ph) * env;

            samples[n] = out;
        }
    }

    void vibrato_sine(int note, int octave) {

        double ph = 0.0;
        double lfo_ph = 0.0;
        double env = 0.9;
        // relfreq is periods per sample = normal freq * seconds_per_sample
        double relfreq = midi2freq(note, octave) * s_p_smp; // p/s * s/smp
        double lfo_relfreq = 8 * s_p_smp;

        for(int n = 0; n < smp_count(); n++) {

            env = 0.9 - 0.9 * 2 * (float)n / smp_count();
            if(env < 0)
                env = 0;

            double out = svin(ph) * env;
            ph += relfreq + svin(lfo_ph) * 0.00006;
            lfo_ph += lfo_relfreq;

            samples[n] = out;
        }
    }

    void impulse_rhythm(float seconds_per_sample) {
        double ph = 0;
        for(int n = 0; n < smp_count(); n += (int)(1.0f/s_p_smp)) {
        
        }
    }

    void save(const string & filename) {
        Wav_file<Sample> wav_file(1, 44100);
        wav_file.save(filename.c_str(), samples);
    }

    unsigned int smp_count() { return samples.size(); }

    void resize(unsigned int new_size) { samples.resize(new_size); }

private:
    vector<float> samples;
    double s_p_smp;
    double smp_p_s;
    unsigned int channels;
};

int main() {
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

    Buffer buffer(44100/2);
    vector<Note> notes;
    ostringstream ss;
    for(int n = 0; n <= 12; ++n) {
        ss.str("");
        ss << "note" << n;
        notes.push_back(Note(ss.str(), n, 3));
    }

    for(auto i = notes.cbegin(); i != notes.cend(); ++i) {
        buffer.vibrato_sine(i->midi_note, i->midi_octave);
        buffer.save(i->note_name + ".wav");
    }

    buffer.resize(44100 * 8);
    buffer.repeator();
    buffer.save("c1_long.wav");

    return 0;
}
