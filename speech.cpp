/**
@author Alexander Hillmer
*/
#include "speech.h"
#include <iostream>
#include <string>
#include <ctime>
#include <sapi.h>
#include <atlbase.h>
#include <sphelper.h>
#include <windows.h>
#include <vector>
using namespace std;

Speech::Speech() {
	this->outputDevice = L"";
}

void Speech::selectOutputDevice() {
	HRESULT	hr = S_OK;
	ISpObjectToken*	cpAudioOutToken;
	IEnumSpObjectTokens* cpEnum;
	ULONG ulCount = 0;

	if (FAILED(CoInitialize(NULL)))
        return; 

	if (SUCCEEDED (hr))	{
	   hr = SpEnumTokens( SPCAT_AUDIOOUT, NULL, NULL, &cpEnum);
	}
	if (SUCCEEDED (hr))	{
	   hr = cpEnum->GetCount( &ulCount);
	}

	std::cout << "Select Audio Device:" << std::endl;
	unsigned int i=0;

	vector<wchar_t*> deviceIds;
	while (SUCCEEDED(hr) && i < ulCount) {
		i++;
		if (SUCCEEDED (hr))	{
		  hr = cpEnum->Next( 1, &cpAudioOutToken, NULL );
		}
		wchar_t* deviceName = nullptr;
		SpGetDescription(cpAudioOutToken, &deviceName);
		wchar_t* deviceId = nullptr;
        hr = cpAudioOutToken->GetStringValue(L"DeviceId", &deviceId);

		deviceIds.push_back(deviceId);
		std::wcout << "(" << i << ") " << deviceName << std::endl;	  
	}
	cpAudioOutToken->Release();
	cpEnum->Release();
	CoUninitialize();
	i=5;
	while(i < 1 || i > ulCount) {

		cin >> i;
		if(i < 1 || i > ulCount) {
			cout << "Invalid Input." << endl;
			cin.clear();
			cin.ignore(10000, '\n');
		}
	}
	outputDevice = deviceIds.at(i-1);
	return;
}
wchar_t* Speech::getOutputDevice() {
	return outputDevice;
}

void Speech::setOutputDevice(const char* dev) {
	const size_t cSize = strlen(dev)+1;
	outputDevice = new wchar_t[cSize];
	size_t ret;
	mbstowcs_s (&ret, outputDevice, cSize, dev, cSize);

}
void Speech::speechOutput(const char* str) {

	ISpVoice* pVoice = NULL;
	ISpObjectToken* cpAudioOutToken;
	IEnumSpObjectTokens* cpEnum;
	ULONG ulCount = 0;

    if (FAILED(::CoInitialize(NULL)))
        return;

    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);   

	if (SUCCEEDED (hr))	{
	   hr = SpEnumTokens( SPCAT_AUDIOOUT, NULL, NULL, &cpEnum);
	}
	if (SUCCEEDED (hr))	{
	   hr = cpEnum->GetCount( &ulCount);
	}

	while (SUCCEEDED(hr) && ulCount--) {
		hr = cpEnum->Next( 1, &cpAudioOutToken, NULL );
		wchar_t* deviceId = nullptr;
		if(SUCCEEDED(hr)) {
			hr = cpAudioOutToken->GetStringValue(L"DeviceId", &deviceId);
			if(wcscmp(outputDevice, deviceId) == 0) break;
		}
	}

	if (SUCCEEDED (hr))	{
		hr = pVoice->SetOutput( cpAudioOutToken, TRUE );
	}

	if(SUCCEEDED(hr)) {		
		const size_t cSize = strlen(str)+1;
		wchar_t* wc = new wchar_t[cSize];
		size_t ret;
		mbstowcs_s (&ret, wc, cSize, str, cSize);

		hr = pVoice->Speak(wc, NULL, NULL);        
	
		pVoice->Release();
        pVoice = NULL;
	}
	
	cpEnum->Release();
	cpAudioOutToken->Release();
    ::CoUninitialize();
}