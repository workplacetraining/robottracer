﻿#include "data_for_graphic.h"   //data_for_graphic deki veriler için çagırıyoruz 
using namespace std;


ROTracer::ROTracer() {
	this->Agv = new AgvData();
	this->Net = new NetData();

	this->SpeedGraphic = new SpeedGraphicData();
	this->WheelGraphic = new WheelGraphicData();
	this->AgvAngleGraphic = new AgvAngleGraphicData();

	this->AgvPositionGraphic = new AgvPositionGraphicData();


	this->ReceivedRateGraphic = new ReceivedRateData();
	this->SignalPingSpeedGraphic = new SignalPingSpeedData();
	this->TransmitededRateGraphic = new TransmitededRateData();

	_loginPageVisibility = true;
}


ROTracer::~ROTracer() {
	delete this->Agv;
	delete this->Net;

}


void ROTracer::StartStreamParser() {
	if (!_isRunning)
	{
		this->_zmqLoopFlag = true;
		std::thread lth(&ROTracer::ZMQDataStreamParser, this);
		lth.detach();
	}
}


void ROTracer::StopStreamParser() {
	this->_zmqLoopFlag = false;
}




/*
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}


static inline ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a)
{
	return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a);
}*/
static ImPlotPoint point1, point2, mousePos, center;
static bool drawLine = false;

