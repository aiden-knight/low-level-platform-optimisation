#include "TimeLogger.h"
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
        std::string* filename = nullptr;
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
        filename = new std::string(buffer);
    }

    void Destroy()
    {
        delete filename;
        filename = nullptr;
    }

    void LogInit(const float initTime)
    {
        std::ofstream outStream("logs/" + *filename + ".txt");
        outStream << "Initialisation took (in milliseconds): " << initTime << std::endl;
        outStream.close();
    }

    void Update(const float deltaTime)
	{
        deltaTimeArray[deltaTimeIndex++] = deltaTime;
        if (deltaTimeIndex == deltaTimeArray.size())
        {
            deltaTimeIndex = 0;

            float sum = 0;
            for (int i = 0; i < deltaTimeArray.size(); i++)
            {
                sum += deltaTimeArray[i];
            }
            sum /= deltaTimeArray.size();
            sum = 1 / sum;

            char buffer[50];
            tm timeInfo = GetTimeInfo();
            strftime(buffer, sizeof(buffer), "%H-%M-%S", &timeInfo);
            std::string temp(buffer);

            std::ofstream outStream("logs/" + *filename + ".txt", std::ios::app);
            outStream << temp << ": average fps over last " << deltaTimeArray.size() << " frames: " << sum << std::endl;
            outStream.close();
        }
	}
}