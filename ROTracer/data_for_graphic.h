#ifndef ROTRACER_H   // bu k�t�phaneye birden fazla yerden �ag�rmak i�in  �simlendirmenini k�saltmas� �eklinde yazd�k burdaki ismi
#define ROTRACER_H    
// ----------------------------B�T�N TANIMLAMA ��LEMLER�N� BURDA YAPIYORUZ --------------------------
#define _CRT_SECURE_NO_WARNINGS
#pragma warning (disable : 4996)
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui.h>               // imgui bile�enlerini kullnmak i�in 
#include <imgui_internal.h>
#include <implot/implot.h>       // implot bile�enleri i�in 
#include <regex>
#include <string.h>
#include <zmq.h>
#include <thread>
#include <stdio.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/ostreamwrapper.h>
#include <fstream>
#include <mutex>

#ifndef PI
#define PI 3.14159265358979323846
#endif

// AgvStreamData
class AgvData {
public: 
    int Fx;                       //x konum
    int Fy;                       // y konum
	int Bx;                       //x konum
	int By;                       // y konum
    float Angle;                 // a��
    int WSpeed;                  // yaz�lan h�z
    int RSpeed;                  // okunan h�z 
    float WAngle;                // yaz�lan teker a��s� 
    float RAngle;                // okunan teker a��s� 

	float CellLx;
	float CellLy;
	float CellSx;
	float CellSy;
};

class NetData {
public:
	std::string DeviceMacAddress;     //mac adresi    
	int Ping;                         // ping degeri 
	int ReceivedRate;                 // al�nma oran�
	int Signal;                       // wifi sinyal kalitesi
	int Speed;                        // wifi h�z� 
	std ::string SSID;                // wifi ismi 
	std::string Status;               // wifi durumu 
	int TransmitededRate;             // iletim oran�
	
};

struct ScrollingBuffer {
	int MaxSize;
	int Offset;
	std::mutex mx;
	ImVector<ImVec2> Data;
	ScrollingBuffer(int max_size = 2000000) {
		MaxSize = max_size;
		Offset = 0;
		Data.reserve(MaxSize);
	}
	void AddPoint(float x, float y) {
		mx.lock();
		if (Data.size() < MaxSize)
			Data.push_back(ImVec2(x, y));
		else {
			Data[Offset] = ImVec2(x, y);
			Offset = (Offset + 1) % MaxSize;
		}
		mx.unlock();
	}
	void Erase() {
		mx.lock();
		if (Data.size() > 0) {
			Data.resize(0);
			Offset = 0;
		}
		mx.unlock();
	}
};



class GraphicData {
public:
	float Time;
	float History = 1.0f;
	bool Enabled = true;
	bool Visibility = false;
};

class WheelGraphicData :public GraphicData
{
public:
	ScrollingBuffer ReadingWheelAngel;
	ScrollingBuffer WritingWheelAngel;

};

class SpeedGraphicData :public GraphicData
{
public:
	ScrollingBuffer ReadingSpeed;        
	ScrollingBuffer WritingSpeed;      
};

class AgvAngleGraphicData : public GraphicData
{
public:
	ScrollingBuffer AgvAngle;
};

class AgvPositionGraphicData : public GraphicData
{
public:
	ScrollingBuffer AgvFrontPosition;
	ScrollingBuffer AgvBackPosition;

	ScrollingBuffer Route;
	ScrollingBuffer Curve;
	ScrollingBuffer Simu;

	void Erase() {
		AgvFrontPosition.Erase();
		AgvBackPosition.Erase();
		Route.Erase();
		Curve.Erase();
		Simu.Erase();
	}

	float AgvAngle;
};

 //net k�sm�
class ReceivedRateData :public GraphicData
{
public:
	ScrollingBuffer StokingReceivedRate;
	
};

class SignalPingSpeedData :public GraphicData
{
public:
	ScrollingBuffer StokingSignal;
	ScrollingBuffer StokingPing;
	ScrollingBuffer StokingWifiSpeed;

};

class TransmitededRateData :public GraphicData
{
public:
	ScrollingBuffer StokingTransmitededRate;
};


class ROTracer
{   
public:                   //constructor 
	ROTracer();
	~ROTracer();

	void SpeedPage();      // agv fonksiyonlar 
	void LoginPage();
	void AgvPositionPage();
	void WheelPage();
	void AgvAngelPage();

	//----------------
	//void WifiSpeedPage();    // net fonksiyonlar� 
	//void PingPage();
	void ReceivedRatePage();
	void SignalPingSpeedPage();
	void TransmitededRatePage();


	void StartStreamParser();
	void StopStreamParser();


	AgvData *Agv;          //agv class�na ait  nesne olu�turduk  
	NetData *Net;
	SpeedGraphicData *SpeedGraphic;    // SpeedGraphicData class'�n�n nesnesini olu�turduk 
	WheelGraphicData *WheelGraphic;
	AgvAngleGraphicData *AgvAngleGraphic;
	AgvPositionGraphicData *AgvPositionGraphic;

	//net k�sm�
	ReceivedRateData* ReceivedRateGraphic;
	SignalPingSpeedData* SignalPingSpeedGraphic;
	TransmitededRateData* TransmitededRateGraphic;

 
	char IpAddress[16] = "192.168.2.125";
	char PortAddress[8] = "5556";
	bool _isRunning;
 


private:
	

	bool _zmqLoopFlag;               //zmq data parse i�lemini yap�p yapmama  
	bool _loginPageVisibility;

	void ZMQDataStreamParser();       // parse i�lemi private onun i�in burda yoksa yukar� da yazabilirdik
	
};

#endif