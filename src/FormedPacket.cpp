#include "../head/FormedPacket.h"
#include <iostream>


FormedPacket::FormedPacket(uint64_t met_num)
{
	meter_number = met_num;
	internal_flag.add_one_element_back(0x7E);
	internal_distination_address = set_internal_distination_address(met_num); // это будет адрес по умолчанию
	
	external_flag.add_one_element_back(0x7A);
	external_address = set_external_distination_address(met_num); // тоже адрес по умолчанию

	rxtx = CS::RXTX::None;
	сurrent_packet_number = 1;

}


//запрос текущие измерения пофазно
void FormedPacket::request_get_current_measurements_by_phase()
{
	internal_lenght.add_one_element_back(0x05);
	internal_type_and_number = set_internal_type_and_number( // функцию нужно обязательно вызывать что увеличивать номер пакета
		CS::INTERNAL_type_and_number::bit_7_not_encoded, CS::INTERNAL_type_and_number_pool::pool_false,
		CS::INTERNAL_type_and_number_pool::pool_false, CS::INTERNAL_type_and_number_pool::pool_false);

	//просто читаем
	internal_command_code = set_internal_command_code(CS::INTERNAL_command_code::bit_7_not_encoded, CS::INTERNAL_command_code_bit_6_0::Read_data);


	internal_information.add_one_element_back(0x03); // ID
	internal_information.add_one_element_back(0xFF); // все параметры по 1 фазе
	internal_information.add_one_element_back(0xFF); // все параметры по 2 фазе
	internal_information.add_one_element_back(0xFF); // все параметры по 3 фазе
	Packet control_byte = Encryption::encryption_сontrol_byte(internal_information);

	internal_information = internal_information + control_byte;
	internal_FCS = Encryption::encryption_fcs16(crc, (internal_lenght + internal_distination_address + internal_type_and_number +
		internal_command_code + internal_information));

	internal_FCS.apply_reverse();

	Packet internal_bytestuffing_encod = Encryption::bytestuffing_encod((internal_lenght + internal_distination_address +
		internal_type_and_number + internal_command_code + internal_information + internal_FCS), 0x7E, 0x7D, 0x5E, 0x7D, 0x7D, 0x5D);


	sent_packet = formed_external_kostil_packet(internal_flag + internal_bytestuffing_encod);

	reset();
}


//запрос Прочитать дату и время
void FormedPacket::request_read_the_date_and_time()
{
	internal_lenght.add_one_element_back(0x02);
	internal_type_and_number = set_internal_type_and_number( // функцию нужно обязательно вызывать что увеличивать номер пакета
		CS::INTERNAL_type_and_number::bit_7_not_encoded, CS::INTERNAL_type_and_number_pool::pool_false,
		CS::INTERNAL_type_and_number_pool::pool_false, CS::INTERNAL_type_and_number_pool::pool_false);
	
	//просто читаем
	internal_command_code = set_internal_command_code(CS::INTERNAL_command_code::bit_7_not_encoded, CS::INTERNAL_command_code_bit_6_0::Read_data);
	
	internal_information.add_one_element_back(0x0B); // костыль?
	Packet control_byte = Encryption::encryption_сontrol_byte(internal_information);

	internal_information = internal_information + control_byte;
	internal_FCS = Encryption::encryption_fcs16(crc, (internal_lenght + internal_distination_address + internal_type_and_number +
		internal_command_code + internal_information));

	internal_FCS.apply_reverse();

	//Packet FullFormedPacket = 
	Packet internal_bytestuffing_encod = Encryption::bytestuffing_encod((internal_lenght + internal_distination_address + 
		internal_type_and_number + internal_command_code + internal_information + internal_FCS), 0x7E, 0x7D, 0x5E, 0x7D, 0x7D, 0x5D);


	sent_packet = formed_external_kostil_packet(internal_flag + internal_bytestuffing_encod);

	//full.print_packet();

	//FullFormedPacket.print_packet();
	reset();
}



void FormedPacket::print_binary_ID12(uint16_t value)
{
	char buffer[17];
	buffer[16] = '\0';

	for (uint8_t i = 0; i < 16; i++)
	{
		if (value & 1)
			buffer[15 - i] = '1';
		else
			buffer[15 - i] = '0';

		value = value >> 1;
	}

	for (int i = 0; i < 16; i++)
	{
		if ((i % 4 == 0) && (i != 0))
			std::cout << " ";
		printf("%c", buffer[i]);
	}
	std::cout << std::endl;
}



