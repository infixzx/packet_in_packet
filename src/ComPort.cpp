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
			cout << sPortName << " �� ������!" << endl;
			//throw std::runtime_error(std::string(sPortName) + " �� ������!");
		}
		cout << "��������� ������ ������ � " << sPortName << "!" << endl;
		//throw runtime_error("��������� ������ ������ � " + string(sPortName) + "!");
	}

	// ����������� ��������� ����������:
	dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams))
	{
		cout << "������ ���������!" << endl;
	}
	dcbSerialParams.BaudRate = CBR_9600; //!!!
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(hSerial, &dcbSerialParams))
	{
		cout << sPortName << " �� ��������� ��������!" << endl;
	}
}

void ComPort::send(uchar* packet, uint16_t lenght)
{
	dwSize = lenght;
	BOOL iRet = WriteFile(hSerial, packet, dwSize, &dwBytesWritten, NULL);
	//cout << dwSize << " ���� � ������. " << dwBytesWritten << " ������������ ������. " << endl;
}

void ComPort::send(Packet pac)
{
	dwSize = pac.lenght();
	BOOL iRet = WriteFile(hSerial, pac.return_uchar(), dwSize, &dwBytesWritten, NULL);
	//cout << dwSize << " ���� � ������. " << dwBytesWritten << " ������������ ������. " << endl;
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
		cerr << "������ ��������� ����-����� �� COM-����. ��� ������: " << dwError << endl;
		// �������������� ��������� ������
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
			cerr << "������ ������ � COM-�����. ��� ������: " << dwError << endl;
			// �������������� ��������� ������
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
			// ���� ��� ������ � ������, ���������� ����������
			//cout << endl;
			//cerr << "����-���: ������ �� �������� � ������� " << dwTimeoutMilliseconds << " ��." << endl;
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
