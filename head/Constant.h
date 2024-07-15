#pragma once
#include <cstdint>
#include <string>
#include <map>

typedef unsigned char uchar;
using std::string;

namespace CS
{
	enum class menu_options : uint8_t
	{
			quit,							   // �����
		reading_the_counter_status,		   // ������ ��������� ��������
			get_current_measurements_by_phase, // �������� ������� ��������� �������
			read_the_date_and_time,		       // ��������� ���� � �����
		set_date_and_time,				   // ���������� ���� � �����
		reading_slice_readings,		       // ������ ������ ���������
		reading_the_load_chart,		       // ������ ������� ��������
		unidentified,					   // ������������ �������
		test
	};

	enum class list_ID_packet : uint8_t
	{
		ID9_Reading_the_counter_status,        // ������ ��������� ��������, NEED
		ID3_Get_current_measurements_by_phase, // �������� ������� ��������� �������, NEED
		ID11_Read_the_date_and_time,		   // ��������� ���� � �����, NEED
		ID11_Set_date_and_time,				   // ���������� ���� � �����
		ID12_Reading_slice_readings,		   // ������ ������ ���������, NEED
		ID10_Reading_the_load_chart			   // ������ ������� ��������
	};

	// Status � ��� ������� ���������� ������� ��� ����������� ������
	enum class internal_status : uchar
	{
		Done_successfully,		  // ��������� �������, 0
		Access_error,			  // ������ �������, 1
		The_cmd_is_not_supported, // ������� �� ��������������, 2
		Access_blocked,           // ������ ������������, 3
		Cmd_data_error,			  // ������ ������ �������, 4
		Parameter_base_error,	  // ������ ���� ����������, 5
		ID_is_not_supported,	  // ID �� ��������������, 6
		No_access_rights,		  // ��� ���� �������, 7
		User_error,				  // ������ ������������ (�� ���������� ������ ������������), 8
		Incorrect_cmd_parameter,  // ������������ �������� �������, 9
		Structure_number_error,   // ������ ������ ���������, 10
		Index_error,			  // ����� �������, 11
		Structure_access_error,   // ������ ������� � ���������, 12
		Data_block_CRC_error,	  // ������ CRC ����� ������, 13
		Device_error,			  // ������ ����������, 14
		No_data,				  // ��� ������, 15
		Attempted_debit_recharge, // ������� ���������� ���������� ������, 16
		The_device_is_occupied	  // ���������� ������, 17
	};

	const std::map<internal_status, std::string> ID9_Current_errors_map =
	{
		{ internal_status::Done_successfully,			string("") },
		{ internal_status::Access_error,				string("Error: ������ �������(1)") },
		{ internal_status::The_cmd_is_not_supported,	string("Error: ������� �� ��������������(2)") },
		{ internal_status::Access_blocked,				string("Error: ������ ������������(3)") },
		{ internal_status::Cmd_data_error,				string("Error: ������ ������ �������(4)") }, 
		{ internal_status::Parameter_base_error,		string("Error: ������ ���� ����������(5)") },
		{ internal_status::ID_is_not_supported,			string("Error: ID �� ��������������(6)") },
		{ internal_status::No_access_rights,			string("Error: ��� ���� �������(7)") },
		{ internal_status::User_error,					string("Error: ������ ������������ (�� ���������� ������ ������������) (8)") },
		{ internal_status::Incorrect_cmd_parameter,		string("Error: ������������ �������� ������� (9)") },
		{ internal_status::Structure_number_error,		string("Error: ������ ������ ���������(10)") },
		{ internal_status::Index_error,					string("Error: ������ �������(11)") },
		{ internal_status::Structure_access_error,		string("Error: ������ ������� � ��������� (12)") }, 
		{ internal_status::Data_block_CRC_error,		string("Error: ������ CRC ����� ������ (13)") },
		{ internal_status::Device_error,				string("Error: ������ ���������� (14)") },
		{ internal_status::No_data,						string("Error: ��� ������ (15)") },
		{ internal_status::Attempted_debit_recharge,	string("Error: ������� ���������� ���������� ������ (16)") },
		{ internal_status::The_device_is_occupied,		string("Error: ���������� ������ (17)") }
	};

