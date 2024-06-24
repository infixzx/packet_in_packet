#include "../head/Encryption.h"


// Котрольная байт расчитывается для моля information 
// включая поля Data и Status
uchar Encryption::сheek_digit(uchar * uk, uchar size)
{
	uchar CRC = 0;
	for (int i = 0; i < size; i++)
		CRC += uk[i];
	return CRC;
}

unsigned short Encryption::fcs16(unsigned short fcs, uchar * cp, int len)
{
	while (len--)
		fcs = (fcs >> 8) ^ fcstab[(fcs^*cp++) & 0xff];
	fcs ^= 0xffff;
	fcs = ((fcs >> 8) & 0xff) | (fcs << 8);
	return (fcs);
}

Packet Encryption::encryption_сontrol_byte(Packet temp)
{
	Packet ret;
	uchar size = temp.lenght();
	uchar* uk = new uchar[size];
	for (int i = 0; i < size; i++)
	{
		uk[i] = temp[i];
	}
	////
	uchar CRC = 0;
	for (int i = 0; i < size; i++)
		CRC += uk[i];
	////
	ret.add_one_element_back(CRC);
	delete[] uk;

	return ret;
}

Packet Encryption::encryption_fcs16(unsigned short fcs, Packet temp)
{
	Packet ret;
	int len = temp.lenght();
	uchar* cp = new uchar[len];
	for (int i = 0; i < len; i++)
	{
		cp[i] = temp[i];
	}
	///
	while (len--)
		fcs = (fcs >> 8) ^ fcstab[(fcs^*cp++) & 0xff];
	fcs ^= 0xffff;
	fcs = ((fcs >> 8) & 0xff) | (fcs << 8);

	uchar high_byte = fcs >> 8;
	fcs = fcs << 8;
	uchar low_byte = fcs >> 8;

	ret.add_one_element_back(high_byte);
	ret.add_one_element_back(low_byte);
	return ret;
}

Packet Encryption::bytestuffing_encod(Packet temp, uchar in_1, uchar out_1, uchar out_2,
	uchar in_2, uchar out_3, uchar out_4)
{
	Packet ret;
	for (uchar i = 0; i < temp.lenght(); i++)
	{
		if (temp[i] == in_1)
		{
			ret.add_one_element_back(out_1);
			ret.add_one_element_back(out_2);
		}
		else if (temp[i] == in_2)
		{
			ret.add_one_element_back(out_3);
			ret.add_one_element_back(out_4);
		}
		else
			ret.add_one_element_back(temp[i]);
	}
	return ret;
}



Packet Encryption::bytestuffing_docode(Packet temp, uchar in_1, uchar in_2, uchar out_1,
	uchar in_3, uchar in_4, uchar out_2)
{
	Packet ret;
	for (uchar i = 0; i < temp.lenght(); i++)
	{
		if ((temp[i] == in_1) && (temp[i+1] == in_2))
		{
			ret.add_one_element_back(out_1);
			i++;
		}
		else if ((temp[i] == in_3) && (temp[i + 1] == in_4))
		{
			ret.add_one_element_back(out_2);
			i++;
		}
		else
			ret.add_one_element_back(temp[i]);
	}
	return ret;
}

