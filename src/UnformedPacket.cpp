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

vector<uchar> UnformedPacket::init_parameter_list(uchar list_phase_A)
{
	vector<uchar> key_ring; // копилка ключей
	uchar PQUIFK[6] = { 'P', 'Q', 'U', 'I', 'F', 'K' };

	for (int i = 0; i < 6; i++)
	{
		int low_bit;

		if (list_phase_A % 2 == 0)
			low_bit = 0;
		else
			low_bit = 1;

		if (low_bit == 1)
			key_ring.push_back(PQUIFK[i]);

		list_phase_A = list_phase_A >> 1;
	}

	return key_ring;
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
	const uint8_t MAGIC_INDEX_internal_flag = 6; // если начанать с нуля, и отрезать внешний флаг
	const uint8_t MAGIC_INDEX_internal_status = 9; // если начанать с нуля, и отрезать внутр. флаг

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

	//internal_information.print_packet();

	//проверка контролько байта
	if (Encryption::encryption_сontrol_byte(internal_information.return_range(0, internal_information.lenght() - 2))[0]
		!= internal_information[internal_information.lenght() - 1])
	{
		std::cout << "Error: ошибка контрольного байта!" << std::endl;
	}

	uchar uch_parameter_list_for_phase_A = internal_information[2];
	//printf("phase_A = %02X\n", uc_parameter_list_for_phase_A);

	vector<uchar> parameter_list = init_parameter_list(uch_parameter_list_for_phase_A);


	//нужен и end и оно должно как то в цикле вертется
	int8_t index_begin_internal_information = 5;

	int32_t  P = 0; double db_P = 0.; bool bool_output_P = false;
	int32_t  Q = 0; double db_Q = 0.; bool bool_output_Q = false;
	uint32_t U = 0; double db_U = 0.; bool bool_output_U = false;
	uint32_t I = 0; double db_I = 0.; bool bool_output_I = false;
	uint16_t F = 0; double db_F = 0.; bool bool_output_F = false;
	int16_t  K = 0; double db_K = 0.; bool bool_output_K = false;

	//Packet catout_internal_information;

	//catout_internal_information = internal_information.return_range(index_begin_internal_information, index_begin_internal_information + 4 - 1);
	//catout_internal_information.print_packet();

	
	for (int8_t i = 0; i < parameter_list.size(); i++)
	{
		uchar symbol = parameter_list[i];
		Packet catout_internal_information;
		int j = 0;
		int8_t number_of_bytes_in_the_type = 0;

		switch (symbol)
		{
		case 'P':
			number_of_bytes_in_the_type = 4;

			catout_internal_information = internal_information.return_range(index_begin_internal_information, index_begin_internal_information + number_of_bytes_in_the_type - 1);
			catout_internal_information = catout_internal_information.reverse();

			index_begin_internal_information += number_of_bytes_in_the_type;

			while (j < catout_internal_information.lenght() - 1)
			{
				P += catout_internal_information[j];
				P = P << 8;
				j++;
			}
			P += catout_internal_information[j];
			j = 0;

			bool_output_P = true;
			db_P = P * 0.01;
			//printf("Итоговое P = %f\n", db_P);
			break;


		case 'Q':
			number_of_bytes_in_the_type = 4;

			catout_internal_information = internal_information.return_range(index_begin_internal_information, index_begin_internal_information + number_of_bytes_in_the_type - 1);
			catout_internal_information = catout_internal_information.reverse();

			index_begin_internal_information += number_of_bytes_in_the_type;

			while (j < catout_internal_information.lenght() - 1)
			{
				Q += catout_internal_information[j];
				Q = Q << 8;
				j++;
			}
			Q += catout_internal_information[j];
			j = 0;

			bool_output_Q = true;
			db_Q = Q * 0.01;
			//printf("Итоговое Q = %f\n", db_Q);
			break;


		case 'U':
			number_of_bytes_in_the_type = 4;

			catout_internal_information = internal_information.return_range(index_begin_internal_information, index_begin_internal_information + number_of_bytes_in_the_type - 1);
			catout_internal_information = catout_internal_information.reverse();

			index_begin_internal_information += number_of_bytes_in_the_type;

			while (j < catout_internal_information.lenght() - 1)
			{
				U += catout_internal_information[j];
				U = U << 8;
				j++;
			}
			U += catout_internal_information[j];
			j = 0;

			bool_output_U = true;
			db_U = U * 0.01;
			//printf("Итоговое U = %f\n", db_U);
			break;
		
		case 'I':
			number_of_bytes_in_the_type = 4;

			catout_internal_information = internal_information.return_range(index_begin_internal_information, index_begin_internal_information + number_of_bytes_in_the_type - 1);
			catout_internal_information = catout_internal_information.reverse();

			index_begin_internal_information += number_of_bytes_in_the_type;

			while (j < catout_internal_information.lenght() - 1)
			{
				I += catout_internal_information[j];
				I = I << 8;
				j++;
			}
			I += catout_internal_information[j];
			j = 0;

			bool_output_I = true;
			db_I = I * 0.001;
			//printf("Итоговое I = %f\n", db_I);
			break;

		case 'F':
			number_of_bytes_in_the_type = 2;

			catout_internal_information = internal_information.return_range(index_begin_internal_information, index_begin_internal_information + number_of_bytes_in_the_type - 1);
			catout_internal_information = catout_internal_information.reverse();

			index_begin_internal_information += number_of_bytes_in_the_type;

			while (j < catout_internal_information.lenght() - 1)
			{
				F += catout_internal_information[j];
				F = F << 8;
				j++;
			}
			F += catout_internal_information[j];
			j = 0;

			bool_output_F = true;
			db_F = F * 0.01;
			//printf("Итоговое F = %f\n", db_F);
			break;

		case 'K':
			number_of_bytes_in_the_type = 2;

			catout_internal_information = internal_information.return_range(index_begin_internal_information, index_begin_internal_information + number_of_bytes_in_the_type - 1);
			catout_internal_information = catout_internal_information.reverse();

			index_begin_internal_information += number_of_bytes_in_the_type;

			while (j < catout_internal_information.lenght() - 1)
			{
				K += catout_internal_information[j];
				K = K << 8;
				j++;
			}
			K += catout_internal_information[j];
			j = 0;

			bool_output_K = true;
			db_K = K * 0.0001;
			//printf("Итоговое I = %f\n", db_K);
			break;
		}
	}

	printf("\n---------------------\n");
	printf("Параметр | Фаза A  \n");
	
	printf("P (W)    |");
	if (bool_output_P)
		printf(" %-10.3f\n", db_P);
	else
		printf(" Н/Д\n");


	printf("Q (Var)  |");
	if (bool_output_Q)
		printf(" %-10.3f\n", db_Q);
	else
		printf(" Н/Д\n");


	printf("U (V)    |");
	if (bool_output_U)
		printf(" %-10.2f\n", db_U);
	else
		printf(" Н/Д\n");


	
	printf("I (A)    |");
	if(bool_output_I)
		printf(" %-10.3f\n", db_I);
	else
		printf(" Н/Д\n");


	printf("F (Hz)   |");
	if(bool_output_F)
		printf(" %-10.2f\n", db_F);
	else
		printf(" Н/Д\n");


	printf("K        |");
	if(bool_output_K)
		printf(" %-10.2f\n", db_K);
	else
		printf(" Н/Д\n");

	printf("---------------------\n");

	reset();
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
	if (Encryption::encryption_сontrol_byte(internal_information.return_range(0, internal_information.lenght() - 2))[0]
		!= internal_information[internal_information.lenght() - 1])
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


	printf("\nДата и время счётчика: %02d.%02d.%02d%02d %02d:%02d:%02d\n", 
		numeral[0], month[0], year_century[0], year_decade[0],
		clocks[0], minutes[0], seconds[0]);


	reset();
}

void UnformedPacket::kostil_response_test()
{
}

