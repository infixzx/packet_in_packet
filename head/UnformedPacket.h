#pragma once
#include "Packet.h"
#include "Constant.h"
#include "Encryption.h"


class UnformedPacket
{
private:
	Packet received_packet; // пакет до обработки
	Packet unformed_packet; // то что будем возвращать через функциюю reset

	Packet external_flag; //Flag; const 0x7A; 1 byte
	Packet external_address_lenght; // LenAdr; В DEC 0...7; 1 byte (Всего уровней ретронсляции и текущий уровень)
	Packet external_address; //Adr[LenAdr]...Adr[0]; Адрес(а) ретрансляции; 2 byte * (MAX 8)
	Packet external_type_and_number; //Type; Тип пакета и текущий номер пакета; 1 byte
	Packet external_lenght; //Len; Размер поля данных; 1 byte
	Packet external_command_code; //CMD; Код команды; 1 byte
	Packet external_information; //Inforamation; Внутренний пакет; Len byte
	Packet external_FCS; //FCS; Котрольная суммаж 2 байта

	Packet internal_flag; //Flag; const 0x7E; 1 byte
	Packet internal_lenght; //Len; В DEC 0..200; 1 byte
	Packet internal_distination_address; //Dst; Адрес назначения (L..H); 6 byte
	Packet internal_type_and_number; //Type; Тип пакета и текущий номер пакета; 1 byte
	Packet internal_command_code; //CMD; Код команды; 1 byte
	Packet internal_information; //Information; Данные; Len byte
	Packet internal_FCS; //FCS; Котрольная сумма; 2 байта

	Packet set_distination_address(uint64_t DEC_distination_address);
	Packet set_external_distination_address(uint64_t DEC_distination_address);

public:
	UnformedPacket(uint64_t met_num);
	void setUnformedPacket(Packet unform_packet);
	void reset();
	Packet returnUnformedPacket();
	bool cheek_internal_status(Packet byte); //если ок - true

	//Ответы:
	void response_reading_the_counter_status();		   //Чтение состояния счётчика
	void response_get_current_measurements_by_phase(); //Получить текущие измерения пофазно
	void kostil_response_read_the_date_and_time();		       //Прочитать дату и время
	//void response_set_date_and_time();			   //Установить дату и время
	void response_reading_slice_readings();		       //Чтение срезов показаний
	//void response_reading_the_load_chart();	       //Чтения графика нагрузки
};