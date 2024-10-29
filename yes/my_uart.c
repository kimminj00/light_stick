/*
 * my_uart.c
 *
 *  Created on: 2018. 5. 24.
 *      Author: sprit
 */
#include <stdio.h>
#include <yes/yes_radio.h>
#include "my_uart.h"
#include "bsp.h"
#include "uartdrv.h"
#include "app_hal.h"

//static volatile bool isTransmit=false;
//volatile bool isTransmit=false;
void receive_callback(UARTDRV_Handle_t handle, Ecode_t transferStatus, uint8_t *data, UARTDRV_Count_t transferCount);
volatile uint32_t rxCount = 0;
volatile uint16_t queue_que=0, queue_retrieve=0;

uint16_t uart_len = 0;
uint16_t test_check=0;

UARTDRV_Handle_t handle = &handleData;
UARTDRV_Handle_t handle2 = &handleData2;

extern bool packetTx;

void initUart(void) {

	// Initialize driver handle
	UARTDRV_InitUart_t initData = MY_UART;
	Ecode_t error = UARTDRV_InitUart(handle, &initData);
	printf("UARTDRV_InitUart() 0x%4x \r\n", error);
	//error = UARTDRV_Receive(handle, &rxBuffer[0], RX_BUFFER_SIZE, receive_callback);
	//printf("UARTDRV_Receive() 0x%4x \r\n", error);
	UARTDRV_Receive(handle, &incoming_data, 1, UART_Rx_cb);
#if RADIO_2P4 == false
	UARTDRV_Receive(handle, &incoming_data, 1, UART_Rx_cb);
#endif
	// Transmit data using a non-blocking transmit function
	//error = UARTDRV_Transmit(handle, txBuffer, TX_BUFFER_SIZE, transmit_callback);
	//printf("UARTDRV_Transmit() 0x%4x \r\n", error);

}

void initB2BUart(void) {

	// Initialize driver handle
	UARTDRV_InitUart_t initData = B2B_UART;
	Ecode_t error = UARTDRV_InitUart(handle2, &initData);
	printf("UARTDRV_InitUart() 0x%4x \r\n", error);
	//error = UARTDRV_Receive(handle, &rxBuffer[0], RX_BUFFER_SIZE, receive_callback);
	UARTDRV_Receive(handle2, &incoming_data, 1, B2B_UART_Rx_cb);

	// Transmit data using a non-blocking transmit function
	//error = UARTDRV_Transmit(handle, txBuffer, TX_BUFFER_SIZE, transmit_callback);
	//printf("UARTDRV_Transmit() 0x%4x \r\n", error);

}

void SendB2BUart(uint8_t *data,UARTDRV_Count_t count)
{
	UARTDRV_Transmit(handle2, data, count, transmit_callback);
	//Ecode_t error = UARTDRV_Transmit(handle2, data, count, transmit_callback);
	//
	//if (error)
	//{
	//	printf("UARTDRV_Transmit() 0x%lx \r\n", error);
	//}
}

void transmit_callback(UARTDRV_Handle_t handle,
              Ecode_t transferStatus,
              uint8_t *data,
              UARTDRV_Count_t transferCount)
{
  (void)handle;
  (void)transferStatus;
  (void)data;
  (void)transferCount;

	//BSP_LedSet(0);//BSP_LedClear(1);/*20200507_gyuplus_revised*/
}

