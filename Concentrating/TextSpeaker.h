#pragma once

class QTextToSpeech;
class QString;

class TextSpeaker
{
	TextSpeaker() {};
public:
	static void say(const QString& text);
	static void stop();
	static void resume();
	static void pause();

	static int volume();
	static void setVolume(int volume);

	static int rate();
	static void setRate(int rate);
private:
	static QTextToSpeech* _speaker;
};

