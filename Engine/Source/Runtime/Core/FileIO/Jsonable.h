#pragma once

class PJsonData;
class IJsonable
{
	friend PJsonData;
public:
	virtual ~IJsonable() = default;
protected:
	virtual void WriteJson(PJsonData& json) const {}
	virtual void ReadJson(const PJsonData& json) {}
};