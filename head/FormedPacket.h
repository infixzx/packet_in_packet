#pragma once
#include "Packet.h"
#include "Constant.h"
#include "Encryption.h"

//ID=9 ��������� ��������
//		1.������ ������ ��������� ��������
//		2.�����  ������ ��������� ��������

//ID=3 ������� ��������� �������
//			1.������ �������� ������� ��������� �������
//		2.�����  �������� ������� ��������� �������


//ID=11 ���� � ����� ���������� �����
//			1. ������ ��������� ���� � �����
//			2. �����  ��������� ���� � �����
//		3. ������ ���������� ���� � �����
//		4. �����  ���������� ���� � �����

//ID=12 ����� ��������� �� ������ ���/������/����, �� ����/�����/���
//		1. ������ ������ ������ ���������
//		2. �����  ������ ������ ���������

//ID=10 ������ ��������
//		1. ������ ������ ������� ��������
//		2. �����  ������ ������� ��������

class FormedPacket
{
private:
		Packet internal_flag;				 // Flag; const 0x7E; 1 byte
		Packet internal_lenght;				 // Len; � DEC 0..200; 1 byte
		Packet internal_distination_address; // Dst; ����� ���������� (L..H); 6 byte
		Packet internal_type_and_number;	 // Type; ��� ������ � ������� ����� ������; 1 byte
		Packet internal_command_code;		 // CMD; ��� �������; 1 byte
		Packet internal_information;		 // Information; ������; Len byte
		Packet internal_FCS;				 // FCS; ���������� �����; 2 �����

		Packet external_flag;			 //Flag; const 0x7A; 1 byte
		Packet external_address_lenght;  // LenAdr; � DEC 0...7; 1 byte (����� ������� ������������ � ������� �������)
	 Packet external_address;			 //Adr[LenAdr]...Adr[0]; �����(�) ������������; 2 byte * (MAX 8)
		Packet external_type_and_number; //Type; ��� ������ � ������� ����� ������; 1 byte
		Packet external_lenght;			 //Len; ������ ���� ������; 1 byte
		Packet external_command_code;	 //CMD; ��� �������; 1 byte
		Packet external_information;	 //Inforamation; ���������� �����; Len byte
	Packet external_FCS;				 //FCS; ���������� ������ 2 �����

	CS::list_ID_packet ID_packet;
	//!!! ����� ENUM ������� ������ � ���� REQUEST � RESPONSE � �� ����� ������������ ���������� ������
	CS::RXTX rxtx; // ������ ��� �����
	uint8_t �urrent_packet_number; //������� ����� ������(�������?)

	Packet sent_packet;
	uint64_t meter_number;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//����������� ����� ���������� ������� ������� ����� ����������
	Packet set_distination_address(uint64_t DEC_distination_address);

	void incriment_�urrent_packet_number(); // �������� � current_packet_number++


	Packet set_internal_distination_address(uint64_t DEC_distination_address); // ����� ��� ���������� ������ ����������� ������
	Packet set_external_distination_address(uint64_t DEC_distination_address); // ����� ��� ���������� ������ �������� ������ (�� ����� ������)


	Packet FormedPacket::set_internal_type_and_number(
		CS::INTERNAL_type_and_number bit_7_rxtx, CS::INTERNAL_type_and_number_pool bit_6_pool,
		CS::INTERNAL_type_and_number_pool bit_5_pool, CS::INTERNAL_type_and_number_pool bit_4_pool);

	Packet FormedPacket::set_external_type_and_number(
		CS::EXTERNAL_type_and_number bit_7_request_response, CS::EXTERNAL_type_and_number_pool bit_6_pool,
		CS::EXTERNAL_type_and_number_pool bit_5_pool, CS::EXTERNAL_type_and_number_pool bit_4_pool);

	Packet set_internal_command_code(CS::INTERNAL_command_code bit_7_code_encode, CS::INTERNAL_command_code_bit_6_0 bit_6_0);

	Packet set_external_command_code(CS::EXTERNAL_command_code bit_7_code_encode, CS::EXTERNAL_command_code_bit_6_0 bit_6_0);

	Packet formed_external_kostil_packet(Packet Information);

	//Encryption??? 
public:
	FormedPacket(uint64_t met_num);
	//~FormedPacket();
	
	//�������:
	void request_reading_the_counter_status();		  //������ ��������� ��������
		void request_get_current_measurements_by_phase(); //�������� ������� ��������� �������
		void request_read_the_date_and_time();		      //��������� ���� � �����
	//void request_set_date_and_time();				  //���������� ���� � �����
	void request_reading_slice_readings();		      //������ ������ ���������
	//void request_reading_the_load_chart();	      //������ ������� ��������
	
	void reset();

	Packet returnSentPacket(); // ������������ �����
};