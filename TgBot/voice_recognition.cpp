#include "voice_recognition.h"
#include <array>
#include <cstdio>
#include <string>
#include <cstdlib>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

// Выполнение команды
std::string exec(const char* cmd)
{
    char buffer[128];
    std::string result = "";

    FILE* pipe = popen(cmd, "r");
    if (pipe == NULL)
    {
        return "Ошибка выполнения команды";
    }

    while (fgets(buffer, 128, pipe) != NULL)
    {
        result = result + buffer;
    }

    pclose(pipe);
    return result;
}

// Транскрибация аудио
std::string transcribeAudio(const std::string& inputFile)
{
    // Временный файл
    std::string wavFile = "temp_" + std::to_string(rand()) + ".wav";

    // Конвертация в wav
    std::string ffmpegCmd = "ffmpeg -i " + inputFile + " -ar 16000 -ac 1 -c:a pcm_s16le " + wavFile + " -y 2>&1";
    exec(ffmpegCmd.c_str());

    // Запуск whisper
    std::string whisperPath = "C:/dev/whisper.cpp/build/bin/main.exe";
    std::string modelPath = "C:/dev/whisper.cpp/models/ggml-base.bin";
    std::string whisperCmd = whisperPath + " -m " + modelPath + " -f " + wavFile + " -nt 2>&1";

    std::string result = exec(whisperCmd.c_str());

    remove(wavFile.c_str());
    remove(inputFile.c_str());

    // Берём последнюю строку
    size_t pos = result.rfind('\n');
    if (pos != std::string::npos)
    {
        result = result.substr(pos + 1);
    }

    return result;
}