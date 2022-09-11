#include "DateTime.h"
#include "Timespan.h"
#include "Math/Math.h"

const int32 PDateTime::DAYS_PER_MONTH[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const int32 PDateTime::DAYS_TO_MONTH[]  = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

PDateTime::PDateTime(int64 ticks) : _ticks(ticks) {}

PDateTime::PDateTime(int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 second, int32 millisecond)
{
	JG_ASSERT(IsValidDay(year, month, day, hour, minute, second, millisecond));

	int32 totalDays = 0;

	if ((month > 2) && IsLeapYear(year))
	{
		totalDays += 1;
	}

	year  -= 1;
	month -= 1;

	totalDays += year * 365;
	totalDays += year / 4;							
	totalDays -= year / 100;						
	totalDays += year / 400;						
	totalDays += DAYS_TO_MONTH[month];				
	totalDays += day - 1;							

	_ticks = totalDays * ETimespan::TicksPerDay
		+ hour * ETimespan::TicksPerHour
		+ minute * ETimespan::TicksPerMinute
		+ second * ETimespan::TicksPerSecond
		+ millisecond * ETimespan::TicksPerMillisecond;
}

PDateTime PDateTime::operator+(const PDateTime& dt) const
{
	return PDateTime(GetTicks() + dt.GetTicks());
}

PDateTime PDateTime::operator+(const PTimespan& ts) const
{
	return PDateTime(GetTicks() + ts.GetTicks());
}

PTimespan PDateTime::operator-(const PDateTime& dt) const
{
	return PTimespan(GetTicks() - dt.GetTicks());
}

PTimespan PDateTime::operator-(const PTimespan& ts) const
{
	return PTimespan(GetTicks() - ts.GetTicks());
}

bool PDateTime::operator ==(const PDateTime& dt) const
{
	return GetTicks() == dt.GetTicks();
}

bool PDateTime::operator !=(const PDateTime& dt) const
{
	return GetTicks() != dt.GetTicks();
}

bool PDateTime::operator<(const PDateTime& dt) const
{
	return GetTicks() < dt.GetTicks();
}

bool PDateTime::operator<=(const PDateTime& dt) const
{
	return GetTicks() <= dt.GetTicks();
}

bool PDateTime::operator>(const PDateTime& dt) const
{
	return GetTicks() > dt.GetTicks();
}

bool PDateTime::operator >=(const PDateTime& dt) const
{
	return GetTicks() >= dt.GetTicks();
}

void PDateTime::GetDate(int32& outYear, int32& outMonth, int32& outDay) const
{
	int32 i, j, k, l, n;

	l = (int32)HMath::FloorToFloat64(GetJulianDay() + 0.5) + 68569;
	n = 4 * l / 146097;
	l = l - (146097 * n + 3) / 4;
	i = 4000 * (l + 1) / 1461001;
	l = l - 1461 * i / 4 + 31;
	j = 80 * l / 2447;
	k = l - 2447 * j / 80;
	l = j / 11;
	j = j + 2 - 12 * l;
	i = 100 * (n - 49) + i + l;

	outYear  = i;
	outMonth = j;
	outDay   = k;
}

int32 PDateTime::GetYear() const
{
	int32 year, month, day;
	GetDate(year, month, day);
	return year;
}

int32 PDateTime::GetMonth() const
{
	int32 year, month, day;
	GetDate(year, month, day);
	return month;
}

int32 PDateTime::GetDay() const
{
	int32 year, month, day;
	GetDate(year, month, day);
	return day;
}

int32 PDateTime::GetHour() const
{
	return GetTotalHours() % 24;
}

int32 PDateTime::GetMinute() const
{
	return GetTotalMinutes() % 60;
}

int32 PDateTime::GetSecond() const
{
	return GetTotalSeconds() % 60;
}

int32 PDateTime::GetMiliSecond() const
{
	return GetTtoalMilliseconds() % 1000;
}

int64 PDateTime::GetTotalDays() const
{
	return _ticks / ETimespan::TicksPerDay;
}
int64 PDateTime::GetTotalHours() const
{
	return _ticks / ETimespan::TicksPerHour;
}
int64 PDateTime::GetTotalMinutes() const
{
	return _ticks / ETimespan::TicksPerMinute;
}
int64 PDateTime::GetTotalSeconds() const
{
	return  _ticks / ETimespan::TicksPerSecond;
}
int64 PDateTime::GetTtoalMilliseconds() const
{
	return _ticks / ETimespan::TicksPerMillisecond;
}

EDayOfWeek PDateTime::GetDayOfWeek() const
{
	return static_cast<EDayOfWeek>(GetTotalDays() % 7);
}

float64 PDateTime::GetJulianDay() const
{
	return (float64)(1721425.5 + _ticks / ETimespan::TicksPerDay);
}

int32 PDateTime::DayInMonth(int32 year, int32 month)
{
	JG_ASSERT((month >= 1) && (month <= 12));

	if ((month == 2) && IsLeapYear(year))
	{
		return 29;
	}

	return DAYS_PER_MONTH[month];
}

int32 PDateTime::DayInYear(int32 year)
{
	int32 day = 0;
	for (int32 currMonth = 1; currMonth <= 12; ++currMonth)
	{
		day += DayInMonth(year, currMonth);
	}

	return day;
}

int64 PDateTime::GetTicks() const
{
	return _ticks;
}

bool PDateTime::IsLeapYear(int32 year)
{
	if ((year % 4 == 0 && year % 100 > 0) || year % 400 == 0)
	{
		return true;
	}

	return false;
}

bool PDateTime::IsValidDay(int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 second, int32 millisecond)
{
	return (year >= 1) && (year <= 9999) &&
		(month >= 1) && (month <= 12) &&
		(day >= 1) && (day <= DayInMonth(year, month)) &&
		(hour >= 0) && (hour <= 23) &&
		(minute >= 0) && (minute <= 59) &&
		(second >= 0) && (second <= 59) &&
		(millisecond >= 0) && (millisecond <= 999);
}

PString PDateTime::ToString() const
{
	int32 year, month, day;
	GetDate(year, month, day);

	return PString::Format("%04i.%02i.%02i:%02i.%02i.%02i", year, month, day, GetHour(), GetMinute(), GetSecond());
}

bool PDateTime::Parse(const PString& str)
{
	PString replaceStr = str;
	replaceStr.ReplaceAll(".", " ").ReplaceAll(":", " ");

	HList<PString> tokens = replaceStr.Split(' ');
	if (tokens.size() != 6)
	{
		return false;
	}

	int32 year   = tokens[0].ToInt(0);
	int32 month  = tokens[1].ToInt(0);
	int32 day    = tokens[2].ToInt(0);
	int32 hour   = tokens[3].ToInt(0);
	int32 minute = tokens[4].ToInt(0);
	int32 second = tokens[5].ToInt(0);

	*this = PDateTime(year, month, day, hour, minute, second);

	return true;
}

void PDateTime::WriteJson(PJsonData& json) const
{
	json.AddMember(ToString());
}

void PDateTime::ReadJson(const PJsonData& json)
{
	PString str;
	json.GetData(&str);

	Parse(str);
}

PDateTime PDateTime::Now()
{
	HSystemClock_TimePoint now = HSystemClock::now();

	time_t time = HSystemClock::to_time_t(now);
	tm*    tm	= std::localtime(&time);
	int64  ms	= (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) - std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())).count();

	return PDateTime(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, (int32)ms);
}
