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
	//unformed_packet = Packet();
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

void UnformedPacket::kostil_response_read_the_date_and_time()
{
	const uint8_t MAGIC_INDEX_internal_cmd = 15; // номер индекса кода команды внутреннего пакета
	const uint8_t MAGIC_INDEX_internal_flag = 6;

	//uint32_t index = 4;
	//index -= 5;
	
	//received_packet = Packet(); 
	/*
	received_packet.add_one_element_back(0x2F);
	received_packet.add_one_element_back(0x12);
	received_packet.add_one_element_back(0x73);
	received_packet.add_one_element_back(0x73);
	received_packet.add_one_element_back(0xA1);
	received_packet.add_one_element_back(0x11);
	received_packet.add_one_element_back(0x73);
	received_packet.add_one_element_back(0x52);
	received_packet.add_one_element_back(0x73);
	received_packet.add_one_element_back(0x11);
	received_packet.add_one_element_back(0xFF);
	received_packet.add_one_element_back(0x22);
	received_packet.add_one_element_back(0x73);
	received_packet.add_one_element_back(0x22);
	received_packet.add_one_element_back(0x31);
	*/
	//received_packet.print_packet_not_id();
	
	//std::cout << std::endl;
	//std::cout << "rec[0, full]:"; received_packet.print_packet_not_id();
	//received_packet.return_range(1, received_packet.lenght() - 1).print_packet_not_id();
	//std::cout << "rec[1, full]:   ";  received_packet.return_range(1, received_packet.lenght() - 1).print_packet_not_id();
	//uint8_t count_error = 0;
	Packet external_bytestuffing_decod_packet = Encryption::bytestuffing_docode(received_packet.return_range(1, received_packet.lenght() - 1),
			0x73, 0x11, 0x7A, 0x73, 0x22, 0x73);
	
	const uint8_t value_internal_len = external_bytestuffing_decod_packet[7];
	//std::cout << std::endl;
	//std::cout << "         ext:   "; external_bytestuffing_decod_packet.print_packet_not_id();
	//std::cout << std::endl;
	Packet external_cheek_fcs = Encryption::encryption_fcs16(crc, external_bytestuffing_decod_packet.return_range(0, 
		external_bytestuffing_decod_packet.lenght() - 3)).reverse();
	
	//external_bytestuffing_decod_packet.print_packet_not_id();
	//std::cout << std::endl;


	if (!(((external_cheek_fcs[0] == external_bytestuffing_decod_packet[external_bytestuffing_decod_packet.lenght() - 2])
		&& (external_cheek_fcs[1] == external_bytestuffing_decod_packet[external_bytestuffing_decod_packet.lenght() - 1]))))
	{
		std::cout << "Error: ошибка внутри контрольной суммы внешнего пакета!" << std::endl;
	}
	
	Packet internal_bytestuffing_decod_packet = Encryption::bytestuffing_docode(external_bytestuffing_decod_packet.return_range(MAGIC_INDEX_internal_flag,
		external_bytestuffing_decod_packet.lenght() - 1), 0x73, 0x11, 0x7A, 0x73, 0x22, 0x73);

	
	//internal_bytestuffing_decod_packet.print_packet_not_id();

	internal_lenght.add_one_element_back(external_bytestuffing_decod_packet[7]);
	//printf("Internal lenght = %d\n", internal_lenght[0]);

	//unformed_packet = internal_bytestuffing_decod_packet; //!!!! ТО ЧТО ПОЙДЁТ НА ВЫВОД

	Packet status;
	status.add_one_element_back(external_bytestuffing_decod_packet[MAGIC_INDEX_internal_cmd + 1]);
	cheek_internal_status(status);

	
	internal_information = external_bytestuffing_decod_packet.return_range(MAGIC_INDEX_internal_cmd + 1, MAGIC_INDEX_internal_cmd + (uint8_t)internal_lenght[0]);
	//internal_information.print_packet_not_id();
	//unformed_packet = internal_information;
	
	//

	//std::cout << "Internal status: ";
	//internal_Status.print_packet_not_id();
	//Packet internal_Date = 
	//Packet internal_Control_byte = 

	///////////////////////////////////////////////////////////////////////
	/*
	Packet internal_bytestuffing_decod_packet = Encryption::bytestuffing_docode(received_packet.return_range(1, received_packet.lenght() - 1),
		0x73, 0x11, 0x7A, 0x73, 0x22, 0x73);
	//std::cout << std::endl;
	//std::cout << "         ext:   "; external_bytestuffing_decod_packet.print_packet_not_id();
	//std::cout << std::endl;
	Packet external_cheek_fcs = Encryption::encryption_fcs16(crc, external_bytestuffing_decod_packet.return_range(0,
		external_bytestuffing_decod_packet.lenght() - 3)).reverse();

	if (!(((external_cheek_fcs[0] == external_bytestuffing_decod_packet[external_bytestuffing_decod_packet.lenght() - 2])
		&& (external_cheek_fcs[1] == external_bytestuffing_decod_packet[external_bytestuffing_decod_packet.lenght() - 1]))))
	{
		std::cout << "Error: ошибка внутри контрольной суммы внутреннего пакета!" << std::endl;
	}
	*/
	
	reset();
}

