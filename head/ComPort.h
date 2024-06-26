#pragma once
#include <windows.h>
#include <tchar.h>
#include "Packet.h"

class ComPort
{
private:
	Packet acceptedPacket; // �������� �����
	//Packet sentPacket;   // ������������ �����
	HANDLE hSerial;		   // ���������� com �����
	LPCTSTR sPortName;	   // ��� ����� COM7
	DCB dcbSerialParams;   // ����������� ��������� ����������
	DWORD dwSize;		   // ���-�� ���� ������� �� ����� ���������
	DWORD dwBytesWritten;  // ���-�� ���� ������� ������� ����������
	BOOL iRet;			   // ���������� ��� ���������� ��������

public:
	//ComPort();
	ComPort(string com_port_name);
	//~ComPort();
	void send(uchar* packet, uint16_t lenght); // ��������� �����
	void send(Packet pac);
	void accept(DWORD dwTimeoutMilliseconds);  // ������ ������ � ���������
	void reset();
	Packet return_accept_packet();
};