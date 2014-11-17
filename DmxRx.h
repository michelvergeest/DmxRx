/* DmxRx */
#include <Arduino.h>

#ifndef _DMXRX_H
#define _DMXRX_H


#define DMXRX_BAUDRATE	((F_CPU / (250000 * 8)) - 1)
#define	DMXRX_UCSRA 	(1<<U2X0)
#define	DMXRX_UCSRC		(3<<UCSZ00) | (1<<USBS0)
#define	DMXRX_UCSRB 	(1<<RXEN0) | (1<<RXCIE0)

#define	DMXRX_TIMEOUT 	1000
#define	DMXRX_STARTCODE 0	

class DmxRx {
	
	typedef void (*func_t)();

	typedef enum { RX_WAITFORBREAK, RX_STARTCODE, RX_STARTADDRESS, RX_DATA } rxState_t;

/*	static const uint16_t TIMEOUT = 1000;
	static const uint8_t STARTCODE = 0;*/
   		
	public:
		// constructor	
		DmxRx();
		void	setMemory();
		void 	setStartAddress(uint16_t address);
		void 	setMaxChannels(uint16_t size);
		void 	check();
		 	
		void 	receive(uint8_t state, uint8_t rxData);

		uint16_t getMaxChannels();
		uint8_t read(uint16_t channel);
		void 	read(uint8_t *buffer, uint16_t size);
		void 	setTimeout(uint16_t time) ;

		void 	attachDisconnected(func_t);
		void 	attachFrameReceived(func_t);
		void 	attachFrameChanged(func_t);
		void 	attachConnected(func_t);
		void	clear();

		uint8_t getFrequency();
		
		
	
	private:
		rxState_t 	_state;
		uint16_t 	_maxChannel;
		uint16_t	_startAddress;
		
		// channel counter
		uint16_t	_cnt;
		uint16_t	_oldCnt;

		// buffer
		uint16_t	_bufIndex; 
		uint16_t 	_bufSize;
		uint8_t *	_buf;

		
		uint16_t	_timeoutValue;
		uint32_t	_timeout;
		
		bool 		_ready;
		bool		_changed;
		bool 		_connected;
		bool 		_hold;

		func_t		_cbDisconnected;
		func_t		_cbFrameReceived;
		func_t		_cbFrameChanged;
		func_t		_cbConnected;

		// frequency counter
		uint32_t	_periodTime;
		uint8_t		_frequencyCounter;
		uint8_t		_frequency;


		void 		clearFrequency();
		void 		calculateFrequency();

};

#endif // _DMXRX_H