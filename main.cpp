#include <unistd.h>
#include <SFML/Audio.hpp>

//Samplerate used in playing and generating sounds
const double SAMPLE_RATE = 44100;

//Magnitude of square wave
const int MAX_SQUARE_WAVE_MAGNITUDE = 5000;

/**
 * Generate a square wave with the given frequency and duration.
 *
 * \param frequency Input frequency in MHz
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

double key_to_pitch(int key, int lower_key, int upper_key)
{
  return (key - lower_key)/(1.0*(upper_key - lower_key));
}

#include <curses.h>
#include <cmath>

int main()
{
  //create base square wave array
  std::vector<sf::Int16> square_wave_freq_1 = generate_square_wave(200, 0.5);
  std::vector<sf::Int16> square_wave_freq_2 = generate_square_wave(300, 0.5);

  //prepare sound from square wave array
  sf::SoundBuffer sound_buffer_1;
  sf::SoundBuffer sound_buffer_2;
  sound_buffer_1.loadFromSamples(&square_wave_freq_1[0], square_wave_freq_1.size(), 2, SAMPLE_RATE);
  sound_buffer_2.loadFromSamples(&square_wave_freq_2[0], square_wave_freq_2.size(), 2, SAMPLE_RATE);

  //play square wave a number of times at increasing pitch
  sf::Sound sound_lowercase(sound_buffer_1);
  sf::Sound sound_uppercase(sound_buffer_2);
	
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
      if (islower(input_key)) {
        double pitch = key_to_pitch(input_key, 'a', 'z');
        sound_lowercase.setPitch(pitch);
        sound_lowercase.play();
      } else {
        double pitch = key_to_pitch(input_key, 'A', 'Z');
        sound_uppercase.setPitch(pitch);
        sound_uppercase.play();
      }
    }
  }
}
