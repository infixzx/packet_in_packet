#pragma once
#include <Windows.h>
#include <map>
#include "Constant.h"
#include "../head/Packet.h"
#include "../head/ComPort.h"
#include "../head/FormedPacket.h"
#include "../head/UnformedPacket.h"
/*
	���� ��� �� main ���� ������ ���� ��� �������, �������� � ������ ������ ����� ������
	��� ������� ������ ��� ����������, ����� ����� ������������� ����� ����� ��������
*/

class Menu
{
private:
	CS::menu_options sw;
	uint64_t meter_number; // 235072
	ComPort* COM;
	FormedPacket*   formed_packet;
	UnformedPacket* unformed_packet;


	void cmd_configuration(int width, int height, int bufferHeight); // �������� �������� ����
public:
	Menu();
	~Menu();

	void init();
	
	void print_input_invitation(); // ����������� �� �����, ����� ������ ������� ����� ������������ � ����
	void print_dash(int16_t N); // ����c����� N * "/" 

	
	CS::menu_options is_key_press(); 
};