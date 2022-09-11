#include "Timespan.h"


PTimespan::PTimespan(int64 ticks) : _ticks(ticks)
{ }

PTimespan::PTimespan(int32 days, int32 hours, int32 minutes, int32 seconds, int32 milliseconds)
{
	_ticks += milliseconds * ETimespan::TicksPerMillisecond;
	_ticks += seconds * ETimespan::TicksPerSecond;
	_ticks += minutes * ETimespan::TicksPerMinute;
	_ticks += hours   * ETimespan::TicksPerHour;
	_ticks += days    * ETimespan::TicksPerDay;
}

PTimespan PTimespan::operator+(const PTimespan& ts) const
{
	return PTimespan(GetTicks() + ts.GetTicks());
}

PTimespan PTimespan::operator-(const PTimespan& ts) const
{
	return PTimespan(GetTicks() - ts.GetTicks());
}

PTimespan& PTimespan::operator+=(const PTimespan& ts)
{
	_ticks += ts.GetTicks();

	return *this;
}

PTimespan& PTimespan::operator-=(const PTimespan& ts)
{
	_ticks -= ts.GetTicks();

	return *this;
}

bool PTimespan::operator==(const PTimespan& ts) const
{
	return GetTicks() == ts.GetTicks();
}

bool PTimespan::operator!=(const PTimespan& ts) const
{
	return GetTicks() != ts.GetTicks();
}

bool PTimespan::operator<=(const PTimespan& ts) const
{
	return GetTicks() <= ts.GetTicks();
}

bool PTimespan::operator>=(const PTimespan& ts) const
{
	return GetTicks() >= ts.GetTicks();
}

bool PTimespan::operator<(const PTimespan& ts) const
{
	return GetTicks() < ts.GetTicks();
}

bool PTimespan::operator>(const PTimespan& ts) const
{
	return GetTicks() > ts.GetTicks();
}

PString PTimespan::ToString() const
{
	return PString::Format("%02i.%02i.%02i.%02i.%03i", GetDays(), GetHours(), GetMinutes(), GetSeconds(), GetMilliseconds());
}

bool PTimespan::Parse(const PString& str)
{
	PString replaceStr = str;
	replaceStr.ReplaceAll(".", " ").ReplaceAll(":", " ");

	HList<PString> tokens = str.Split(' ');
	if (tokens.size() != 5)
	{
		return false;
	}

	int32 day  = tokens[0].ToInt(0);
	int32 hour = tokens[1].ToInt(0);
	int32 minute   = tokens[2].ToInt(0);
	int32 second  = tokens[3].ToInt(0);
	int32 millisecond = tokens[4].ToInt(0);

	*this = PTimespan(day, hour, minute, second, millisecond);

	return true;
}

int64 PTimespan::GetTicks() const
{
	return _ticks;
}

int32 PTimespan::GetDays() const
{
	return (int32)GetTotalDays();
}

int32 PTimespan::GetHours() const
{
	return GetTotalHours() % 24;
}

int32 PTimespan::GetMinutes() const
{
	return GetTotalMinutes() % 60;
}

int32 PTimespan::GetSeconds() const
{
	return GetTotalSeconds() % 60;
}

int32 PTimespan::GetMilliseconds() const
{
	return GetTotalMilliseconds() % 1000;
}

int64 PTimespan::GetTotalDays() const
{
	return _ticks / ETimespan::TicksPerDay;
}

int64 PTimespan::GetTotalHours() const
{
	return _ticks / ETimespan::TicksPerHour;
}

int64 PTimespan::GetTotalMinutes() const
{
	return _ticks / ETimespan::TicksPerMinute;
}

int64 PTimespan::GetTotalSeconds() const
{
	return _ticks / ETimespan::TicksPerSecond;
}

int64 PTimespan::GetTotalMilliseconds() const
{
	return _ticks / ETimespan::TicksPerMillisecond;
}

void PTimespan::WriteJson(PJsonData& json) const
{
	json.AddMember(ToString());
}

void PTimespan::ReadJson(const PJsonData& json)
{
	PString str;
	json.GetData(&str);

	Parse(str);
}

PTimespan PTimespan::FromMilliseconds(int64 miliSeconds)
{
	return PTimespan(miliSeconds * ETimespan::TicksPerMillisecond);
}

PTimespan PTimespan::FromSeconds(int64 seconds)
{
	return PTimespan(seconds * ETimespan::TicksPerSecond);
}

PTimespan PTimespan::FromMinutes(int64 minutes)
{
	return PTimespan(minutes * ETimespan::TicksPerMinute);
}

PTimespan PTimespan::FromHours(int64 hours)
{
	return PTimespan(hours * ETimespan::TicksPerHour);
}

PTimespan PTimespan::FromDays(int64 days)
{
	return PTimespan(days * ETimespan::TicksPerDay);
}
