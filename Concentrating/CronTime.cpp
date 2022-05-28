#include "CronTime.h"
#include <qdatetime.h>
#include <qvariant.h>


CronTime::CronField::CronField(const QString& data, int maxVal)
{
	(*this) = fromString(data, maxVal);
}

CronTime::CronField CronTime::CronField::fromString(const QString& data, int maxVal)
{
	if (data.isNull() || data.isEmpty())
		return false;

	QSet<int> d;

	for (auto i : data.split(',')) {
		QSet<int> res = prase(data.simplified(),maxVal);

		if (res.empty())
			return false;

		d += res;
	}

	CronField res;

	res._d = d;
	res._vaild = true;
	res._m = maxVal;
	return res;
}

void CronTime::CronField::setMaxValue(int maxVal)
{
	_m = maxVal;
}


QSet<int> CronTime::CronField::prase(QString data,int maxVal)
{
	if (data.isNull() || data.isEmpty())
		return {};

	int s, e, t;

	if (data[0].isNumber()) {
		if (data.contains('-')) {
			int res = sscanf(data.toStdString().c_str(), "%d-%d", &s, &e);
			if (res < 2)
				return {};
		}
		else {
			bool res;
			int t;
			t = data.toInt(&res);

			if (!res)
				return {};
			return { t };
		}
	}
	else if (data[0] == '*') {
		s = 0, e = maxVal;
	}
	else return {};

	if (data.contains('/')) {
		QString step = data.split('/')[1];
		bool res;
		
		t = step.toInt(&res);
		if (!res)
			return {};
	}
	else t = 1;

	QSet<int> res;

	if (s > e)
		qSwap(s, e);

	for (int i = s; i <= e; i += t)
		res.insert(i);

	return res;
}

CronTime CronTime::fromString(const QString& data)
{
	QStringList list = data.simplified().split(' ');

	if (list.size() != 6)
		return false;

	CronTime res;

	CronField* fields[]{ &res.second,&res.minute,&res.hour,&res.day,&res.month,&res.week };
	CronField ts[6];
	int maxVals[]{ 60,60,24,31,12,7 };

	for (int i = 0; i < 6; i++) {
		auto res = CronField::fromString(list[i], maxVals[i]);
		if (!res.isVaild())
			return false;

		ts[i] = res;
	}

	for (int i = 0; i < 6; i++)
		*fields[i] = ts[i];

	res._str = data;
	res._vaild = true;

	return res;
}

bool CronTime::check(const QDateTime& dateTime) const
{
	QTime time = dateTime.time();
	QDate date = dateTime.date();
	return second.check(time.second()) &&
		minute.check(time.minute()) &&
		hour.check(time.hour()) &&
		day.check(date.day()) &&
		month.check(date.month()) &&
		week.check(date.dayOfWeek());
}