void FormedPacket::print_date_ID12(uchar high_byte, uchar low_byte)
{
	const uint8_t month_lenght_bit = 4;
	const uint8_t date_lenght_bit = 5;
	const uint8_t year_lenght_bit = 7;

	uint16_t result = 0;

	result += high_byte;
	result = result << 8;
	result += low_byte;

	uint16_t high_bit_result = result % 2;
	high_bit_result = high_bit_result << 15;
	result = result >> 1;
	result += high_bit_result;

	uint16_t month = result >> (16 - month_lenght_bit);

	uint16_t date = result << month_lenght_bit;
	date = date >> (month_lenght_bit + year_lenght_bit);

	uint16_t year = result << (month_lenght_bit + date_lenght_bit);
	year = year >> (month_lenght_bit + date_lenght_bit);

	//printf("%02u.%02u.20%02u\n", date, month, year);
}


// число:месяц:год, вернёт массив из двух байт(high, low)
uchar* FormedPacket::input_print_date_ID12(uint16_t date, uint16_t month, uint16_t year)
{
	const uint8_t date_lenght_bit = 5;
	const uint8_t year_lenght_bit = 7;

	uchar *result = new uchar[2];
	uint16_t temp_res = 0;

	temp_res += year << 1;
	temp_res += date << (year_lenght_bit + 1);
	temp_res += month << (date_lenght_bit + year_lenght_bit + 1);
	temp_res += (month >> 3) % 2;

	result[0] = temp_res >> 8; // high 
	result[1] = ((temp_res << 8) >> 8); // low

	//printf("%2X %2X\n", result[0], result[1]);

	return result;
}


