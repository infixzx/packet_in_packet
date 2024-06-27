#include <iostream>
#include <map>
#include "../head/Packet.h"
#include "../head/ComPort.h"
#include "../head/FormedPacket.h"
#include "../head/UnformedPacket.h"


using namespace std; 

int main(void)
{
	setlocale(LC_ALL, "Russian");

	uint64_t meter_number = 235072;
	ComPort COM("COM7");

	Packet adr;
	FormedPacket formed_packet(meter_number);
	UnformedPacket unformed_packet(meter_number);

	for (int i = 0; i < 20; i++)
	{
		cout << "///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
		formed_packet.request_get_current_measurements_by_phase();
		Packet send_packet = formed_packet.returnSentPacket();
		cout << "Отправленный пакет " << i << ": ";
		send_packet.print_packet_not_id();
		COM.send(send_packet);
		cout << "    Принятый пакет " << i << ": ";
		COM.accept(2000);
		Packet received_packet = COM.return_accept_packet();


		received_packet.print_packet_not_id();
		unformed_packet.setUnformedPacket(received_packet);
		
		//unformed_packet.kostil_response_read_the_date_and_time();
		

		Packet decode_received_packet = unformed_packet.returnUnformedPacket();
		decode_received_packet.print_packet_not_id();
		
		cout << "///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << endl;
		cout << endl;
	}

	system("pause");
	return 0;
}