void receive_callback(UARTDRV_Handle_t handle,
		Ecode_t transferStatus,
		uint8_t *data,
		UARTDRV_Count_t transferCount) {
	(void) data;
	(void) handle;
	(void) transferCount;

	//printf("\n rxCount : %d \n", rxCount);
	if (transferStatus == ECODE_EMDRV_UARTDRV_OK)
	{
		uint8_t index;
		for (index = 0; index < transferCount; index++) {
				//putchar(&data[index]);
			printf(" %x", data[index]);
			}

	    rxCount += transferCount;
	    printf("\n rxCount : %d \n", rxCount);

	    UARTDRV_Receive(handle, rxBuffer, RX_BUFFER_SIZE, receive_callback);
	}
}
extern uint8_t Led_Status;
extern uint8_t Led_Status_1;
void UART_Rx_cb(UARTDRV_Handle_t handle, Ecode_t transferStatus, uint8_t *data, UARTDRV_Count_t transferCount)
{
  //printf("Rx CB called: 0x%4x\r\n", transferStatus);
	//GPIO_PinOutClear(gpioPortB, 11);
	//Led_Status_1=1;
  if(transferStatus!=ECODE_OK)
    return;


  MyQueue[queue_que++]=*data;
  //printf("Rx CB called: 0x%4x  %d\r\n", transferStatus,queue_que);
  if(queue_que>=UART_QUEUE_SIZE)
    queue_que = 0;

  UARTDRV_Receive(handle, &incoming_data, 1, UART_Rx_cb);
}

void B2B_UART_Rx_cb(UARTDRV_Handle_t handle, Ecode_t transferStatus, uint8_t *data, UARTDRV_Count_t transferCount)
{
  //printf("Rx CB called: 0x%4x\r\n", transferStatus);
	//GPIO_PinOutClear(gpioPortB, 11);
	//Led_Status_1=1;
  if(transferStatus!=ECODE_OK)
    return;

  MyQueue[queue_que++]=*data;
  if(queue_que>=UART_QUEUE_SIZE)
    queue_que = 0;
  UARTDRV_Receive(handle, &incoming_data, 1, B2B_UART_Rx_cb);
}

void UART_Tx_cb(UARTDRV_Handle_t handle, Ecode_t transferStatus, uint8_t *data, UARTDRV_Count_t transferCount)
{
  //printf("Tx CB called: 0x%4x\r\n", transferStatus);
  if(transferStatus!=ECODE_OK)
    return;
  //isTransmit=false;
  //makePacket(queue_retrieve, queue_que - queue_retrieve);
}

static int calc_crc( unsigned char *data, int length )
{
	int i, j;
	unsigned short crc;
	
	crc = 0;
	for( i = 0; i < length; i++ )
	{
		crc ^= ( unsigned short )( data[ i ] << 8 );
		for( j = 0; j < 8; j++ )
		{
			if( ( crc & 0x8000 ) != 0 )
			{
				crc = ( unsigned short )( crc << 1 ^ 0x1021 );
			}
			else
			{
				crc = ( unsigned short )( crc << 1 );
			}
		}
	}
	return crc;
}

#define MAGIC_FIRST		0x69
#define MAGIC_SECOND	0xBF

#define SSPE_MAGIC_FIRST	0x99
#define SSPE_MAGIC_SECOND	0x79

#define MAIN_HDR_SIZE			6
#define test_d_length 48
static const uint8_t U_data_test[test_d_length] = {0x69,0xBF,0x2A,0x00,0x11,0x6B,0x0A,0x28,0x99,0x79,0x25,0x4D,\
		0x01,0x00,0x01,0x0A,0x00,0x02,0x00,0x00,0x00,0x38,0x02,0x18,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,\
		0x00,0x00,0x00,0x41,0x03,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x64};
