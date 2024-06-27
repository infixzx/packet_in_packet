#include "../head/FormedPacket.h"

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

	//temp += сurrent_packet_number;
	//incriment_сurrent_packet_number(); номер должен увеличиваться но почемуто у них в программе этого не происходит

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
		CS::EXTERNAL_type_and_number_pool::pool_false, CS::EXTERNAL_type_and_number_pool::pool_false);

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

	Packet sent_packet = Packet();
	Packet received_packet = Packet();
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

