#include "TimeLogger.h"
#include "globals.h"
#include <array>
#include <ctime>
#include <fstream>
#include <string>

namespace TimeLogger
{
	namespace
	{
		unsigned int deltaTimeIndex = 0;
		std::array<float, 50> deltaTimeArray;
        std::ofstream* outStream = nullptr;
	}

    tm GetTimeInfo()
    {
        time_t rawTime;
        tm timeInfo;

        time(&rawTime);
        localtime_s(&timeInfo, &rawTime);
        return timeInfo;
    }

    void Init()
    {
        char buffer[80];
        tm timeInfo = GetTimeInfo();
#ifdef _DEBUG
        strftime(buffer, sizeof(buffer), "%d-%m-%Y %H-%M-%S_DEBUG", &timeInfo);
#else
        strftime(buffer, sizeof(buffer), "%d-%m-%Y %H-%M-%S_RELEASE", &timeInfo);
#endif
        std::string filename(buffer);

        outStream = new std::ofstream("logs/" + filename + ".txt");
    }

    void Destroy()
    {
        if (outStream != nullptr)
        {
            outStream->close();
            delete outStream;
        }
    }

    void LogInit(const float initTime)
    {
        if (outStream == nullptr) return;
        *outStream << "Initialisation took (in milliseconds): " << initTime << std::endl;
        *outStream << "Octree Depth: " << octreeDepth << ". Thread count: " << threadCount << std::endl;
    }

    void Update(const float deltaTime)
	{
        deltaTimeArray[deltaTimeIndex++] = deltaTime;
        if (deltaTimeIndex == deltaTimeArray.size())
        {
            deltaTimeIndex = 0;
            if (outStream == nullptr) return;

            float sum = 0;
            for (int i = 0; i < deltaTimeArray.size(); i++)
            {
                sum += deltaTimeArray[i];
            }
            sum /= deltaTimeArray.size();

            *outStream << "\nCounts - Cube: " << boxCount << ", Sphere:" << sphereCount << ", Total: " << boxCount + sphereCount << std::endl;
            *outStream << "Average time (in seconds) taken to update physics over last " << deltaTimeArray.size() << " frames: " << sum << std::endl;
        }
	}
}