uint8_t all_Off_flag=0;
uint8_t all_On_flag=0;
#define test_D_offset 11
void UART_process(void)
{
	static uint8_t cnt = 0;
	static uint8_t prev_data[ 64 ];
	const int sspe_f_pos = 8;
	int find, length, header_size,len_for;
	uint16_t max, pos, prev,i;
	uint8_t *data;

	//printf("##  test_check    %d,, ", test_check );

	if( queue_que != queue_retrieve )
	{
		find = 0;
		prev = queue_que;
		length = prev - queue_retrieve;
		if( prev < queue_retrieve )
			length = UART_QUEUE_SIZE + prev - queue_retrieve;
		len_for=length;
		data = MyQueue;
		max = prev;
		pos = queue_retrieve;

		//SendB2BUart(&MyQueue[ pos ],length);

		//pos = 0;
				//printf("## %d, %d, %d, %d \r\n",queue_que,pos,queue_retrieve,length);
				if( length > KME_HEADER_SIZE )
				{

					while( pos != max )
					{

						if( (data[ pos ] == MAGIC_FIRST) && (data[ pos + 1 ] == MAGIC_SECOND))// && (data[ pos + 14 ] == 0x7F))
						{
							 //printf("## %d,, ", pos );
							 //SendMessageIEEE802154( YES_BCAST_ADDR, &data[ pos ], pos );

							if( length > sspe_f_pos && data[ pos + sspe_f_pos ] == SSPE_MAGIC_FIRST && data[ pos + sspe_f_pos + 1 ] == SSPE_MAGIC_SECOND )
							{

								uart_len = data[ pos + 2 ];
								//GPIO_PinOutClear(gpioPortB, 12);

								 //printf("### %d, %d, %d,, ", uart_len, length, data[ pos + 3 ] );


								if( data[ pos + 3 ] == 0 )  /* FIXME: length < 255 */
								{
									length -= MAIN_HDR_SIZE;
									if( length >= uart_len )
									{
										uint16_t crc;

										//crc = calc_crc( &data[ pos + MAIN_HDR_SIZE ], uart_len );

										//SendMessageIEEE802154( YES_BCAST_ADDR, &data[ pos ], pos );

										//if( ( uint8_t )( crc & 0xFF ) == data[ pos + 4 ] && ( uint8_t )( ( crc >> 8 ) & 0xFF ) == data[ pos + 5 ] )
										if( (0x78 == data[ pos + 4 ]) && (0x87 == data[ pos + 5 ]) )
										{
											Led_Status=1;
											Led_Status_1=1;
											//if( memcmp( prev_data, &MyQueue[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE ) != 0 )
											if( memcmp( prev_data, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE ) != 0 )
											{
												gReTxCount802154=3;
												//memcpy( prev_data, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );

#if 0
												if((data[ pos + KME_HEADER_SIZE ] == 0x4D)&&(data[ pos + KME_HEADER_SIZE + 13 ] == 0x01))
												{
													if(data[ pos + KME_HEADER_SIZE + 14 ] == 0x00)
													{
														all_Off_flag=1;
														gReTxCount802154 = data[ pos + KME_HEADER_SIZE + 3 ]-1;
														memcpy( prev_data, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );
														SendMessageIEEE802154( YES_BCAST_ADDR, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );
													}
													else if(data[ pos + KME_HEADER_SIZE + 14 ] == 0x01)
													{
														if(all_Off_flag == 1)
														{
															all_Off_flag=0;
														}
														else if(all_Off_flag == 0)
														{
															gReTxCount802154 = data[ pos + KME_HEADER_SIZE + 3 ]-1;
															memcpy( prev_data, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );
															SendMessageIEEE802154( YES_BCAST_ADDR, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );
														}
													}

												}
												else
												{
													gReTxCount802154 = data[ pos + KME_HEADER_SIZE + 3 ]-1;
													memcpy( prev_data, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );
													SendMessageIEEE802154( YES_BCAST_ADDR, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );
												}
												//[H]: 4d 01 00 14 30 41 88 84 75 24 ff ff 00 [D]: 00 4d 01 00 14 01 00 02 00 10 00 6c 01 18 01 01 00 00 00 90 10 00 00 00 [RSSI]: -58
												//[H]: 4d 01 00 14 01 00 02 00 10 00 6c 01 18 [D]: 30 41 88 8d 75 24 ff ff 00 00 4d 01 00 14 01 00 02 00 10 00 6c 01 18 01 [RSSI]: -82
												//[H]: 4d 01 00 14 01 00 02 00 10 00 6c 01 30 [D]: 41 88 b2 75 24 ff ff 00 00 4d 01 00 14 01 00 02 00 10 00 6c 01 18 01 01 [RSSI]: -98
												//->[H]: 4d 01 00 14 01 00 02 00 10 00 6c 01 18 [D]: 01 01 00 00 00 c9 10 00 00 00 00 30 41 88 bd 75 24 ff ff 00 00 4d 01 00 [RSSI]: -90
												//[H]: 4d 01 00 14 01 00 02 00 30 41 88 f5 75 [D]: 24 ff ff 00 00 4d 01 00 14 01 00 02 00 10 00 6c 01 18 01 01 00 00 00 fd [RSSI]: -98



#else
												//gReTxCount802154 = data[ pos + KME_HEADER_SIZE + 3 ]-1;

												//gReTxtime802154 = data[ pos + KME_HEADER_SIZE + 5 ]<<8;

												//gReTxtime802154 |= data[ pos + KME_HEADER_SIZE + 4 ];


												memcpy( prev_data, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );
												SendMessageIEEE802154( YES_BCAST_ADDR, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );
#endif
												// for(i=0 ; i < length; i++)
												// {
												// 	printf("%X ",data[ pos + i ]);
												// }
												// printf("\r\n");


												//SendMessageIEEE802154( YES_BCAST_ADDR, &MyQueue[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );

												//SendMessageIEEE802154( YES_BCAST_ADDR, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );

												//GPIO_PinOutClear(gpioPortB, 11);
											}
											else if(data[ pos + KME_HEADER_SIZE ] == 0x72)
											{
												gReTxCount802154 = data[ pos + KME_HEADER_SIZE + 3 ]-1;

												gReTxtime802154 = data[ pos + KME_HEADER_SIZE + 5 ]<<8;

												gReTxtime802154 |= data[ pos + KME_HEADER_SIZE + 4 ];


												memcpy( prev_data, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );
												SendMessageIEEE802154( YES_BCAST_ADDR, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );
											}

											//usDelay( 300 );


											 //printf("## FIND %d,, ", queue_retrieve );
										}
										else
										{
											 //printf("## FIND CHK %d(%d:%d,%d:%d),, ", queue_retrieve, crc & 0xFF, ( crc >> 8 ) & 0xFF, data[ pos + 4 ], data[ pos + 5 ] );

										}
										queue_retrieve = pos + uart_len + MAIN_HDR_SIZE;
										if( queue_retrieve >= UART_QUEUE_SIZE )
											queue_retrieve -= UART_QUEUE_SIZE;
									}
									find = 1;
									 //printf( "\r\n" );

									break;
								}
								else
								{

									find = 1;
									queue_retrieve = pos + MAIN_HDR_SIZE;
									if( queue_retrieve >= UART_QUEUE_SIZE )
										queue_retrieve -= UART_QUEUE_SIZE;

									 //printf("## ERR %d\r\n", queue_retrieve );

									break;
								}
							}

							 //printf( "\r\n" );
						}

						pos++;
						length--;
						if( pos >= UART_QUEUE_SIZE )
							pos = 0;
					}
					if( find == 0 )
					{
						queue_retrieve = 0;
						if( pos > 0 )
							queue_retrieve = pos - 1;
					}


				}



				length = queue_que - queue_retrieve;
				if( queue_que < queue_retrieve )
					length = UART_QUEUE_SIZE + queue_que - queue_retrieve;

				if( length == 0 )
				{
					CORE_DECLARE_IRQ_STATE;
					CORE_ENTER_CRITICAL();

					queue_que = 0;
					queue_retrieve = 0;

					CORE_EXIT_CRITICAL();
				}
		}
}





