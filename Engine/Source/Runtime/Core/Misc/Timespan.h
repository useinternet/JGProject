#pragma once
#include "CoreDefines.h"
#include "Memory/Memory.h"
#include "String/String.h"
#include "FileIO/Json.h"

class ETimespan
{
public:
	enum : int64
	{
		MaxTicks = 9223372036854775807,
		MinTicks = -9223372036854775807 - 1,

		TicksPerMillisecond = 10000,
		TicksPerSecond      = 10000000,
		TicksPerMinute      = 600000000,
		TicksPerHour        = 36000000000,
		TicksPerDay         = 864000000000,

		MillisecondsPerSecond = 1000,
		MillisecondsPerMinute = 60000,
		MillisecondsPerHour = 3600000,
		MillisecondsPerDay = 86400000,

		SecondsPerMinute = 60,
		SecondsPerHour   = 3600,
		SecondsPerDay    = 86400,

		MinutesPerHour = 60,
		MinutesPerDay  = 1440,

		HoursPerDay = 24,
	};

};

class PTimespan : public IMemoryObject, public IJsonable
{
	int64 _ticks;
public:
	PTimespan(int64 ticks = 0);
	PTimespan(int32 days, int32 hours, int32 minutes, int32 seconds = 0, int32 milliseconds = 0);
	
	virtual ~PTimespan() = default;

public:
	PTimespan operator+(const PTimespan& ts) const;
	PTimespan operator-(const PTimespan& ts) const;
	PTimespan& operator+=(const PTimespan& ts);
	PTimespan& operator-=(const PTimespan& ts);

	bool operator==(const PTimespan& ts) const;
	bool operator!=(const PTimespan& ts) const;

	bool operator<=(const PTimespan& ts) const;
	bool operator>=(const PTimespan& ts) const;
	bool operator<(const PTimespan& ts) const;
	bool operator>(const PTimespan& ts) const;

	PString ToString() const;
	bool Parse(const PString& str);

public:
	int64  GetTicks() const;
	int32 GetDays() const;
	int32 GetHours() const;
	int32 GetMinutes() const;
	int32 GetSeconds() const;
	int32 GetMilliseconds() const;
	int64 GetTotalDays() const;
	int64 GetTotalHours() const;
	int64 GetTotalMinutes() const;
	int64 GetTotalSeconds() const;
	int64 GetTotalMilliseconds() const;

protected:
	virtual void WriteJson(PJsonData& json) const;
	virtual void ReadJson(const PJsonData& json);

public:
	static PTimespan FromMilliseconds(int64 milliseconds);
	static PTimespan FromSeconds(int64 seconds);
	static PTimespan FromMinutes(int64 minutes);
	static PTimespan FromHours(int64 hours);
	static PTimespan FromDays(int64 days);
};