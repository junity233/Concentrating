#include "MusicPlayer.h"
#include <qmediaplayer.h>
#include <qurl.h>
#include <qdir.h>

QMediaPlayer* MusicPlayer::backgroundPlayer = nullptr;
QMediaPlayer* MusicPlayer::effectPlayer = nullptr;
QDir MusicPlayer::_path;

void MusicPlayer::load()
{
	backgroundPlayer = new QMediaPlayer;
	effectPlayer = new QMediaPlayer;
}

void MusicPlayer::free()
{
	backgroundPlayer->deleteLater();
	effectPlayer->deleteLater();
}

void MusicPlayer::setPath(const QString& path)
{
	_path = QDir(path);
}

void MusicPlayer::playBackground(const QString& name)
{
	qDebug() << QUrl::fromLocalFile(_path.absoluteFilePath(name)).toString();
	backgroundPlayer->setMedia(QUrl::fromLocalFile(_path.absoluteFilePath(name)));
	effectPlayer->play();
}

void MusicPlayer::playEffect(const QString& name)
{
	qDebug() << QUrl::fromLocalFile(_path.absoluteFilePath(name)).toString();
	effectPlayer->setMedia(QUrl::fromLocalFile(_path.absoluteFilePath(name)));
	effectPlayer->play();
}

void MusicPlayer::setBackgroundVolume(int volume)
{
	backgroundPlayer->setVolume(volume);
}

int MusicPlayer::backgroundVolume()
{
	return backgroundPlayer->volume();
}

void MusicPlayer::setEffectVolume(int volume)
{
	effectPlayer->setVolume(volume);
}

int MusicPlayer::effectVolume()
{
	return effectPlayer->volume();
}