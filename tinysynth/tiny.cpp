/* By Jakob Schmid 2012. */
#include <cmath>
#include <sstream>
#include <vector>
#include "wav_file.hpp"
#include "samples.hpp"
using namespace std;

// Standard synth functions
float svin(float periods) {	return sin(periods * 6.283185307179586); }
float spike(float periods) { return periods > 1 ? 0 : periods; }
float saw(float periods) { return periods - floor(periods); }
float midi2freq(int note, int octave) {
    return 32.70319566257483 * pow(2., note / 12. + octave);
}

class Buffer {
public:
    typedef signed short Sample;

    Buffer(unsigned int smp_count, unsigned int smp_p_s = 44100, unsigned int channels = 1)
        : smp_p_s(smp_p_s), s_p_smp(1.0/smp_p_s), channels(channels), samples(smp_count)
    {}

    ~Buffer() {}

    float read_8bit_unsigned(unsigned char data) {
         return data / 128.0f - 1.0f;
    }

    void amen() {
        
        unsigned char sequence[] = {
            1, 0,
            0, 0,
            2, 0,
            1, 0,
            0, 0,
            1, 0,
            3, 0,
            3, 0,
        };

        unsigned char *waveform[] = { 0, amen_bd, amen_hh, amen_sn };
        unsigned int waveform_length[] = { 0, amen_bd_size, amen_hh_size, amen_sn_size };

        for(int n = 0; n < smp_count(); ++n) {

            int beat = n * 8 / smp_count();
            int beat_time = n;
            int wi = sequence[beat*2];
            int pfi = sequence[beat*2+1];
            
            unsigned char * w = waveform[wi];
            
            double t = beat_time * s_p_smp;
            double ph;
            
            switch(pfi) {
                case 0: ph = t; break;
            }
            
            if(wi == 0)
                samples[n] = 0.f;
            else
                samples[n] = w[(int)(ph * waveform_length[wi])];
        }
        
        
        double p = 0;
        int seq[] = { 0, 0, 1, 2, 2, 1, 0, 0 };
        for(int n = 0; n < smp_count(); ++n) {
            //int wi = seq[ n 
            p = spike(n * s_p_smp * 4.0); 
            samples[n] = read_8bit_unsigned(amen_bd[(unsigned int)(p*amen_bd_size)]);
        }
    }

    void unit_impulse(unsigned int sample_index) {
        samples[sample_index] = 1.f;
    }

    void impulse_rhythm() {
        double ph = 0;
        for(int n = 0; n < smp_count(); n += smp_count() / 4) {
            unit_impulse(n);
        }
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

    void save(const string & filename) {
        Wav_file<Sample> wav_file(channels, smp_p_s);
        wav_file.save(filename.c_str(), samples);
    }

    unsigned int smp_count() { return samples.size(); }

    void resize(unsigned int new_size) { samples.resize(new_size); }

private:
    vector<float> samples;
    double s_p_smp;
    unsigned int smp_p_s;
    unsigned int channels;
};

void make_notes() {
    Buffer buffer(44100/2);
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

    for(auto i = notes.cbegin(); i != notes.cend(); ++i) {
        buffer.vibrato_sine(i->midi_note, i->midi_octave);
        buffer.save(i->note_name + ".wav");
    }
}

void make_long_note() {

    Buffer buffer(44100 * 8);
    buffer.repeator();
    buffer.save("c1_long.wav");
}

int main() {

    Buffer buffer(44100*4);
    buffer.amen();
    buffer.save("output.wav");

    return 0;
}
