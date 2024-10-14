#pragma once
#ifdef _DEBUG

#define TRACKERS \
TI(Default), \
TI(Box), \
TI(Sphere)

namespace MemoryManager
{
#define TI(name) name
	enum TrackerIndex : unsigned int
	{
		TRACKERS,
		NUM_TRACKERS
	};
#undef TI
}

#endif