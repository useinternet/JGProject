#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "String/String.h"
#include "FileIO/Json.h"

class PTimespan;

enum class EDayOfWeek
{
	Monday = 0,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Sunday,
};

enum class EMonthOfYear
{
	January = 1,
	February,
	March,
	April,
	May,
	June,
	July,
	August,
	September,
	October,
	November,
	December
};

class PDateTime : public IMemoryObject, public IJsonable
{
	static const int32 DAYS_PER_MONTH[];
	static const int32 DAYS_TO_MONTH[];

	int64 _ticks;
public:
	PDateTime(int64 ticks = 0);
	PDateTime(int32 year, int32 month, int32 day, int32 hour = 0, int32 minute = 0, int32 second = 0, int32 milisecond = 0);
	virtual ~PDateTime() = default;

public:
	PDateTime operator+(const PDateTime& dt) const;
	PDateTime operator+(const PTimespan& dt) const;

	PTimespan operator-(const PDateTime& dt) const;
	PTimespan operator-(const PTimespan& dt) const;

	bool operator==(const PDateTime& dt) const;
	bool operator!=(const PDateTime& dt) const;

	bool operator<(const PDateTime& dt) const;
	bool operator<=(const PDateTime& dt) const;

	bool operator>(const PDateTime& dt) const;
	bool operator>=(const PDateTime& dt) const;
	
	void GetDate(int32& outYear, int32& outMonth, int32& outDay) const;
	int32 GetYear() const;
	int32 GetMonth() const;
	int32 GetDay() const;
	int32 GetHour() const;
	int32 GetMinute() const;
	int32 GetSecond() const;
	int32 GetMiliSecond() const;
	int64 GetTotalDays() const;
	int64 GetTotalHours() const;
	int64 GetTotalMinutes() const;
	int64 GetTotalSeconds() const;
	int64 GetTtoalMilliseconds() const;
	int64 GetTicks() const;

	float64 GetJulianDay() const;
	EDayOfWeek GetDayOfWeek() const;

	int32 DayInMonth(int32 year, int32 month);
	int32 DayInYear(int32 year);

	bool IsLeapYear(int32 year);
	bool IsValidDay(int32 year, int32 month, int32 day, int32 hour, int32 minute, int32 second, int32 millisecond);

	PString ToString() const;
	bool Parse(const PString& str);

protected:
	virtual void WriteJson(PJsonData& json) const;
	virtual void ReadJson(const PJsonData& json);

public:
	static PDateTime Now();
};