void ROTracer::LoginPage() {


	if (this->_loginPageVisibility == true)
	{
		ImGui::SetNextWindowSize(ImVec2(920, 300));

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_::ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		ImGui::Begin("PAGE", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);  //, &no_collapse
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;
		style.WindowRounding = 0.9f; // Pencere köşelerinin yuvarlanma miktarı
		colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.95f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
		colors[ImGuiCol_Button] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		ImGui::Text(" ");
		ImGui::Text(" ");
		ImGui::Text(" ");
		ImGui::Text(" ");
		ImGui::Text(" ");
		// Pencere başlığı ve IP adresi giriş kutusu
		ImGui::SetCursorPosX(300);
		ImGui::Text("Enter IP Address");
		ImGui::SameLine();
		ImGui::SetCursorPosX(510);
		ImGui::Text("Port Address");
		ImGui::Spacing();

		ImGui::SetCursorPosX(300);
		ImGui::PushItemWidth(200);
		ImGui::InputText("##Ip", IpAddress, 24, ImGuiInputTextFlags_CharsNoBlank);
		ImGui::SameLine();


		ImGui::PushItemWidth(100);
		ImGui::InputText("##Port", PortAddress, 8, ImGuiInputTextFlags_CharsNoBlank);
		ImGui::Spacing();
		ImGui::Spacing();

		// IP adresi girişi ve "Login" düğmesi
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Login", NULL, true).x) / 2);

		// IP adresi girişi ve "Login" düğmesi
		if (ImGui::Button("Login")) {
			// IP adresi doğrulama
			std::regex ip_regex("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
			if (!std::regex_match(IpAddress, ip_regex)) {
				ImGui::OpenPopup("Uyari");
			}
			else {

				this->_loginPageVisibility = false;
				this->StartStreamParser();
			}
		}

		// IP adresi uyarı mesajı
		if (ImGui::BeginPopupModal("Uyari", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
			ImGui::Text("Please enter a valid IP address.");
			if (ImGui::Button("Ok")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::End();


	}
	else {

		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("Agv"))
		{
			ImGui::MenuItem("Wheel Angle", "", &this->WheelGraphic->Visibility);
			ImGui::MenuItem("Speed", "", &this->SpeedGraphic->Visibility);
			ImGui::MenuItem("Angle", "", &this->AgvAngleGraphic->Visibility);
			ImGui::MenuItem("Position", "", &this->AgvPositionGraphic->Visibility);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();






		if (this->SpeedGraphic->Visibility)
		{

			ImGui::BeginChild("Speed Page", ImVec2(1000, 600), true);
			this->SpeedGraphic->Visibility = true;
			this->SpeedPage();
			ImGui::EndChild();
		}

		if (this->WheelGraphic->Visibility)
		{


			ImGui::SameLine();

			ImGui::BeginChild("Wheel Angle Page", ImVec2(1000, 600), true);



			this->WheelGraphic->Visibility = true;
			this->WheelPage();
			ImGui::EndChild();

		}



		if (this->AgvAngleGraphic->Visibility)
		{

			ImGui::BeginChild("Agv Angle Page", ImVec2(1000, 600), true);

			this->AgvAngleGraphic->Visibility = true;
			this->AgvAngelPage();
			ImGui::EndChild();

		}

		if (this->AgvPositionGraphic->Visibility)
		{
			ImGui::SameLine();

			ImGui::BeginChild("Agv Position Page", ImVec2(1000, 600), true);

			this->AgvPositionGraphic->Visibility = true;
			this->AgvPositionPage();
			ImGui::EndChild();
		}

		/*	if (this->ReceivedRateGraphic->Visibility)
			{
				ImGui::SameLine();

				ImGui::BeginChild("received Position Page", ImVec2(1000, 600), true);

				this->ReceivedRateGraphic->Visibility = true;
				this->ReceivedRatePage();
				ImGui::EndChild();
			}*/

			// END MENU

			//// Arrange the visibility flags in a vector for easier manipulation
			//std::vector<bool*> visibilityFlags = { &this->WheelGraphic->Visibility, &this->SpeedGraphic->Visibility,
			//									   &this->AgvAngleGraphic->Visibility, &this->AgvPositionGraphic->Visibility };

			//// Calculate the number of columns
			//const int columns = 3;
			//int visibleItemCount = 0;
			//for (const bool* flag : visibilityFlags) {
			//	if (*flag) visibleItemCount++;
			//}
			//const int rows = std::max(1, (visibleItemCount + columns - 1) / columns);

			//// Calculate the item width and height
			//const float itemWidth = ImGui::GetWindowWidth() / columns;
			//const float itemHeight = 30;
			//bool visibilityFlagNames[] = {};
			//// Draw the visible items
			//int itemIndex = 0;
			//for (int row = 0; row < rows; row++) {
			//	ImGui::Columns(columns, nullptr, false);
			//	for (int col = 0; col < columns; col++) {
			//		if (itemIndex >= visibilityFlags.size()) break;
			//		if (*visibilityFlags[itemIndex]) {
			//			ImGui::SetColumnWidth(col, itemWidth);
			//			ImGui::SetCursorPosX(col * itemWidth);
			//			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + itemHeight * row);
			//			if (itemIndex == visibleItemCount - 1 && rows > 1) {
			//				ImGui::SetNextWindowContentSize(ImVec2(itemWidth, itemHeight));
			//				ImGui::SetNextWindowPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + itemHeight));
			//			}
			//			if (itemIndex == 0) ImGui::BeginChild("##child", ImVec2(0, 0), false);
			//			ImGui::Text("%s", visibilityFlagNames[itemIndex].c_str());
			//			if (itemIndex == visibilityFlags.size() - 1) ImGui::EndChild();
			//			itemIndex++;
			//		}
			//	}
			//	ImGui::Columns(1);
			//}
			// END MENU

			//if (ImGui::CollapsingHeader("NET"))
			//{
			//	ImGui::TableNextColumn();
			//	ImGui::Text("DeviceMacAddress: %s", this->Net->DeviceMacAddress.c_str());

			//	ImGui::TableNextColumn();
			//	ImGui::Text("SSID: %s", this->Net->SSID.c_str());
			//	ImGui::TableNextColumn();
			//	ImGui::Text("Status: %s", this->Net->Status.c_str());

			//	 //İlk iki grafiği yan yana sırala
			//	ImGui::BeginChild("Ping", ImVec2(600, 400), true);

			//	this->ReceivedRateGraphic->Visibility = true;
			//	this->ReceivedRatePage();
			//	ImGui::EndChild();
			//}


			/*ImGui::SetNextWindowSize(ImVec2(920, 520));


			if (this->SpeedGraphic->Visibility) {
				this->SpeedPage();
			}

			if (this->WheelGraphic->Visibility) {
				this->WheelPage();
			}

			if (this->AgvAngleGraphic->Visibility) {
				this->AgvAngelPage();
			}

			if (this->AgvPositionGraphic->Visibility) {
				this->AgvPositionPage();
			} */

		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("Net"))
		{
			ImGui::MenuItem("Signal Ping Speed Graphic", "", &this->SignalPingSpeedGraphic->Visibility);

			ImGui::MenuItem("Transmiteded Rate Graphic", "", &this->TransmitededRateGraphic->Visibility);

			ImGui::MenuItem("Received Rate Graphic", "", &this->ReceivedRateGraphic->Visibility);

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

		if (this->TransmitededRateGraphic->Visibility)
		{
			ImGui::SameLine();
			ImGui::SetNextWindowSize(ImVec2(415, 762), ImGuiCond_Appearing);
			ImGui::Begin("Style Editor (ImPlot)");
			this->TransmitededRateGraphic->Visibility = true;
			this->TransmitededRatePage();
			ImGui::End();
		}



		if (this->SignalPingSpeedGraphic->Visibility)
		{
			ImGui::SameLine();
			ImGui::SetNextWindowSize(ImVec2(415, 762), ImGuiCond_Appearing);
			ImGui::Begin("Style Editor (ImPlot)");
			this->SignalPingSpeedGraphic->Visibility = true;
			this->SignalPingSpeedPage();
			ImGui::End();
		}

		if (this->ReceivedRateGraphic->Visibility)
		{
			ImGui::SameLine();
			ImGui::SetNextWindowSize(ImVec2(415, 762), ImGuiCond_Appearing);
			ImGui::Begin("Style Editor (ImPlot)");
			this->ReceivedRateGraphic->Visibility = true;
			this->ReceivedRatePage();
			ImGui::End();
		}

	}

}




void ROTracer::ZMQDataStreamParser()
{
	void* context = zmq_ctx_new();
	void* subscriber = zmq_socket(context, ZMQ_SUB);

	char ip[100];
	sprintf(ip, "tcp://%s:%s", this->IpAddress, this->PortAddress);

	int rc = zmq_connect(subscriber, ip);  //"tcp://192.168.2.125:5556"
	rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);

	size_t index = 0;
	char* pch;
	_isRunning = true;
	rapidjson::Document doc;
	rapidjson::Value::ConstValueIterator itr;

	while (this->_zmqLoopFlag)
	{
		zmq_msg_t zmq_topic;
		zmq_msg_t zmq_message;



		zmq_msg_init(&zmq_topic);
		rc = zmq_msg_recv(&zmq_topic, subscriber, 0);

		zmq_msg_init(&zmq_message);
		rc = zmq_msg_recv(&zmq_message, subscriber, 0);

		int zmq_topic_size = zmq_msg_size(&zmq_topic);
		int zmq_message_size = zmq_msg_size(&zmq_message);

		char* zmq_topic_data = (char*)malloc(zmq_topic_size + 1);
		memcpy(zmq_topic_data, zmq_msg_data(&zmq_topic), zmq_topic_size);
		zmq_msg_close(&zmq_topic);
		zmq_topic_data[zmq_topic_size] = 0;

		char* zmq_message_data = (char*)malloc(zmq_message_size + 1);
		memcpy(zmq_message_data, zmq_msg_data(&zmq_message), zmq_message_size);
		zmq_msg_close(&zmq_message);
		zmq_message_data[zmq_message_size] = 0;

		if (rc != -1) {
			fprintf(stdout, "TOPIC: %s \n", zmq_topic_data);
			fprintf(stdout, "MSG: %s \n", zmq_message_data);
		}

		if (rc != -1) {
			index = 0;
			if (std::strcmp("real", zmq_topic_data) == 0) {
				pch = strtok(zmq_message_data, ";");
				while (pch != NULL)
				{
					if (index == 0) {
						Agv->Fx = stoi(pch);
					}
					else if (index == 1) {
						Agv->Fy = stoi(pch);
					}
					else if (index == 2) {
						Agv->Bx = stoi(pch);
					}
					else if (index == 3) {
						Agv->By = stoi(pch);
					}
					else if (index == 4) {
						Agv->Angle = std::stof(pch);
					}
					else if (index == 5) {
						Agv->WSpeed = stoi(pch);
					}
					else if (index == 6) {
						Agv->RSpeed = stoi(pch);
					}
					else if (index == 7) {
						Agv->WAngle = std::stof(pch);
					}
					else if (index == 8) {
						Agv->RAngle = std::stof(pch);
					}

					index++;
					pch = strtok(NULL, ";");
				}


				if (this->SpeedGraphic->Enabled == true)
				{
					this->SpeedGraphic->Time += ImGui::GetIO().DeltaTime;
					this->SpeedGraphic->ReadingSpeed.AddPoint(this->SpeedGraphic->Time, this->Agv->RSpeed);
					this->SpeedGraphic->WritingSpeed.AddPoint(this->SpeedGraphic->Time, this->Agv->WSpeed);
				}

				if (this->WheelGraphic->Enabled == true)
				{
					this->WheelGraphic->Time += ImGui::GetIO().DeltaTime;
					this->WheelGraphic->ReadingWheelAngel.AddPoint(this->WheelGraphic->Time, this->Agv->RAngle);
					this->WheelGraphic->WritingWheelAngel.AddPoint(this->WheelGraphic->Time, this->Agv->WAngle);
				}

				if (this->AgvAngleGraphic->Enabled == true)
				{
					this->AgvAngleGraphic->Time += ImGui::GetIO().DeltaTime;
					this->AgvAngleGraphic->AgvAngle.AddPoint(this->AgvAngleGraphic->Time, this->Agv->Angle);
				}

				if (this->AgvPositionGraphic->Enabled == true)
				{
					this->AgvPositionGraphic->Time += ImGui::GetIO().DeltaTime;
					this->AgvPositionGraphic->AgvFrontPosition.AddPoint(Agv->Fx, Agv->Fy);
					this->AgvPositionGraphic->AgvBackPosition.AddPoint(Agv->Bx, Agv->By);
					this->AgvPositionGraphic->AgvAngle = Agv->Angle;
				}



			}
			else if (std::strcmp("net", zmq_topic_data) == 0) {
				pch = strtok(zmq_message_data, ";");
				while (pch != NULL)
				{
					if (index == 0) {
						Net->DeviceMacAddress = std::string(pch);
					}
					else if (index == 1) {
						Net->Ping = stoi(pch);
					}
					else if (index == 2) {
						Net->ReceivedRate = stoi(pch);
					}
					else if (index == 3) {
						Net->Signal = stoi(pch);
					}
					else if (index == 4) {
						Net->Speed = stoi(pch);
					}
					else if (index == 5) {
						Net->SSID = std::string(pch);;
					}
					else if (index == 6) {
						Net->Status = std::string(pch);
					}
					else if (index == 7) {
						Net->TransmitededRate = stoi(pch);
					}

					//	printf("%s\n", pch);
					pch = strtok(NULL, ";");
				}
				//net kısmı

				if (this->ReceivedRateGraphic->Enabled == true)
				{
					this->ReceivedRateGraphic->Time += ImGui::GetIO().DeltaTime;
					this->ReceivedRateGraphic->StokingReceivedRate.AddPoint(this->ReceivedRateGraphic->Time, this->Net->ReceivedRate);
				}

				if (this->SignalPingSpeedGraphic->Enabled == true)
				{
					this->SignalPingSpeedGraphic->Time += ImGui::GetIO().DeltaTime;
					this->SignalPingSpeedGraphic->StokingSignal.AddPoint(this->SignalPingSpeedGraphic->Time, this->Net->Signal);
					this->SignalPingSpeedGraphic->StokingPing.AddPoint(this->SignalPingSpeedGraphic->Time, this->Net->Ping);
					this->SignalPingSpeedGraphic->StokingWifiSpeed.AddPoint(this->SignalPingSpeedGraphic->Time, this->Net->Speed);
				}

				if (this->TransmitededRateGraphic->Enabled == true)
				{
					this->TransmitededRateGraphic->Time += ImGui::GetIO().DeltaTime;
					this->TransmitededRateGraphic->StokingTransmitededRate.AddPoint(this->TransmitededRateGraphic->Time, this->Net->TransmitededRate);
				}
			}
			else if (std::strcmp("route", zmq_topic_data) == 0)
			{
				doc.Parse(zmq_message_data);

				if (doc.HasParseError()) {
					continue;
				}

				this->AgvPositionGraphic->Erase();

				for (itr = doc.Begin(); itr != doc.End(); ++itr)
				{
					this->AgvPositionGraphic->Route.AddPoint(itr->GetObject()["X"].GetFloat(), itr->GetObject()["Y"].GetFloat());
				}
			}
			else if (std::strcmp("sim", zmq_topic_data) == 0) {

				doc.Parse(zmq_message_data);

				if (doc.HasParseError()) {
					continue;
				}

				for (itr = doc.Begin(); itr != doc.End(); ++itr)
				{
					this->AgvPositionGraphic->Simu.AddPoint(itr->GetObject()["X"].GetFloat(), itr->GetObject()["Y"].GetFloat());
				}
			}
			else if (std::strcmp("curve", zmq_topic_data) == 0) {

				doc.Parse(zmq_message_data);

				if (doc.HasParseError()) {
					continue;
				}

				for (itr = doc.Begin(); itr != doc.End(); ++itr)
				{
					this->AgvPositionGraphic->Curve.AddPoint(itr->GetObject()["X"].GetFloat(), itr->GetObject()["Y"].GetFloat());
				}
			}
			else if (std::strcmp("cell", zmq_topic_data) == 0) {

				doc.Parse(zmq_message_data);

				if (doc.HasParseError()) {
					continue;
				}

				this->Agv->CellLx = doc[0].GetObject()["X"].GetFloat();
				this->Agv->CellLy = doc[0].GetObject()["Y"].GetFloat();

				this->Agv->CellSx = doc[1].GetObject()["X"].GetFloat();
				this->Agv->CellSy = doc[1].GetObject()["Y"].GetFloat();

			}

		}


		//  serbest bırakma 
		free(zmq_topic_data);
		free(zmq_message_data);
	}

	zmq_close(subscriber);
	zmq_ctx_destroy(context);

	_isRunning = false;

}



void ROTracer::SpeedPage() {

	ImGui::SetNextWindowSize(ImVec2(920, 520));

	/*ImGui::Begin("Agv Speed Graphic",&this->SpeedGraphic->Visibility);*/

	if (this->Agv == NULL) {
		return;
	}

	ImGui::Checkbox("Enabled", &this->SpeedGraphic->Enabled);
	ImGui::SameLine();
	ImGui::SliderFloat("History", &this->SpeedGraphic->History, 1, 30, "%.1f s");  // .1f yaparsak milisaniye olarak ayarlanıyor 

	static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

	if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, -1))) {     // grafik ölçeklendirme 
		ImPlot::SetupAxes("Time [s]", "Speed [mm/s]");

		ImPlot::SetupAxisLimits(ImAxis_X1, this->SpeedGraphic->Time - this->SpeedGraphic->History, this->SpeedGraphic->Time, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, -500, 2000);

		if (this->SpeedGraphic->ReadingSpeed.Data.Size > 0) {
			ImPlot::PlotLine("Reading Speed", &this->SpeedGraphic->ReadingSpeed.Data[0].x, &this->SpeedGraphic->ReadingSpeed.Data[0].y, this->SpeedGraphic->ReadingSpeed.Data.size(), 0, this->SpeedGraphic->ReadingSpeed.Offset, 2 * sizeof(float));
			ImPlot::PlotLine("Writing Speed", &this->SpeedGraphic->WritingSpeed.Data[0].x, &this->SpeedGraphic->WritingSpeed.Data[0].y, this->SpeedGraphic->WritingSpeed.Data.size(), 0, this->SpeedGraphic->WritingSpeed.Offset, 2 * sizeof(float));
		}

		ImPlot::EndPlot();
	}

	//ImGui::End();
}