	// ���-�� ��������� �������
	enum class ID9_Amount_of_available_energy : uint8_t
	{
		Active_A_plus_phase_A,	  // �������� �+(���� �), 0
		Active_A_minus_phase_A,	  // �������� �-(���� �), 1 
		Reactive_R_plus_phase_A,  // ���������� R+(���� �), 2
		Reactive_R_minus_phase_A, // ���������� R-(���� �), 3
		Active_A_plus_phase_B,	  // �������� �+(���� B), 4
		Active_A_minus_phase_B,	  // �������� �-(���� B), 5
		Reactive_R_plus_phase_B,  // ���������� R+(���� B), 6
		Reactive_R_minus_phase_B, // ���������� R-(���� B), 7
		Active_A_plus_phase_C,	  // �������� �+(���� C), 8
		Active_A_minus_phase_C,	  // �������� �-(���� C), 9
		Reactive_R_plus_phase_C,  // ���������� R+(���� C), 10(A)
		Reactive_R_minus_phase_C  // ���������� R-(���� C), 11(B)
	};

	// ������ ����������
	enum class ID3_Parameter_list : uint8_t
	{
		Active_power,   // �������� ��������, 0
		Reactive_power, // ���������� ��������, 1
		Voltage,		// ����������, 2
		Current,		// ���, 3
		Frequency,		// �������, 4
		Power_factor	// �����. ��������, 5
	};

	/*
	enum class ID3_sign : uint8_t
	{
		type_signed,  // �������� -128..127 �������
		type_unsigned // ����������� 0..255 �������
	};

	struct ID3_size_and_sign
	{
		uint8_t size;
		ID3_sign sign;
	};

	const std::map<uchar, ID3_size_and_sign> ID3_key_value_phase_pars
	{
		{ 'P', ID3_init_PQUIFK(4, ID3_sign::type_signed) },   // int32
		{ 'Q', ID3_init_PQUIFK(4, ID3_sign::type_signed) },	  // int32
		{ 'U', ID3_init_PQUIFK(4, ID3_sign::type_unsigned) }, // uint32
		{ 'I', ID3_init_PQUIFK(4, ID3_sign::type_unsigned) }, // uint32
		{ 'F', ID3_init_PQUIFK(2, ID3_sign::type_unsigned) }, // uint16
		{ 'K', ID3_init_PQUIFK(2, ID3_sign::type_signed) }    // int16
	};
	*/

	// ��� �������
	enum class ID12_Type_of_energy : uint8_t
	{
		Active_A_plus_phase_A,	 // �������� �+(���� �), 0
		Active_A_minus_phase_A,	 // �������� �-(���� �), 1 
		Reactive_R_plus_phase_A, // ���������� R+(���� �), 2
		Reactive_R_minus_phase_A,// ���������� R-(���� �), 3
		Active_A_plus_phase_B,	 // �������� �+(���� B), 4
		Active_A_minus_phase_B,	 // �������� �-(���� B), 5
		Reactive_R_plus_phase_B, // ���������� R+(���� B), 6
		Reactive_R_minus_phase_B,// ���������� R-(���� B), 7
		Active_A_plus_phase_C,	 // �������� �+(���� C), 8
		Active_A_minus_phase_C,	 // �������� �-(���� C), 9
		Reactive_R_plus_phase_C, // ���������� R+(���� C), 10(A)
		Reactive_R_minus_phase_C,// ���������� R-(���� C), 11(B)
		Active_A_plus_sum,	     // �������� �+(��������), 12(C)
		Active_A_minus_sum,		 // �������� �-(��������), 13(D)
		Reactive_R_plus_sum,     // ���������� R+(��������), 14(E)
		Reactive_R_minus_sum	 // ���������� R-(��������), 15(F)
	};

	// ��� �����
	enum class ID12_cut_type : uint8_t
	{
		current,			   // �������, � ��������� "������������", 0
		start_t0_day_reading,  // ���� ��������� �� ������ ���, 1
		month_to_date_reading, // ���� ��������� �� ������ ������, 2
		year_to_date_reading,  // ���� ��������� �� ������ ����, 3
		energy_gain__day,      // ���������� ������� �� ����, 4
		energy_gain__month,    // ���������� ������� �� �����, 5
		energy_increment_year  // ���������� ������� �� ���, 6
	};

