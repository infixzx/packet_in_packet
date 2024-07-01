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
		case CS::menu_options::quit: // �����
			return;
			break;

		case CS::menu_options::reading_the_counter_status: // ������ ��������� ��������
			//��������
			break;

		case CS::menu_options::get_current_measurements_by_phase: // �������� ������� ��������� �������
			formed_packet->request_get_current_measurements_by_phase();
			break;

		case CS::menu_options::read_the_date_and_time: // ��������� ���� � �����
			formed_packet->request_read_the_date_and_time();
			break;

		case CS::menu_options::set_date_and_time:  // ���������� ���� � �����
			//��������
			break;

		case CS::menu_options::reading_slice_readings:  // ������ ������ ���������
			//��������
			break;

		case CS::menu_options::reading_the_load_chart: // ������ ������� ��������
			//��������
			break;
		}

		Packet send_packet = formed_packet->returnSentPacket();
		formed_packet->reset_sent_packet();
		std::cout << "������������ ����� " << ": ";
		send_packet.print_packet_not_id();

		COM->send(send_packet);
		std::cout << "    �������� ����� " << ": ";
		COM->accept(2000);
		Packet received_packet = COM->return_accept_packet();

		received_packet.print_packet_not_id();
		unformed_packet->setUnformedPacket(received_packet);

		//unformed_packet.kostil_response_get_current_measurements_by_phase();
		switch (sw)
		{
		case CS::menu_options::quit: // �����
			return;
			break;

		case CS::menu_options::reading_the_counter_status: // ������ ��������� ��������
			//��������
			break;

		case CS::menu_options::get_current_measurements_by_phase: // �������� ������� ��������� �������
			unformed_packet->kostil_response_get_current_measurements_by_phase();
			break;

		case CS::menu_options::read_the_date_and_time: // ��������� ���� � �����
			unformed_packet->kostil_response_read_the_date_and_time();
			break;

		case CS::menu_options::set_date_and_time:  // ���������� ���� � �����
			//��������
			break;

		case CS::menu_options::reading_slice_readings:  // ������ ������ ���������
			//��������
			break;

		case CS::menu_options::reading_the_load_chart: // ������ ������� ��������
			//��������
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
	printf("�������� ��� �������������� ������:\n");
	printf("Q - �����.\n");
	printf("A - ������ ��������� ��������.\n");
	printf("S - �������� ������� ��������� �������.\n");
	printf("D - ��������� ���� � �����.\n");
	printf("Z - ���������� ���� � �����.\n");
	printf("X - ������ ������ ���������.\n");
	printf("� - ������ ������� ��������.\n\n");
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
	
	const wchar_t Q[lenght_key_arr] = { L'q', L'Q', L'�', L'�' }; // �����
	
	const wchar_t A[lenght_key_arr] = { L'a', L'A', L'�', L'�' }; // reading_the_counter_status,		// ������ ��������� ��������
	const wchar_t S[lenght_key_arr] = { L's', L'S', L'�', L'�' }; // get_current_measurements_by_phase, // �������� ������� ��������� �������
	const wchar_t D[lenght_key_arr] = { L'd', L'D', L'�', L'�' }; // read_the_date_and_time,		    // ��������� ���� � �����
	
	const wchar_t Z[lenght_key_arr] = { L'z', L'Z', L'�', L'�' }; // set_date_and_time,				    // ���������� ���� � �����
	const wchar_t X[lenght_key_arr] = { L'x', L'X', L'�', L'�' }; // reading_slice_readings,		    // ������ ������ ���������
	const wchar_t C[lenght_key_arr] = { L'c', L'C', L'�', L'�' }; // reading_the_load_chart		        // ������ ������� ��������
	
	while (true)
	{	
		std::cout << "    ������� �������: ";
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

		std::cout << "������������ �������!" << std::endl;
		std::cout << "        ���������� ��� ���." << std::endl << std::endl;
	}
}

void Menu::cmd_configuration(int width, int height, int bufferHeight)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// ��������� ������ ������ �������
	COORD newSize = { static_cast<SHORT>(width), static_cast<SHORT>(bufferHeight) };
	SetConsoleScreenBufferSize(hConsole, newSize);

	// ��������� ������ ���� �������
	SMALL_RECT newWindow = { 0, 0, static_cast<SHORT>(width - 1), static_cast<SHORT>(height - 1) };
	SetConsoleWindowInfo(hConsole, TRUE, &newWindow);
}