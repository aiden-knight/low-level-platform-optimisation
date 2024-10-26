#pragma once

namespace TimeLogger
{
	void Init();
	void Destroy();
	void LogInit(const float initTime);
	void Update(const float deltaTime);
}