void ROTracer::WheelPage() {

	ImGui::SetNextWindowSize(ImVec2(920, 520));
	//ImGui::Begin("Agv Wheel Graphic", &this->WheelGraphic->Visibility);


	if (this->Agv == NULL) {
		return;
	}

	ImGui::Checkbox("Enabled", &this->WheelGraphic->Enabled);
	ImGui::SameLine();
	ImGui::SliderFloat("History", &this->WheelGraphic->History, 1, 30, "%.1f s");  // .1f yaparsak milisaniye olarak ayarlanıyor 

	static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

	if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, -1))) {     // grafik ölçeklendirme 
		ImPlot::SetupAxes("Time [s]", "Wheel Angle [deg]");

		ImPlot::SetupAxisLimits(ImAxis_X1, this->WheelGraphic->Time - this->WheelGraphic->History, this->WheelGraphic->Time, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, -100, 100);

		if (this->WheelGraphic->ReadingWheelAngel.Data.Size > 0) {
			ImPlot::PlotLine("Reading Wheel", &this->WheelGraphic->ReadingWheelAngel.Data[0].x, &this->WheelGraphic->ReadingWheelAngel.Data[0].y, this->WheelGraphic->ReadingWheelAngel.Data.size(), 0, this->WheelGraphic->ReadingWheelAngel.Offset, 2 * sizeof(float));
			ImPlot::PlotLine("Writing Wheel", &this->WheelGraphic->WritingWheelAngel.Data[0].x, &this->WheelGraphic->WritingWheelAngel.Data[0].y, this->WheelGraphic->WritingWheelAngel.Data.size(), 0, this->WheelGraphic->WritingWheelAngel.Offset, 2 * sizeof(float));
		}

		ImPlot::EndPlot();
	}

	//ImGui::End();
}


