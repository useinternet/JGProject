#include "pch.h"
#include "Timer.h"


void Timer::Start() {
	IsStop = false;
}
void Timer::Reset() {
	RunningTime = 0.0f;
	StopTime = 0.0f;
	IsStop = true;
}
void Timer::Pause() {
	IsStop = true;
}
void Timer::Tick(float deltaTime) {
	(IsStop) ? StopTime += deltaTime : RunningTime += deltaTime;
}
bool Timer::IsRunning() const {
	return !IsStop;
}
float Timer::GetRunTime() const {
	return RunningTime;
}
float Timer::GetPasueTime() const {
	return StopTime;
}


void StopWatch::Set(float GoalTime) {
	this->GoalTime = GoalTime;
}
void StopWatch::Start() {
	IsStop = false;
}
void StopWatch::Reset() {
	RunningTime = 0.0f;
	IsGoal = false;
	IsStop = true;
}
void StopWatch::Pause() {
	IsStop = true;
}
void StopWatch::Tick(float deltaTime) {
	if (IsStop || IsGoal) return;

	RunningTime += deltaTime;

	if (RunningTime >= GoalTime)
		IsGoal = true;
}
bool StopWatch::IsCompelete() const {
	return IsGoal;
}
float StopWatch::GetRemainingTime() const {
	return std::max<float>(0.0f, GoalTime - RunningTime);
}
float StopWatch::GetRunningTime() const {
	return std::min<float>(GoalTime, RunningTime);
}
float StopWatch::GetGoalTime() const {
	return GoalTime;
}



void TimerByFunction::Set(const std::function<void()>& function, float delay , int repeat ) {
	Function = function;
	Delay = delay;
	Repeat = repeat;
	RepeatCount = repeat;
}
void TimerByFunction::Start() {
	IsStop = false;
}
void TimerByFunction::Reset() {
	RunningTime = 0.0f;
	RepeatCount = Repeat;
	IsStop = true;
}
void TimerByFunction::Pause() {
	IsStop = true;
}
void TimerByFunction::Tick(float deltaTime) {
	if (IsStop || IsCompelete()) return;

	RunningTime += deltaTime;

	if (RunningTime >= Delay)
	{
		RunningTime -= Delay;

		Function();

		RepeatCount--;
		RepeatCount = std::max<int>(0, RepeatCount);
	}
}
int TimerByFunction::RemainingRepeatCount() const {

	if (Repeat == -1)
		return -1;
	else
	{
		return std::max<int>(0, RepeatCount);
	}
}
bool TimerByFunction::IsCompelete() const {
	if (Repeat == -1)
		return false;
	else
	{
		return (RemainingRepeatCount() == 0);
	}
}