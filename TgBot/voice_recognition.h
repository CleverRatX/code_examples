#ifndef VOICE_RECOGNITION_H
#define VOICE_RECOGNITION_H

#include <string>

std::string exec(const char* cmd);
std::string transcribeAudio(const std::string& inputFile);

#endif