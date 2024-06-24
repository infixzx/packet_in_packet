#include <iostream>
#include "../head/Packet.h"

using std::cout;
using std::endl;


Packet::Packet()
{
	ptr = NULL;
	name = "ID";
}

Packet::Packet(string name_class)
{
	ptr = NULL;
	name = name_class;
}

Packet::~Packet()
{
	if (ptr != NULL)
		delete[] ptr;
}


Packet Packet::operator+(const Packet& add)
{
	Packet temp;

	for (int i = 0; i < packet.size(); i++)
		temp.packet.push_back(packet[i]);

	for (int i = 0; i < add.packet.size(); i++)
		temp.packet.push_back(add.packet[i]);
	
	return temp;
}


Packet& Packet::operator=(const Packet& add)
{
	packet.clear();
	
	for (int i = 0; i < add.packet.size(); i++)
		packet.push_back(add.packet[i]);
	
	return *this;
}

uchar Packet::operator[](uint32_t index)
{
	return packet[index];
}

uchar* Packet::return_uchar()
{
	delete[] ptr;
	ptr = new uchar[packet.size()];
	for (int i = 0; i < packet.size(); i++)
		ptr[i] = packet[i];
	return ptr;
}

uint32_t Packet::return_uchar_lenght()
{
	return packet.size();
}

uint32_t Packet::lenght()
{
	return packet.size();
}

void Packet::print_packet()
{
	cout << name.c_str() << ": ";
	for (int i = 0; i < packet.size(); i++)
		printf("%02x ", packet[i]);
	printf("\n");
}

void Packet::print_packet_not_id()
{
	for (int i = 0; i < packet.size(); i++)
		printf("%02x ", packet[i]);
	printf("\n");
}



void Packet::setPacket(uchar* value, uint32_t lenght)
{
	packet.clear();
	for (int i = 0; i < lenght; i++)
		packet.push_back(value[i]);
}

void Packet::setPacket(vector<uchar> value)
{
	packet.clear();
	for (int i = 0; i < value.size(); i++)
		packet.push_back(value[i]);
}

void Packet::setName(string name_class)
{
	name = name_class;
}

void Packet::apply_reverse()
{
	Packet reverse_packet;
	for (int i = packet.size() - 1; i >= 0; i--)
	{
		reverse_packet.packet.push_back(packet[i]);
	}

	for (int i = 0; i < packet.size(); i++)
	{
		packet[i] = reverse_packet.packet[i];
	}
}

void Packet::add_one_element_back(uchar temp)
{
	packet.push_back(temp);
}

Packet Packet::reverse()
{
	Packet reverse_packet;
	
	for (int i = packet.size() - 1; i >= 0; i--)
	{
		reverse_packet.packet.push_back(packet[i]);
	}

	return reverse_packet;
}

Packet Packet::return_range(uint16_t low, uint16_t high)
{
	Packet temp;
	for (int i = low; i <= high; i++)
		temp.add_one_element_back(packet[i]);
	return temp;
}

