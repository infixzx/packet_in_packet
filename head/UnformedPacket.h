#pragma once
#include "Packet.h"
#include "Constant.h"
#include "Encryption.h"


class UnformedPacket
{
private:
	Packet received_packet; // ����� �� ���������
	Packet unformed_packet; // �� ��� ����� ���������� ����� �������� reset

	Packet external_flag; //Flag; const 0x7A; 1 byte
	Packet external_address_lenght; // LenAdr; � DEC 0...7; 1 byte (����� ������� ������������ � ������� �������)
	Packet external_address; //Adr[LenAdr]...Adr[0]; �����(�) ������������; 2 byte * (MAX 8)
	Packet external_type_and_number; //Type; ��� ������ � ������� ����� ������; 1 byte
	Packet external_lenght; //Len; ������ ���� ������; 1 byte
	Packet external_command_code; //CMD; ��� �������; 1 byte
	Packet external_information; //Inforamation; ���������� �����; Len byte
	Packet external_FCS; //FCS; ���������� ������ 2 �����

	Packet internal_flag; //Flag; const 0x7E; 1 byte
	Packet internal_lenght; //Len; � DEC 0..200; 1 byte
	Packet internal_distination_address; //Dst; ����� ���������� (L..H); 6 byte
	Packet internal_type_and_number; //Type; ��� ������ � ������� ����� ������; 1 byte
	Packet internal_command_code; //CMD; ��� �������; 1 byte
	Packet internal_information; //Information; ������; Len byte
	Packet internal_FCS; //FCS; ���������� �����; 2 �����

	Packet set_distination_address(uint64_t DEC_distination_address);
	Packet set_external_distination_address(uint64_t DEC_distination_address);

public:
	UnformedPacket(uint64_t met_num);
	void setUnformedPacket(Packet unform_packet);
	void reset();
	Packet returnUnformedPacket();
	bool cheek_internal_status(Packet byte); //���� �� - true

	//������:
	void response_reading_the_counter_status();		   //������ ��������� ��������
	void response_get_current_measurements_by_phase(); //�������� ������� ��������� �������
	void kostil_response_read_the_date_and_time();		       //��������� ���� � �����
	//void response_set_date_and_time();			   //���������� ���� � �����
	void response_reading_slice_readings();		       //������ ������ ���������
	//void response_reading_the_load_chart();	       //������ ������� ��������
};