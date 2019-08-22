#pragma once


class NoneCopy
{
public:
	NoneCopy() = default;
	virtual ~NoneCopy() = default;
private:
	NoneCopy(const NoneCopy& copy) = delete;
	NoneCopy(NoneCopy&& copy) = delete;
	NoneCopy& operator=(const NoneCopy& rhs) = delete;
	NoneCopy& operator=(NoneCopy&& rhs) = delete;
};