void ROTracer::AgvAngelPage() {

	ImGui::SetNextWindowSize(ImVec2(920, 520));
	//ImGui::Begin("Agv Angle Graphic", &this->AgvAngleGraphic->Visibility);

	if (this->Agv == NULL) {
		return;
	}

	ImGui::Checkbox("Enabled", &this->AgvAngleGraphic->Enabled);
	ImGui::SameLine();
	ImGui::SliderFloat("History", &this->AgvAngleGraphic->History, 1, 30, "%.1f s");

	static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

	if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, -1))) {
		ImPlot::SetupAxes("Time [s]", "Angle [deg]");

		ImPlot::SetupAxisLimits(ImAxis_X1, this->AgvAngleGraphic->Time - this->AgvAngleGraphic->History, this->AgvAngleGraphic->Time, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 359);

		if (this->WheelGraphic->ReadingWheelAngel.Data.Size > 0) {
			ImPlot::PlotLine("Agv Angle", &this->AgvAngleGraphic->AgvAngle.Data[0].x, &this->AgvAngleGraphic->AgvAngle.Data[0].y, this->AgvAngleGraphic->AgvAngle.Data.size(), 0, this->AgvAngleGraphic->AgvAngle.Offset, 2 * sizeof(float));
		}

		ImPlot::EndPlot();
	}

	//ImGui::End();
}
void ROTracer::AgvPositionPage() {

	ImGui::SetNextWindowSize(ImVec2(920, 520));
	//ImGui::Begin("Agv Position Graphic", &this->AgvPositionGraphic->Visibility);
	if (this->Agv == NULL) {
		return;
	}
	static int cnt = 0;
	double dx = point2.x - point1.x;
	double dy = point2.y - point1.y;
	double distance = sqrt(dx * dx + dy * dy);
	double adistance = sqrt(dx * dx + dy * dy);

	ImGui::InputFloat2("Mouse", new float[2] {(float)point1.x, (float)point1.y});
	//ImGui::InputFloat("Length ", new float[1] {(float)distance});	
	ImGui::SameLine();
	ImGui::Checkbox("Draw Line", &drawLine);
	ImGui::SameLine();

	if (ImGui::Button("Clear Line")) {
		//drawLine = false;
		point1 = ImVec2(0.0f, 0.0f);
		point2 = ImVec2(0.0f, 0.0f);
		cnt = 0;
	}

	float radius = 0.0f;


	if (ImPlot::BeginPlot("Scatter Plot", ImVec2(-1, -1), ImPlotFlags_Equal)) {

		//ImPlot::SetupAxisLimits(ImAxis_X1, this->Agv->X - 500, this->Agv->X + 1000, ImGuiCond_Always);
		//ImPlot::SetupAxisLimits(ImAxis_Y1, this->Agv->Y - 500, this->Agv->Y + 1000, ImGuiCond_Always);

		//ImPlot::SetupAxesLimits(10000, 30000, 30000, 80000);



		if (drawLine)
		{

			if (ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyCtrl) {

				point2 = ImPlot::GetPlotMousePos();
				point1 = ImPlot::GetPlotMousePos();
				cnt = 1;
			}
			else
			{
				if (cnt == 1)
				{
					point1 = ImPlot::GetPlotMousePos();

					if (ImGui::IsMouseClicked(0))
						cnt = 2;
				}
			}

		}
		else if (drawLine == false)
		{
			/*point1 = ImVec2(0.0f, 0.0f);
			point2 = ImVec2(0.0f, 0.0f);*/
			//cnt = 0;
		}

		if (cnt > 0)
		{

			radius = sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2));
			static double xs[360], ys[360];
			for (int i = 0; i < 360; ++i)
			{
				double angle = i * 2 * PI / 359.0;
				xs[i] = point2.x + radius * cos(angle);
				ys[i] = point2.y + radius * sin(angle);
			}
			ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1, 0, 0, 1));
			ImPlot::PlotLine("Circle", xs, ys, 360);
			ImPlot::PopStyleColor();
			ImPlot::DragPoint(0, &point1.x, &point1.y, ImVec4(1, 0, 0, 1), 4, ImPlotDragToolFlags_None);
			ImPlot::DragPoint(1, &point2.x, &point2.y, ImVec4(1, 0, 0, 1), 4, ImPlotDragToolFlags_None);

			ImPlot::GetPlotDrawList()->AddLine(
				ImPlot::PlotToPixels(ImPlotPoint(point1)),
				ImPlot::PlotToPixels(ImPlotPoint(point2)),
				IM_COL32(255, 0, 0, 255),
				3.0f
			);

			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Text("Line Length: %.2f", distance);
				ImGui::EndTooltip();
			}
		}

		if (this->AgvPositionGraphic->AgvFrontPosition.Data.size() > 0) {

			ImPlot::PlotScatter("AgvFPosition",
				&this->AgvPositionGraphic->AgvFrontPosition.Data[0].x,
				&this->AgvPositionGraphic->AgvFrontPosition.Data[0].y,
				this->AgvPositionGraphic->AgvFrontPosition.Data.size(), 0, 0, 2 * sizeof(int));

			ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
			ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 4, ImPlot::GetColormapColor(5), IMPLOT_AUTO, ImPlot::GetColormapColor(5));
			ImPlot::PlotScatter("AgvBPosition",
				&this->AgvPositionGraphic->AgvBackPosition.Data[0].x,
				&this->AgvPositionGraphic->AgvBackPosition.Data[0].y,
				this->AgvPositionGraphic->AgvBackPosition.Data.size(), 0, 0, 2 * sizeof(int));
			ImPlot::PopStyleVar();

			if (this->AgvPositionGraphic->Route.Data.size() > 0) {
				ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
				ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 2, ImPlot::GetColormapColor(2), IMPLOT_AUTO, ImPlot::GetColormapColor(2));
				ImPlot::PlotScatter("AgvRoute",
					&this->AgvPositionGraphic->Route.Data[0].x,
					&this->AgvPositionGraphic->Route.Data[0].y,
					this->AgvPositionGraphic->Route.Data.size(), 0, 0, 2 * sizeof(int));

				ImPlot::PopStyleVar();
			}

			if (this->AgvPositionGraphic->Curve.Data.size() > 0) {
				ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
				ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 2, ImPlot::GetColormapColor(4), IMPLOT_AUTO, ImPlot::GetColormapColor(4));
				ImPlot::PlotScatter("AgvCurve",
					&this->AgvPositionGraphic->Curve.Data[0].x,
					&this->AgvPositionGraphic->Curve.Data[0].y,
					this->AgvPositionGraphic->Curve.Data.size(), 0, 0, 2 * sizeof(int));

				ImPlot::PopStyleVar();
			}

			if (this->AgvPositionGraphic->Simu.Data.size() > 0) {
				ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
				ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 2, ImPlot::GetColormapColor(5), IMPLOT_AUTO, ImPlot::GetColormapColor(5));
				ImPlot::PlotScatter("AgvSim",
					&this->AgvPositionGraphic->Simu.Data[0].x,
					&this->AgvPositionGraphic->Simu.Data[0].y,
					this->AgvPositionGraphic->Simu.Data.size(), 0, 0, 2 * sizeof(int));

				ImPlot::PopStyleVar();
			}

			int lastIndex = this->AgvPositionGraphic->AgvFrontPosition.Data.Size - 1;

			float xRange = ImPlot::GetPlotLimits().X.Max - ImPlot::GetPlotLimits().X.Min;
			float yRange = ImPlot::GetPlotLimits().Y.Max - ImPlot::GetPlotLimits().Y.Min;
			float r = fminf(xRange, yRange) / 50.0f; // r değerini grafik boyutuna göre ölçeklendirin

			float x1 = this->AgvPositionGraphic->AgvFrontPosition.Data[lastIndex].x;
			float y1 = this->AgvPositionGraphic->AgvFrontPosition.Data[lastIndex].y;

			float x_1 = x1 + cos(this->Agv->Angle * PI / 180) * r * 2;
			float y_1 = y1 + sin(this->Agv->Angle * PI / 180) * r * 2;

			float x_2 = x1 + cos(fmod(this->Agv->Angle + 120, 360) * PI / 180) * r;
			float y_2 = y1 + sin(fmod(this->Agv->Angle + 120, 360) * PI / 180) * r;

			float x_3 = x1 + cos(fmod(this->Agv->Angle + 240, 360) * PI / 180) * r;
			float y_3 = y1 + sin(fmod(this->Agv->Angle + 240, 360) * PI / 180) * r;


			ImVec2 p1 = ImPlot::PlotToPixels(ImPlotPoint(x_2, y_2));//sol alt 
			ImVec2 p2 = ImPlot::PlotToPixels(ImPlotPoint(x_3, y_3));// sag alt 
			ImVec2 p3 = ImPlot::PlotToPixels(ImPlotPoint(x_1, y_1));// tepe
			ImPlot::PushPlotClipRect();
			ImPlot::GetPlotDrawList()->AddTriangleFilled(p1, p2, p3, IM_COL32(255, 127, 0, 255));

			//mouse tıklamasıyla çizgi çizen kod	
			if (drawLine && ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyCtrl)
			{
				if (cnt == 0)
				{
					point1 = ImPlot::GetPlotMousePos();
					cnt = 1;
				}
				else if (cnt == 1)
				{
					point2 = ImPlot::GetPlotMousePos();
					cnt = 2;

					//point2 = ImVec2(0.0f, 0.0f);
				}

			}
			else if (drawLine == false)
			{
				/*point1 = ImVec2(0.0f, 0.0f);
				point2 = ImVec2(0.0f, 0.0f);*/
				cnt = 0;
			}

			if (cnt == 2) {

				const float arrowSize = 10.0f;
				const ImVec2 p1 = ImPlot::PlotToPixels(ImPlotPoint(point1));
				const ImVec2 p2 = ImPlot::PlotToPixels(ImPlotPoint(point2));
				const ImVec2 dir = ImVec2(p2.x - p1.x, p2.y - p1.y);
				const float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
				const ImVec2 norm = ImVec2(dir.x / len, dir.y / len);
				const ImVec2 perp = ImVec2(-norm.y, norm.x);

				//ImPlot::GetPlotDrawList()->AddLine(p1, p2, IM_COL32(255, 0, 0, 255), 3.0f);
				ImPlot::GetPlotDrawList()->AddLine(
					ImPlot::PlotToPixels(ImPlotPoint(point1)),
					ImPlot::PlotToPixels(ImPlotPoint(point2)),
					IM_COL32(255, 0, 0, 255),
					3.0f
				);


				const float crossSize = 4.0f;
				ImPlot::GetPlotDrawList()->AddLine(ImVec2(p1.x + perp.x * crossSize, p1.y + perp.y * crossSize), ImVec2(p1.x - perp.x * crossSize, p1.y - perp.y * crossSize), IM_COL32(255, 255, 255, 255), 1.5f);
				ImPlot::GetPlotDrawList()->AddLine(ImVec2(p1.x + perp.x * crossSize, p1.y - perp.y * crossSize), ImVec2(p1.x - perp.x * crossSize, p1.y + perp.y * crossSize), IM_COL32(255, 255, 255, 255), 1.5f);

				ImPlot::GetPlotDrawList()->AddLine(ImVec2(p2.x + perp.x * crossSize, p2.y + perp.y * crossSize), ImVec2(p2.x - perp.x * crossSize, p2.y - perp.y * crossSize), IM_COL32(255, 255, 255, 255), 1.5f);
				ImPlot::GetPlotDrawList()->AddLine(ImVec2(p2.x + perp.x * crossSize, p2.y - perp.y * crossSize), ImVec2(p2.x - perp.x * crossSize, p2.y + perp.y * crossSize), IM_COL32(255, 255, 255, 255), 1.5f);

				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("Line Length: %.2f", distance);
					ImGui::EndTooltip();
				}
			}



			ImPlot::GetPlotDrawList()->AddLine(ImPlot::PlotToPixels(ImPlotPoint(this->Agv->CellLx, this->Agv->CellLy)), ImPlot::PlotToPixels(ImPlotPoint(this->Agv->CellSx, this->Agv->CellSy)), IM_COL32(255, 127, 0, 255));
			ImPlot::GetPlotDrawList()->AddCircleFilled(ImPlot::PlotToPixels(ImPlotPoint(this->Agv->CellLx, this->Agv->CellLy)), 3, IM_COL32(255, 127, 0, 255));
			ImPlot::GetPlotDrawList()->AddLine(ImPlot::PlotToPixels(ImPlotPoint(this->Agv->Fx, this->Agv->Fy)), ImPlot::PlotToPixels(ImPlotPoint(this->Agv->Bx, this->Agv->By)), IM_COL32(255, 127, 0, 255));

			//ImPlot::SetNextMarkerStyle(ImPlotMarker_Cross, 6.0f, ImVec4(1, 0, 0, 1), 2.0f);

		//}

		}
		ImPlot::EndPlot();
	}

	//ImGui::End();

}

