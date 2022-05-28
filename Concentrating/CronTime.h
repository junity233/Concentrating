#pragma once
#include <qset.h>

class QDateTime;
class QVariant;

class CronTime
{
	CronTime(bool vaild) { _vaild = vaild; }
public:
	CronTime() { _vaild = false; };

	static CronTime fromString(const QString& data);
	QString toString()const {return _str;};

	bool check(const QDateTime& dateTime)const;

	bool isVaild()const { return _vaild; };
private:

	class CronField {
		CronField(bool vaild) { _vaild = vaild; }
	public:
		CronField() { _vaild = false; }
		CronField(const QString& data, int maxVal);

		static CronField fromString(const QString& data,int maxVal);

		bool inline check(int t)const { return _d.contains(t); }

		void setMaxValue(int maxVal);
		int inline maxValue()const { return _m; }

		bool isVaild()const { return _vaild; }

	private:
		static QSet<int> prase(QString data,int maxVal);
	private:
		QSet<int> _d;

		bool _vaild = false;
		int _m;
	};
private:
	CronField second,minute, hour, day, month, week;
	QString _str;

	bool _vaild;
};