#define B2B_MAGIC_FIRST		0x69//0x41
#define B2B_MAGIC_SECOND	0xBF//0x88

void B2B_UART_process(void)
{
	static uint8_t cnt = 0;
	static uint8_t prev_data[ 64 ];
	const int sspe_f_pos = 8;
	int find, length, header_size;
	uint16_t max, pos, prev, i;
	uint8_t *data;

	if( queue_que != queue_retrieve )
	{
		find = 0;
		prev = queue_que;
		length = prev - queue_retrieve;
		if( prev < queue_retrieve )
			length = UART_QUEUE_SIZE + prev - queue_retrieve;
		data = MyQueue;
		max = prev;
		pos = queue_retrieve;



		if( length > KME_HEADER_SIZE )
		{

			while( pos != max )
			{

				if( data[ pos ] == MAGIC_FIRST && data[ pos + 1 ] == MAGIC_SECOND )
				{
					 //printf("## %d,, ", pos );


					if( length > sspe_f_pos && data[ pos + sspe_f_pos ] == SSPE_MAGIC_FIRST && data[ pos + sspe_f_pos + 1 ] == SSPE_MAGIC_SECOND )
					{

						uart_len = data[ pos + 2 ];
						//GPIO_PinOutClear(gpioPortB, 12);

						 //printf("### %d, %d, %d,, ", uart_len, length, data[ pos + 3 ] );


						if( data[ pos + 3 ] == 0 )  /* FIXME: length < 255 */
						{
																length -= MAIN_HDR_SIZE;
							if( length >= uart_len )
							{
								uint16_t crc;

								crc = calc_crc( &data[ pos + MAIN_HDR_SIZE ], uart_len );


								if( ( uint8_t )( crc & 0xFF ) == data[ pos + 4 ] && ( uint8_t )( ( crc >> 8 ) & 0xFF ) == data[ pos + 5 ] )
								{
									Led_Status=1;
									//if( memcmp( prev_data, &MyQueue[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE ) != 0 )
									if( memcmp( prev_data, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE ) != 0 )
									{
										//memcpy( prev_data, &MyQueue[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );
										memcpy( prev_data, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );

										//MakeIEEE802154Packet();
										SendMessageIEEE802154( YES_BCAST_ADDR, &MyQueue[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );
										//SendMessageIEEE802154( YES_BCAST_ADDR, &data[ pos + KME_HEADER_SIZE ], uart_len + MAIN_HDR_SIZE - KME_HEADER_SIZE );
										//GPIO_PinOutClear(gpioPortB, 11);
									}
									//usDelay( 1000 );


									 //printf("## FIND %d,, ", queue_retrieve );
								}
								else
								{
									 //printf("## FIND CHK %d(%d:%d,%d:%d),, ", queue_retrieve, crc & 0xFF, ( crc >> 8 ) & 0xFF, data[ pos + 4 ], data[ pos + 5 ] );

								}
								queue_retrieve = pos + uart_len + MAIN_HDR_SIZE;
								if( queue_retrieve >= UART_QUEUE_SIZE )
									queue_retrieve -= UART_QUEUE_SIZE;
							}
							find = 1;
							 //printf( "\r\n" );

							break;
						}
						else
						{

							find = 1;
							queue_retrieve = pos + MAIN_HDR_SIZE;
							if( queue_retrieve >= UART_QUEUE_SIZE )
								queue_retrieve -= UART_QUEUE_SIZE;

							 //printf("## ERR %d\r\n", queue_retrieve );

							break;
						}
					}

					 //printf( "\r\n" );
				}

				pos++;
				length--;
				if( pos >= UART_QUEUE_SIZE )
					pos = 0;
			}
			if( find == 0 )
			{
				queue_retrieve = 0;
				if( pos > 0 )
					queue_retrieve = pos - 1;
			}


		}



		length = queue_que - queue_retrieve;
		if( queue_que < queue_retrieve )
			length = UART_QUEUE_SIZE + queue_que - queue_retrieve;

		if( length == 0 )
		{
			CORE_DECLARE_IRQ_STATE;
			CORE_ENTER_CRITICAL();

			queue_que = 0;
			queue_retrieve = 0;

			CORE_EXIT_CRITICAL();
		}
	}
}
