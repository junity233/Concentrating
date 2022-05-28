#include "TextSpeaker.h"
#include <qtexttospeech.h>

QTextToSpeech* TextSpeaker::_speaker = new QTextToSpeech;


void TextSpeaker::say(const QString& text)
{
	_speaker->say(text);
}

void TextSpeaker::stop()
{
	_speaker->stop();
}

void TextSpeaker::resume()
{
	_speaker->resume();
}

void TextSpeaker::pause()
{
	_speaker->pause();
}

int TextSpeaker::volume()
{
	return _speaker->volume() * 100;
}

void TextSpeaker::setVolume(int volume)
{
	_speaker->setVolume((double)volume / 100.0);
}

int TextSpeaker::rate()
{
	return _speaker->rate() * 100;
}

void TextSpeaker::setRate(int rate)
{
	_speaker->setRate((double)rate / 100.0);
}