void ROTracer::ReceivedRatePage() {
	ImGui::SetNextWindowSize(ImVec2(920, 520));


	if (this->Net == NULL) {
		return;
	}

	ImGui::Checkbox("Enabled", &this->ReceivedRateGraphic->Enabled);
	ImGui::SameLine();
	ImGui::SliderFloat("History", &this->ReceivedRateGraphic->History, 1, 30, "%.1f s");  // .1f yaparsak milisaniye olarak ayarlanıyor 

	static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

	if (ImPlot::BeginPlot("##Scrollinga")) {     // grafik ölçeklendirme 
		ImPlot::SetupAxes("Time [s]", "Received rate  [ ]");

		ImPlot::SetupAxisLimits(ImAxis_X1, this->ReceivedRateGraphic->Time - this->ReceivedRateGraphic->History, this->ReceivedRateGraphic->Time, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, -500, 2000);
		if (this->ReceivedRateGraphic->StokingReceivedRate.Data.Size > 0) {
			//ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 1.f);
			ImPlot::PlotLine("received rate", &this->ReceivedRateGraphic->StokingReceivedRate.Data[0].x, &this->ReceivedRateGraphic->StokingReceivedRate.Data[0].y, this->ReceivedRateGraphic->StokingReceivedRate.Data.size(), 0, this->ReceivedRateGraphic->StokingReceivedRate.Offset, 2 * sizeof(float));
		}
		ImPlot::EndPlot();
	}
}