//Чтение срезов показаний
void FormedPacket::request_reading_slice_readings_base()
{
		// Cпрашиваемая дату (число, месяц, год)
		// Cпрашиваем тип запрашиваемой энергии 0..15
		// Cпрашиваем номер тарифа на самом деле от 0..3(но в программе 1..4). Если указать 0xFF то выводятся суммарные данные по всем тарифам. Тип среза при этот игнорируется.
	// Cпрашиваем тип среза 0..6

	internal_lenght.add_one_element_back(0x07);
	internal_type_and_number = set_internal_type_and_number( // функцию нужно обязательно вызывать что увеличивать номер пакета
		CS::INTERNAL_type_and_number::bit_7_not_encoded, CS::INTERNAL_type_and_number_pool::pool_false,
		CS::INTERNAL_type_and_number_pool::pool_false, CS::INTERNAL_type_and_number_pool::pool_false);

	//просто читаем
	internal_command_code = set_internal_command_code(CS::INTERNAL_command_code::bit_7_not_encoded, CS::INTERNAL_command_code_bit_6_0::Read_data);

	internal_information.add_one_element_back(0x0C); // ID


	// дата в формата u16
	do 
	{
		uint16_t day = 0, month = 0, year = 0;

		printf("	Введите дату в формате (число месяц год): ");
		
		int8_t count_arg = scanf_s("%hu %hu %hu", &day, &month, &year);
		
		while (getchar() != '\n');

		if (count_arg == 3)
		{
			if ((day >= 1 && day <= 31) && (month >= 1 && month <= 12) && (year >= 2000 && year <= 2100))
			{
				printf("%02hu.%02hu.%04hu\n", day, month, year);
				year -= 2000;
				uchar* temp_date = input_print_date_ID12(day, month, year);
				internal_information.add_one_element_back(temp_date[0]);
				internal_information.add_one_element_back(temp_date[1]);
				delete[] temp_date;
				break;
			}
		}

		printf("Попробуйте ещё раз!\n");
	} while (true);



	// тип запрашиваемой энергии
	do
	{	
		uint16_t key = 16;

		printf("\n0  - Активная А+(фаза А)    \n");
		printf("1  - Активная А-(фаза А)    \n");
		printf("2  - Реактивная R+(фаза А)  \n");
		printf("3  - Реактивная R-(фаза А)  \n");
		printf("4  - Активная А+(фаза B)    \n");
		printf("5  - Активная А-(фаза B)    \n");
		printf("6  - Реактивная R+(фаза B)  \n");
		printf("7  - Реактивная R-(фаза B)  \n");
		printf("8  - Активная А+(фаза C)    \n");
		printf("9  - Активная А-(фаза C)    \n");
		printf("10 - Реактивная R+(фаза C)  \n");
		printf("11 - Реактивная R-(фаза C)  \n");
		printf("12 - Активная А+(суммарно)  \n");
		printf("13 - Активная А-(суммарно)  \n");
		printf("14 - Реактивная R+(суммарно)\n");
		printf("15 - Реактивная R-(суммарно)\n");
		printf("	Выберите тип запрашиваемой энергии: ");

		int8_t count_arg = scanf_s("%hu", &key);

		while (getchar() != '\n');

		if (count_arg == 1)
		{
			if(key >= 0 && key <= 15)
			{
				printf("%02hu\n", key);
				internal_information.add_one_element_back((uchar)key);
				break;
			}
		}

		printf("Попробуйте ещё раз!\n");

	} while (true);


	bool switch_cut_type = true;

	// номер тарифа
	do
	{
		uint16_t tariff_number = 255;
		printf("\nВведите номер тарифа (1..4), \n");
		printf("если выбрать 0 то, выводятся суммарные данные по всем тарифам,\n");
		printf("	тип среза при этом игнорируется: ");

		int8_t count_arg = scanf_s("%hu", &tariff_number);

		while (getchar() != '\n');

		if (count_arg == 1)
		{
			if (tariff_number >= 1 && tariff_number <= 4)
			{
				printf("%02hu\n", tariff_number);
				tariff_number--;
				internal_information.add_one_element_back((uchar)tariff_number);
				break;
			}

			if (tariff_number == 0)
			{
				switch_cut_type = false;
				internal_information.add_one_element_back(0xFF);
				internal_information.add_one_element_back(0x00);
				break;
			}
		}

		printf("Попробуйте ещё раз!\n");


	} while (true);


	// тип среза
	while (switch_cut_type)
	{
		uint16_t cut_type = 16;


		printf("\n0 - текущие (пакопительно)\n");
		printf("1 - срез показаний на начало дня\n");
		printf("2 - срез показаний на начало месяца\n");
		printf("3 - срез показаний на начало года\n");
		printf("4 - приращение энергии за день\n");
		printf("5 - приращение энергии за месяц\n");
		printf("6 - приращение энергии за год\n");
		printf("	Выберите тип среза: ");

		int8_t count_arg = scanf_s("%hu", &cut_type);

		while (getchar() != '\n');

		if (count_arg == 1)
		{
			if (cut_type >= 0 && cut_type <= 6)
			{
				printf("%02hu\n", cut_type);
				internal_information.add_one_element_back((uchar)cut_type);
				break;
			}
		}

		printf("Попробуйте ещё раз!\n");
	}

	//internal_information.print_packet_not_id();

	
	Packet control_byte = Encryption::encryption_сontrol_byte(internal_information);
	internal_information = internal_information + control_byte;


	internal_FCS = Encryption::encryption_fcs16(crc, (internal_lenght + internal_distination_address + internal_type_and_number +
		internal_command_code + internal_information));

	internal_FCS.apply_reverse();

	//Packet FullFormedPacket = 
	Packet internal_bytestuffing_encod = Encryption::bytestuffing_encod((internal_lenght + internal_distination_address +
		internal_type_and_number + internal_command_code + internal_information + internal_FCS), 0x7E, 0x7D, 0x5E, 0x7D, 0x7D, 0x5D);


	sent_packet = formed_external_kostil_packet(internal_flag + internal_bytestuffing_encod);

	//full.print_packet();

	//FullFormedPacket.print_packet();
	

	reset();
}


