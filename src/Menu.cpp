#include <iostream>
#include "../head/Menu.h"

Menu::Menu()
{
	cmd_configuration(160, 40, 100);
	meter_number = 235072;
	COM = new ComPort("COM7");
	formed_packet   = new FormedPacket(meter_number);
	unformed_packet = new UnformedPacket(meter_number);

}

Menu::~Menu()
{
	delete COM;
	delete formed_packet;
	delete unformed_packet;
}


void Menu::init()
{
	do
	{
		print_dash(160);

		print_input_invitation();
		sw = is_key_press();

		//formed_packet.request_get_current_measurements_by_phase();
		switch (sw)
		{
		case CS::menu_options::quit: // выход
			return;
			break;

		case CS::menu_options::reading_the_counter_status: // Чтение состояния счётчика
			//заглушка
			break;

		case CS::menu_options::get_current_measurements_by_phase: // Получить текущие измерения пофазно
			formed_packet->request_get_current_measurements_by_phase();
			break;

		case CS::menu_options::read_the_date_and_time: // Прочитать дату и время
			formed_packet->request_read_the_date_and_time();
			break;

		case CS::menu_options::set_date_and_time:  // Установить дату и время
			//заглушка
			break;

		case CS::menu_options::reading_slice_readings:  // Чтение срезов показаний
			//заглушка
			break;

		case CS::menu_options::reading_the_load_chart: // Чтения графика нагрузки
			//заглушка
			break;
		}

		Packet send_packet = formed_packet->returnSentPacket();
		formed_packet->reset_sent_packet();
		std::cout << "Отправленный пакет " << ": ";
		send_packet.print_packet_not_id();

		COM->send(send_packet);
		std::cout << "    Принятый пакет " << ": ";
		COM->accept(2000);
		Packet received_packet = COM->return_accept_packet();

		received_packet.print_packet_not_id();
		unformed_packet->setUnformedPacket(received_packet);

		//unformed_packet.kostil_response_get_current_measurements_by_phase();
		switch (sw)
		{
		case CS::menu_options::quit: // выход
			return;
			break;

		case CS::menu_options::reading_the_counter_status: // Чтение состояния счётчика
			//заглушка
			break;

		case CS::menu_options::get_current_measurements_by_phase: // Получить текущие измерения пофазно
			unformed_packet->kostil_response_get_current_measurements_by_phase();
			break;

		case CS::menu_options::read_the_date_and_time: // Прочитать дату и время
			unformed_packet->kostil_response_read_the_date_and_time();
			break;

		case CS::menu_options::set_date_and_time:  // Установить дату и время
			//заглушка
			break;

		case CS::menu_options::reading_slice_readings:  // Чтение срезов показаний
			//заглушка
			break;

		case CS::menu_options::reading_the_load_chart: // Чтения графика нагрузки
			//заглушка
			break;
		}

		Packet decode_received_packet = unformed_packet->returnUnformedPacket();
		decode_received_packet.print_packet_not_id();

		print_dash(160); std::cout << std::endl;
		system("pause"); system("cls");
	
	} while (sw != CS::menu_options::quit);
}

void Menu::print_input_invitation()
{
	printf("Выберите тип запрашиваемого пакета:\n");
	printf("Q - выход.\n");
	printf("A - чтение состояния счётчика.\n");
	printf("S - получить текущие измерения пофазно.\n");
	printf("D - прочитать дату и время.\n");
	printf("Z - установить дату и время.\n");
	printf("X - чтение срезов показаний.\n");
	printf("С - чтение графика нагрузки.\n\n");
}

void Menu::print_dash(int16_t N)
{
	for (int16_t i = 0; i < N; i++)
		std::cout << "/";
	std::cout << std::endl;
}

CS::menu_options Menu::is_key_press()
{
	const int8_t lenght_key_arr = 4;
	
	const wchar_t Q[lenght_key_arr] = { L'q', L'Q', L'й', L'Й' }; // выход
	
	const wchar_t A[lenght_key_arr] = { L'a', L'A', L'ф', L'Ф' }; // reading_the_counter_status,		// Чтение состояния счётчика
	const wchar_t S[lenght_key_arr] = { L's', L'S', L'ы', L'Ы' }; // get_current_measurements_by_phase, // Получить текущие измерения пофазно
	const wchar_t D[lenght_key_arr] = { L'd', L'D', L'в', L'В' }; // read_the_date_and_time,		    // Прочитать дату и время
	
	const wchar_t Z[lenght_key_arr] = { L'z', L'Z', L'я', L'Я' }; // set_date_and_time,				    // Установить дату и время
	const wchar_t X[lenght_key_arr] = { L'x', L'X', L'ч', L'Ч' }; // reading_slice_readings,		    // Чтение срезов показаний
	const wchar_t C[lenght_key_arr] = { L'c', L'C', L'с', L'С' }; // reading_the_load_chart		        // Чтения графика нагрузки
	
	while (true)
	{	
		std::cout << "    Введите клавишу: ";
		wchar_t key = _getwch();

		for (int8_t i = 0; i < lenght_key_arr; i++)
		{
			if (key == Q[i])
			{
				printf("Q\n");
				return CS::menu_options::quit;
			}

			if (key == A[i])
			{
				printf("A\n");
				return CS::menu_options::reading_the_counter_status;
			}

			if (key == S[i])
			{
				printf("S\n");
				return CS::menu_options::get_current_measurements_by_phase;
			}

			if (key == D[i])
			{
				printf("D\n");
				return CS::menu_options::read_the_date_and_time;
			}

			if (key == Z[i])
			{
				printf("Z\n");
				return CS::menu_options::set_date_and_time;
			}

			if (key == X[i])
			{
				printf("X\n");
				return CS::menu_options::reading_slice_readings;
			}

			if (key == C[i])
			{
				printf("C\n");
				return CS::menu_options::reading_the_load_chart;

			}
		}

		std::cout << "неопознанная клавиша!" << std::endl;
		std::cout << "        Попробуйте ещё раз." << std::endl << std::endl;
	}
}

void Menu::cmd_configuration(int width, int height, int bufferHeight)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// Установим размер буфера консоли
	COORD newSize = { static_cast<SHORT>(width), static_cast<SHORT>(bufferHeight) };
	SetConsoleScreenBufferSize(hConsole, newSize);

	// Установим размер окна консоли
	SMALL_RECT newWindow = { 0, 0, static_cast<SHORT>(width - 1), static_cast<SHORT>(height - 1) };
	SetConsoleWindowInfo(hConsole, TRUE, &newWindow);
}