	//��� �������
	enum class ID10_Type_of_energy : uint8_t
	{
		Active_A_plus_phase_A,	 // �������� �+(���� �), 0
		Active_A_minus_phase_A,	 // �������� �-(���� �), 1 
		Reactive_R_plus_phase_A, // ���������� R+(���� �), 2
		Reactive_R_minus_phase_A,// ���������� R-(���� �), 3
		Active_A_plus_phase_B,	 // �������� �+(���� B), 4
		Active_A_minus_phase_B,	 // �������� �-(���� B), 5
		Reactive_R_plus_phase_B, // ���������� R+(���� B), 6
		Reactive_R_minus_phase_B,// ���������� R-(���� B), 7
		Active_A_plus_phase_C,	 // �������� �+(���� C), 8
		Active_A_minus_phase_C,	 // �������� �-(���� C), 9
		Reactive_R_plus_phase_C, // ���������� R+(���� C), 10(A)
		Reactive_R_minus_phase_C,// ���������� R-(���� C), 11(B)
		Active_A_plus_sum,	     // �������� �+(��������), 12(C)
		Active_A_minus_sum,		 // �������� �-(��������), 13(D)
		Reactive_R_plus_sum,     // ���������� R+(��������), 14(E)
		Reactive_R_minus_sum,	 // ���������� R-(��������), 15(F)
		Voltage_phase_A,	     // U(���� �)**, 16(10)
		Voltage_phase_B,		 // U(���� B)**, 17(11)
		Voltage_phase_C,		 // U(���� C)**, 18(12)
		Current_phase_A,		 // I(���� �)**, 19(13)
		Current_phase_B,		 // I(���� B)**, 20(14)
		Current_phase_C			 // I(���� C)**, 21(15)
	};

	enum class RXTX : uint8_t
	{
		Request,
		Response,
		None
	};

	enum class INTERNAL_type_and_number : uint8_t
	{
		bit_7_not_encoded, // 0 - �� ����������
		bit_7_encoded	   // 1 - ����������
	};

	enum class INTERNAL_type_and_number_pool : uint8_t
	{
		pool_false, // 0
		pool_true   // 1
	};

	enum class INTERNAL_command_code : uint8_t
	{
		bit_7_not_encoded, // 0 - �� ����������
		bit_7_encoded	   // 1 - ����������
	};

	enum class INTERNAL_command_code_bit_6_0 : uint8_t
	{
		None,								  // �������
		Open_access_to_the_meter,			  // ������� ������ � ��������, 0x01
		Command_to_close_access_to_the_meter, // ������� ������� ������ � ��������, 0x02
		Read_data,							  // ��������� ������, 0x03
		Write_data,							  // �������� ������, 0x04
	};

	enum class EXTERNAL_command_code : uint8_t
	{
		bit_7_not_encoded, // 0 - �� ����������
		bit_7_encoded	   // 1 - ����������
	};

	enum class EXTERNAL_command_code_bit_6_0 : uint8_t
	{
		None_0,
		None_1,
		None_2,
		None_3,
		None_4,
		None_5,
		None_6,
		None_7,
		data_transfer
	};

	enum class EXTERNAL_type_and_number : uint8_t
	{
		bit_7_request, // 0 - ������
		bit_7_response // 1 - �����
	};

	enum class EXTERNAL_type_and_number_pool : uint8_t
	{
		pool_false, // 0
		pool_true   // 1
	};


	//������� ������ (���� - ��������) 

	const std::map<uint8_t, unsigned short> ID9_Current_errors =
	{
		{ 0, 0x0001 }, // ������ ������ dataflash/eeprom
		{ 1, 0x0002 }, // ������ �������
		{ 2, 0x0004 }, // ������ ��� ����������
		{ 3, 0x0008 }, // ������ ����������
		{ 4, 0x0010 }, // ������ ������ ����������� � ������ �������
		{ 5, 0x0020 }, // ������ ���� � ������� ����������
		{ 6, 0x0040 }, // ������ ����������� ���������� ����
		{ 7, 0x0080 }, // ������ ����������� ��������������
		{ 8, 0x0100 }, // ������ fram
		{ 11, 0x8000 } // ������ ������� ������� ��������� (CR2032 ���� ������)
	};
}