void FormedPacket::request_test(std::vector<uchar> temp)
{
	/*
	for (int i = 0; i < temp.size(); i++)
	{
		sent_packet.add_one_element_back(temp[i]);
	}
	*/

	/*
	Packet cheek_external;
	cheek_external.add_one_element_back(0x00);
	cheek_external.add_one_element_back(0x40);
	cheek_external.add_one_element_back(0x96);
	cheek_external.add_one_element_back(0x09);
	cheek_external.add_one_element_back(0x0E);
	cheek_external.add_one_element_back(0x08);
	cheek_external.add_one_element_back(0x7E);
	cheek_external.add_one_element_back(0xAA);
	cheek_external.add_one_element_back(0xE2);

	Packet bg;
	bg.add_one_element_back(0x7A);


	sent_packet = bg + cheek_external + cheek_internal;
	*/

	//Packet prr = Encryption::encryption_fcs16(crc, (cheek_external + cheek_internal));
	//prr.apply_reverse();
	//prr.print_packet_not_id();
	
	
	/*
	sent_packet.add_one_element_back(0x7A);
	sent_packet.add_one_element_back(0x00);
	sent_packet.add_one_element_back(0x40);
	sent_packet.add_one_element_back(0x96);
	sent_packet.add_one_element_back(0x0A);
	sent_packet.add_one_element_back(0x0E);
	sent_packet.add_one_element_back(0x08);
	sent_packet.add_one_element_back(0x7E);
	sent_packet.add_one_element_back(0x02);
	sent_packet.add_one_element_back(0x40);
	sent_packet.add_one_element_back(0x96);
	sent_packet.add_one_element_back(0x03);
	sent_packet.add_one_element_back(0x00);
	sent_packet.add_one_element_back(0x00);
	sent_packet.add_one_element_back(0x00);
	sent_packet.add_one_element_back(0x0A);
	sent_packet.add_one_element_back(0x03);
	sent_packet.add_one_element_back(0x09);
	sent_packet.add_one_element_back(0x09);
	sent_packet.add_one_element_back(0x99);
	sent_packet.add_one_element_back(0x12);
	sent_packet.add_one_element_back(0xBC); // ДОЛЖНО БЫТЬ BC, А У МЕНЯ 9D
	sent_packet.add_one_element_back(0xCC); // ДОЛЖНО БЫТЬ СС, А У МЕНЯ 32
	*/

	reset();
}


Packet FormedPacket::set_distination_address(uint64_t DEC_distination_address)
{
	Packet result;
	uint8_t div = 16;

	for (uint8_t i = 0; i < 8; i++)
	{
		uint8_t low_rem = DEC_distination_address % div; 
		DEC_distination_address /= div;
		uint8_t high_rem = DEC_distination_address % div;
		DEC_distination_address /= div;
		uchar HEX_byte = high_rem * div + low_rem;
		Packet temp;
		temp.add_one_element_back(HEX_byte);
		result = result + temp;
	}

	return result;
}

Packet FormedPacket::set_internal_distination_address(uint64_t DEC_distination_address)
{
	Packet hex_distination_address_8_byte = set_distination_address(DEC_distination_address);
	Packet hex_disitination_address_6_byte = hex_distination_address_8_byte.return_range(0, 5);
	return hex_disitination_address_6_byte;
}

Packet FormedPacket::set_external_distination_address(uint64_t DEC_distination_address)
{
	Packet hex_distination_address_8_byte = set_distination_address(DEC_distination_address);
	Packet hex_disitination_address_2_byte = hex_distination_address_8_byte.return_range(0, 1);
	return hex_disitination_address_2_byte;
}

Packet FormedPacket::set_internal_type_and_number(CS::INTERNAL_type_and_number bit_7_rxtx, CS::INTERNAL_type_and_number_pool bit_6_pool,
	CS::INTERNAL_type_and_number_pool bit_5_pool, CS::INTERNAL_type_and_number_pool bit_4_pool)
{
	uint8_t temp = 0;
	Packet tmp_packet;

	if (bit_7_rxtx == CS::INTERNAL_type_and_number::bit_7_encoded)
		temp += 128;
	if (bit_6_pool == CS::INTERNAL_type_and_number_pool::pool_true)
		temp += 64;
	if (bit_5_pool == CS::INTERNAL_type_and_number_pool::pool_true)
		temp += 32;
	if (bit_4_pool == CS::INTERNAL_type_and_number_pool::pool_true)
		temp += 16;

	temp += сurrent_packet_number;
	incriment_сurrent_packet_number();

	tmp_packet.add_one_element_back(temp);

	return tmp_packet;
}

