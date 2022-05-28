#pragma once
#include <qdir.h>

class QString;
class QMediaPlayer;

class MusicPlayer
{
public:
	static void setPath(const QString& path);
	static inline QDir path() { return _path; }

	static void playBackground(const QString& name);
	static void playEffect(const QString& name);

	static void setBackgroundVolume(int volume);
	static int backgroundVolume();

	static void setEffectVolume(int volume);
	static int effectVolume();

private:
	static QMediaPlayer* backgroundPlayer;
	static QMediaPlayer* effectPlayer;
	static QDir _path;
};

