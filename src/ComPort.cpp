#include <iostream>
#include "../head/Packet.h"
#include "../head/ComPort.h"
#include <windows.h>

using std::cout;
using std::endl;
using std::cerr;
//using std::runtime_error;
//using std::string;

ComPort::ComPort(string com_port_name)
{
	dwSize = 0;
	dwBytesWritten = 0;
	sPortName = _T(com_port_name.c_str());
	hSerial = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			cout << sPortName << " не открыт!" << endl;
			//throw std::runtime_error(std::string(sPortName) + " не открыт!");
		}
		cout << "Произошла другая ошибка с " << sPortName << "!" << endl;
		//throw runtime_error("Произошла другая ошибка с " + string(sPortName) + "!");
	}

	// Настраиваем параметры соеденения:
	dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams))
	{
		cout << "Ошибка состояния!" << endl;
	}
	dcbSerialParams.BaudRate = CBR_9600; //!!!
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(hSerial, &dcbSerialParams))
	{
		cout << sPortName << " не правильно настроен!" << endl;
	}
}

void ComPort::send(uchar* packet, uint16_t lenght)
{
	dwSize = lenght;
	BOOL iRet = WriteFile(hSerial, packet, dwSize, &dwBytesWritten, NULL);
	//cout << dwSize << " байт в строке. " << dwBytesWritten << " отправленных байтов. " << endl;
}

void ComPort::send(Packet pac)
{
	dwSize = pac.lenght();
	BOOL iRet = WriteFile(hSerial, pac.return_uchar(), dwSize, &dwBytesWritten, NULL);
	//cout << dwSize << " байт в строке. " << dwBytesWritten << " отправленных байтов. " << endl;
}

void ComPort::accept(DWORD dwTimeoutMilliseconds)
{
	reset();
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutConstant = dwTimeoutMilliseconds;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;

	if (!SetCommTimeouts(hSerial, &timeouts))
	{
		DWORD dwError = GetLastError();
		cerr << "Ошибка установки тайм-аутов на COM-порт. Код ошибки: " << dwError << endl;
		// Дополнительная обработка ошибок
	}
	////////////////////////////////////////////////////////////////////////////////////////
	//uint64_t count = 1;

	DWORD iSize;
	uchar sReceivedChar;
	while (true)
	{
		if (!ReadFile(hSerial, &sReceivedChar, 1, &iSize, NULL))
		{
			DWORD dwError = GetLastError();
			cerr << "Ошибка чтения с COM-порта. Код ошибки: " << dwError << endl;
			// Дополнительная обработка ошибок
			return;
		}

		if (iSize > 0)
		{
			acceptedPacket.add_one_element_back(sReceivedChar);
			//cout << sReceivedChar << endl;
			//printf("%02x ", sReceivedChar);
		}
		else
		{
			// Если нет данных в потоке, прекратить выполнение
			//cout << endl;
			//cerr << "Тайм-аут: Данные не получены в течение " << dwTimeoutMilliseconds << " мс." << endl;
			return;
		}
	}
}

void ComPort::reset()
{
	acceptedPacket = Packet();
}

Packet ComPort::return_accept_packet()
{
	Packet temp;
	for (int i = 0; i < acceptedPacket.lenght(); i++)
		temp.add_one_element_back(acceptedPacket[i]);
	return temp;
}
