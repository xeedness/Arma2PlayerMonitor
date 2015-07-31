/**
@author Alexander Hillmer
*/
#ifndef SPEECH_H
#define SPEECH_H


class Speech {
public:
	Speech();
	void selectOutputDevice();
	wchar_t* getOutputDevice();
	void setOutputDevice(const char* dev);
	void speechOutput(const char* str);

private:
	wchar_t*  outputDevice;
};

#endif