void ROTracer::SignalPingSpeedPage() {

	ImGui::SetNextWindowSize(ImVec2(920, 520));

	if (this->Net == NULL) {
		return;
	}

	ImGui::Checkbox("Enabled", &this->SignalPingSpeedGraphic->Enabled);
	ImGui::SameLine();
	ImGui::SliderFloat("History", &this->SignalPingSpeedGraphic->History, 1, 30, "%.1f s");  // .1f yaparsak milisaniye olarak ayarlanıyor 

	static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

	if (ImPlot::BeginPlot("##Scrollingc")) {     // grafik ölçeklendirme 
		ImPlot::SetupAxes("Time [s]", "Speed [mm/s]");

		ImPlot::SetupAxisLimits(ImAxis_X1, this->SignalPingSpeedGraphic->Time - this->SignalPingSpeedGraphic->History, this->SignalPingSpeedGraphic->Time, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, -100, 100);
		if (this->SignalPingSpeedGraphic->StokingWifiSpeed.Data.Size > 0) {
			//ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 1.f);
			ImPlot::PlotLine("Signal", &this->SignalPingSpeedGraphic->StokingSignal.Data[0].x, &this->SignalPingSpeedGraphic->StokingSignal.Data[0].y, this->SignalPingSpeedGraphic->StokingSignal.Data.size(), 0, this->SignalPingSpeedGraphic->StokingSignal.Offset, 2 * sizeof(float));
			ImPlot::PlotLine("Ping", &this->SignalPingSpeedGraphic->StokingPing.Data[0].x, &this->SignalPingSpeedGraphic->StokingPing.Data[0].y, this->SignalPingSpeedGraphic->StokingPing.Data.size(), 0, this->SignalPingSpeedGraphic->StokingPing.Offset, 2 * sizeof(float));
			ImPlot::PlotLine("Speed", &this->SignalPingSpeedGraphic->StokingWifiSpeed.Data[0].x, &this->SignalPingSpeedGraphic->StokingWifiSpeed.Data[0].y, this->SignalPingSpeedGraphic->StokingWifiSpeed.Data.size(), 0, this->SignalPingSpeedGraphic->StokingWifiSpeed.Offset, 2 * sizeof(float));
			// plotline düz cizgili grafik <=> PlotShaded gölgeli grafik  
		}
		ImPlot::EndPlot();
	}
}







