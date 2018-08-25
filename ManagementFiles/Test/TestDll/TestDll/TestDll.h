#pragma once
#include<Windows.h>
#include<stdio.h>

#ifndef TEST_EXPORT
#define TEST_EXPORT extern"C" __declspec(dllexport)
#else
#define TEST_EXPORT extern"C" __declspec(dllimport)
#endif


extern"C" __declspec(dllexport) void HelloWorld();