#pragma once
#include <Windows.h>
#include <map>
#include "Constant.h"
#include "../head/Packet.h"
#include "../head/ComPort.h"
#include "../head/FormedPacket.h"
#include "../head/UnformedPacket.h"
/*
	хочу что бы main была только одна две фукнции, создание я вызова метода этого класса
	для запуска работы все приложения, здесь будет осуществлятся выбор между пакетами
*/

class Menu
{
private:
	CS::menu_options sw;
	uint64_t meter_number; // 235072
	ComPort* COM;
	FormedPacket*   formed_packet;
	UnformedPacket* unformed_packet;
	std::vector<std::vector<uchar>> test_array; // TEST
	int32_t counter_test_packet; // TEST

	void cmd_configuration(int width, int height, int bufferHeight); // настройа размеров окна

	uchar hex_char_to_value(char c); // TEST
	std::vector<std::vector<uchar>> init_array_test_packet(std::string name_file); // TEST
	void output_array_test_packet(std::vector<std::vector<uchar>> output, std::string name_file); // TEST

public:
	Menu();
	~Menu();

	void init();
	
	void print_input_invitation(); // приглащение ко вводу, вывод клавиш которые будут участововать в меню
	void print_dash(int16_t N); // нариcовать N * "/" 

	
	CS::menu_options is_key_press(); 
};