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

class Buffer {
public:
    typedef signed short Sample;

    Buffer(unsigned int smp_count, unsigned int smp_p_s = 44100, unsigned int channels = 1)
        : smp_p_s(smp_p_s), s_p_smp(1.0/smp_p_s), smp_count(smp_count), samples(0), output(0)
    {
        samples = new float [smp_count]; // internal buffer
        output = new Sample[smp_count]; // output buffer
    }

    ~Buffer() {
        if(samples!=0) delete [] samples;
        if(output!=0) delete [] output;
    }

    void unit_impulse(unsigned int sample_index) {
        samples[sample_index] = 1;
    }

    void impulse_rhythm(float seconds_per_sample) {
        double ph = 0;
        for(int n = 0; n < smp_count; n += (int)(1.0f/s_p_smp)) {
        
        }
    }

    void vibrato_sine(int note, int octave) {

        int o = 0; // internal offset
        double ph = 0.0;
        double lfo_ph = 0.0;
        double env = 0.9;
        // relfreq is periods per sample = normal freq * s_p_smp
        double relfreq = midi2freq(note, octave) * s_p_smp /* p/s * s/smp */;
        double lfo_relfreq = 8 * s_p_smp;

        for(int n = 0; n < smp_count; n++) {

            env -= 0.00001;
            if(env < 0)
                env = 0;

            double out = svin(ph) * env;
            ph += relfreq + svin(lfo_ph) * 0.00006;
            lfo_ph += lfo_relfreq;

            samples[o++] = out;
        }
    
    }

    void save(const string & filename) {
        Wav_file<Sample> wav_file(1, 44100);
        for(int s = 0; s < smp_count; ++s) {
            output[s] = (Sample) (samples[s] * (1<<15));
        }
        wav_file.save(filename.c_str(), output, smp_count);
    }

private:
    float * samples;
    Sample * output;
    double s_p_smp;
    double smp_p_s;
    unsigned int channels;
    unsigned int smp_count;
};

int main() {
    Buffer buf(44100);

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

    for(auto i = notes.cbegin(); i != notes.cend(); ++i) {
        buf.vibrato_sine(i->midi_note, i->midi_octave);
        buf.save(i->note_name + ".wav");
    }

    return 0;
}
