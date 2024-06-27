#include "../head/UnformedPacket.h"
#include "../head/Constant.h"
#include <iostream>

Packet  UnformedPacket::set_distination_address(uint64_t DEC_distination_address)
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

Packet UnformedPacket::set_external_distination_address(uint64_t DEC_distination_address)
{
	Packet hex_distination_address_8_byte = set_distination_address(DEC_distination_address);
	Packet hex_disitination_address_2_byte = hex_distination_address_8_byte.return_range(0, 1);
	return hex_disitination_address_2_byte;
}

UnformedPacket::UnformedPacket(uint64_t met_num)
{
	//internal_flag.add_one_element_back(0x7E);
	//internal_distination_address = set_internal_distination_address(235072); // это будет адрес по умолчанию

	external_flag.add_one_element_back(0x7A);
	external_address = set_external_distination_address(235072); // тоже адрес по умолчанию
	received_packet = Packet();
	unformed_packet = Packet();
	internal_lenght = Packet();
	//received_packet = rec;

	//rxtx = CS::RXTX::None;
	//сurrent_packet_number = 1;
}

void UnformedPacket::setUnformedPacket(Packet unform_packet)
{
	received_packet = unform_packet;
}

void UnformedPacket::reset()
{
	received_packet = Packet();
	internal_lenght = Packet();
	internal_information = Packet();
	unformed_packet = Packet();
}

Packet UnformedPacket::returnUnformedPacket()
{
	return unformed_packet;
}

bool UnformedPacket::cheek_internal_status(Packet byte)
{
	if (byte[0] == (uchar)CS::internal_status::Done_successfully)
	{
		return true;
	}
	else
	{
		CS::internal_status key = (CS::internal_status)byte[0];
		string err_str = CS::ID9_Current_errors_map.at(key);
		std::cout << "Status " << err_str.c_str() << std::endl;
		return false;
	}
}

void UnformedPacket::kostil_response_get_current_measurements_by_phase()
{
	// тут должны быть две магические константы

}



void UnformedPacket::kostil_response_read_the_date_and_time()
{
	//const uint8_t MAGIC_INDEX_internal_cmd = 15; // номер индекса кода команды внутреннего пакета
	const uint8_t MAGIC_INDEX_internal_flag = 6; // если начанать с нуля, и отрезать внешний флаг
	const uint8_t MAGIC_INDEX_internal_status = 9;


	// байтстаффинг внешний
	Packet external_bytestuffing_decod_packet = Encryption::bytestuffing_docode(received_packet.return_range(1, received_packet.lenght() - 1),
			0x73, 0x11, 0x7A, 0x73, 0x22, 0x73);

	// проверка внешнего CRC
	Packet external_cheek_fcs = Encryption::encryption_fcs16(crc, external_bytestuffing_decod_packet.return_range(0, 
		external_bytestuffing_decod_packet.lenght() - 3)).reverse();

	// вывод ошибки внешнего СRC
	if (!(((external_cheek_fcs[0] == external_bytestuffing_decod_packet[external_bytestuffing_decod_packet.lenght() - 2])
		&& (external_cheek_fcs[1] == external_bytestuffing_decod_packet[external_bytestuffing_decod_packet.lenght() - 1]))))
	{
		std::cout << "Error: ошибка внутри контрольной суммы внешнего пакета!" << std::endl;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// байтстаффинг внутренний
	Packet internal_bytestuffing_decod_packet = Encryption::bytestuffing_docode(external_bytestuffing_decod_packet.return_range(MAGIC_INDEX_internal_flag + 1,
		external_bytestuffing_decod_packet.lenght() - 3), 0x7D, 0x5E, 0x7E, 0x7D, 0x5D, 0x7D);

	// проверка внутреннего CRC
	Packet internal_cheek_fcs = Encryption::encryption_fcs16(crc, internal_bytestuffing_decod_packet.return_range(0,
		internal_bytestuffing_decod_packet.lenght() - 3)).reverse();

	// вывод ошибки внутреннего СRC
	if (!(((internal_cheek_fcs[0] == internal_bytestuffing_decod_packet[internal_bytestuffing_decod_packet.lenght() - 2])
		&& (internal_cheek_fcs[1] == internal_bytestuffing_decod_packet[internal_bytestuffing_decod_packet.lenght() - 1]))))
	{
		std::cout << "Error: ошибка внутри контрольной суммы внутреннего пакета!" << std::endl;
	}



	Packet status;
	status.add_one_element_back(internal_bytestuffing_decod_packet[MAGIC_INDEX_internal_status]);
	cheek_internal_status(status);

	
	internal_information = internal_bytestuffing_decod_packet.return_range(MAGIC_INDEX_internal_status,
		internal_bytestuffing_decod_packet.lenght() - 3);
	
	

	//проверка контролько байта
	if (Encryption::encryption_сontrol_byte(internal_information.return_range(0, 9))[0]
		!= internal_information[10])
	{
		std::cout << "Error: ошибка контрольного байта!" << std::endl;
	}


	Packet clocks;  clocks.add_one_element_back(internal_information[2]);  // часы, 0..23
	Packet minutes; minutes.add_one_element_back(internal_information[3]); // минуты, 0..59
	Packet seconds; seconds.add_one_element_back(internal_information[4]); // секунды, 0..59
	Packet weekday; weekday.add_one_element_back(internal_information[5]); // день недели, 0..6
	Packet numeral; numeral.add_one_element_back(internal_information[6]); // число, 1..31
	Packet month;   month.add_one_element_back(internal_information[7]);   // месяц, 1..12
	
	Packet year_decade;  year_decade.add_one_element_back(internal_information[8]);  // год(десятилетие), 00..99
	Packet year_century; year_century.add_one_element_back(internal_information[9]); // год(столетие)


	printf("    Дата и время счётчика: %02d.%02d.%02d%02d %02d:%02d:%02d\n", 
		numeral[0], month[0], year_century[0], year_decade[0],
		clocks[0], minutes[0], seconds[0]);


	reset();
}

