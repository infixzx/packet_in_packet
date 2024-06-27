#pragma once
#include "Packet.h"
#include "Constant.h"
#include "Encryption.h"

//ID=9 Состояния счётчика
//		1.запрос Чтение состояния счётчика
//		2.ответ  Чтение состояния счётчика

//ID=3 Текущие измерения пофазно
//			1.запрос Получить текущие измерения пофазно
//		2.ответ  Получить текущие измерения пофазно


//ID=11 Дата и время встроенных часов
//			1. запрос Прочитать дату и время
//			2. ответ  Прочитать дату и время
//		3. запрос Установить дату и время
//		4. ответ  Установить дату и время

//ID=12 Срезы показаний на начало дня/месяца/года, за день/месяц/год
//		1. запрос Чтение срезов показаний
//		2. ответ  Чтение срезов показаний

//ID=10 График нагрузки
//		1. запрос Чтения графика нагрузки
//		2. ответ  Чтения графика нагрузки

class FormedPacket
{
private:
		Packet internal_flag;				 // Flag; const 0x7E; 1 byte
		Packet internal_lenght;				 // Len; В DEC 0..200; 1 byte
		Packet internal_distination_address; // Dst; Адрес назначения (L..H); 6 byte
		Packet internal_type_and_number;	 // Type; Тип пакета и текущий номер пакета; 1 byte
		Packet internal_command_code;		 // CMD; Код команды; 1 byte
		Packet internal_information;		 // Information; Данные; Len byte
		Packet internal_FCS;				 // FCS; Котрольная сумма; 2 байта

		Packet external_flag;			 //Flag; const 0x7A; 1 byte
		Packet external_address_lenght;  // LenAdr; В DEC 0...7; 1 byte (Всего уровней ретронсляции и текущий уровень)
	 Packet external_address;			 //Adr[LenAdr]...Adr[0]; Адрес(а) ретрансляции; 2 byte * (MAX 8)
		Packet external_type_and_number; //Type; Тип пакета и текущий номер пакета; 1 byte
		Packet external_lenght;			 //Len; Размер поля данных; 1 byte
		Packet external_command_code;	 //CMD; Код команды; 1 byte
		Packet external_information;	 //Inforamation; Внутренний пакет; Len byte
	Packet external_FCS;				 //FCS; Котрольная суммаж 2 байта

	CS::list_ID_packet ID_packet;
	//!!! НУЖЕН ENUM КОТОРЫЙ ХРАНИТ В СЕБЕ REQUEST И RESPONSE и от будет релизововать внутреннюю логику
	CS::RXTX rxtx; // запрос или ответ
	uint8_t сurrent_packet_number; //текущий номер пакета(костыль?)

	Packet sent_packet;
	uint64_t meter_number;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Абстрактный метод вычисления адресса который нужно допиливать
	Packet set_distination_address(uint64_t DEC_distination_address);

	void incriment_сurrent_packet_number(); // работает с current_packet_number++


	Packet set_internal_distination_address(uint64_t DEC_distination_address); // метод для вычисления адреса внутриннего пакета
	Packet set_external_distination_address(uint64_t DEC_distination_address); // метод для вычисления адреса внешнего пакета (всё очень сложно)


	Packet FormedPacket::set_internal_type_and_number(
		CS::INTERNAL_type_and_number bit_7_rxtx, CS::INTERNAL_type_and_number_pool bit_6_pool,
		CS::INTERNAL_type_and_number_pool bit_5_pool, CS::INTERNAL_type_and_number_pool bit_4_pool);

	Packet FormedPacket::set_external_type_and_number(
		CS::EXTERNAL_type_and_number bit_7_request_response, CS::EXTERNAL_type_and_number_pool bit_6_pool,
		CS::EXTERNAL_type_and_number_pool bit_5_pool, CS::EXTERNAL_type_and_number_pool bit_4_pool);

	Packet set_internal_command_code(CS::INTERNAL_command_code bit_7_code_encode, CS::INTERNAL_command_code_bit_6_0 bit_6_0);

	Packet set_external_command_code(CS::EXTERNAL_command_code bit_7_code_encode, CS::EXTERNAL_command_code_bit_6_0 bit_6_0);

	Packet formed_external_kostil_packet(Packet Information);

	//Encryption??? 
public:
	FormedPacket(uint64_t met_num);
	//~FormedPacket();
	
	//Запросы:
	void request_reading_the_counter_status();		  //Чтение состояния счётчика
		void request_get_current_measurements_by_phase(); //Получить текущие измерения пофазно
		void request_read_the_date_and_time();		      //Прочитать дату и время
	//void request_set_date_and_time();				  //Установить дату и время
	void request_reading_slice_readings();		      //Чтение срезов показаний
	//void request_reading_the_load_chart();	      //Чтения графика нагрузки
	
	void reset();

	Packet returnSentPacket(); // отправленный пакет
};