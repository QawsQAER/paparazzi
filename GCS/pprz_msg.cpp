/*

        This source code is written by Edward HUANG, and Tony Yi
        For the development of their Final Year Project
        Department of Computer Science and Engineering
        The Chinese University of HONG KONG

*/

#include "pprz_msg.h"


pprz_msg::pprz_msg()
{
	this->_data_ptr = new uint8_t[PPRZ_MSG_MAX_SIZE];
	this->_length = 0;
	this->_pos = 0;
	this->_errorcode = 0;
}

pprz_msg::~pprz_msg()
{
	delete this->_data_ptr;
}


void pprz_msg::pprz_put_byte(uint8_t *ptr)
{
	if(_length + 1 > PPRZ_MSG_MAX_SIZE)
	{
		//SHOULD SET _errorcode
	}
	else
	{
		//printf("puting *ptr %02x\n",*ptr);
		*(this->_data_ptr + this->_length++) = *ptr;
	}
}

void pprz_msg::pprz_put_byte(uint8_t value)
{
	if(_length + 1 > PPRZ_MSG_MAX_SIZE)
        {
                //SHOULD SET _errorcode
        }
        else
        {
                //printf("puting *ptr %02x\n",*ptr);
                *(this->_data_ptr + this->_length++) = value;
        }

}
void pprz_msg::pprz_put_2bytes(uint8_t *ptr)
{
	//for little endian, the ptr would be pointing at the LSB of the uint16_t
	//ptr + 1 to make the MSB comes first
	this->pprz_put_byte(ptr + 1);
	this->pprz_put_byte(ptr);
}

void pprz_msg::pprz_put_4bytes(uint8_t *ptr)
{
	this->pprz_put_2bytes(ptr + 2);
	this->pprz_put_2bytes(ptr);
}

uint8_t pprz_msg::pprz_read_byte()
{
	if(this->_pos >= this->_length)
	{
		//set error code when there's no data and the user want to read
		return 0;
	}
	else
	{
		//printf("reading *ptr %02x\n",*(this->_data_ptr + this->_pos));
		return *(this->_data_ptr + this->_pos++); 
	}
}

uint16_t pprz_msg::pprz_read_2bytes()
{
	return (((uint16_t)this->pprz_read_byte()) << 8) + this->pprz_read_byte();
}

uint32_t pprz_msg::pprz_read_4bytes()
{
	return (((uint32_t)this->pprz_read_2bytes()) << 16) + this->pprz_read_2bytes();
}

void pprz_msg::show_hex()
{
	uint16_t count = 0;
	while(count < this->_length)
	{
		printf("%02x ",*(this->_data_ptr + count++));
	}
	printf("\n");
}

void pprz_msg::pprz_set_block(uint8_t &ac_id,uint8_t &block_id)
{
	//TODO Find out what should be the sender id here
	//sender id
	this->pprz_put_byte(&ac_id);	
	//message id
	this->pprz_put_byte((uint8_t)FORWARD_MSG_ID_BLOCK);
	//message content
	this->pprz_put_byte(&block_id);
	this->pprz_put_byte(&ac_id);	
}























