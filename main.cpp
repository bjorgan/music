#include <unistd.h>
#include <SFML/Audio.hpp>
#include <map>
#include <cmath>
#include <curses.h>

//Samplerate used in playing and generating sounds
const double SAMPLE_RATE = 44100;

//Magnitude of square wave
const int MAX_SQUARE_WAVE_MAGNITUDE = 5000;

/**
 * Generate a square wave with the given frequency and duration.
 *
 * \param frequency Input frequency in Hz
 * \param duration Duration of signal in seconds
 * \return Square wave sampled at SAMPLE_RATE
 **/
std::vector<sf::Int16> generate_square_wave(double frequency, double duration)
{
	//initialize entire square wave array to low value
	size_t samples = duration*SAMPLE_RATE;
	std::vector<sf::Int16> square_wave(samples, -MAX_SQUARE_WAVE_MAGNITUDE);

	size_t numSamplesInPeriod = SAMPLE_RATE/frequency;
	size_t numPeriods = samples/numSamplesInPeriod;

	//assign square wave array to high value during first half of each period
	for (size_t i = 0; i < numPeriods; i++) {
		std::vector<sf::Int16>::iterator start = square_wave.begin() + i*numSamplesInPeriod;
		std::fill_n(start, numSamplesInPeriod/2, MAX_SQUARE_WAVE_MAGNITUDE);
	}
	return square_wave;
}

/**
 * Convert piano key number to a pitch.
 *
 * \param key_number
 * \return Pitch
 **/
double piano_key_to_pitch(int key_number)
{
	return powf(2, (key_number - 49.0)/12.0);
}

/**
 * Convert input key to a piano key frequency pitch, assuming base frequency to be 440 Hz.
 *
 * \param key Input key, corresponding to a keyboard character
 * \param key_map Map from characters to piano key numbers
 * \return Pitch required for transforming from 440 MHz to the given piano key
 **/
double key_to_pitch(int key, std::map<int, int> key_map)
{
	int key_number = key_map[key];
	return piano_key_to_pitch(key_number);
}

/**
 * Chord type used in play_chord().
 **/
enum chord_type {
	CHORD_MINOR,
	CHORD_MAJOR
};

/**
 * Convert input key to some pitch by distributing the keys
 * from 1 to 2, corresponding to from lower_key to upper_key.
 *
 * \param key Input character
 * \param lower_key Lower key in range, e.g. 'a'
 * \param upper_key Upper key in range, e.g. 'z'
 * \return Pitch between 1 and 2
 **/
double key_to_pitch(int key, int lower_key, int upper_key)
{
	return 1.0 + (key - lower_key)/(1.0*(upper_key - lower_key));
}

/**
 * Play a chord in sequence.
 *
 * \param chord_type Minor or major
 * \param root_key Root piano key of chord
 * \param sound Sound instance to be pitched and played
 **/
void play_chord(enum chord_type chord_type, int root_key, sf::Sound &sound)
{
	std::vector<int> keys(3);
	keys[0] = root_key;
	switch (chord_type) {
		case CHORD_MINOR:
			keys[1] = root_key + 3;
			break;
		case CHORD_MAJOR:
			keys[1] = root_key + 4;
			break;
	}
	keys[2] = root_key + 7;

	std::sort(keys.begin(), keys.end());

	const double SLEEP_TIME = 0.1*1.0e06;
	for (size_t i=0; i < keys.size(); i++) {
		double pitch = piano_key_to_pitch(keys[i]);
		sound.setPitch(pitch);
		sound.play();
		usleep(SLEEP_TIME);
	}
}

int main(int argc, char *argv[])
{
	double square_wave_frequency = 200;

	//use keyboard mode or chord mode if specified in CLI arguments, otherwise use default
	bool keyboard = false;
	bool chords = false;
	if ((argc > 1) && (std::string(argv[1]) == "keyboard")) {
		keyboard = true;
		square_wave_frequency = 440;
	} else if ((argc > 1) && (std::string(argv[1]) == "chords")) {
		chords = true;
	} else {
	}

	//create base square wave array
	std::vector<sf::Int16> square_wave = generate_square_wave(square_wave_frequency, 0.5);

	//prepare sound from square wave array
	sf::SoundBuffer sound_buffer;
	sound_buffer.loadFromSamples(&square_wave[0], square_wave.size(), 2, SAMPLE_RATE);
	sf::Sound sound(sound_buffer);

	//map from keyboard symbol to piano key
	std::map<int, int> key_map = {{'z', 40},
				{'x', 42}, {'s', 41},
                                {'c', 44}, {'d', 43},
                                {'v', 45},
                                {'b', 47}, {'g', 46},
                                {'n', 49}, {'h', 48},
                                {'m', 51}, {'j', 50},
                                {',', 52}};

	//set up ncurses in order to catch the keyboard
	initscr();
	keypad(stdscr, TRUE);
	start_color();
	cbreak();
	noecho();
	scrollok(stdscr,TRUE);
	curs_set(0);
	halfdelay(5);

	while (true) {
		int input_key = getch();
		if (input_key > 0) {
			double pitch = 1;
			if (keyboard) {
				//piano keyboard along z-m
				pitch = key_to_pitch(input_key, key_map);
				sound.setPitch(pitch);
				sound.play();
			} else if (chords) {
				//play chords along z-m
				enum chord_type chord_type;
				if (isupper(input_key)) {
					chord_type = CHORD_MINOR;
				} else {
					chord_type = CHORD_MAJOR;
				}
				play_chord(chord_type, key_map[tolower(input_key)], sound);
			} else {
				//play wild sounds when pressing any key
				pitch = key_to_pitch(input_key, 'a', 'z');
				sound.setPitch(pitch);
				sound.play();
			}
		}
	}
}