void ROTracer::TransmitededRatePage() {
	ImGui::SetNextWindowSize(ImVec2(920, 520));


	if (this->Net == NULL) {
		return;
	}


	ImGui::Checkbox("Enabled", &this->TransmitededRateGraphic->Enabled);
	ImGui::SameLine();
	ImGui::SliderFloat("History", &this->TransmitededRateGraphic->History, 1, 30, "%.1f s");  // .1f yaparsak milisaniye olarak ayarlanıyor 

	static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

	if (ImPlot::BeginPlot("##Scrollingf")) {     // grafik ölçeklendirme 
		ImPlot::SetupAxes("Time [s]", "transmiteded rate  [ ]");

		ImPlot::SetupAxisLimits(ImAxis_X1, this->TransmitededRateGraphic->Time - this->TransmitededRateGraphic->History, this->TransmitededRateGraphic->Time, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 359);
		if (this->TransmitededRateGraphic->StokingTransmitededRate.Data.Size > 0) {
			//ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 1.f);
			ImPlot::PlotLine("transmiteded rate ", &this->TransmitededRateGraphic->StokingTransmitededRate.Data[0].x, &this->TransmitededRateGraphic->StokingTransmitededRate.Data[0].y, this->TransmitededRateGraphic->StokingTransmitededRate.Data.size(), 0, this->TransmitededRateGraphic->StokingTransmitededRate.Offset, 2 * sizeof(float));
		}
		ImPlot::EndPlot();
	}
}

