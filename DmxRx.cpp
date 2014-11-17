#include <DmxRx.h>

// constructor 
DmxRx::DmxRx() {
  _cnt = 0;
  _oldCnt = !_cnt; 
  _state = RX_WAITFORBREAK;
  _ready = false;
  _timeoutValue = DMXRX_TIMEOUT;
  _timeout = 0;
  _connected = false;
  _hold = false;
  _changed = false;
  _cbDisconnected = NULL; 
  _cbConnected = NULL;
  _cbFrameReceived = NULL;
  _cbFrameChanged = NULL;
  clearFrequency();
  _startAddress = 1;
  _buf = NULL;
  _bufSize = 512;
} 


// alloc memory
void DmxRx::setMemory() {
	_buf = (uint8_t *)calloc(_bufSize, sizeof(uint8_t));
}	

// set the maximum used channel
void DmxRx::setMaxChannels(uint16_t size) {
  _bufSize = min(size, 512);
}

// set start address
void DmxRx::setStartAddress(uint16_t address) {
  if (address == 0) return;
  _startAddress = min(address, 512);
}

void DmxRx::receive(uint8_t state, uint8_t rxData) {

  //check for break (frame error)       
  if (state & (1<<FE0)) { 
    // index(N) must be index(N - 1) for a valid packet
 	  if (_cnt == _oldCnt) {
      _ready = true;
      _maxChannel = _cnt; 
    } else {
      _oldCnt = _cnt; // save index
    }
   	// check buffer
    if (_buf != NULL) _state = RX_STARTCODE;
    return;
  }

  switch (_state) {
  	case  RX_STARTCODE:
      if (rxData == DMXRX_STARTCODE) {
        _bufIndex = 0;
        _cnt  = 0; // reset counter		
        _state = RX_STARTADDRESS; // startadress 
      } else {
        _state = RX_WAITFORBREAK;
      }
  		break; 	
  	case RX_STARTADDRESS:
  		if (_cnt < _startAddress - 1) {
  			_cnt++;
  			break; // wait for a startaddress
  		}
  		// startaddress is ok now
  		_state = RX_DATA;
   	case RX_DATA:
   		_cnt++;
		if (_buf[_bufIndex] != rxData) {
			_buf[_bufIndex] = rxData;
			_changed = true;
		}
		_bufIndex++;

		if (_bufIndex > _bufSize) {
			 // maximum channels received
			_state = RX_WAITFORBREAK; // wait for a break
		} 
		break;
  }
}	



// set timeout in milliseconds
void DmxRx::setTimeout(uint16_t time) {
  _timeoutValue = time;
}

// return maximum received channels
uint16_t DmxRx::getMaxChannels() {
  return _maxChannel;
}

// read the current value of a channel
uint8_t DmxRx::read(uint16_t channel) {
  if (channel == 0) return 0;
  if (channel > _bufSize) return 0;
  // remove offset
  return _buf[channel - 1];
}

// read size bytes into buffer
void DmxRx::read(uint8_t *buffer, uint16_t size) {
  size = min(size, _bufSize);
  memcpy(buffer, _buf, size);
}


// clear data in buffer   
void DmxRx::clear() {
  memset(_buf, 0, _bufSize);
}

// get frequency (updates per second)
uint8_t DmxRx::getFrequency() {
  return _frequency; 
}

void DmxRx::clearFrequency() {
  _frequency = 0;
  _frequencyCounter = 0;
  _periodTime = millis() + 1000;
}

void DmxRx::calculateFrequency() {
  _frequencyCounter++;
  if ((long)(millis() - _periodTime) >= 0) {
    _periodTime += 1000; 
    _frequency = _frequencyCounter;
    _frequencyCounter = 0;
  }  
}


// check if data received
void DmxRx::check() {
  if (_ready) {
    _ready = false;
   
    // callback for connected
    if (!_connected) { 
       _connected = true;
      if (_cbConnected) _cbConnected();
      clearFrequency();
    }
   
    // callback for frame Received
    if (_cbFrameReceived) _cbFrameReceived();

    // callback for update
    if (_changed) { 
        _changed = false;
        if (_cbFrameChanged) _cbFrameChanged();
    }

    calculateFrequency();
    _timeout = millis() + _timeoutValue;
    return;
  } 

  // timeout once
  if (_connected) {
    if ((long)(millis() - _timeout) >= 0) {
      _connected = false;
      _state = RX_WAITFORBREAK;
      // callback for disconnected 
      if (_cbDisconnected) _cbDisconnected();
      
      if (!_hold) clear();
      
      clearFrequency();
    } 
  }    
 
}

// attach Disconnected function 
void DmxRx::attachDisconnected(func_t function) {
  _cbDisconnected = function;
}

// attach Connected function 
void DmxRx::attachConnected(func_t function) {
  _cbConnected = function;
}

// attach Frame Received function 
void DmxRx::attachFrameReceived(func_t function) {
  _cbFrameReceived = function;
}

// attach Frame update function 
void DmxRx::attachFrameChanged(func_t function) {
  _cbFrameChanged = function;
}


