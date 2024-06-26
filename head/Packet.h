#pragma once

#include <vector>

using std::vector;
using std::string;
typedef unsigned char uchar;

class Packet
{
private:
	vector<uchar> packet;
	string name;
	uchar* ptr; //нужен для фукнции которая возвращает uchar*

public:
	Packet();
	Packet(string name_class);
	~Packet();
	Packet operator+(const Packet& add);
	Packet& operator=(const Packet& add);
	uchar operator[](uint32_t index);
	uchar* return_uchar();
	uint32_t return_uchar_lenght();
	uint32_t lenght();
	void print_packet();
	void print_packet_not_id();
	void setPacket(uchar* value, uint32_t lenght);
	void setPacket(vector<uchar> value);
	void setName(string name_class);
	void apply_reverse();
	void add_one_element_back(uchar temp);
	Packet reverse();
	Packet return_range(uint16_t low, uint16_t high); // вернуть нам нужный диапозон packeta
	Packet hex_to_dec_byte(Packet hex);
};

