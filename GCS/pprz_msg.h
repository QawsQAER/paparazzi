/*

        This source code is written by Edward HUANG, and Tony Yi
        For the development of their Final Year Project
        Department of Computer Science and Engineering
        The Chinese University of HONG KONG

*/
#ifndef _PPRZ_MSG_H
#define _PPRZ_MSG_H
#include "main.h"


#define FORWARD_MSG_ID_BLOCK 5
#define FORWARD_MSG_ID_TAR_DEST 14

#define BLOCK_ID_HOME_POINT 1
#define BLOCK_ID_HOLDING_POINT 2
#define BLOCK_ID_START_ENGINE 3
#define BLOCK_ID_TAKE_OFF 4
#define BLOCK_ID_LAND_HERE 5

#define PPRZ_MSG_MAX_SIZE 512
#define PPRZ_SENDER_ID_OFFSET 0
#define PPRZ_MSG_ID_OFFSET 1
#define PPRZ_DATA_OFFSET 2
class pprz_msg
{
	private:
		//pointer to the data
		uint8_t *_data_ptr;
		//_pos will be holding the current position to where the user has read
		uint16_t _pos;
		//_length will be holding the length of the current message
		uint16_t _length;
		uint8_t _errorcode;	
	public:
		pprz_msg();
		~pprz_msg();
		const uint8_t *pprz_get_data_ptr(){return _data_ptr;}
		const uint16_t pprz_get_length(){return _length;}
		void pprz_put_byte(uint8_t *ptr);
		void pprz_put_byte(uint8_t value);
		void pprz_put_2bytes(uint8_t *ptr);
		void pprz_put_4bytes(uint8_t *ptr);
		uint8_t pprz_read_byte();
		uint16_t pprz_read_2bytes();
		uint32_t pprz_read_4bytes();
		void show_hex();
		void pprz_set_block(uint8_t &ac_id,uint8_t &block_id);		
};

#endif