Packet FormedPacket::set_external_type_and_number(CS::EXTERNAL_type_and_number bit_7_request_response, CS::EXTERNAL_type_and_number_pool bit_6_pool,
	CS::EXTERNAL_type_and_number_pool bit_5_pool, CS::EXTERNAL_type_and_number_pool bit_4_pool)
{
	uint8_t temp = 0;
	Packet tmp_packet;

	if (bit_7_request_response == CS::EXTERNAL_type_and_number::bit_7_response)
		temp += 128;
	if (bit_6_pool == CS::EXTERNAL_type_and_number_pool::pool_true)
		temp += 64;
	if (bit_5_pool == CS::EXTERNAL_type_and_number_pool::pool_true)
		temp += 32;
	if (bit_4_pool == CS::EXTERNAL_type_and_number_pool::pool_true)
		temp += 16;

	temp += сurrent_packet_number - 1; //??? номер должен увеличиваться но почемуто у них в программе этого не происходит

	tmp_packet.add_one_element_back(temp);

	return tmp_packet;
}

Packet FormedPacket::set_internal_command_code(CS::INTERNAL_command_code bit_7_code_encode, CS::INTERNAL_command_code_bit_6_0 bit_6_0)
{
	uint8_t temp = 0;
	Packet tmp_packet;
	if (bit_7_code_encode == CS::INTERNAL_command_code::bit_7_encoded)
		temp += 128;
	
	switch (bit_6_0)
	{
	case CS::INTERNAL_command_code_bit_6_0::Open_access_to_the_meter:
		temp += 1;
		break;
	case CS::INTERNAL_command_code_bit_6_0::Command_to_close_access_to_the_meter:
		temp += 2;
		break;
	case CS::INTERNAL_command_code_bit_6_0::Read_data:
		temp += 3;
		break;
	case CS::INTERNAL_command_code_bit_6_0::Write_data:
		temp += 4;
		break;
	}

	tmp_packet.add_one_element_back(temp);
	return tmp_packet;
}

Packet FormedPacket::set_external_command_code(CS::EXTERNAL_command_code bit_7_code_encode, CS::EXTERNAL_command_code_bit_6_0 bit_6_0)
{
	uchar temp = 0;
	Packet tmp_packet;

	if (bit_7_code_encode == CS::EXTERNAL_command_code::bit_7_encoded)
		temp += 128;
	if (bit_6_0 == CS::EXTERNAL_command_code_bit_6_0::data_transfer)
		temp += 8;

	tmp_packet.add_one_element_back(temp);
	return tmp_packet;
}

Packet FormedPacket::formed_external_kostil_packet(Packet Information) 
{
	external_lenght.add_one_element_back(Information.lenght());
	external_information = Information;
	external_command_code = set_external_command_code(CS::EXTERNAL_command_code::bit_7_not_encoded, CS::EXTERNAL_command_code_bit_6_0::data_transfer);
	external_address_lenght.add_one_element_back(0x00);
	

	external_type_and_number = set_external_type_and_number(CS::EXTERNAL_type_and_number::bit_7_request, CS::EXTERNAL_type_and_number_pool::pool_false,
		CS::EXTERNAL_type_and_number_pool::pool_false, CS::EXTERNAL_type_and_number_pool::pool_false); //???

	external_FCS = Encryption::encryption_fcs16(crc, (external_address_lenght + external_address + external_type_and_number +
		external_lenght + external_command_code + external_information));

	external_FCS = external_FCS.reverse();

	Packet external_bytestuffing_encod = Encryption::bytestuffing_encod((external_address_lenght + external_address 
		+ external_type_and_number + external_lenght + external_command_code + external_information + external_FCS), 0x7A, 0x73, 0x11, 0x73, 0x73, 0x22);
	
	return external_flag + external_bytestuffing_encod;
}

void FormedPacket::reset()
{
	internal_lenght = Packet();
	internal_information = Packet();
	
	external_lenght = Packet();
	external_information = Packet();
	external_address_lenght = Packet();

	//sent_packet = Packet();
	//Packet received_packet = Packet();
}

void FormedPacket::reset_sent_packet()
{
	sent_packet = Packet();
}

Packet FormedPacket::returnSentPacket()
{
	return sent_packet;
}

void FormedPacket::incriment_сurrent_packet_number()
{
	if (сurrent_packet_number >= 0 && сurrent_packet_number < 15)
		сurrent_packet_number++;
	else if (сurrent_packet_number == 15)
		сurrent_packet_number = 0;
}
