#include "main.h"
#include "pprz_msg.h"
#include "XBEE_msg.h"

int main()
{
	uint8_t sender_id = 1;
	uint8_t msg_id = 90;
	uint32_t pos_x = 10;
	uint32_t pos_y = 100;
	uint32_t pos_z = 88;
	pprz_msg data;
	
	uint8_t ac_id = 1;
	uint8_t block_id = 2;
	//data.pprz_set_block(ac_id,block_id);
	float test = 1.1;
	data.pprz_put_4bytes((uint8_t *) &test);
	data.show_hex();
	//XBEE_msg xbee_msg;
	//xbee_msg.set_tran_packet(0x00000000,0x0000ffff,0xff,0xfe,data.pprz_get_data_ptr(),data.pprz_get_length());
	//xbee_msg.set_pprz_msg(data);
	//printf("data in xbee_msg\n");
	//xbee_msg.show_hex();
	//xbee_msg.get_pprz_msg().show_hex();
	//pprz_msg recv_data = xbee_msg.get_pprz_msg();
	data.pprz_reset_msg();
	data.show_hex();
	float tmp = data.pprz_read_float();
	printf("tmp is %f\n",tmp);
	printf("tmp is %x\h",tmp);


	return 0;
}
