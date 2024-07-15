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
			quit,							   // выход
		reading_the_counter_status,		   // Чтение состояния счётчика
			get_current_measurements_by_phase, // Получить текущие измерения пофазно
			read_the_date_and_time,		       // Прочитать дату и время
		set_date_and_time,				   // Установить дату и время
		reading_slice_readings,		       // Чтение срезов показаний
		reading_the_load_chart,		       // Чтения графика нагрузки
		unidentified,					   // неопознанная клавиша
		test
	};

	enum class list_ID_packet : uint8_t
	{
		ID9_Reading_the_counter_status,        // Чтение состояния счётчика, NEED
		ID3_Get_current_measurements_by_phase, // Получить текущие измерения пофазно, NEED
		ID11_Read_the_date_and_time,		   // Прочитать дату и время, NEED
		ID11_Set_date_and_time,				   // Установить дату и время
		ID12_Reading_slice_readings,		   // Чтение срезов показаний, NEED
		ID10_Reading_the_load_chart			   // Чтения графика нагрузки
	};

	// Status – код статуса выполнения команды для внутреннего пакета
	enum class internal_status : uchar
	{
		Done_successfully,		  // Выполнено успешно, 0
		Access_error,			  // Ошибка доступа, 1
		The_cmd_is_not_supported, // Команда не поддерживается, 2
		Access_blocked,           // Доступ заблокирован, 3
		Cmd_data_error,			  // Ошибка данных команды, 4
		Parameter_base_error,	  // Ошибка базы параметров, 5
		ID_is_not_supported,	  // ID не поддерживается, 6
		No_access_rights,		  // Нет прав доступа, 7
		User_error,				  // Ошибка пользователя (не существует такого пользователя), 8
		Incorrect_cmd_parameter,  // Неправильный параметр команды, 9
		Structure_number_error,   // Ошибка номера структуры, 10
		Index_error,			  // Ошиба индекса, 11
		Structure_access_error,   // Ошибка доступа к структуре, 12
		Data_block_CRC_error,	  // Ошибка CRC блока данных, 13
		Device_error,			  // Ошибка устройства, 14
		No_data,				  // Нет данных, 15
		Attempted_debit_recharge, // Попытка повторного пополнения дебета, 16
		The_device_is_occupied	  // Устройство занято, 17
	};

	const std::map<internal_status, std::string> ID9_Current_errors_map =
	{
		{ internal_status::Done_successfully,			string("") },
		{ internal_status::Access_error,				string("Error: ошибка доступа(1)") },
		{ internal_status::The_cmd_is_not_supported,	string("Error: команда не поддерживается(2)") },
		{ internal_status::Access_blocked,				string("Error: доступ заблокирован(3)") },
		{ internal_status::Cmd_data_error,				string("Error: ошибка данных команды(4)") }, 
		{ internal_status::Parameter_base_error,		string("Error: ошибка базы параметров(5)") },
		{ internal_status::ID_is_not_supported,			string("Error: ID не поддерживается(6)") },
		{ internal_status::No_access_rights,			string("Error: нет прав доступа(7)") },
		{ internal_status::User_error,					string("Error: ошибка пользователя (не существует такого пользователя) (8)") },
		{ internal_status::Incorrect_cmd_parameter,		string("Error: неправильный параметр команды (9)") },
		{ internal_status::Structure_number_error,		string("Error: ошибка номера структуры(10)") },
		{ internal_status::Index_error,					string("Error: ошибка индекса(11)") },
		{ internal_status::Structure_access_error,		string("Error: ошибка доступа к структуре (12)") }, 
		{ internal_status::Data_block_CRC_error,		string("Error: ошибка CRC блока данных (13)") },
		{ internal_status::Device_error,				string("Error: ошибка устройства (14)") },
		{ internal_status::No_data,						string("Error: нет данных (15)") },
		{ internal_status::Attempted_debit_recharge,	string("Error: попытка повторного пополнения дебета (16)") },
		{ internal_status::The_device_is_occupied,		string("Error: устройство занято (17)") }
	};

	// Кол-во доступных энергий
	enum class ID9_Amount_of_available_energy : uint8_t
	{
		Active_A_plus_phase_A,	  // Активная А+(фаза А), 0
		Active_A_minus_phase_A,	  // Активная А-(фаза А), 1 
		Reactive_R_plus_phase_A,  // Реактивная R+(фаза А), 2
		Reactive_R_minus_phase_A, // Реактивная R-(фаза А), 3
		Active_A_plus_phase_B,	  // Активная А+(фаза B), 4
		Active_A_minus_phase_B,	  // Активная А-(фаза B), 5
		Reactive_R_plus_phase_B,  // Реактивная R+(фаза B), 6
		Reactive_R_minus_phase_B, // Реактивная R-(фаза B), 7
		Active_A_plus_phase_C,	  // Активная А+(фаза C), 8
		Active_A_minus_phase_C,	  // Активная А-(фаза C), 9
		Reactive_R_plus_phase_C,  // Реактивная R+(фаза C), 10(A)
		Reactive_R_minus_phase_C  // Реактивная R-(фаза C), 11(B)
	};

	// Список параметров
	enum class ID3_Parameter_list : uint8_t
	{
		Active_power,   // активная мощность, 0
		Reactive_power, // реактивная мощность, 1
		Voltage,		// напряжение, 2
		Current,		// ток, 3
		Frequency,		// частота, 4
		Power_factor	// коэфф. мощности, 5
	};

	/*
	enum class ID3_sign : uint8_t
	{
		type_signed,  // знаковый -128..127 условно
		type_unsigned // беззнаковый 0..255 условно
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

	// Тип энергии
	enum class ID12_Type_of_energy : uint8_t
	{
		Active_A_plus_phase_A,	 // Активная А+(фаза А), 0
		Active_A_minus_phase_A,	 // Активная А-(фаза А), 1 
		Reactive_R_plus_phase_A, // Реактивная R+(фаза А), 2
		Reactive_R_minus_phase_A,// Реактивная R-(фаза А), 3
		Active_A_plus_phase_B,	 // Активная А+(фаза B), 4
		Active_A_minus_phase_B,	 // Активная А-(фаза B), 5
		Reactive_R_plus_phase_B, // Реактивная R+(фаза B), 6
		Reactive_R_minus_phase_B,// Реактивная R-(фаза B), 7
		Active_A_plus_phase_C,	 // Активная А+(фаза C), 8
		Active_A_minus_phase_C,	 // Активная А-(фаза C), 9
		Reactive_R_plus_phase_C, // Реактивная R+(фаза C), 10(A)
		Reactive_R_minus_phase_C,// Реактивная R-(фаза C), 11(B)
		Active_A_plus_sum,	     // Активная А+(суммарно), 12(C)
		Active_A_minus_sum,		 // Активная А-(суммарно), 13(D)
		Reactive_R_plus_sum,     // Реактивная R+(суммарно), 14(E)
		Reactive_R_minus_sum	 // Реактивная R-(суммарно), 15(F)
	};

	// Тип среза
	enum class ID12_cut_type : uint8_t
	{
		current,			   // текущие, в программе "пакопительно", 0
		start_t0_day_reading,  // срез показаний на начало дня, 1
		month_to_date_reading, // срез показаний на начало месяца, 2
		year_to_date_reading,  // срез показаний на начало года, 3
		energy_gain__day,      // приращение энергии за день, 4
		energy_gain__month,    // приращение энергии за месяц, 5
		energy_increment_year  // приращение энергии за год, 6
	};

	//Тип энергии
	enum class ID10_Type_of_energy : uint8_t
	{
		Active_A_plus_phase_A,	 // Активная А+(фаза А), 0
		Active_A_minus_phase_A,	 // Активная А-(фаза А), 1 
		Reactive_R_plus_phase_A, // Реактивная R+(фаза А), 2
		Reactive_R_minus_phase_A,// Реактивная R-(фаза А), 3
		Active_A_plus_phase_B,	 // Активная А+(фаза B), 4
		Active_A_minus_phase_B,	 // Активная А-(фаза B), 5
		Reactive_R_plus_phase_B, // Реактивная R+(фаза B), 6
		Reactive_R_minus_phase_B,// Реактивная R-(фаза B), 7
		Active_A_plus_phase_C,	 // Активная А+(фаза C), 8
		Active_A_minus_phase_C,	 // Активная А-(фаза C), 9
		Reactive_R_plus_phase_C, // Реактивная R+(фаза C), 10(A)
		Reactive_R_minus_phase_C,// Реактивная R-(фаза C), 11(B)
		Active_A_plus_sum,	     // Активная А+(суммарно), 12(C)
		Active_A_minus_sum,		 // Активная А-(суммарно), 13(D)
		Reactive_R_plus_sum,     // Реактивная R+(суммарно), 14(E)
		Reactive_R_minus_sum,	 // Реактивная R-(суммарно), 15(F)
		Voltage_phase_A,	     // U(фаза А)**, 16(10)
		Voltage_phase_B,		 // U(фаза B)**, 17(11)
		Voltage_phase_C,		 // U(фаза C)**, 18(12)
		Current_phase_A,		 // I(фаза А)**, 19(13)
		Current_phase_B,		 // I(фаза B)**, 20(14)
		Current_phase_C			 // I(фаза C)**, 21(15)
	};

	enum class RXTX : uint8_t
	{
		Request,
		Response,
		None
	};

	enum class INTERNAL_type_and_number : uint8_t
	{
		bit_7_not_encoded, // 0 - не кодируется
		bit_7_encoded	   // 1 - кодируется
	};

	enum class INTERNAL_type_and_number_pool : uint8_t
	{
		pool_false, // 0
		pool_true   // 1
	};

	enum class INTERNAL_command_code : uint8_t
	{
		bit_7_not_encoded, // 0 - не кодируется
		bit_7_encoded	   // 1 - кодируется
	};

	enum class INTERNAL_command_code_bit_6_0 : uint8_t
	{
		None,								  // затычка
		Open_access_to_the_meter,			  // Открыть доступ к счётчику, 0x01
		Command_to_close_access_to_the_meter, // Команда закрыть доступ к счётчику, 0x02
		Read_data,							  // Прочитать данные, 0x03
		Write_data,							  // Записать данные, 0x04
	};

	enum class EXTERNAL_command_code : uint8_t
	{
		bit_7_not_encoded, // 0 - не кодируется
		bit_7_encoded	   // 1 - кодируется
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
		bit_7_request, // 0 - запрос
		bit_7_response // 1 - ответ
	};

	enum class EXTERNAL_type_and_number_pool : uint8_t
	{
		pool_false, // 0
		pool_true   // 1
	};


	//Текущие ошибки (ключ - значение) 

	const std::map<uint8_t, unsigned short> ID9_Current_errors =
	{
		{ 0, 0x0001 }, // ошибка чтения dataflash/eeprom
		{ 1, 0x0002 }, // ошибка таймера
		{ 2, 0x0004 }, // ошибка баз накоплений
		{ 3, 0x0008 }, // ошибка параметров
		{ 4, 0x0010 }, // ошибка разные направления в разных каналах
		{ 5, 0x0020 }, // ошибка токи в каналах отличаются
		{ 6, 0x0040 }, // ошибка воздействие магнитного поля
		{ 7, 0x0080 }, // ошибка воздействие радиоизлучения
		{ 8, 0x0100 }, // ошибка fram
		{ 11, 0x8000 } // ошибка батарея таймера разряжена (CR2032 ниже уровня)
	};
}