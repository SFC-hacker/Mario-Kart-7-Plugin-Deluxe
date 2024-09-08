#include "CTRPluginFramework.hpp"
#include "cheats.hpp"

namespace CTRPluginFramework
{
	u32 RmDtc (u32 r1, u32 r2, u32 r3, u32 r4, u32 r5)
	{
		u32 rm = *(u32 *)0x5EBF4C;
		
		if (rm == 0x75BCD15)
		{
			return(r1);
		}
		
		else if (rm == 0x9A2112)
		{
			return(r2);
		}
		
		else if (rm == 0x3ADE68B1)
		{
			return(r3);
		}
		
		else if (rm == 0x3E7)
		{
			return(r4);
		}
		
		else if (rm == 0x989680)
		{
			return(r5);
		}
		return 0;
	}
	// if the patch is applied.
	// another checker needed for hook protection.
	
	u8	GetNetwork()
	{
		u32 offset = 0;
		u8 network;
		
		if (Process::Read32(0x663D00, offset))
        {
            if (Process::Read32(offset + 0x13C, offset))
            {
				if (Process::Read8(offset + 0x18, network) && network == 1)
				{
					return network;
				}
			}
        }
		return 0;
	}
	
	
	u32 GetMatch()
	{
		u32 offset = 0;
		u32 matching;
		
		if (Process::Read32(0x663D04, offset) && Process::Read32(offset + 0x50, offset) && Process::Read32(offset + 0x44, offset))
		{
			if (Process::Read32(offset + 0x40, matching) && matching != 0)
			{
				return matching;
			}
		}
		return 0;
	}
	
	u32 GetBattleTime()
	{
		u32 offset = 0;
		u32 timeout;
		if (Process::Read32(0x65C528, offset))
		{
			if (Process::Read32(offset + 0x80, timeout))
			{
				return timeout;
			}
		}
		return 0;
	}
	// v3.0.0 to v3.0.2 for references.
}