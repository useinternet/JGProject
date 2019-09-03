#pragma once
#include <iostream>
#include <functional>
#include <algorithm>
class Timer
{
	float RunningTime = 0.0f;
	float StopTime = 0.0f;
	bool  IsStop = true;
public:
	void Start(); 
	void Reset();
	void Pause();
	void Tick(float deltaTime);
	bool IsRunning() const;
	float GetRunTime() const;
	float GetPasueTime() const;
};
class StopWatch
{
public:
	float GoalTime = 0.0f;
	float RunningTime = 0.0f;
	bool  IsGoal = false;
	bool  IsStop = true;
public:
	void Set(float GoalTime); 
	void Start();
	void Reset(); 
	void Pause(); 
	void Tick(float deltaTime);
	bool IsCompelete() const;
	float GetRemainingTime() const;
	float GetRunningTime() const;
	float GetGoalTime() const;
};
class TimerByFunction
{
	float RunningTime = 0.0f;
	std::function<void()> Function;
	float Delay = 0.0f;
	int   Repeat = -1;
	int   RepeatCount = 0;
	bool  IsStop = true;
public:
	void Set(const std::function<void()>& function, float delay = 0.0f, int repeat = -1);
	void Start();
	void Reset(); 
	void Pause(); 
	void Tick(float deltaTime);
	int  RemainingRepeatCount() const; 
	bool IsCompelete() const;
};

