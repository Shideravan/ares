// Casio PJ-01 port circular pinout:
//    __________
//   /(1) (2) (3)\
//  | (4)  (5) (6)|
//   \   (7) (8) /
//    ---------

//Example: Master System port
// pin  name  bit
//  1:  D0    0
//  2:  D1    1
//  3:  D2    2
//  4:  D3    3
//  5:  VCC   -
//  6:  TL    4
//  7:  TH    5
//  8:  GND   -
//  9:  TR    6

// MAME's driver
// uint8_t pv1000_state::io_r(offs_t offset)
// {
// 	uint8_t data = m_io_regs[offset];

// //  logerror("io_r offset=%02x\n", offset );

// 	switch ( offset )
// 	{
// 	case 0x04:
// 		/* Bit 1 = 1 => Data is available in port FD */
// 		/* Bit 0 = 1 => Buffer at port FD is empty */
// 		data = 0;
// 		data = m_fd_buffer_flag & 1;
// 		data |= m_fd_data ? 2 : 0;
// 		m_fd_buffer_flag &= ~1;
// 		break;
// 	case 0x05:
// 		static const char *const joynames[] = { "IN0", "IN1", "IN2", "IN3" };

// 		data = 0;

// 		for (int i = 0; i < 4; i++)
// 		{
// 			if (m_io_regs[5] & 1 << i)
// 			{
// 				data |= ioport(joynames[i])->read();
// 				m_fd_data &= ~(1 << i);
// 			}
// 		}

// 		//m_fd_data = 0;
// 		break;
// 	}

// 	return data;
// }

struct Controller {
  Node::Peripheral node;

  virtual ~Controller() = default;

  virtual auto read() -> n7 { return 0x7f; }
  virtual auto write(n8 data) -> void {}
};

#include "port.hpp"
#include "gamepad/gamepad.hpp"
