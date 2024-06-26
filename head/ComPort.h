#pragma once
#include <windows.h>
#include <tchar.h>
#include "Packet.h"

class ComPort
{
private:
	Packet acceptedPacket; // принятый пакет
	//Packet sentPacket;   // отправленный пакет
	HANDLE hSerial;		   // обработчик com порта
	LPCTSTR sPortName;	   // ник порта COM7
	DCB dcbSerialParams;   // Настраиваем параметры соеденения
	DWORD dwSize;		   // кол-во байт которое мы хотим отправить
	DWORD dwBytesWritten;  // кол-во байт которое успешно передалось
	BOOL iRet;			   // переменная для результата операций

public:
	//ComPort();
	ComPort(string com_port_name);
	//~ComPort();
	void send(uchar* packet, uint16_t lenght); // отправить пакет
	void send(Packet pac);
	void accept(DWORD dwTimeoutMilliseconds);  // чтение данных с таймаутом
	void reset();
	Packet return_accept_packet();
};