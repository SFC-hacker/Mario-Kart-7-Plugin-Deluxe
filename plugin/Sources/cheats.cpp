#include "CTRPluginFramework.hpp"
#include "cheats.hpp"
#include "Helpers/MenuEntryHelpers.hpp"
#include "Unicode.h"
#include "csvc.h"
#include "3ds.h"
#include <math.h>
#include <vector>

namespace CTRPluginFramework
{
	using StringVector = std::vector<std::string>;
	u32 offset = 0, data = 0;
	static u32 original_speed[0x2D];
	u32 patch_address = 0;
	
	// Custom functions

	bool is_in_range(u32 value, u32 lowerbound, u32 upperbound)
	{
		return value > lowerbound && value < upperbound;
	}
	
	static bool WasShown = false;

	bool IsInRace()
	{
		if (Process::Read32(0x14000084, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			if (Process::Read32(offset + 0x316C, offset) && is_in_range(offset, 0x14000000, 0x18000000))
				return Process::Read32(offset + 0x118, offset) && (offset & 0xFF);
		return 0;
	}

	u32 AutoRegion(u32 eur, u32 jpn, u32 usa) 
	{
		u64 titleID = Process::GetTitleID();
		u32 instruction = *(u32 *)0x500000;

		if (titleID == 0x0004000000030600)
		{
			if (instruction == 0xE1A00008) return(jpn);
			//OSD::Notify("JPN", Color::LimeGreen);
		}
		
		else if (titleID == 0x0004000000030700)
		{
			if (instruction == 0x10900002) return(eur);
			//OSD::Notify("EUR", Color::LimeGreen);
		}

		else if (titleID == 0x0004000000030800)
		{
			if (instruction == 0xEBF5F921) return(usa);
			//OSD::Notify("USA", Color::LimeGreen);
		}
		
		return 0;
	}

	u32 GetRacePointer()
	{
		if (IsInRace())
			if (Process::Read32(0x140002F4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
				if (Process::Read32(offset + 0x14, offset) && is_in_range(offset, 0x14000000, 0x18000000))
					if (Process::Read32(offset + 0x518, offset) && is_in_range(offset, 0x14000000, 0x18000000))
						if (Process::Read32(offset + 0x1C, offset) && is_in_range(offset, 0x14000000, 0x18000000))
							return offset;
		return 0;
	}

	u32	GetFNsPointer()
	{
		if (IsInRace() && Process::Read32(0xFFFF6F0, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			return offset + 8;
		return 0;
	}
	
	u32	GetOldPointer5CC()
	{
		if (IsInRace() && Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			if (Process::Read32(offset + 0x5CC, offset) && is_in_range(offset, 0x14000000, 0x18000000))
				return offset;
		return 0;
	}

	u32	GetOldPointer5D0()
	{
		if (IsInRace() && Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			if (Process::Read32(offset + 0x5D0, offset) && is_in_range(offset, 0x14000000, 0x18000000))
				return offset;
		return 0;
	}

	u32	GetItemPointer()
	{
		if (IsInRace() && Process::Read32(0x14000074, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			if (Process::Read32(offset - 0x1B5C, offset) && is_in_range(offset, 0x14000000, 0x18000000))
				if (Process::Read32(offset + 0x27AC, offset) && is_in_range(offset, 0x14000000, 0x18000000))
					return offset;
		return 0;
	}
	
	u8 GetTotalPlayers()
	{
		u8 total;
		if (Process::Read32(0x14000074, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			if (Process::Read32(offset - 0xC10, offset) && is_in_range(offset, 0x14000000, 0x18000000))
				if (Process::Read32(offset + 0x23C, offset) && is_in_range(offset, 0x14000000, 0x18000000))
					if (Process::Read8(offset - 0xF28, total) && is_in_range(offset, 0x14000000, 0x18000000))
						return total;
		return 0;
	}
	
	u32 GetBattleTime()
	{
		u32 timeout;
		if (IsInRace() && Process::Read32(0x65C528, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			if (Process::Read32(offset + 0x80, timeout))
				return timeout;
		}
		return 0;
	}
	
	u8	GetNetwork()
	{
		u8 network;
		if (Process::Read32(0x663D00, offset) && is_in_range(offset, 0x14000000, 0x18000000))
        {
            if (Process::Read32(offset + 0x13C, offset))
            {
				if (Process::Read8(offset + 0x18, network))
					return network;
			}
        }
		return 0;
	}

	u16	GetTime()
	{
		u16 time = 0;
		if (GetOldPointer5D0())
		{
			Process::Read16(GetOldPointer5D0() + 0x484, time);
			time /= 60;
			if (300 - time < 0)
				return 0;
			else
				return 300 - time;
		}
		return 0;
	}

	u16	GetMinutes()
	{
		u16 time = GetTime(), minutes = 0;
		while (time - 60 >= 0)
		{
			time -= 60;
			minutes++;
		}
		return minutes;
	}

	u16	GetSeconds()
	{
		u16 time = GetTime();
		while (time - 60 >= 0)
			time -= 60;
		return time;
	}

	void	SubToTime(u16 seconds)
	{
		u16 time = 0;
		if (offset)
		{
			Process::Read16(GetOldPointer5D0() + 0x484, time);
			time -= (seconds * 60);
			Process::Write16(GetOldPointer5D0() + 0x484, time);
			Process::Write16(GetRacePointer() + 0xC4, time);
		}
	}

	void	writeItem(u32 item)
	{
		if (GetItemPointer())
		{
			Process::Write32(GetItemPointer() + 0x3C, 0xFFFFFFFF);
			Process::Write32(GetItemPointer() + 0xA8, 3);
			Process::Write32(GetItemPointer() + 0xC8, item);
			Process::Write32(GetItemPointer() + 0xD8, 0x3F800000);
		}
	}

	void	writeSpeed(u32 speed)
	{
		for (int i = 0; i < 0x2D; i++)
		{
			if (Process::Read32(0x140002F4, offset) && is_in_range(offset, 0x14000000, 0x18000000) && Process::Read32(offset - 0xA4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				if (original_speed[2] != speed)
					Process::Read32((offset - 0x2C3B0) + (i * 4), original_speed[i]);
				Process::Write32((offset - 0x2C3B0) + (i * 4), speed);
				Process::Write32((offset - 0x28E90) + (i * 4), speed);
				Process::Write32((offset - 0x1C730) + (i * 4), speed);
			}
		}
	}
	
	void	writeAcc(int speed)
	{
		for (int i = 0; i < 0x2B; i++)
		{
			Process::Write32(0x15386670, speed);
			Process::Write32(0x15386CF0, speed);
		}
	}
	
	void	writeVr(u32 vr)
	{
		if (Process::Read32(0x663D04, offset) && is_in_range(offset, 0x10000000, 0x18000000))
			Process::Write32(offset - 0xE30, vr);
	}
	
	void	writeVc(u32 vc)
	{
		if (Process::Read32(0x6673C8, offset) && is_in_range(offset, 0x10000000, 0x18000000))
			Process::CopyMemory((void *)(offset + 0x6BA0), (void *)&vc, 3);
	}
	
	void	writeDriver(u32 driver)
	{
		if (Process::Read32(0x6673C8, offset) && is_in_range(offset, 0x10000000, 0x18000000))
			Process::Write8(offset + 0x6BA2, driver);
	}

	void	writeStreetpass(u32 tag)
	{
		if (Process::Read32(0x6673C8, offset) && is_in_range(offset, 0x10000000, 0x18000000))
			Process::Write32(offset + 0x6B88, tag);
	}

	void	writeVictories(u32 vic)
	{
		if (Process::Read32(0x6673C8, offset) && is_in_range(offset, 0x10000000, 0x18000000))
			Process::Write32(offset + 0x6B98, vic);
	}
	
	void	writeDefeats(u32 def)
	{
		if (Process::Read32(0x6673C8, offset) && is_in_range(offset, 0x10000000, 0x18000000))
			Process::Write32(offset + 0x6B9C, def);
	}
	
	void	writeCoins(u32 coi)
	{
		if (Process::Read32(0x6673C8, offset) && is_in_range(offset, 0x10000000, 0x18000000))
			Process::Write32(offset + 0x698C, coi);
	}

	void	writeLocation(u32 location)
	{
		if (Process::Read32(0x6673C8, offset) && is_in_range(offset, 0x10000000, 0x18000000))
			Process::Write32(offset + 0xF7CC, location);
	}

	void	writeFlag(u8 flag)
	{
		if (Process::Read32(0x6673C8, offset) && is_in_range(offset, 0x10000000, 0x18000000))
		{
			Process::Write8(offset + 0xF7D0, flag);
		}
	}
	
	void	writeState(u8 state)
	{
		if (Process::Read32(0x6673C8, offset) && is_in_range(offset, 0x10000000, 0x18000000))
		{
			Process::Write8(offset + 0xE7CA, state);
		}
	}
	
	void	writePoint(u16 point)
	{
		if (Process::Read32(0x65C9A8, offset) && is_in_range(offset, 0x10000000, 0x18000000))
		{
			Process::Write8(offset + 0x175D4, point);
		}
	}
	
	void writeComment(u8 message)
	{
		u32 value = 0;
		if(Process::Read32(0x663D00, value) && value != 0)
		{
			Process::Read32(value + 0x298, value);
			Process::Write8(value + 0x66, message);
		}
	}
	
	void	writeMusic(u8 music)
	{
		u32 u0 = AutoRegion(0x3D2D3C, 0x3D2D44, 0x3D2D1C);
		Process::Write32(u0, 0xE3A00000 + music);
	}
	
	void	writeTime(u32 time)
	{
		if (IsInRace() && Process::Read32(0x65C528, offset) && is_in_range(offset, 0x10000000, 0x18000000))
		{
			Process::Write8(offset + 0x80, time);
		}
	}
	
	void	writeCoinScore(u8 cscore)
	{
		if (IsInRace() && (Process::Read32(0x140005AC, offset)))
		{
			if (Process::Read32(offset + 0x12EC, offset))
			{
				Process::Write8(offset + 0x46, cscore);
			}
		}
	}
	
	void	writeBalloonScore(u8 bscore)
	{
		if (IsInRace() && (Process::Read32(0x140005AC, offset)))
		{
			if (Process::Read32(offset + 0x12EC, offset))
			{
				Process::Write8(offset + 0x54, bscore);
			}
		}
	}
	
	void	writeBalloonAmount(u8 amount)
	{
		if (IsInRace() && (Process::Read32(0x140005AC, offset)))
		{
			if (Process::Read32(offset + 0x12EC, offset))
			{
				Process::Write8(offset + 0x56, amount);
			}
		}
	}
	
	void	writeHandler(u32 handler)
	{
		if (IsInRace() && (Process::Read32(0x65C9A8, offset)))
		{
			if(Process::Read32(offset + 0xF4, offset))
			{
				Process::Write32(offset + 0x88, handler);
			}
		}
	}
	
	void	writeRmID(u32 rmID)// Unused
	{
		if (Process::Read32(0, offset))
		{
			Process::Read32(offset + 0, offset);
			Process::Read32(offset + 0, offset);
			Process::Write32(offset + 0, rmID);
			// For GetrmID, GetPIDs, GetIPs, InfoCopy ... see prv.cpp.
		}
	}

	
	
	
	
	
	
	
	
	// Kart codes

	void	alwaysBlinking(MenuEntry *entry)
	{	
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Blinking Invincibility";
		}

		if (IsInRace())
		{
			Process::Write16(GetRacePointer() + 0x102C, 0xFFFF);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Invincibility";
			
			if (IsInRace())
			{
				Process::Write16(GetRacePointer() + 0x102C, 0);
			}
		}
	}
	
	void	alwaysStarPower(MenuEntry *entry)
	{	
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Star Power";
		}
		
		if (IsInRace())
		{
			Process::Write16(GetRacePointer() + 0xFF4, 0xFFFF);
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Star Power";
			
			if (IsInRace())
			{
				Process::Write32(GetRacePointer() + 0xFF4, 0);
			}
		}
	}
	
	struct Cycle
	{
		const char  *name;
		const u8    id;
	};

	static const std::vector<Cycle> g_cycles =
	{
		{ "Very Slow Cycle", 0x80 },
		{ "Slow Cycle", 0x60 },
		{ "Normal Cycle", 0x50 },
		{ "Fast Cycle", 0x40 }
		//{ "Very Fast Cycle", 0x30 } closet cheat risk with such delay?
	};

	void    CycleSetter(MenuEntry *entry)
    {
        u8 *cycle = GetArg<u8>(entry);
		
        static StringVector cycles;
		if (cycles.empty())
			for (const Cycle &i : g_cycles)
				cycles.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select your star cycler speed", cycles);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *cycle = g_cycles[selOpt].id;
    }	
	
	void	starPowerCycler(MenuEntry *entry)
	{
		u8 cycle = *GetArg<u8>(entry);
		u8 up = 0;
		u8 count = 0;
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Star Power Cycler";
		}
		
		if (IsInRace())
		{
			Process::Read8(offset + 0x6BE900, up);
			up += 0x2;
			Process::Write8(offset + 0x6BE900, up);
		
			if (Process::Read8(offset + 0x6BE900, count) && count >= cycle)
			{
				Process::Write8(offset + 0x6BE900, 0);
			}
			
			if (Process::Read8(offset + 0x6BE900, count) && count == 0)
			{
				Process::Write32(GetRacePointer() + 0xFF4, 0);
			}
			
			if (Process::Read8(offset + 0x6BE900, count) && count == 0x20)
			{
				Process::Write32(GetRacePointer() + 0xFF4, 0xFFFF);
			}
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Star Power Cycler";
			Process::Write8(offset + 0x6BE900, 0);
			
			if (IsInRace())
			{
				Process::Write32(GetRacePointer() + 0xFF4, 0);
			}
		}
	}
	
	void	alwaysSmall(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Always Small";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Always Small";
		}
	
		if (IsInRace())
		{
			Process::Write16(GetRacePointer() + 0x1000, 0xFFFF);
		}
	}
	
	void	instantSmallRecovery(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Instant Small Recovery";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Instant Small Recovery";
		}
	
		if (IsInRace())
		{
			Process::Write16(GetRacePointer() + 0x1000, 0x0000);
		}
	}
	
	void	alwaysBlackKart(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Always Black Kart";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Always Black Kart";
		}
	
		if (IsInRace())
		{
			Process::Write16(GetRacePointer() + 0xFF8, 0xFFFF);
		}
	}
	
	void	instantBlooperRecovery(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Instant Blooper Recovery";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Instant Blooper Recovery";
		}
	
		if (IsInRace())
		{
			Process::Write16(GetRacePointer() + 0xFF8, 0x0000);
		}
	}
	
	void	alwaysFlat(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Always Flat";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Always Flat";
		}
	
		if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			offset += 0xFFFFF930;
			Process::Read32(offset + 0, offset);
			offset += 0xFFFFF26C;
			Process::Write8(offset + 0, 0x3);
			Process::Write32(offset + 0x10, 0x3E4CCCCD);
		}
	}
	
	void	instantFlatRecovery(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Instant Flat Recovery";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Instant Flat Recovery";
		}
	
		if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			offset += 0xFFFFF930;
			Process::Read32(offset + 0, offset);
			offset += 0xFFFFF26C;
			Process::Write8(offset + 0, 0);
		}
	}
	
	void	neverFlatRecovery(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Never Flat Recovery";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Never Flat Recovery";
		}
	
		if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			offset += 0xFFFFF930;
			Process::Read32(offset + 0, offset);
			offset += 0xFFFFF26C;
			Process::Write32(offset + 0, 3);
		}
	}
	
	void	alwaysGlider(MenuEntry *entry)
	{
		if (entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Always Glider";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Always Glider";
		}
	
		if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			offset += 0xFFFFF930;
			Process::Read32(offset + 0, offset);
			offset += 0xFFFFEE99;
			Process::Write8(offset + 0, 1);
		}
	}
	
	void	alwaysConfused(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Always Confused";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Always Confused";
		}
	
		if (IsInRace())
		{
			Process::Write16(GetRacePointer() + 0xFFC, 0xFFFF);
		}
	}
	
	void	microKart(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2F9860, 0x2F9868, 0x2F9840);
		u32 u1 = AutoRegion(0x2BE2B8, 0x2BE2C0, 0x2BE298);
		
		if (entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Micro Kart";
		}
		
		if (Controller::IsKeysDown(DPadDown + R))
		{
			if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				offset += 0xFFFFF930;
				Process::Read32(offset + 0, offset);
				offset += 0xFFFFEE9B;
				Process::Write8(offset + 0, 2);
				Process::Write32(u0, 0xE5850240);
				Process::Write32(u1, 0xE1A00000);
			}
		}

		if (Controller::IsKeyDown(Start))
		{
			Process::Write32(u0, 0xE5850243);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Micro Kart";
			Process::Write32(u0, 0xE5850244);
			Process::Write32(u1, 0x1A0000A5);
		}
	}
	
	void	controlKiller(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x444C00, 0x444B7C, 0x444B84);
		u32 u1 = AutoRegion(0x2BE2C0, 0x2BE2C8, 0x2BE2A0);
		u32 u2 = AutoRegion(0x2E4C3C, 0x2E4C44, 0x2E4C1C);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Control Bullet Bill";
			Process::Write32(u0, 0xE1A00000);
			Process::Write32(u1, 0xE1A00000);
			Process::Write32(u2, 0xE3A01000);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Control Bullet Bill";
			Process::Write32(u0, 0xE5D01086);
			Process::Write32(u1, 0x1A0000A3);
			Process::Write32(u2, 0xE3A01001);
			Process::Write32(0x6655A4, 0x41000000);
		}
		
		if (Controller::IsKeysDown(DPadRight + Y))
		{
			if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				offset += 0xFFFFF930;
				Process::Read32(offset + 0, offset);
				offset += 0xFFFFEE9A;
				Process::Write8(offset + 0, 0x40);
				Process::Write16(offset + 0x135, 1);
			}
        }
		
		if (Controller::IsKeysDown(DPadLeft + Y))
		{
			if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				offset += 0xFFFFF930;
				Process::Read32(offset + 0, offset);
				offset += 0xFFFFEE9A;
				Process::Write8(offset + 0, 0);
				Process::Write16(offset + 0x135, 0);
			}
		}
		
		if (Controller::IsKeyDown(A))
		{
			Process::Write32(0x6655A4, 0x40F33333);
		}
		
		if (Controller::IsKeyDown(B))
		{
			Process::Write32(0x6655A4, 0xC1000000);
		}	
	}
	
	void	driveBounds(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2F9848, 0x2F9850, 0x2F9828);
		u32 u1 = AutoRegion(0x2D9344, 0x2D934C, 0x2D9324);
		u32 u2 = AutoRegion(0x2D9348, 0x2D9350, 0x2D9328);
		u32 u3 = AutoRegion(0x2D640C, 0x2D6414, 0x2D63EC);
		
        if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Drive Out Of Bounds";
		}

		if (Controller::IsKeyDown(Y))
		{
			Process::Write32(u0, 0xE3800000);
			Process::Write32(u1, 0xE3A01000);
			Process::Write32(u2, 0xE5840C2E);
			Process::Write32(u3, 0xE3A01001);
		}
		
		if (Controller::IsKeyDown(Start))
		{
			Process::Write32(u0, 0xE3800040);
			Process::Write32(u1, 0xE3C00001);
			Process::Write32(u2, 0xE5840C38);
			Process::Write32(u3, 0xE3A01000);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Drive Out Of Bounds";
			Process::Write32(u0, 0xE3800040);
			Process::Write32(u1, 0xE3C00001);
			Process::Write32(u2, 0xE5840C38);
			Process::Write32(u3, 0xE3A01000);
		}
	}
	
	void	rapidfire(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2BE354, 0x2BE35C, 0x2BE334);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Rapidfire";
		}
	
		if(Controller::IsKeyPressed(X))
		{
			Process::Write32(u0, 0xE3A08001);
		}

		if(Controller::IsKeyPressed(L))
		{
            Process::Write32(u0, 0xE3A08000);
        }
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Rapidfire";
			Process::Write32(u0, 0xE3A08000);
		}
	}
	
	void	itemsKiller(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2BE2C0, 0x2BE2C8, 0x2BE2A0);
		u32 u1 = AutoRegion(0x2E4C3C, 0x2E4C44, 0x2E4C1C);
		
        if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Item Use In Bullet";
			Process::Write32(u0, 0xE1A00000);
			Process::Write32(u1, 0xE3A01000);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Item Use In Bullet";
			Process::Write32(u0, 0x1A0000A3);
			Process::Write32(u1, 0xE3A01001);
		}
	}
	
	void	lookBack(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Look Backward";
		}
		
		if (Controller::IsKeysDown(B + Y))
		{
			Process::Write32(0x663EA4, 0xC2800000);
		}

		if (Controller::IsKeyDown(A))
		{
			Process::Write32(0x663EA4, 0x42480000);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Look Backward";
			Process::Write32(0x663EA4, 0x42480000);
		}
	}
	
	void	itemDespawner(MenuEntry *entry)
	{
        u32 u0 = AutoRegion(0x2B71C0, 0x2B71C8, 0x2B71A0);

        if(entry->WasJustActivated())
		{
            entry->Name() = Color::LimeGreen << "Item Despawner";
        }

        if(Controller::IsKeyDown(X))
		{
            Process::Write32(u0, 0xE3A00003);
        }

        if(Controller::IsKeyDown(L))
		{
            Process::Write32(u0, 0xE3A00000);
        }

        if (!entry->IsActivated())
		{
            entry->Name() = "Item Despawner";
            Process::Write32(u0, 0xE3A00003);
        }
    }
	
	void	turningSpeed(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Better Turning Sensitivity";
			Process::Write32(0x6657F4, 0x40A00000);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Better Turning Sensitivity";
			Process::Write32(0x6657F4, 0x40000000);
		}
	}
	
	void	disableStarAcceleration(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Disable Star Power Acceleration";
			Process::Write32(0x6657B4, 0);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Disable Star Power Acceleration";
			Process::Write32(0x6657B4, 0x40400000);
		}
	}
	
	void	disableLuckyShroom(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Disable Lucky-7 Mushroom Acceleration";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Disable Lucky-7 Mushroom Acceleration";
		}
	
		if (IsInRace())
		{
			Process::Write16(GetRacePointer() + 0xEE90, 0x0000);
		}
	}
	
	void	disableFirstPersonView(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x444C00, 0x444B7C, 0x444B84);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Disable First Person View";
			Process::Write32(u0, 0xE1A00000);
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Disable First Person View";
			Process::Write32(u0, 0xE5D01086);
		}
	}
	
	void	trickAnywhere(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Trick Anywhere";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Trick Anywhere";
		}
	
		if (IsInRace())
		{
			Process::Write16(GetRacePointer() + 0xFF0, 0xFFFF);
		}
	}
	
	void	driveWalls(MenuEntry *entry)
	{	
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Drive On Walls";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Drive On Walls";
		}
		
		if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			offset += 0xFFFFF930;
			Process::Read32(offset + 0, offset);
			offset += 0xFFFFEE9A;
			Process::Write8(offset + 0, 9);
		}
	}
	
	void	disableWalls(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2D64D0, 0x2D64D8, 0x2D64B0);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Disable Wall Collisions";
			Process::Write32(u0, 0xE3A00000);
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Disable Wall Collisions";
			Process::Write32(u0, 0xE3A00003);
		}
	}

	void	autopilot(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Autopilot";
		}
		
		if (IsInRace() && (Process::Read32(0x140005AC, offset)))
		{
			Process::Read32(offset + 0x650, offset);
			offset += 0xFFFFE30E;
			Process::Write16(offset + 0, 1);
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Autopilot";
		}
	}
	
	void	autoJump(MenuEntry *entry)
	{	
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Autojump";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Autojump";
		}
		
		if (Controller::IsKeyDown(R))
		{
			if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				offset += 0xFFFFF930;
				Process::Read32(offset + 0, offset);
				offset += 0xFFFFEE91;
				Process::Write32(offset + 0, 0xF00);
			}
		}
	}
	
	void	stopTanooki(MenuEntry *entry)
	{	
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Instant Stop Super Leaf";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Instant Stop Super Leaf";
		}
		
		if (Controller::IsKeyDown(DPadDown))
		{
			if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				offset += 0xFFFFF930;
				Process::Read32(offset + 0, offset);
				offset += 0xFFFFEE9B;
				Process::Write8(offset + 0, 0);
			}
		}
	}
	
	void	instantMT(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Instant Mini Turbo";
			Process::Write32(0x665634, 0);
			Process::Write32(0x665638, 0);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Instant Mini Turbo";
			Process::Write32(0x665634, 0x435C0000);
			Process::Write32(0x665638, 0x43E60000);
		}
	}
	
	void	insideDrift(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Inside Drifing Kart";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Inside Drifing Kart";
		}
	
		if (IsInRace())
		{
			Process::Write16(GetRacePointer() + 0x1036, 0xFFFF);
		}
	}
	
	void	hugeBunnyhop(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Huge Bunnyhop";
			Process::Write32(0x6655D0, 0x40400000);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Huge Bunnyhop";
			Process::Write32(0x6655D0, 0x3FA66666);
		}
	}

	void	sizeChanger(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Size Changer";
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Size Changer";
		}
	
		static float PlayerSize = 1.f, speed = 0.03f;
		static bool adding = true, held = false;
		bool in_race = IsInRace();
		if (in_race && GetRacePointer())
		{
			if (PlayerSize < 3.f && adding)
				PlayerSize += speed;
			if (PlayerSize > 0.1f && !adding)
				PlayerSize -= speed;
			if (PlayerSize > 3.f && adding)
			{
				adding = false;
				PlayerSize -= speed;
			}
			if (PlayerSize < 0.1f && !adding)
			{
				adding = true;
				PlayerSize += speed;
			}
			Process::WriteFloat(GetRacePointer() + 0x100C, PlayerSize);
		}
		if (!held && in_race && -0.015f < speed && speed < 0.15f && speed != 0.15f && Controller::IsKeyDown(DPadRight))
		{
			held = true;
			speed += 0.01f;
		}
		if (!held && in_race && -0.015f < speed && speed < 0.15f && speed != -0.15f && Controller::IsKeyDown(DPadLeft))
		{
			held = true;
			speed -= 0.01f;
		}
		if (!held && in_race && Controller::IsKeyDown(DPadDown))
		{
			held = true;
			speed = 0;
		}
		if (!Controller::IsKeyDown(DPadRight) && !Controller::IsKeyDown(DPadLeft))
		{
			held = false;
		}
	}
	
	void	instantAcceleration(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Instant Acceleration";
		}
		
		if (IsInRace() && Controller::IsKeyDown(A) && GetRacePointer() && Process::Read32(0x140002F4, data) && Process::Read32(data - 0xA4, data) && Process::Read32(data - 0x2C3B0, data))
		{
			Process::Write32(GetRacePointer() + 0xF2C, data);
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Instant Acceleration";
		}
	}
	
	void	instantBackAcceleration(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Instant Backward Acceleration";
		}
	
		float speed;
		if (IsInRace() && Controller::IsKeyDown(B) && GetRacePointer() && Process::Read32(0x140002F4, data) && Process::Read32(data - 0xA4, data) && Process::ReadFloat(data - 0x2C3B0, speed))
		{
			Process::WriteFloat(GetRacePointer() + 0xF2C, -1.f * speed);
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Instant Backward Acceleration";
		}
	}
	
	void	instantStop(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Instant Stop";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Instant Stop";
		}
	
		if (IsInRace() && Controller::IsKeysDown(A + B) && GetRacePointer())
		{
			Process::WriteFloat(GetRacePointer() + 0xF2C, 0);
		}
	}
	
	struct Autom
	{
		const char  *name;
		const u32    id;
	};

	static const std::vector<Autom> g_automs =
	{
		{ "50cc", 0x409A5000 },
		{ "100cc", 0x40C00000 },
		{ "150cc", 0x40F00000 },
		{ "Online", 0x40F526D8 }
	};

	void    AutomSetter(MenuEntry *entry)
    {
        u32 *autom = GetArg<u32>(entry);
		
        static StringVector automs;
		if (automs.empty())
			for (const Autom &i : g_automs)
				automs.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select your automatic speed", automs);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *autom = g_automs[selOpt].id;
    }	
	
	void	autoForward(MenuEntry *entry)
	{
		u32 autom = *GetArg<u32>(entry);
		u32 hit = 0;
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Automatic Kart Acceleration";
		}
		
		if (IsInRace())
		{
			Process::Write32(GetRacePointer() + 0xF2C, autom);
		}
		
		if (IsInRace() && Process::Read32(0x65DA40, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			Process::Read32(offset + 0x4130, offset);
			if (Process::Read32(offset + 0xC30, hit) && hit == 0x00010000)
			{
				Process::Write32(GetRacePointer() + 0xF2C, 0);
			}
			
			if (Controller::IsKeyDown(B))
			{
				Process::Write32(GetRacePointer() + 0xF2C, 0x0);
			}
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Automatic Kart Acceleration";
		}
	}
	
	void	fastReverse(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Fast Reverse Speeds";
			Process::Write32(0x6655A8, 0x41800000);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Fast Reverse Speeds";
			Process::Write32(0x6655A8, 0x40400000);
		}
	}
	
	struct Speed
	{
		const char  *name;
		const u32    id;
	};

	static const std::vector<Speed> g_speeds =
	{
		{ "1cc", 0x00000001 },
		{ "200cc", 0x413C0000 },
		{ "500cc", 0x41A00000 },
		{ "9000cc", 0x42C80000 }
	};

	void    SpeedSetter(MenuEntry *entry)
    {
        u32 *speed = GetArg<u32>(entry);
		
        static StringVector speeds;
		if (speeds.empty())
			for (const Speed &i : g_speeds)
				speeds.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select your forward speed", speeds);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *speed = g_speeds[selOpt].id;
    }	
	
	void	forceSpeed(MenuEntry *entry)
	{
		u32 speed = *GetArg<u32>(entry);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Speed";
		}
		
		if (IsInRace() && Controller::IsKeyDown(A) && GetRacePointer() && Process::Read32(0x140002F4, data) && Process::Read32(0x14000014, data) && Process::Read32(0x14000518, data) && Process::Read32(0x1400001C, data))
		{
			Process::Write32(GetRacePointer() + 0xF2C, speed);
		}
		
		if (IsInRace() && Controller::IsKeysDown(A + B) && GetRacePointer())
		{
			Process::WriteFloat(GetRacePointer() + 0xF2C, 0);
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Configurable Speed";
		}
	}
	
	void	KeySetter(MenuEntry *entry)
	{
		u32 key = 0;
		
		Keyboard keyboard(""<< Color::Red <<"Type a private key");
		keyboard.IsHexadecimal(false);
		if (keyboard.Open(key) != -1)
		{
			Process::Write32(offset + 0x5EBF4C, key);
		}
	}
	
	void	privateMatchmake(MenuEntry *entry)
	{
		u64 titleID = Process::GetTitleID();
		u32 hook0 = *(u32 *)0x5EBF40;
		u32 u0 = AutoRegion(0x29FB50, 0x29FB58, 0x29FB38);
		u32 u1 = AutoRegion(0x2A1474, 0x2A147C, 0x2A145C);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Private Room Matchmaking";
		}

		static const u8 buffer[] =
		{
			0x02, 0x10, 0xA0, 0xE1,
			0x00, 0x20, 0x9F, 0xE5,
			0x1E, 0xFF, 0x2F, 0xE1,
		};
		
		static const u8 buffer_cancel[] =
		{
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
		};
		
		if(hook0 == 0)
		{
			memcpy((void *)0x5EBF40, buffer, 0xC);
		}

		if (titleID == 0x0004000000030700)
		{
			Process::Write32(u0, 0xE12FFF1E);
			Process::Write32(u1, 0xEB0D2AB1);
		}
		
		else if (titleID == 0x0004000000030600)
		{
			Process::Write32(u0, 0xE12FFF1E);
			Process::Write32(u1, 0xEB0D2AAF);
		}

		else if (titleID == 0x0004000000030800)
		{
			Process::Write32(u0, 0xE12FFF1E);
			Process::Write32(u1, 0xEB0D2AB7);
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Private Room Matchmaking";
			memcpy((void *)0x5EBF40, buffer_cancel, 0xC);
			Process::Write32(u0, 0xE92D41F0);
			Process::Write32(u1, 0xE1A01002);
			Process::Write32(offset + 0x5EBF4C, 0);
		}
	}
	
	struct Back
	{
		const char  *name;
		const u32    id;
	};

	static const std::vector<Back> g_backs =
	{
		{ "1cc", 0x000000C1 },
		{ "200cc", 0xC13C0000 },
		{ "500cc", 0xC1A00000 },
		{ "9000cc", 0xC2C80000 }
	};

	void    BackSetter(MenuEntry *entry)
    {
        u32 *back = GetArg<u32>(entry);
		
        static StringVector backs;
		if (backs.empty())
			for (const Back &i : g_backs)
				backs.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select your backward speed", backs);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *back = g_backs[selOpt].id;
    }	
	
	void	forceBack(MenuEntry *entry)
	{
		u32 back = *GetArg<u32>(entry);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Backward Speed";
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Configurable Backward Speed";
		}
	
		if (IsInRace() && Controller::IsKeyDown(B) && GetRacePointer() && Process::Read32(0x140002F4, data) && Process::Read32(0x14000014, data) && Process::Read32(0x14000518, data) && Process::Read32(0x1400001C, data))
		{
			Process::Write32(GetRacePointer() + 0xF2C, back);
		}
		
		if (IsInRace() && Controller::IsKeysDown(A + B) && GetRacePointer())
		{
			Process::WriteFloat(GetRacePointer() + 0xF2C, 0);
		}
	}
	
	void	moonjump(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Moonjump";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Moonjump";
		}
	
		if (IsInRace() && Controller::IsKeysDown(Start + R))
		{
			Process::Write32(GetRacePointer() + 0x3C, 0x41200000);
		}
		
		if (IsInRace() && Controller::IsKeysDown(Start + L))
		{
			Process::Write32(GetRacePointer() + 0x3C, 0xC1400000);
		}
	}
	
	void	freefly(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x444C00, 0x444B7C, 0x444B84);

		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Freefly";
			Process::Write32(u0, 0xE1A00000);
		}
	
		if (Controller::IsKeyDown(R))
		{
			if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				offset += 0xFFFFF930;
				Process::Read32(offset + 0, offset);
				offset += 0xFFFFE2A4;
				Process::Write32(offset + 0, 0x41000000);
			}
		}
		
		if (Controller::IsKeyDown(L))
		{
			if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				offset += 0xFFFFF930;
				Process::Read32(offset + 0, offset);
				offset += 0xFFFFE2A4;
				Process::Write32(offset + 0, 0xC1000000);
			}
		}
		
		if (Controller::IsKeyDown(Y))
		{
			if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				offset += 0xFFFFF930;
				Process::Read32(offset + 0, offset);
				offset += 0xFFFFE2A5;
				Process::Write32(offset + 0, 0x40000000);
			}
		}
		
		if (Controller::IsKeyDown(DPadUp))
		{
			if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				offset += 0xFFFFF930;
				Process::Read32(offset + 0, offset);
				offset += 0xFFFFE2A8;
				Process::Write32(offset + 0, 0xC1000000);
			}
		}
		
		if (Controller::IsKeyDown(DPadDown))
		{
			if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				offset += 0xFFFFF930;
				Process::Read32(offset + 0, offset);
				offset += 0xFFFFE2A8;
				Process::Write32(offset + 0, 0x41000000);
			}
		}
		
		if (Controller::IsKeyDown(DPadLeft))
		{
			if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				offset += 0xFFFFF930;
				Process::Read32(offset + 0, offset);
				offset += 0xFFFFE2A0;
				Process::Write32(offset + 0, 0xC1000000);
			}
		}
		
		if (Controller::IsKeyDown(DPadRight))
		{
			if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				offset += 0xFFFFF930;
				Process::Read32(offset + 0, offset);
				offset += 0xFFFFE2A0;
				Process::Write32(offset + 0, 0x41000000);
			}
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Freefly";
			Process::Write32(u0, 0xE5D01086);
		}
	}
	
	void	stalking(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Stalking Hack";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Stalking Hack";
		}
	
		static u32 player = 1, cpuPointer = 0;
		static bool active = false;
		if (!IsInRace())
		{
			player = 1;
			active = false;
			return;
		}
		if (Controller::IsKeysPressed(Y + DPadRight) && player < (GetTotalPlayers() ? GetTotalPlayers() + 1 : 9))
			player++;
		if (Controller::IsKeysPressed(Y + DPadLeft) && player > 1)
			player--;
		if (Controller::IsKeysPressed(Y + DPadUp))
			active = true;
		if (Controller::IsKeysPressed(Y + DPadDown))
			active = false;
		if (player > (GetTotalPlayers() ? GetTotalPlayers() : 8))
			player = 1;
		if (Controller::IsKeyDown(Y) || active)
		{
			Process::Read32(0x65DA44, cpuPointer);
			Process::Read32(cpuPointer + 0x209C + (player * 0x44), cpuPointer);
			if (cpuPointer != GetRacePointer() && is_in_range(player, 0, (GetTotalPlayers() ? GetTotalPlayers() + 1 : 9)) && cpuPointer && GetRacePointer() && is_in_range(*(u32*)(cpuPointer + 0x24), 0x37000000, 0xC9000000) && is_in_range(*(u32*)(cpuPointer + 0x2C), 0x37000000, 0xC9000000))
			{
				memcpy((void*)(GetRacePointer()), (void*)(cpuPointer), 0x30);
				Process::WriteFloat(GetRacePointer() + 0x28, *(float*)(cpuPointer + 0x28) + 40);
				return;
			}
			player++;
		}
	}
	
	void	driveAnywhere(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Drive Anywhere";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Drive Anywhere";
		}
	
		static bool enabled = false;
		if (!IsInRace())
		{
			enabled = false;
			Process::Write32(0x6656D8, 0x14);
			return;
		}
		else
		{
			if (Controller::IsKeysDown(Start + DPadLeft))
			{
				enabled = false;
				Process::Write32(0x6656D8, 0x14);
				return;
			}
			if (!enabled && Controller::IsKeysDown(Start + DPadRight))
			{
				enabled = true;
				Process::Write32(0x6656D8, 0xFFFF);
			}
			if (enabled)
			{
				if (Controller::IsKeysDown(R + Y))
				{
					Process::Write32(GetRacePointer() + 0x3C, 0x41000000);
					return;
				}
				if (Controller::IsKeysDown(L + Y))
				{
					Process::Write32(GetRacePointer() + 0x3C, 0xC1000000);
					return;
				}
				Process::Write32(GetRacePointer() + 0x3C, 0);
			}
		}
	}
	
	void	aimbot(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Aimbot";
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Aimbot";
		}
	
		u32 g_racePointer = GetRacePointer(), player_pointer = 0;
		static u32 playerID = 0;
		static const int x = 0, y = 1, z = 2, user = 0, player = 1, difference = 2;
		bool in_race = IsInRace();
		static bool held = false, active = false;
		float coordinates[3][3], temp[3];
		if (!in_race)
		{
			active = playerID = 0;
			return;
		}
		else
		{
			if (!held && Controller::IsKeyDown(DPadUp))
			{
				held = true;
				active = true;
				playerID++;
			}
			if (!Controller::IsKeyDown(DPadUp))
				held = false;
			if (Controller::IsKeyDown(DPadDown))
			{
				active = false;
				playerID = 0;
			}
			if (!active)
				return;
			Process::Read32(0x65DA44, player_pointer);
			Process::Read32(0x209C + player_pointer + (playerID * 0x44), player_pointer); // player pointer
			if (player_pointer == g_racePointer || !is_in_range(player_pointer, 0x16000000, 0x18000000))
			{
				playerID++;
				return;
			}
			if ((GetTotalPlayers() != 0 && playerID > GetTotalPlayers()) || (GetTotalPlayers() == 0 && playerID > 7))
				playerID = 0;
			for (int i = 0; i < 3; i++)
			{
				Process::ReadFloat(player_pointer + (i * 4) + 0x24, coordinates[player][i]);
				Process::ReadFloat(g_racePointer + (i * 4) + 0x24, coordinates[user][i]);
				coordinates[difference][i] = coordinates[player][i] - coordinates[user][i];
			}
			temp[x] = sqrtf((coordinates[difference][x] * coordinates[difference][x]) + (coordinates[difference][z] * coordinates[difference][z])); // stores XZ hypotenuse
			temp[y] = sqrtf((coordinates[difference][x] * coordinates[difference][x]) + (coordinates[difference][y] * coordinates[difference][y])); // stores XY hypotenuse
			Process::WriteFloat(g_racePointer + 0x18, coordinates[difference][x] / temp[x]); // works fine
			Process::WriteFloat(g_racePointer + 0x20, coordinates[difference][z] / temp[x]); // works fine
		}
	}
	
	void	blueShellRide(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Blue Shell Ride";
		}
	
		u32 dataX = 0, dataY = 0, dataZ = 0, g_racePointer = GetRacePointer();
		if (IsInRace() && Controller::IsKeyDown(DPadLeft))
		{
			if (Process::Read32(0xFFFFBF4, offset) && is_in_range(offset, 0x14000000, 0x18000000) && Process::Read32(offset - 0x63C, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				Process::Read32(offset - 0x3CB0, dataX);
				Process::Read32(offset - 0x3CAC, dataY);
				Process::Read32(offset - 0x3CA8, dataZ);
				if (is_in_range(dataX, 0x10000, 0xD0000000) && is_in_range(dataY, 0x10000, 0xD0000000) && is_in_range(dataZ, 0x10000, 0xD0000000))
				{
					Process::Write32(g_racePointer + 0x24, dataX);
					Process::Write32(g_racePointer + 0x28, dataY);
					Process::Write32(g_racePointer + 0x2C, dataZ);
				}
			}
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Blue Shell Ride";
		}
	}
	
	struct Gravity
	{
		const char  *name;
		const u32   id;
	};

	static const std::vector<Gravity> g_gravities =
	{
		{ "Weak Gravity", 0x3F660FFF },
		{ "Default Gravity", 0x3F7C28F6 },
		{ "Strong Gravity", 0x3F830000 }
	};

	void    GravitySetter(MenuEntry *entry)
    {
        u32 *gravity = GetArg<u32>(entry);
		
        static StringVector gravities;
		if (gravities.empty())
			for (const Gravity &i : g_gravities)
				gravities.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select a gravity", gravities);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *gravity = g_gravities[selOpt].id;
    }
	
	void	gravityForce(MenuEntry *entry)
	{
		u32 gravity = *GetArg<u32>(entry);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Gravity";
		}
		
		if (IsInRace())
		{
			Process::Write32(0x665388, gravity);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Configurable Gravity";
			Process::Write32(0x665388, 0x3F7C28F6);
		}
	}

	void	touchCode(MenuEntry *entry)
	{
		u32 map = 0;
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Touch Screen Kart Teleporter";
		}
		
		if (IsInRace() && Process::Read32(0x65C528, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			if (Process::Read32(offset + 0xF4, map) && map == 0x102)
			{
				Process::Read32(0x6673C8, offset);
				Process::Read32(offset + 0x1900, offset);
				Process::Write32(offset + 0x248, 1);
				Process::Write32(offset + 0x2C4, 1);
			}
		}
	
		u32 g_racePointer = GetRacePointer(), map_pointer = 0;
		bool in_race = IsInRace();
		if (Process::Read32(0x6789C8, data) && Process::Read32(data + 0x1A48, data) && data == 1 && in_race && is_in_range(g_racePointer, 0x14000000, 0x18000000))
		{
			unsigned int touch_pointer = *(unsigned int *)0x6789C8;
			float X_Axis = *(float *)(touch_pointer + 0x1BCC);
			float Y_Axis = *(float *)(touch_pointer + 0x1BD0);
			if (Process::Read32(0x140002F4, map_pointer) && Process::Read32(map_pointer + 0x14, map_pointer) && Process::Read32(map_pointer + 0x370, map_pointer) && Process::Read32(map_pointer + 0x270, map_pointer) && Process::Read32(map_pointer + 0x60, map_pointer) && Process::Read32(map_pointer + 0x688, map_pointer))
			{
				float MapStatic1 = *(float *)map_pointer;
				float Map_Scale1 = *(float *)(map_pointer + 4);
				float MapStatic2 = *(float *)(map_pointer + 8);
				float Map_Scale2 = *(float *)(map_pointer + 12);
				Map_Scale1 -= Map_Scale2;
				Map_Scale1 /= 240.f;
				X_Axis *= Map_Scale1;
				Y_Axis *= Map_Scale1;
				X_Axis -= ((320.f * Map_Scale1) - MapStatic2);
				Y_Axis += Map_Scale2;
				*(float *)(g_racePointer + 0x3C) = 0;
				*(float *)(g_racePointer + 0x24) = X_Axis;
				*(float *)(g_racePointer + 0x2C) = Y_Axis;
			}
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Touch Screen Kart Teleporter";
		}
	}
	
	void	saveSlotTeleporter(MenuEntry *entry)
	{	
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Save Slot Teleporter";
		}

		static u32 x = 0, y = 0, z = 0;
		if (IsInRace() && offset)
		{
			if (Controller::IsKeysDown(Start + X))
			{
				Process::Read32(GetRacePointer() + 0x24, x);
				Process::Read32(GetRacePointer() + 0x28, y);
				Process::Read32(GetRacePointer() + 0x2C, z);
			}
			if (Controller::IsKeysDown(Start + Y))
			{
				Process::Write32(GetRacePointer() + 0x24, x);
				Process::Write32(GetRacePointer() + 0x28, y);
				Process::Write32(GetRacePointer() + 0x2C, z);
			}
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Save Slot Teleporter";
		}
	}
	
	
	
	
	
	
	
	
	
	
	// Item Codes
	
	void	itemWheelDeluxe(MenuEntry *entry)
	{
		static const u8 itemhere[18] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 17, 18, 19};
		static int item = -1;
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Item Cycler";
		}

		if(Controller::IsKeyPressed(DPadRight))
		{
			if(item >= 18)
				item = 0;
			item++;
			writeItem(itemhere[item]);
			//OSD::Notify(Utils::Format("Item ID: 0x%01X", itemhere[item]));
		}

		if(Controller::IsKeyPressed(DPadLeft))
		{
			if(item <= 0)
				item = 18;
			item--;
			writeItem(itemhere[item]);
			//OSD::Notify(Utils::Format("Item ID: 0x%01X", itemhere[item]));
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Item Cycler";
		}
	}
	
	void	randomShell(MenuEntry *entry)
	{
		u8 list[2] = {1, 2};
		u8	random_shell = list[rand() % (sizeof(list) / sizeof(list[0]))];
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Single Shell Randomizer";
		}
		
		if (IsInRace())
		{
			writeItem(random_shell);
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Single Shell Randomizer";
		}
	}
	
	void	masterItemHack(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x444C00, 0x444B7C, 0x444B84);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Classic Item Hack";
			Process::Write32(u0, 0xE1A00000);
		}
		
		if (Controller::IsKeyDown(DPadLeft))
		{
			writeItem(14);//Lucky-7
		}
		if (Controller::IsKeyDown(DPadRight))
		{
			writeItem(2);//Single Red
		}
		
		if (Controller::IsKeyDown(DPadUp))
		{
			writeItem(1);//Single Green
		}
		
		if (Controller::IsKeyDown(DPadDown))
		{
			writeItem(4);//Bob-Omb
		}
		
		if (Controller::IsKeysDown(A+B))
		{
			writeItem(9);//Bullet
		}
		
		if (Controller::IsKeyDown(Y))
		{
			writeItem(6);//Blue
		}
		
		if (Controller::IsKeyDown(Start))
		{
			writeItem(10);//Lightning
		}

		if(!entry->IsActivated())
		{
			entry->Name() = "Classic Item Hack";
			Process::Write32(u0, 0xE5D01086);
		}
	}
	
	struct Box
	{
		const char  *name;
		const u8    id;
	};

	static const std::vector<Box> g_boxes =
	{
		{ "Banana", 0 },
		{ "Green Shell", 1 },
		{ "Red Shell", 2 },
		{ "Mushroom", 3 },
		{ "Bob-Omb", 4 },
		{ "Blooper", 5 },
		{ "Blue Shell", 6 },
		{ "Triple Mushroom", 7 },
		{ "Star", 8 },
		{ "Bullet Bill", 9 },
		{ "Lightning", 0xA },
		{ "Golden Mushroom", 0xB },
		{ "Fire Flower", 0xC },
		{ "Super Leaf", 0xD },
		{ "Triple Bananas", 0x11 }
	};

	void    ItemBoxSetter(MenuEntry *entry)
    {
        u8 *box = GetArg<u8>(entry);
		
        static StringVector boxes;
		if (boxes.empty())
			for (const Box &i : g_boxes)
				boxes.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select which Item Box you want", boxes);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *box = g_boxes[selOpt].id;
    }
	
	void	forceBox(MenuEntry *entry)
	{
		u8 box = *GetArg<u8>(entry);
		u32 u0 = AutoRegion(0x2CABE4, 0x2CABEC, 0x2CABC4);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Item Box";
		}
		
		if (IsInRace())
		{
			Process::Write32(u0, 0xE3A08000 + box);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Configurable Item Box";
			Process::Write32(u0, 0xE0855000);
		}
	}
	
	struct Item
	{
		const char  *name;
		const u8    id;
	};

	static const std::vector<Item> g_items =
	{
		{ "Banana", 0 },
		{ "Green Shell", 1 },
		{ "Red Shell", 2 },
		{ "Mushroom", 3 },
		{ "Bob-Omb", 4 },
		{ "Blooper", 5 },
		{ "Blue Shell", 6 },
		{ "Triple Mushroom", 7 },
		{ "Star", 8 },
		{ "Bullet Bill", 9 },
		{ "Lightning", 0xA },
		{ "Golden Mushroom", 0xB },
		{ "Fire Flower", 0xC },
		{ "Super Leaf", 0xD },
		{ "Lucky-7", 0xE },
		{ "Triple Bananas", 0x11 },
		{ "Triple Green Shells", 0x12 },
		{ "Triple Red Shells", 0x13 }
	};

	void    ItemSetter(MenuEntry *entry)
    {
        u8 *item = GetArg<u8>(entry);
		
        static StringVector items;
		if (items.empty())
			for (const Item &i : g_items)
				items.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select your item", items);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *item = g_items[selOpt].id;
    }
	
	void	forceItem(MenuEntry *entry)
	{
		u8 item = *GetArg<u8>(entry);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Item";
		}
		
		if (IsInRace())
		{
			writeItem(item);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Configurable Item";
		}
	}
	
	void    randomItems(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Cycle Completely Random Item";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Cycle Completely Random Item";
		}
	
		u32 number = Utils::Random(0, 0x14);
		writeItem(number == 0xF || number == 0x10 ? Utils::Random(0, 0xE) : number);
	}
	
	void    trulyRandomItems(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Truly Random Items";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Truly Random Items";
		}
	
		u32 randNumber = Utils::Random(0, 0x14), g_itemPointer = GetItemPointer(), tmp = 0, tmp2 = 0;
		u16 tmp3 = 0;
		bool in_race = IsInRace(), alreadyGivenItem = false;
		if (randNumber == 0xF || randNumber == 0x10)
			randNumber = Utils::Random(0, 0xE);
		if (!alreadyGivenItem && in_race && is_in_range(g_itemPointer, 0x14000000, 0x18000000) && Process::Read32(g_itemPointer + 0x3C, tmp) && is_in_range(tmp, 0, 4) && Process::Read32(g_itemPointer + 0xC8, tmp2) && tmp2 != 0xFFFFFFFF && Process::Read32(0x14000074, tmp2) && is_in_range(tmp2, 0x14000000, 0x18000000) && Process::Read32(tmp2 - 0x1B5C, tmp2) && is_in_range(tmp2, 0x14000000, 0x18000000) && Process::Read16(tmp2 + 0x882, tmp3) && tmp3 == 1)
		{
			if ((randNumber == 7 || randNumber == 0x11 || randNumber == 0x12 || randNumber == 0x13) && tmp == 1)
				Process::Write32(g_itemPointer + 0x3C, 3);
			else if ((randNumber != 7 && randNumber != 0x11 && randNumber != 0x12 && randNumber != 0x13) && tmp == 3)
				Process::Write32(g_itemPointer + 0x3C, 1);
			Process::Write32(g_itemPointer + 0xA8, 3);
			Process::Write32(g_itemPointer + 0xC8, randNumber);
			Process::Write32(g_itemPointer + 0xD8, 0x3F800000);
			alreadyGivenItem = true;
		}
		if (tmp3 == 0)
			alreadyGivenItem = false;
	}
	
	void	popItems(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2BE2EC, 0x2BE2F4, 0x2BE2CC);
		u32 u1 = AutoRegion(0x2D3F20, 0x2D3F28, 0x2D3F00);
		u32 u2 = AutoRegion(0x2F958C, 0x2F9594, 0x2F956C);
		u32 u3 = AutoRegion(0x2F9590, 0x2F9598, 0x2F9570);
		
		if(entry->WasJustActivated())//add activator!
		{
			entry->Name() = Color::LimeGreen << "Pop Items";
		}
		
		Process::Write32(u0, 0xE1A00000);
		Process::Write32(u1, 0x1A000002);
		Process::Write32(u2, 0xE1A00000);
		Process::Write32(u3, 0xE1A00000);

		if (Controller::IsKeyDown(X))
		{
			Process::Write32(u1, 0xEA000038);
			Process::Write32(u2, 0xE1A00004);
			Process::Write32(u3, 0xEBFF6A5E);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Pop Items";
			Process::Write32(u0, 0x03100008);
			Process::Write32(u1, 0x1A000002);
			Process::Write32(u2, 0xE3100201);
			Process::Write32(u3, 0x1A00014D);
		}
	}
	
	void	dropItems(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2B4D7C, 0x2B4D84, 0x2B4D5C);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Drop Items";
			Process::Write32(u0, 0xE5C4B1AE);
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Drop Items";
			Process::Write32(u0, 0xE5C4B05E);
		}
	}
	
	void	dropLuckyShroom(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2CF684, 0x2CF68C, 0x2CF664);
	
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Drop Lucky-7 Mushroom";
			Process::Write32(u0, 0xE1A00000);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Drop Lucky-7 Mushroom";
			Process::Write32(u0, 0xE3510003);
		}
	}
	
	void	dropLuckyStar(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2CF694, 0x2CF69C, 0x2CF674);
	
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Drop Lucky-7 Star";
			Process::Write32(u0, 0xE1A00000);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Drop Lucky-7 Star";
			Process::Write32(u0, 0xE3510004);
		}
	}
	
	void	dropLuckyBlooper(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2CF69C, 0x2CF6A4, 0x2CF67C);
	
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Drop Lucky-7 Blooper";
			Process::Write32(u0, 0xE1A00000);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Drop Lucky-7 Blooper";
			Process::Write32(u0, 0xE351000A);
		}
	}
	
	void	dropMushroom(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Drop Mushroom";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Drop Mushroom";
		}
	
		if (IsInRace() && GetOldPointer5D0())
		{
			Process::Read32(GetOldPointer5D0() + 0x1F8, data);
			Process::Write32(GetOldPointer5D0() + 0x1D0, data);
		}
	}
	
	void	starLimit(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2CECE8, 0x2CECF0, 0x2CECC8);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Extended Star Limit";
			Process::Write32(u0, 0xE3A00032);
        }
        
        if (!entry->IsActivated())
		{
            entry->Name() = "Extended Star Limit";
            Process::Write32(u0, 0xEBFFFADE);
        }
    }
	
	void	shroomLimit(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2D0C78, 0x2D0C80, 0x2D0C58);
		u32 u1 = AutoRegion(0x2D0C84, 0x2D0C8C, 0x2D0C64);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Extended Mushroom Limit";
			Process::Write32(u0, 0xE3A00032);
			Process::Write32(u1, 0xE3A00032);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Extended Mushroom Limit";
			Process::Write32(u0, 0xEBFFF2FA);
			Process::Write32(u1, 0xEBFFF2C1);
		}
    }

	void	bombLimit(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2D08B0, 0x2D08B8, 0x2D0890);
		u32 u1 = AutoRegion(0x2D08BC, 0x2D08C4, 0x2D089C);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Extended Bob-Omb Limit";
			Process::Write32(u0, 0xE3A00032);
			Process::Write32(u1, 0xE3A00032);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Extended Bob-Omb Limit";
			Process::Write32(u0, 0xEBFFF3EC);
			Process::Write32(u1, 0xEBFFF3B3);
		}
    }
	
	void	bananaLimit(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2D06B4, 0x2D06BC, 0x2D0694);
		u32 u1 = AutoRegion(0x2D06C0, 0x2D06C8, 0x2D06A0);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Extended Banana Limit";
			Process::Write32(u0, 0xE3A00032);
			Process::Write32(u1, 0xE3A00032);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Extended Banana Limit";
			Process::Write32(u0, 0xEBFFF46B);
			Process::Write32(u1, 0xEBFFF432);
		}
    }
	
	void	fireballLimit(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2D0AA8, 0x2D0AB0, 0x2D0A88);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Extended Fireball Limit";
			Process::Write32(u0, 0xE3A00032);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Extended Fireball Limit";
			Process::Write32(u0, 0xEBFFF36E);
        }
    }
	
	void	blooperLimit(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2CF188, 0x2CF190, 0x2CF168);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Extended Blooper Limit";
			Process::Write32(u0, 0xE3A00032);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Extended Blooper Limit";
			Process::Write32(u0, 0xEBFFF9B6);
        }
    }
	
	void	killerSpeed(MenuEntry *entry)
	{	
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Bullet Speed Modifier";
		}
		
		if (Controller::IsKeyDown(X))
		{
			Process::Write32(0x6655A4, 0x41C00000);
		}
		if (Controller::IsKeyDown(B))
		{
			Process::Write32(0x6655A4, 0xFF7FFFFF);
		}
		if (Controller::IsKeyDown(A))
		{
			Process::Write32(0x6655A4, 0x40F33333);
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Bullet Speed Modifier";
		}
	}
	
	void	fastBlueShell(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Fast Blue Shell";
			Process::Write32(0x666094, 0x43000000);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Fast Blue Shell";
			Process::Write32(0x666094, 0x41A00000);
		}
	}
	
	void	stoppedBlueShell(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Stopped Blue Shell";
			WasShown = false;
			Process::Write32(0x666094, 0);
		}
		
		if (GetNetwork() == 0x1)
		{
			entry->Disable();
			if(!WasShown)
			{
				OSD::Notify("Stopped Blue Shell is disabled online!", Color::Red);
			}
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Stopped Blue Shell";
			WasShown = true;
			Process::Write32(0x666094, 0x41A00000);
		}
	}
	
	void	blueShellInstantHit(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Instant Blue Shell Hit";
			Process::Write32(0x6660C8, 0x00000009);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Instant Blue Shell Hit";
			Process::Write32(0x6660C8, 0x00000036);
		}
	}
	
	void	fastGreenShell(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Fast Green Shell";
			Process::Write32(0x66619C, 0x43000000);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Fast Green Shell";
			Process::Write32(0x66619C, 0x41300000);
		}
	}
	
	void	stoppedGreenShell(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Stopped Green Shell";
			Process::Write32(0x66619C, 0);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Stopped Green Shell";
			Process::Write32(0x66619C, 0x41300000);
		}
	}
	
	void	giantBanana(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Giant Banana";
			Process::Write32(0x665CBC, 0x41000000);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Giant Banana";
			Process::Write32(0x665CBC, 0x40000000);
		}
	}
	
	struct Size // Global item sizes settings
	{
		const char  *name;
		const u32   id;
	};

	static const std::vector<Size> g_sizes =
	{
		{ "Micro Size", 0x20000000 },
		{ "Default Size", 0x40000000 },
		{ "Giant Size", 0x40A00000 }
	};

	void	SizeSetter(MenuEntry *entry) // Global item sizes settings
    {
        u32 *size = GetArg<u32>(entry);
		
        static StringVector sizes;
		if (sizes.empty())
			for (const Size &i : g_sizes)
				sizes.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select a size", sizes);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *size = g_sizes[selOpt].id;
    }
	
	void	bananaSize(MenuEntry *entry)
	{
		u32 size = *GetArg<u32>(entry);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Banana Size";
		}
		
		if (IsInRace())
		{
			Process::Write32(0x665CBC, size);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Configurable Banana Size";
			Process::Write32(0x665CBC, 0x40000000);
		}
	}
	
	void	fireballSize(MenuEntry *entry)
	{
		u32 size = *GetArg<u32>(entry);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Fireball Size";
		}
		
		if (IsInRace())
		{
			Process::Write32(0x665F0C, size);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Configurable Fireball Size";
			Process::Write32(0x665F0C, 0x40000000);
		}
	}
	
	void	bombSize(MenuEntry *entry)
	{
		u32 size = *GetArg<u32>(entry);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Bob-Omb Size";
		}
		
		if (IsInRace())
		{
			Process::Write32(0x665E20, size);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Configurable Bob-Omb Size";
			Process::Write32(0x665E20, 0x40000000);
		}
	}
	
	void	rotatingBananas(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2C0EF4, 0x2C0EFC, 0x2C0ED4);
		u32 u1 = AutoRegion(0x2C0EFC, 0x2C0F04, 0x2C0EDC);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Triple Rotating Bananas";
			Process::Write32(u0, 0xE3A00000);
			Process::Write32(u1, 0xE3A00000);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Triple Rotating Bananas";
			Process::Write32(u0, 0xE59010EC);
			Process::Write32(u1, 0xE3510001);
		}
    }
	
	void	invisibleLightning(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x3DC5B8, 0x3DC5C0, 0x3DC598);
		u32 u1 = AutoRegion(0x3E1C40, 0x3E1C48, 0x3E1C20);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Invisible Lightning";
			Process::Write32(u0, 0xEA000000);
			Process::Write32(u1, 0xEA000007);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Invisible Lightning";
			Process::Write32(u0, 0x0A000000);
			Process::Write32(u1, 0x0A000007);
		}
    }
	
	struct Lightning
	{
		const char  *name;
		const u8    id;
	};

	static const std::vector<Lightning> g_lightnings =
	{
		{ "Dark Cyan", 0 },
		{ "Bright Cyan", 1 },
		{ "Marine Blue", 2 },
		{ "Invisible", 8 },
		{ "Default", 0xF },
		{ "Dark Yellow", 0x13 },
		{ "Pink", 0x18 },
		{ "Lime Green", 0x19 },
		{ "Dark Red", 0x31 },
		{ "Bright Red", 0x38 },
	};

	void    LightningColorSetter(MenuEntry *entry)
    {
        u8 *color = GetArg<u8>(entry);
		
        static StringVector lightnings;
		if (lightnings.empty())
			for (const Lightning &i : g_lightnings)
				lightnings.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select which Lightning Color you want", lightnings);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *color = g_lightnings[selOpt].id;
    }
	
	void	lightningColor(MenuEntry *entry)
	{
		u8 lightning = *GetArg<u8>(entry);
	
		u32 u0 = AutoRegion(0x429304, 0x42930C, 0x4292E4);
		
		if(entry->IsActivated())
		{
			entry->Name() = Color::LimeGreen << "Lightning Color Selector";
			Process::Write32(u0, 0xE88C0000 + lightning);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Lightning Color Selector";
			Process::Write32(u0, 0xE88C000F);
		}
	}
	
	void	lightningSelfstriking(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2BA5E8, 0x2BA5F0, 0x2BA5C8);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Lightning Selfstriking";
			Process::Write32(u0, 0xE3A02001);
		}

        if (!entry->IsActivated())
		{
			entry->Name() = "Lightning Selfstriking";
			Process::Write32(u0, 0xE3A02000);
        }
    }
	
	void	blooperSelfstriking(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2BA4D0, 0x2BA4D8, 0x2BA4B0);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Blooper Selfstriking";
			Process::Write32(u0, 0xE3A02000);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Blooper Selfstriking";
			Process::Write32(u0, 0xE3A02001);
		}
    }
	
	void	noItemClear(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2D2550, 0x2D2558, 0x2D2530);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Never Lose Items";
			Process::Write32(u0, 0xE12FFF1E);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Never Lose Items";
			Process::Write32(u0, 0xE92D41F0);
		}
	}
	
	void	stuckedLucky(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "No Lucky-7 Rotation";
			Process::Write32(0x665DBC, 0x00000000);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "No Lucky-7 Rotation";
			Process::Write32(0x665DBC, 0x3D4CCCCD);
		}
	}
	
	void	stuckedLuckyItem(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "No Lucky-7 Items Rotation";
			Process::Write32(0x665DC0, 0x00000000);
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "No Lucky-7 Items Rotation";
			Process::Write32(0x665DC0, 0x41000000);
		}
	}
	
	void	noBombJump(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "No Bob-Omb Jump";
			Process::Write32(0x665E40, 0x3D000000);
		}

		if (!entry->IsActivated())
		{
			entry->Name() = "No Bob-Omb Jump";
			Process::Write32(0x665E40, 0x3F000000);
		}
	}
	
	void	jumpingBomb(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Jumping Bob-Omb";
			Process::Write32(0x665E40, 0x2F000000);
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Jumping Bob-Omb";
			Process::Write32(0x665E40, 0x3F000000);
		}
	}
	
	void	noRotation(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "No Shell Rotation";
			Process::Write32(0x666170, 0x00000000);
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "No Shell Rotation";
			Process::Write32(0x666170, 0x41700000);
		}
	}
	
	void	flyingStar(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Flying Dropped Star";
			Process::Write32(0x6662BC, 0x42600000);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Flying Dropped Star";
			Process::Write32(0x6662BC, 0x40600000);
		}
	}
	
	void	inflatableStar(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Inflatable Dropped Star";
			Process::Write32(0x6662C0, 0x41100000);
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Inflatable Dropped Star";
			Process::Write32(0x6662C0, 0x40000000);
		}
	}
	
	
	
	
	
	
	
	
	
	
	// Race codes
	
	void	NoDC(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "No Disconnect";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "No Disconnect";
		}
	
		if (IsInRace() && Process::Read32(0x65C528, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			Process::Read32(offset + 0xC8, offset);
			Process::Write32(offset + 0x34, 0);
			Process::Write32(offset + 0x38, 0);
		}
	}
	
	bool TimeOSD(const Screen &screen)
	{
		if(!screen.IsTop)
			return false;
		screen.Draw(Utils::Format("Time: %01d:%02d", GetMinutes(), GetSeconds()), 10, 223, Color::Yellow);
		return true;
	}

	void	TimeIndicator(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Race Time Displayer";
		}
		
		if (IsInRace())
		{
			OSD::Run(TimeOSD);
		}
		else OSD::Stop(TimeOSD);
        
		if(!entry->IsActivated())
		{
			entry->Name() = "Race Time Displayer";
			OSD::Stop(TimeOSD);
		}
	}
	
	void	removeLakitu(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x3C5F38, 0x3C5F40, 0x3C5F18);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Remove Lakitu";
			Process::Write32(u0, 0xE1A00000);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Remove Lakitu";
			Process::Write32(u0, 0xE3510000);
		}
    }
	
	void	driveDuringCountdown(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2FA800, 0x2FA808, 0x2FA7E0);
        u32 u1 = AutoRegion(0x2BCAC0, 0x2BCAC8, 0x2BCAA0);
		
        if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Drive During Countdown";
			Process::Write32(u0, 0xE1A00000);
			Process::Write32(u1, 0xE1A00000);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Drive During Countdown";
			Process::Write32(u0, 0xE3500002);
			Process::Write32(u1, 0x9A000023);
		}
	}
	
	void	noCountdown(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "No Countdown";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "No Countdown";
		}
	
		if (IsInRace() && Process::Read32(0x65C528, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			Process::Write8(offset + 0x109, 1);
			Process::Write8(offset + 0x589, 1);
		}
	}
	
	void	instantCountdown(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Instant Countdown";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Instant Countdown";
		}
	
		if (IsInRace() && Process::Read32(0x65C528, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			Process::Write8(offset + 0x589, 1);
		}
	}
	
	void	cpuBrawl(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "CPU Brawl";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "CPU Brawl";
		}
	
		u32 pointer2 = 0, temp = 0;
		if (!IsInRace())
			return;
		for (int i = 2; i < 9; i++)
		{
			Process::Read32(0x65DA44, temp);
			Process::Read32(0x209C + temp + (i * 0x44), offset);
			Process::Read32(0x209C + temp + ((i + 1) * 0x44), pointer2);
			if (is_in_range(offset, 0x14000000, 0x18000000) && is_in_range(pointer2, 0x14000000, 0x18000000))
				memcpy((void *)(pointer2 + 0x20), (void*)(offset + 0x20), 24);
		}
	}
	
	void	cpuPropeller(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x421410, 0x421418, 0x4213F0);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "CPU Propeller Spawn Controller";
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "CPU Propeller Spawn Controller";
		}
		
		if (Controller::IsKeysDown(DPadRight))
		{
			Process::Write32(u0, 0xE1A00000);
        }
		if (Controller::IsKeysDown(DPadLeft))
		{
			Process::Write32(u0, 0xE3500000);
        }
    }
	
	void	waterEverywhere(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Water Everywhere";
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Water Everywhere";
		}
	
		if (IsInRace() && Process::Read32(0x663954, offset) && is_in_range(offset, 0x14000000, 0x18000000) && Process::Read32(offset + 0x58, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			Process::Write32(offset + 0x420, 0x48000000);
		}
	}
	
	void	SetPoints(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Rank Points Modifier";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Rank Points Modifier";
		}

		Keyboard keyboard(""<< Color::Red <<"Set your points:");
		keyboard.IsHexadecimal(false);
		if (keyboard.Open(data) != -1)
		{
			writePoint(data);
		}
	}
	
	void	randomPoints(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Random Rank Points";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Random Rank Points";
		}
		
		writePoint(Utils::Random(0, 999));
	}
	
	void	instantWin(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Instant Win";
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Instant Win";
		}
		
		if (IsInRace() && Process::Read32(0x65C528, offset) && is_in_range(offset, 0x14000000, 0x18000000) && Controller::IsKeysDown(Start + DPadDown))
		{
			Process::Write32(offset + 0xBBC, 9);
		}
	}
	
	
	
	
	
	
	
	
	
	
	// Battle codes
	
	void	battleCoin (MenuEntry *entry)
	{
		static const u8 pointlist[100] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99};
		static int coin = -1;
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Coin Score Modifier";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Coin Score Modifier";
		}
		
		if (Controller::IsKeysDown(DPadRight + R))
		{
			if(coin >= 100)
				coin = 0;
			coin++;
			writeCoinScore(pointlist[coin]);
		}
		
		if (Controller::IsKeysDown(DPadLeft + R))
		{
			if(coin <= 0)
				coin = 100;
			coin--;
			writeCoinScore(pointlist[coin]);
		}
		
		if (!entry->IsActivated())
		{
			writeCoinScore(0);
		}
	}
	
	void	battleBalloon (MenuEntry *entry)
	{
		static const u8 pointlist[100] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99};
		static int balloon = -1;
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Balloon Score Modifier";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Balloon Score Modifier";
		}
		
		if (Controller::IsKeysDown(DPadRight + R))
		{
			if(balloon >= 100)
				balloon = 0;
			balloon++;
			writeBalloonScore(pointlist[balloon]);
		}
		
		if (Controller::IsKeysDown(DPadLeft + R))
		{
			if(balloon <= 0)
				balloon = 100;
			balloon--;
			writeBalloonScore(pointlist[balloon]);
		}
		
		if (!entry->IsActivated())
		{
			writeBalloonScore(0);
		}
	}
	
	void	randCoins (MenuEntry *entry)
	{
		u32 randNumber = Utils::Random(0, 0x63);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Coin Score Randomizer";
		}
		
		if (IsInRace)
		{
			writeCoinScore(randNumber);
		}

		if (!entry->IsActivated())
		{
			entry->Name() = "Coin Score Randomizer";
			writeCoinScore(0);
		}
	}
	
	void	randBalloons (MenuEntry *entry)
	{
		u32 randNumber = Utils::Random(0, 0x63);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Balloon Score Randomizer";
		}
		
		if (IsInRace)
		{
			writeBalloonScore(randNumber);
		}

		if (!entry->IsActivated())
		{
			entry->Name() = "Balloon Score Randomizer";
			writeBalloonScore(0);
		}
	}
	
	void	coinLimit (MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x4163BC, 0x4163C4, 0x41639C);
		u32 u1 = AutoRegion(0x4163D0, 0x4163D8, 0x4163B0);

		if (entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Coins Limit Extender";
			Process::Write32(u0, 0xE3560063);
			Process::Write32(u1, 0xE2663063);
		}
		
		if (!entry->IsActivated())
		{
			writeCoinScore(0);
			Process::Write32(u0, 0xE356000A);
			Process::Write32(u1, 0xE266300A);
		}
	}

	void	maxTimer(MenuEntry *entry)
	{
        if(entry->WasJustActivated())
		{
          entry->Name() = Color::LimeGreen << "Max Timer";
          WasShown = false;
        }
    
        if (!entry->IsActivated())
		{
          entry->Name() = "Max Timer";
          WasShown = true;
        }
        
        if (GetNetwork() == 0x1)
		{
			entry->Disable();
			if(!WasShown)
			{
				OSD::Notify("Max Timer is disabled online!", Color::Red);
			}
		}
    
        if (IsInRace() && Process::Read32(0x65C528, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			Process::Write32(offset + 0x80, 0xFFFFFFFF);
		}
    }

	void	minTimer(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Min Timer";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Min Timer";
		}
	
		if (IsInRace() && Process::Read32(0x65C528, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			Process::Write32(offset + 0x80, 0);
		}
	}
	
	struct Blink
	{
		const char  *name;
		const u32   id;
	};

	static const std::vector<Blink> g_blinks =
	{
		{ "Disable Blinking", 0 },
		{ "Default Blinking", 0x000000B4 },
		{ "Constant Blinking", 0x7FFFFFFF },
	};

	void    BlinkSetter(MenuEntry *entry)
    {
        u32 *blink = GetArg<u32>(entry);
		
        static StringVector blinks;
		if (blinks.empty())
			for (const Blink &i : g_blinks)
				blinks.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select a blink effect", blinks);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *blink = g_blinks[selOpt].id;
    }
	
	void	blinkingEffect(MenuEntry *entry)
	{
		u32 blink = *GetArg<u32>(entry);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Blinking Effect";
		}
		
		if (IsInRace())
		{
			Process::Write32(0x665734, blink);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Configurable Blinking Effect";
			Process::Write32(0x665734, 0x000000B4);
		}
	}
	
	void	antiKouraB(MenuEntry *entry)
	{
		u32 item = 0;
		u32 u0 = AutoRegion(0x2C7B94, 0x2C7B9C, 0x2C7B74);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Anti Blue Shell Crash";
        }
		
		if (IsInRace() && Process::Read32(0x667140, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			Process::Read32(offset + 0x3C4, offset);
			offset += 0xFFFFFF10;
			Process::Read32(offset + 0, offset);
			Process::Read32(offset + 0x94, offset);
			Process::Write32(offset + 0x240, 0);
		}
		
		if (GetNetwork() == 0x1 && GetItemPointer())
		{
			if(Process::Read32(GetItemPointer() + 0xC8, item) && item == 0x6)
			{
				Process::Write32(u0, 0xE1A00000);
			}
			
			else
			{
				Process::Write32(u0, 0xE1500001);
			}
		}
		
		if (GetNetwork() == 0)
		{
			Process::Write32(u0, 0xE1500001);
		}
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Anti Blue Shell Crash";
			Process::Write32(u0, 0xE1500001);
		}
    }
	
	void	balloonsCycler(MenuEntry *entry)
	{
		static const u8 balloonlist[18] = {0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5};
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Automatic Balloons Cycler";
		}
		
		static int bvar = -1;
		
		if(IsInRace)
		{
			if(bvar >= 18)
				bvar = 0;
			bvar++;
			writeBalloonAmount(balloonlist[bvar]);
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Automatic Balloons Cycler";
			writeBalloonAmount(0x3);
		}
	}
	
	struct Bal
	{
		const char  *name;
		const u8    id;
	};

	static const std::vector<Bal> g_bals =
	{
		{ "0 Balloons", 0 },
		{ "1 Balloon", 0x1 },
		{ "2 Balloons", 0x2 },
		{ "3 Balloons", 0x3 },
		{ "4 Balloons", 0x4 },
		{ "5 Balloons", 0x5 }
	};

	void    BalSetter(MenuEntry *entry)
    {
        u8 *bal = GetArg<u8>(entry);
		
        static StringVector bals;
		if (bals.empty())
			for (const Bal &i : g_bals)
				bals.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select your amount of balloons", bals);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *bal = g_bals[selOpt].id;
    }
	
	void	forceBal(MenuEntry *entry)
	{	
		u8 bal = *GetArg<u8>(entry);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Balloons Amount";
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Configurable Balloons Amount";
		}
		
		if (IsInRace() && Process::Read32(0x65C528, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			Process::Write32(offset + 0xBDA, bal);
		}
	}
	
	
	
	
	
	
	
	
	
	
	// Profile codes
	
	void SetCountry(MenuEntry *entry)
	{
		static StringVector flags;
		if(flags.empty())
			for(const Countries &i : g_flags)
				flags.push_back(i.countryName);

		Keyboard keyboard1(Color::Red << "Select a country in the list", flags);
		s8 choiceflag = keyboard1.Open();
		if(choiceflag < 0)
			return;
		
		StringVector states;
		if(states.empty())
			for(const States &i : g_flags[choiceflag].countryStates)
				states.push_back(i.stateName);

		Keyboard keyboard2(Color::Red << "Select a state in the list", states);
		s8 choicestate = keyboard2.Open();
		if(choicestate < 0)
			return;
	
		writeFlag(g_flags[choiceflag].countryID);

		//if country has states
		if(!g_flags[choiceflag].countryStates.empty())
			writeState(g_flags[choiceflag].countryStates[choicestate].stateID);
	}
	
	void	SetCoord(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Coordinates";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Configurable Coordinates";
		}

		static StringVector coords;
		if (coords.empty())
			for (const Coordinates &i : g_coords)
				coords.push_back(i.globeLocation);
		Keyboard keyboard(""<< Color::Red <<"Select your coordinates", coords);
		int choice = keyboard.Open();
		if (choice != -1)
		{
			writeLocation(g_coords[choice].id);
		}
	}
	
	void	randomCountry(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Random Country";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Random Country";
		}
	
		writeFlag(Utils::Random(1, 0xB9));
		writeState(1);
	}
	
	void	randomCoordinates(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Random Coordinates";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Random Coordinates";
		}
	
		writeLocation(Utils::Random(0, 0xFFFFFFFF));
	}
	
	void    SetFlag(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Set Custom Flag";
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Set Custom Flag";
		}
	
		static u8 flag = 0;
		Keyboard keyboard(""<< Color::Red <<"Set a flag ID");
		if (keyboard.Open(flag) != -1)
		{
			writeFlag(flag);
		}
	}
	
	void    SetState(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Set Custom State";
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Set Custom State";
		}

		static u16 state = 0;
		Keyboard keyboard(""<< Color::Red <<"Set a state ID");
		if (keyboard.Open(state) != -1)
		{
			writeState(state);
		}
	}
	
	void    SetCoordinates(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Set Custom Globe Coordinates";
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Set Custom Globe Coordinates";
		}
		
		static u32 coordinates = 0;
		Keyboard keyboard(""<< Color::Red <<"Set your globe coordinates (HEX value):");
		if (keyboard.Open(coordinates) != -1)
		{
			writeLocation(coordinates);
		}
	}
	
	void    SetVR(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Set Custom VR";
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Set Custom VR";
		}
		
		static u32 vr = 0;
		Keyboard keyboard(""<< Color::Red <<"Set your VR amount:");
		keyboard.IsHexadecimal(false);
		if (keyboard.Open(vr) != -1)
		{
			writeVr(vr);
		}
	}
	
	void	vrExtender(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "VR Extender";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "VR Extender";
		}
		
		if (Controller::IsKeyDown(Start))
			writeVr(999999);
		if (Controller::IsKeyDown(Select))
			writeVr(111111);
		if (Controller::IsKeyDown(DPadDown))
			writeVr(0);
		if (Controller::IsKeyDown(DPadUp))
			writeVr(696969);
		if (Controller::IsKeyDown(DPadRight))
			writeVr(666666);
		if (Controller::IsKeyDown(DPadLeft))
			writeVr(777777);
	}

	void	vrRandomizer(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "VR Randomizer";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "VR Randomizer";
		}
	
		writeVr(Utils::Random(0, 999999));
	}
	
	bool VrOSD (const Screen &screen)
	{
		if(!screen.IsTop)
			return false;
		screen.Draw("VR: " << std::to_string(*(u32 *)(*(u32 *)(0x663D04) - 0xE30)), 10, 211, Color::Yellow);
		return true;
	}

	void	vrIndicator(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "VR Indicator";
			OSD::Run(VrOSD);
		}

		if	(IsInRace())
		{
			OSD::Stop(VrOSD);
		}
		
		else
		{
			OSD::Run(VrOSD);
		}

        
		if(!entry->IsActivated())
		{
			entry->Name() = "VR Indicator";
			OSD::Stop(VrOSD);
		}
	}
	
	struct Mark
	{
		const char  *name;
		const u32    id;
	};

	static const std::vector<Mark> g_marks =
	{
		{ "Black Handlemark", 0x01010101 },
		{ "Gold Handlemark", 0x65656565 }
	};

	void    MarkSetter(MenuEntry *entry)
    {
        u32 *mark = GetArg<u32>(entry);
		
        static StringVector marks;
		if (marks.empty())
			for (const Mark &i : g_marks)
				marks.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select a handlemark", marks);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *mark = g_marks[selOpt].id;
    }
	
	void	markSelector(MenuEntry *entry)
	{
		u32 mark = *GetArg<u32>(entry);
		
		if(entry->IsActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Handlemark";
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Configurable Handlemark";
		}
		
		if (IsInRace() && Process::Read32(0x65C528, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			Process::Write32(offset + 0x774, 0x100);
			Process::Write32(offset + 0x77C, 0x100);
			offset = 0;
			Process::Read32(offset + 0x6673C8, offset);
			Process::Write32(offset + 0x6BC4, mark);
			Process::Write32(offset + 0x6BC8, mark);
			Process::Write32(offset + 0x6BCC, mark);
			Process::Write32(offset + 0x6BD0, mark);
			Process::Write32(offset + 0x6BD4, mark);
			Process::Write32(offset + 0x6BD8, mark);
			Process::Write32(offset + 0x6BDC, mark);
			Process::Write32(offset + 0x6BE0, mark);
			Process::Write32(offset + 0x6BE4, mark);
			Process::Write32(offset + 0x6BE8, mark);
			Process::Write32(offset + 0x6BEC, mark);
			Process::Write32(offset + 0x6BF0, mark);
			Process::Write32(offset + 0x6BF4, mark);
			Process::Write32(offset + 0x6BF8, mark);
			Process::Write32(offset + 0x6BFC, mark);
			Process::Write32(offset + 0x6C00, mark);
			Process::Write32(offset + 0x6C04, mark);
			Process::Write32(offset + 0x6C08, mark);
			Process::Write32(offset + 0x6C0C, mark);
			Process::Write32(offset + 0x6C10, mark);
			Process::Write32(offset + 0x6C14, mark);
			Process::Write32(offset + 0x6C18, mark);
			Process::Write32(offset + 0x6C1C, mark);
			Process::Write32(offset + 0x6C20, mark);
			Process::Write32(offset + 0x6C24, mark);
		}
	}
	
	void	unlockEverything(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Unlock Everything";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Unlock Everything";
		}
	
		if (Process::Read32(0x6673C8, offset) && is_in_range(offset, 0x10000000, 0x18000000))
		{
			Process::Write8(0x6BA3 + offset, 0x3F);
			Process::Write32(0x6BA4 + offset, 0x1FF003F);
			Process::Write16(0x6BAC + offset, 0x3FFF);
			Process::Write8(0x6BB0 + offset, 0x7F);
			Process::Write8(0x6BB4 + offset, 0x3F);
			Process::Write8(0x6BB8 + offset, 0xFF);
		}
	}
	
	void	starAmount(MenuEntry *entry)
	{
		u32 value = 0;
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Stars & Trophies Patcher";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Stars & Trophies Patcher";
		}
	
		if (Process::Read32(0x6673C8, value) && value != 0)
		{
			Process::Write32(value + 0x2470, 0x3D3D3D3D);
			Process::Write32(value + 0x2474, 0x3D3D3D3D);
			Process::Write32(value + 0x2478, 0x3D3D3D3D);
			Process::Write32(value + 0x247C, 0x3D3D3D3D);
			Process::Write32(value + 0x2480, 0x3D3D3D3D);
			Process::Write32(value + 0x2484, 0x3D3D3D3D);
			Process::Write32(value + 0x2488, 0x3D3D3D3D);
			Process::Write32(value + 0x248C, 0x3D3D3D3D);
			//
			Process::Write32(value + 0x9DD8, 0x3D3D3D3D);
			Process::Write32(value + 0x9DDC, 0x3D3D3D3D);
			Process::Write32(value + 0x9DE0, 0x3D3D3D3D);
			Process::Write32(value + 0x9DE4, 0x3D3D3D3D);
			Process::Write32(value + 0x9DE8, 0x3D3D3D3D);
			Process::Write32(value + 0x9DEC, 0x3D3D3D3D);
			Process::Write32(value + 0x9DF0, 0x3D3D3D3D);
			Process::Write32(value + 0x9DF4, 0x3D3D3D3D);
		}
	}
	
	void	starReset(MenuEntry *entry)
	{
		u32 value = 0;
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Stars & Trophies Resetter";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Stars & Trophies Resetter";
		}
	
		if (Process::Read32(0x6673C8, value) && value != 0)
		{
			Process::Write32(value + 0x2470, 0);
			Process::Write32(value + 0x2474, 0);
			Process::Write32(value + 0x2478, 0);
			Process::Write32(value + 0x247C, 0);
			Process::Write32(value + 0x2480, 0);
			Process::Write32(value + 0x2484, 0);
			Process::Write32(value + 0x2488, 0);
			Process::Write32(value + 0x248C, 0);
			//
			Process::Write32(value + 0x9DD8, 0);
			Process::Write32(value + 0x9DDC, 0);
			Process::Write32(value + 0x9DE0, 0);
			Process::Write32(value + 0x9DE4, 0);
			Process::Write32(value + 0x9DE8, 0);
			Process::Write32(value + 0x9DEC, 0);
			Process::Write32(value + 0x9DF0, 0);
			Process::Write32(value + 0x9DF4, 0);
		}
	}
	
	void    SetVrCounter(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Set VR Counter";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Set VR Counter";
		}
		
		Keyboard	keyboard(""<< Color::Red <<"Set your VR:");
		keyboard.IsHexadecimal(false);
		if (keyboard.Open(data) != -1)
		{
			writeVc(data);
		}
	}

	void	randomVrCounter(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Random VR Counter";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Random VR Counter";
		}
	
		writeVc(Utils::Random(0, 99999));
	}
	
	void    SetVictories(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Set Victories";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Set Victories";
		}
	
		Keyboard	keyboard(""<< Color::Red <<"Set your victories:");
		keyboard.IsHexadecimal(false);
		if (keyboard.Open(data) != -1)
		{
			writeVictories(data);
		}
	}
	
	void	randomVictories(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Random Victories";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Random Victories";
		}
	
		writeVictories(Utils::Random(0, 99999));
	}
	
	void    SetDefeats(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Set Defeats";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Set Defeats";
		}
	
		Keyboard	keyboard(""<< Color::Red <<"Set your defeats:");
		keyboard.IsHexadecimal(false);
		if (keyboard.Open(data) != -1)
		{
			writeDefeats(data);
		}
	}
	
	void	randomDefeats(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Random Defeats";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Random Defeats";
		}
	
		writeDefeats(Utils::Random(0, 99999));
	}
	
	void    SetCoins(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Set Coins";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Set Coins";
		}
	
		Keyboard	keyboard(""<< Color::Red <<"Set your coins:");
		keyboard.IsHexadecimal(false);
		if (keyboard.Open(data) != -1)
		{
			writeCoins(data);
		}
	}
	
	void	randomCoins(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Random Coins";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Random Coins";
		}
	
		writeCoins(Utils::Random(0, 99999));
	}
	
	void    SetStreetpass(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Set Streetpass Tags";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Set Streetpass Tags";
		}
	
		Keyboard	keyboard(""<< Color::Red <<"Set your streetpass tags:");
		keyboard.IsHexadecimal(false);
		if (keyboard.Open(data) != -1)
		{
			writeStreetpass(data);
		}
	}
	
	void	randomStreetpass(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Random Streetpass Tags";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Random Streetpass Tags";
		}
	
		writeStreetpass(Utils::Random(0, 99999));
	}
	
	void	SetDriver(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Driver Class";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Configurable Driver Class";
		}

		struct Driver
		{
			const char  *name;
			const u8    id;
		};

		static const std::vector<Driver> g_drivers =
		{
			{ "Shell Shark", 2 },
			{ "Banana Blitzer", 4 },
			{ "Bob-Omb Ace", 6 },
			{ "Pro Defender", 8 },
			{ "Rowdy Racer", 0xA },
			{ "Boost Jumper", 0xC },
			{ "Aviator", 0xE },
			{ "Dolphin", 0x10 },
			{ "Drift Wizard", 0x12 },
			{ "Quick Starter", 0x14 },
			{ "Comeback King", 0x16 },
			{ "Star Racer", 0x18 },
			{ "Model Driver", 0x1A },
			{ "Zig-Zagger", 0x1C },
			{ "Safe Driver", 0x1E },
			{ "Rookie", 0x20 }
		};

		static StringVector drivers;
		if (drivers.empty())
			for (const Driver &i : g_drivers)
				drivers.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select your class", drivers);
		int choice = keyboard.Open();
		{
			writeDriver(g_drivers[choice].id);
		}
		
	}
	
	void	randomDriverClass(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Random Driver Class";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Random Driver Class";
		}
	
		writeDriver(Utils::Random(0, 22));
	}
	
	
	
	struct Message
	{
		const char  *name;
		const u8    id;
	};

	static const std::vector<Message> g_messages =
	{
		{ "Menu", 0x62 },
		{ "Favourites", 0x63 },
		{ "Recommendations", 0x64 },
		{ "Enter code", 0x65 },
		{ "Create community", 0x66 },
		{ "Join", 0x67 },
		{ "Join this community now?", 0x68 },
		{ "Searching for groups...", 0x69 },
		{ "Leave this group?", 0x6A },
		{ "Continue playing?", 0x6B },
		{ "Enter a community code.", 0x6C },
		{ "Invalid community code", 0x6D },
		{ "Why not have a go at...", 0x6E },
		{ "You cannot create any...", 0x6F },
		{ "You cannot create a...", 0x70 },
		{ "Enter a name for this...", 0x71 },
		{ "Choose the game settings...", 0x72 },
		{ "OK", 0x73 },
		{ "Enter the community...", 0x74 },
		{ "The community will be...", 0x75 },
		{ "Your community has been...", 0x76 },
		{ "Your community code is:...", 0x77 },
		{ "Community not found.", 0x78 },
		{ "This community has been...", 0x79 },
		{ "Mario Kart Channel data...", 0x7A },
		{ "Communities Play against...", 0x7B },
		{ "Players", 0x7D },
		{ "Select an icon for...", 0x7E },
		{ "Items", 0x7F },
		{ "Characters", 0x80 },
		{ "Frames", 0x81 },
		{ "Tyres", 0x82 },
		{ "Gliders", 0x83 },
		{ "Other", 0x84 },
		{ "SD Card error: Please...", 0x85 },
		{ "Items are required...", 0x86 },
		{ "Streetpass List", 0xC7 },
		{ "Community...", 0xC8 },
		{ "Ghost data has arrived...", 0xC9 },
		{ "VR", 0xCC },
		{ "Wins", 0xCD },
		{ "Losses", 0xCE },
		{ "Coins Collected", 0xCF },
		{ "Play", 0xD1 },
		{ "Grand Prix", 0xD2 },
		{ "Time Trials", 0xD3 },
		{ "Join Online", 0xD4 },
		{ "StreetPass Hits", 0xD5 },
		{ "Other Mario Kart 7...", 0xD6 },
		{ "Community...", 0xD7 },
		{ "Ghost data has arrived...", 0xD8 },
		{ "Change Mii", 0xDC },
		{ "Edit Message", 0xDD },
		{ "Customise Vehicle", 0xDE },
		{ "Create Grand Prix", 0xDF },
		{ "Settings", 0xE0 },
		{ "Please note that...", 0xE2 },
		{ "Please do not include...", 0xE3 },
		{ "1st", 0xE6 },
		{ "2nd", 0xE7 },
		{ "3rd", 0xE8 },
		{ "4th", 0xE9 },
		{ "StreetPass", 0xEF },
		{ "Activated", 0xF0 },
		{ "Deactivated", 0xF1 },
		{ "SpotPass", 0xF9 }
	};
	
	void    MessageSetter(MenuEntry *entry)
    {
        u8 *message = GetArg<u8>(entry);
		
        static StringVector messages;
		if (messages.empty())
			for (const Message &i : g_messages)
				messages.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select a message", messages);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *message = g_messages[selOpt].id;
    }
	
	void	messageConfig(MenuEntry *entry)
	{
		u8 message = *GetArg<u8>(entry);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Custom Message";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Configurable Custom Message";
		}
		
		if (Controller::IsKeyDown(Start))
		{
			writeComment(message);
		}
	}

	void	randomMessage(MenuEntry *entry)
	{
		u32	value = 0;
		u8	list[] = {0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0xC7, 0xC8, 0xC9, 0xCC, 0xCD, 0xCE, 0xCF, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE2, 0xE3, 0xE6, 0xE7, 0xE8, 0xE9, 0xEF, 0xF0, 0xF1, 0xF9};
		u8	random_id = list[rand() % (sizeof(list) / sizeof(list[0]))];
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Custom Message Randomizer";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Custom Message Randomizer";
		}
		
		if (Controller::IsKeyDown(R))
		{
			if(Process::Read32(0x663D00, value) && value != 0)
			{
				Process::Read32(value + 0x298, value);
				Process::Write8(value + 0x66, random_id);
			}
		}
	}
	
	void	itemSpam(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x29AFC8, 0x29AFD0, 0x29AFB0);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Disable Item Spam Delay";
			Process::Write32(u0, 0xE2847002);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Disable Item Spam Delay";
			Process::Write32(u0, 0xE28470F0);
		}
    }
	
	void	highDataRate(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x290690, 0x290698, 0x290684);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "High Data Rate";
			Process::Write32(u0, 0xE3E05000);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "High Data Rate";
			Process::Write32(u0, 0xE1A05001);
		}
    }
	
	void	lagElim(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2E7444, 0x2E744C, 0x2E7424);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Lag Elimination";
			Process::Write32(u0, 0xEA000003);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Lag Elimination";
			Process::Write32(u0, 0x1A000003);
		}
    }
	
	void	lagKart(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2E86F8, 0x2E8700, 0x2E86D8);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "No Kart Position Lag";
			Process::Write32(u0, 0x00000000);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "No Kart Position Lag";
			Process::Write32(u0, 0x0A000005);
		}
    }
	
	void	pauseOnline(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x47E7A4, 0x47E728, 0x47E724);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Pause Menu Online";
			Process::Write32(u0, 0xE1A00000);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Pause Menu Online";
			Process::Write32(u0, 0x0A00001C);
		}
    }
	
	void	homeAnywhere(MenuEntry *entry)
	{
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "HOME Menu Anywhere";
		}
		
		if (Process::Read32(0x6673C8, offset))
		{
			Process::Write32(offset + 0x196C, 0x00010001);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "HOME Menu Anywhere";
		}
	}
	
	void	bypassEULA(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x18FFC0, 0x18FFC8, 0x18FFC0);
		u32 u1 = AutoRegion(0x18FFC4, 0x18FFCC, 0x18FFC4);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Bypass User Agreement Prompt";
			Process::Write32(u0, 0xE3A00001);
			Process::Write32(u1, 0xE12FFF1E);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Bypass User Agreement Prompt";
			Process::Write32(u0, 0xE92D4038);
			Process::Write32(u1, 0xE3A0280D);
		}
    }
	
	void	skipPass(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x48A200, 0x48A184, 0x48A180);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Skip Streetpass/Spotpass Prompt";
			Process::Write32(u0, 0xE3A00000);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Skip Streetpass/Spotpass Prompt";
			Process::Write32(u0, 0xEAFFFF71);
		}
    }

	void	skipLive(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2910EC, 0x2910F4, 0x2910E0);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Skip Live View";
			Process::Write32(u0, 0xEA000007);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Skip Live View";
			Process::Write32(u0, 0xAA000007);
		}
    }
	
	void	skipWait(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2A7044, 0x2A704C, 0x2A7024);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Skip Please Wait...";
			Process::Write32(u0, 0xEA000000);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Skip Please Wait...";
			Process::Write32(u0, 0x03A00002);
		}
    }
	
	void	antiVR(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x4E4390, 0x4E4314, 0x4E4310);
		u32 u1 = AutoRegion(0x4E44A4, 0x4E4428, 0x4E4424);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Anti VR Crasher";
			Process::Write32(u0, 0xE24DD010);
			Process::Write32(u1, 0xE28DD010);
        }
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Anti VR Crasher";
			Process::Write32(u0, 0xE24DD00C);
			Process::Write32(u1, 0xE28DD00C);
		}
    }
	
	void	antiMatchCrash(MenuEntry *entry)
	{
		u64 titleID = Process::GetTitleID();
		u32 hook0 = *(u32 *)0x5EBF00;
		u32 u0 = AutoRegion(0x5AA210, 0x5AA180, 0x5AA190);

		static const u8 buffer[] =
		{
			0x04, 0x40, 0x91, 0xE5,
			0xA0, 0x00, 0x54, 0xE3,
			0x00, 0x40, 0xA0, 0x01,
			0x1E, 0xFF, 0x2F, 0x01,
			0x00, 0x00, 0xA0, 0xE3,
			0x70, 0x80, 0xBD, 0xE8,
		};//18
		
		static const u8 buffer_cancel[] =
		{
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
		};
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Anti Matchmaking Packet Crasher";
		}
		
		if(hook0 == 0)
		{
			memcpy((void *)0x5EBF00, buffer, 0x18);
		}

		if (titleID == 0x0004000000030700)
		{
			Process::Write32(u0, 0xEB01073A);
		}
		
		else if (titleID == 0x0004000000030600)
		{
			Process::Write32(u0, 0xEB01075E);
		}

		else if (titleID == 0x0004000000030800)
		{
			Process::Write32(u0, 0xEB01076E);
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Anti Matchmaking Packet Crasher";
			memcpy((void *)0x5EBF00, buffer_cancel, 0x18);
			Process::Write32(u0, 0xE1A04000);
		}
	}

	void	antiKoura(MenuEntry *entry)
	{
		u32 item = 0;
		u32 u0 = AutoRegion(0x2C7B94, 0x2C7B9C, 0x2C7B74);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Anti Blue Shell Crash";
        }
		
		if (IsInRace() && Process::Read32(0x667140, offset) && is_in_range(offset, 0x14000000, 0x18000000))
		{
			Process::Read32(offset + 0x3C4, offset);
			offset += 0xFFFFFF10;
			Process::Read32(offset + 0, offset);
			Process::Read32(offset + 0x94, offset);
			Process::Write32(offset + 0x240, 0);
		}
		
		if (GetNetwork() == 0x1 && GetItemPointer())
		{
			if(Process::Read32(GetItemPointer() + 0xC8, item) && item == 0x6)
			{
				Process::Write32(u0, 0xE1A00000);
			}
			
			else
			{
				Process::Write32(u0, 0xE1500001);
			}
		}
		
		if (GetNetwork() == 0)
		{
			Process::Write32(u0, 0xE1500001);
		}
        
        if (!entry->IsActivated())
		{
			entry->Name() = "Anti Blue Shell Crash";
			Process::Write32(u0, 0xE1500001);
		}
    }
	
	struct Music
	{
		const char  *name;
		const u8    id;
	};

	static const std::vector<Music> g_musics =
	{
		{ "Toad Circuit", 4 },
		{ "Daisy Hills", 3 },
		{ "Cheep Cheep Lagoon", 2 },
		{ "Shy Guy Bazaar", 5 },
		{ "Wuhu Loop", 8 },
		{ "Mario Circuit", 0 },
		{ "Music Park", 0xF },
		{ "Rock Rock Mountain", 1 },
		{ "Piranha Plant Slide", 0xC },
		{ "Wario Shipyard", 0xE },
		{ "Neo Bowser City", 6 },
		{ "Maka Wuhu", 9 },
		{ "DK Jungle", 7 },
		{ "Rosalina's Ice World", 0xA },
		{ "Bowser's Castle", 0xB },
		{ "Rainbow Road", 0xD },
		{ "N64 Luigi Raceway", 0x1A },
		{ "GBA Bowser's Castle 1", 0x1D },
		{ "WII Mushroom Gorge", 0x13 },
		{ "DS Luigi's Mansion", 0x14 },
		{ "N64 Koopa Troopa Beach", 0x1C },
		{ "SNES Mario Circuit 2", 0x9C },
		{ "WII Coconut Mall", 0x10 },
		{ "DS Waluigi Pinball", 0x17 },
		{ "N64 Kalimari Desert", 0x1B },
		{ "DK Pass", 0x16 },
		{ "GCN Daisy Cruiser", 0x19 },
		{ "WII Maple Treeway", 0x12 },
		{ "WII Koopa Cape", 0x11 },
		{ "GCN Dino Dino Jungle", 0x18 },
		{ "DS Airship Fortress", 0x15 },
		{ "SNES Rainbow Road", 0x1F },
		{ "Battles Theme", 0x20 }
	};

	void    MusicSetter(MenuEntry *entry)
    {
        u8 *color = GetArg<u8>(entry);
		
        static StringVector musics;
		if (musics.empty())
			for (const Music &i : g_musics)
				musics.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select a music", musics);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *color = g_musics[selOpt].id;
    }
	
	void	trackMusicChanger(MenuEntry *entry)
	{
		u8 music = *GetArg<u8>(entry);
	
		u32 u0 = AutoRegion(0x3D2D3C, 0x3D2D44, 0x3D2D1C);
		
		if(entry->IsActivated())
		{
			entry->Name() = Color::LimeGreen << "Track Music Changer";
			Process::Write32(u0, 0xE3A00000 + music);
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Track Music Changer";
			Process::Write32(u0, 0xE5900160);
		}
	}
	
	void	timeTrialGhost(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Live Time Trial Ghost Replay";
		}
	
		if(!entry->IsActivated())
		{
			entry->Name() = "Live Time Trial Ghost Replay";
		}

		if (Process::Read32(0xFFFF5D4, data) && Process::Read32(data - 4, data) && Process::Read32(data + 0x18, data))
		{
			if (Process::Read32(0x14000084, offset) && Process::Read32(offset + 0x316C, offset) && is_in_range(offset, 0x14000000, 0x18000000))
			{
				Process::Write8(offset + 0x119, 0);
				Process::Write32(data + 0x208, 0x10001);
				if (IsInRace() && Process::Read32(0x65DA44, data) && Process::Read32(data + 0x20E0, data))
				{
					Process::Write32(data + 0x24, 0x49000000);
				}
			}
		}
	}
	
	void    miiDumper(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Mii Dumper";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Mii Dumper";
		}
	
		struct Mii
		{
			std::string player;
			const u8 playerSlot;
		};
		std::vector<std::string> names(8);
		std::string input;
		StringVector miis;
		Process::Read32(0x663C90, offset);
		Process::Read32(offset + 0x2B8, offset);
		for (int player = 0; player < 8; player++) // GetTotalPlayers()
			Process::ReadString(offset + 0x6E6 + (player * 0xA8), names[player], 0x14, StringFormat::Utf16);
		std::vector<Mii> g_miis =
		{
			{ names[0], 0 },
			{ names[1], 1 },
			{ names[2], 2 },
			{ names[3], 3 },
			{ names[4], 4 },
			{ names[5], 5 },
			{ names[6], 6 },
			{ names[7], 7 },
		};
		if (miis.empty())
			for (const Mii &i : g_miis)
				miis.push_back(i.player);
		File file;
		Directory dir("miis", true);
		Keyboard kb("Mii Dumper\n\nName the file for the dump of the Mii.");
		Keyboard keyboard(""<< Color::Red <<"Mii Dumper\n\nSelect a Mii", miis);
		int choice = keyboard.Open();
		if (choice == -1)
			return;
		if (kb.Open(input) != -1 && choice != -1)
			if (dir.OpenFile(file, input + ".3dsmii", File::RWC) == 0)
				if (!file.Dump(offset + 0x6CC + (g_miis[choice].playerSlot * 0xA8), 0x5C))
					MessageBox("Mii dumped to:\n" + file.GetFullName() + "\nNow, use \"3DS Mii Edit Tool\" to import this Mii into your CFL_DB.dat file.")();
				else
					MessageBox("Error\nMii dump failed. Offset:" + offset)();
		file.Flush();
		file.Close();
	}
	
	
	
	
	// Various Codes

	void    speedometer(MenuEntry *entry)
    {
		static std::vector<std::string> keys;
		
		if (entry->WasJustActivated())
        {
            if (keys.empty()) keys.push_back("Speedometer");

            OSDManager[keys[0]].SetScreen(true).SetPos(340, 223).Disable();
        }
        
        if (entry->IsActivated())
        {
			if (IsInRace())
			{
				float base{};
				
				if (Process::ReadFloat(GetRacePointer() + 0xF2C, base))
				{
					base *= 10.376756f; // km/h
					
					 OSDManager[keys[0]] = (Utils::Format("%01d", (int)base) + " km/h");
                    
                    OSDManager[keys[0]].Enable();
                }
            }
            else OSDManager[keys[0]].Disable();
        }
        else
        {
            OSDManager[keys[0]].Disable();
            
            for (u32 i = 0; i < keys.size(); i++) OSDManager.Remove(keys[i]);
        }
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Speedometer";
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Speedometer";
		}
    }
	
	struct View
	{
		const char  *name;
		const u32    id;
	};

	static const std::vector<View> g_views =
	{
		{ "Default View", 0x42480000 },
		{ "Zoomed View", 0x42180000 },
		{ "Large View", 0x42680000 },
		{ "Extended View", 0x42C80000 }
	};

	void    ViewSetter(MenuEntry *entry)
    {
        u32 *view = GetArg<u32>(entry);
		
        static StringVector views;
		if (views.empty())
			for (const View &i : g_views)
				views.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select your field of view", views);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *view = g_views[selOpt].id;
    }	
	
	void	forceView(MenuEntry *entry)
	{
		u32 view = *GetArg<u32>(entry);
		
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Configurable Field Of View";
		}
		
		if (IsInRace())
		{
			Process::Write32(0x663EA4, view);
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Configurable Field Of View";
		}
	}

	void	disableStarMusic(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Disable Star Music";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Disable Star Music";
		}
	
		u8 temp = 0;
		if (IsInRace() && GetFNsPointer() && Process::Read8(GetFNsPointer() + 0x1F7, temp) && temp == 1)
		{
			Process::Write8(GetFNsPointer() + 0x1F7, 0);
		}
	}
	
	void	fastGame(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Fast Game";
		}
	
		if (!entry->IsActivated())
		{
			entry->Name() = "Fast Game";
		}
	
		if (Process::Read32(0x140002F4, offset) && is_in_range(offset, 0x14000000, 0x18000000) && Process::Read32(offset + 0x14, offset))
		{
			Process::Write32(offset + 0x2B4, 0);
		}
		
		if (!entry->IsActivated())
		{
			if (Process::Read32(0x140002F4, offset) && is_in_range(offset, 0x14000000, 0x18000000) && Process::Read32(offset + 0x14, offset))
			{
				Process::Write32(offset + 0x2B4, 0x601);
			}
		}
	}
	
	
	
	
	
	
	
	
	
	// Gamemodes

	void	explosiveMode(MenuEntry *entry)
	{
		u32 u0 = AutoRegion(0x2CABE4, 0x2CABEC, 0x2CABC4);// Item Boxes
		u32 u1 = AutoRegion(0x2D08BC, 0x2D08C4, 0x2D089C);// Bob-Omb Limits
		u32 u2 = AutoRegion(0x2D0DC4, 0x2D0DCC, 0x2D0DA4);// Blue Shell Limits
		u32 list[5] = {0xE3A08004, 0xE3A08006, 0xE3A08006, 0xE3A08009, 0xE3A0800A};//Item Boxes
		u32	random_item = list[rand() % (sizeof(list) / sizeof(list[0]))];
		
		if (IsInRace())
		{
			Process::Write32(u0, random_item);
		}
		
		if(entry->WasJustActivated())
		{
			WasShown = false;
			entry->Name() = Color::LimeGreen << "Explosive Grand-Prix";
			Process::Write32(u1, 0xE3A00012);// 18 Bombs
			Process::Write32(u2, 0xE3A0000A);// 10 Blue Shells
		}
	
		if (!entry->IsActivated())
		{
			WasShown = true;
			entry->Name() = "Explosive Grand-Prix";
			Process::Write32(u0, 0xE0855000);
			Process::Write32(u1, 0xEBFFF3B3);
			Process::Write32(u2, 0xEBFFF2A7);
		}
		
		if (GetNetwork() == 0x1)
		{
			entry->Disable();
			if(!WasShown)
			{
				OSD::Notify("Explosive Grand-Prix is disabled online!", Color::Red);
			}
		}
	}

	void	mariokartdsMode(MenuEntry *entry)
	{
		u16 value = 0;
		u32 u0 = AutoRegion(0x461F14, 0x461E98, 0x461E94);
		
		if	(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Mario Kart DS Balloon Battle Mode";
		}
		
		if (GetNetwork() == 0x1)
		{
			Process::Write32(u0, 0x12411001);
		}
		
		if (GetNetwork() == 0)
		{
			Process::Write32(u0, 0x12811001);
		}
		
		if (Process::Read32(0x65C528, offset))
		{
			Process::Write32(0x65C528, offset);
			
			//CPU0
			if (Process::Read16(offset + 0xBDA, value) && value == 0)
			{
				Process::Write8(offset + 0xBD8, 0);
				
				if (Process::Read16(offset + 0xC4A, value) && value >= 0)
				{
					Process::Write8(offset + 0xC4A, 1);
					Process::Write8(offset + 0xCBA, 0);
					Process::Write8(offset + 0xD2A, 0);
					Process::Write8(offset + 0xD9A, 0);
					Process::Write8(offset + 0xE0A, 0);
					Process::Write8(offset + 0xE7A, 0);
					Process::Write8(offset + 0xEEA, 0);
				}
				else if	(Process::Read16(offset + 0xCBA, value) && value >= 0)
				{
					Process::Write8(offset + 0xC4A, 0);
					Process::Write8(offset + 0xCBA, 1);
					Process::Write8(offset + 0xD2A, 0);
					Process::Write8(offset + 0xD9A, 0);
					Process::Write8(offset + 0xE0A, 0);
					Process::Write8(offset + 0xE7A, 0);
					Process::Write8(offset + 0xEEA, 0);
				}
				else if (Process::Read16(offset + 0xD2A, value) && value >= 0)
				{
					Process::Write8(offset + 0xC4A, 0);
					Process::Write8(offset + 0xCBA, 0);
					Process::Write8(offset + 0xD2A, 1);
					Process::Write8(offset + 0xD9A, 0);
					Process::Write8(offset + 0xE0A, 0);
					Process::Write8(offset + 0xE7A, 0);
					Process::Write8(offset + 0xEEA, 0);
				}
				else if (Process::Read16(offset + 0xD9A, value) && value >= 0)
				{
					Process::Write8(offset + 0xC4A, 0);
					Process::Write8(offset + 0xCBA, 0);
					Process::Write8(offset + 0xD2A, 0);
					Process::Write8(offset + 0xD9A, 1);
					Process::Write8(offset + 0xE0A, 0);
					Process::Write8(offset + 0xE7A, 0);
					Process::Write8(offset + 0xEEA, 0);
				}
				else if (Process::Read16(offset + 0xE0A, value) && value >= 0)
				{
					Process::Write8(offset + 0xC4A, 0);
					Process::Write8(offset + 0xCBA, 0);
					Process::Write8(offset + 0xD2A, 0);
					Process::Write8(offset + 0xD9A, 0);
					Process::Write8(offset + 0xE0A, 1);
					Process::Write8(offset + 0xE7A, 0);
					Process::Write8(offset + 0xEEA, 0);
				}
				else if (Process::Read16(offset + 0xE7A, value) && value >= 0)
				{
					Process::Write8(offset + 0xC4A, 0);
					Process::Write8(offset + 0xCBA, 0);
					Process::Write8(offset + 0xD2A, 0);
					Process::Write8(offset + 0xD9A, 0);
					Process::Write8(offset + 0xE0A, 0);
					Process::Write8(offset + 0xE7A, 1);
					Process::Write8(offset + 0xEEA, 0);
				}
				else if (Process::Read16(offset + 0xEEA, value) && value >= 0)
				{
					Process::Write8(offset + 0xC4A, 0);
					Process::Write8(offset + 0xCBA, 0);
					Process::Write8(offset + 0xD2A, 0);
					Process::Write8(offset + 0xD9A, 0);
					Process::Write8(offset + 0xE0A, 0);
					Process::Write8(offset + 0xE7A, 0);
					Process::Write8(offset + 0xEEA, 1);
				}
				Process::Write32(offset + 0x80, 0);
			}
			
			if (Process::Read16(offset + 0xBDA, value) && value == 0x1)
			{
				Process::Write8(offset + 0xBD8, 1);
			}
			
			if (Process::Read16(offset + 0xBDA, value) && value == 0x2)
			{
				Process::Write8(offset + 0xBD8, 2);
			}

			if (Process::Read16(offset + 0xBDA, value) && value == 0x3)
			{
				Process::Write8(offset + 0xBD8, 3);
			}

			if (Process::Read16(offset + 0xBDA, value) && value == 0x4)
			{
				Process::Write8(offset + 0xBD8, 4);
			}

			if (Process::Read16(offset + 0xBDA, value) && value == 0x5)
			{
				Process::Write8(offset + 0xBD8, 5);
			}
			//CPU1
			if (Process::Read16(offset + 0xC4A, value) && value == 0x0)
			{
				Process::Write8(offset + 0xC48, 0);
				Process::Write8(offset + 0xC34, 0xD);
			}
			
			if (Process::Read16(offset + 0xC4A, value) && value == 0x1)
			{
				Process::Write8(offset + 0xC48, 1);
			}
			
			if (Process::Read16(offset + 0xC4A, value) && value == 0x2)
			{
				Process::Write8(offset + 0xC48, 2);
			}

			if (Process::Read16(offset + 0xC4A, value) && value == 0x3)
			{
				Process::Write8(offset + 0xC48, 3);
			}

			if (Process::Read16(offset + 0xC4A, value) && value == 0x4)
			{
				Process::Write8(offset + 0xC48, 4);
			}

			if (Process::Read16(offset + 0xC4A, value) && value == 0x5)
			{
				Process::Write8(offset + 0xC48, 5);
			}
			//CPU2
			if (Process::Read16(offset + 0xCBA, value) && value == 0x0)
			{
				Process::Write8(offset + 0xCB8, 0);
				Process::Write8(offset + 0xCA4, 0xD);
			}
			
			if (Process::Read16(offset + 0xCBA, value) && value == 0x1)
			{
				Process::Write8(offset + 0xCB8, 1);
			}
			
			if (Process::Read16(offset + 0xCBA, value) && value == 0x2)
			{
				Process::Write8(offset + 0xCB8, 2);
			}

			if (Process::Read16(offset + 0xCBA, value) && value == 0x3)
			{
				Process::Write8(offset + 0xCB8, 3);
			}

			if (Process::Read16(offset + 0xCBA, value) && value == 0x4)
			{
				Process::Write8(offset + 0xCB8, 4);
			}

			if (Process::Read16(offset + 0xCBA, value) && value == 0x5)
			{
				Process::Write8(offset + 0xCB8, 5);
			}
			//CPU3
			if (Process::Read16(offset + 0xD2A, value) && value == 0x0)
			{
				Process::Write8(offset + 0xD28, 0);
				Process::Write8(offset + 0xD14, 0xD);
			}
			
			if (Process::Read16(offset + 0xD2A, value) && value == 0x1)
			{
				Process::Write8(offset + 0xD28, 1);
			}
			
			if (Process::Read16(offset + 0xD2A, value) && value == 0x2)
			{
				Process::Write8(offset + 0xD28, 2);
			}

			if (Process::Read16(offset + 0xD2A, value) && value == 0x3)
			{
				Process::Write8(offset + 0xD28, 3);
			}

			if (Process::Read16(offset + 0xD2A, value) && value == 0x4)
			{
				Process::Write8(offset + 0xD28, 4);
			}

			if (Process::Read16(offset + 0xD2A, value) && value == 0x5)
			{
				Process::Write8(offset + 0xD28, 5);
			}
			//CPU4
			if (Process::Read16(offset + 0xD9A, value) && value == 0x0)
			{
				Process::Write8(offset + 0xD98, 0);
				Process::Write8(offset + 0xD84, 0xD);
			}
			
			if (Process::Read16(offset + 0xD9A, value) && value == 0x1)
			{
				Process::Write8(offset + 0xD98, 1);
			}
			
			if (Process::Read16(offset + 0xD9A, value) && value == 0x2)
			{
				Process::Write8(offset + 0xD98, 2);
			}

			if (Process::Read16(offset + 0xD9A, value) && value == 0x3)
			{
				Process::Write8(offset + 0xD98, 3);
			}

			if (Process::Read16(offset + 0xD9A, value) && value == 0x4)
			{
				Process::Write8(offset + 0xD98, 4);
			}

			if (Process::Read16(offset + 0xD9A, value) && value == 0x5)
			{
				Process::Write8(offset + 0xD98, 5);
			}
			//CPU5
			if (Process::Read16(offset + 0xE0A, value) && value == 0x0)
			{
				Process::Write8(offset + 0xE08, 0);
				Process::Write8(offset + 0xDF4, 0xD);
			}
			
			if (Process::Read16(offset + 0xE0A, value) && value == 0x1)
			{
				Process::Write8(offset + 0xE08, 1);
			}
			
			if (Process::Read16(offset + 0xE0A, value) && value == 0x2)
			{
				Process::Write8(offset + 0xE08, 2);
			}

			if (Process::Read16(offset + 0xE0A, value) && value == 0x3)
			{
				Process::Write8(offset + 0xE08, 3);
			}

			if (Process::Read16(offset + 0xE0A, value) && value == 0x4)
			{
				Process::Write8(offset + 0xE08, 4);
			}

			if (Process::Read16(offset + 0xE0A, value) && value == 0x5)
			{
				Process::Write8(offset + 0xE08, 5);
			}
			//CPU6
			if (Process::Read16(offset + 0xE7A, value) && value == 0x0)
			{
				Process::Write8(offset + 0xE78, 0);
				Process::Write8(offset + 0xE64, 0xD);
			}
			
			if (Process::Read16(offset + 0xE7A, value) && value == 0x1)
			{
				Process::Write8(offset + 0xE78, 1);
			}
			
			if (Process::Read16(offset + 0xE7A, value) && value == 0x2)
			{
				Process::Write8(offset + 0xE78, 2);
			}

			if (Process::Read16(offset + 0xE7A, value) && value == 0x3)
			{
				Process::Write8(offset + 0xE78, 3);
			}

			if (Process::Read16(offset + 0xE7A, value) && value == 0x4)
			{
				Process::Write8(offset + 0xE78, 4);
			}

			if (Process::Read16(offset + 0xE7A, value) && value == 0x5)
			{
				Process::Write8(offset + 0xE78, 5);
			}
			//CPU7
			if (Process::Read16(offset + 0xEEA, value) && value == 0x0)
			{
				Process::Write8(offset + 0xEE8, 0);
				Process::Write8(offset + 0xED4, 0xD);
			}
			
			if (Process::Read16(offset + 0xEEA, value) && value == 0x1)
			{
				Process::Write8(offset + 0xEE8, 1);
			}
			
			if (Process::Read16(offset + 0xEEA, value) && value == 0x2)
			{
				Process::Write8(offset + 0xEE8, 2);
			}

			if (Process::Read16(offset + 0xEEA, value) && value == 0x3)
			{
				Process::Write8(offset + 0xEE8, 3);
			}

			if (Process::Read16(offset + 0xEEA, value) && value == 0x4)
			{
				Process::Write8(offset + 0xEE8, 4);
			}

			if (Process::Read16(offset + 0xEEA, value) && value == 0x5)
			{
				Process::Write8(offset + 0xEE8, 5);
			}
			
			if (Process::Read16(offset + 0xC4A, value) && value == 0)
			{
				if (Process::Read16(offset + 0xCBA, value) && value == 0)
				{
					if (Process::Read16(offset + 0xD2A, value) && value == 0)
					{
						if (Process::Read16(offset + 0xD9A, value) && value == 0)
						{
							if (Process::Read16(offset + 0xE0A, value) && value == 0)
							{
								if (Process::Read16(offset + 0xE7A, value) && value == 0)
								{
									if (Process::Read16(offset + 0xEEA, value) && value == 0)
									{
										Process::Write8(offset + 0xBDA, 1);
										Process::Write32(offset + 0x80, value);
									}
								}
							}
						}
					}
				}
			}
		}
		
		if	(!entry->IsActivated())
		{
			entry->Name() = "Mario Kart DS Balloon Battle Mode";
			Process::Write32(u0, 0x12411001);
		}
	}
	
	struct Difficulty
	{
		const char  *name;
		const u8    id;
	};

	static const std::vector<Difficulty> g_difficulties =
	{
		{ "Normal", 0 },
		{ "Hard", 1 },
		{ "Ultra Hard", 2 }
	};

	void    DifficultySetter(MenuEntry *entry)
    {
        u8 *difficulty = GetArg<u8>(entry);
		
        static StringVector difficulties;
		if (difficulties.empty())
			for (const Difficulty &i : g_difficulties)
				difficulties.push_back(i.name);
		Keyboard keyboard(""<< Color::Red <<"Select a difficulty", difficulties);
		int selOpt = keyboard.Open();
		if (selOpt != -1) *difficulty = g_difficulties[selOpt].id;
    }

	void	survivorMode(MenuEntry *entry)
    {
		u8 difficulty = *GetArg<u8>(entry);
        u32 u0 = AutoRegion(0x2CABE4, 0x2CABEC, 0x2CABC4);
		u8 u1 = AutoRegion(0x41F550, 0x41F558, 0x41F530);

		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Survivor Battle Mode";
        }
		
		if (IsInRace())
		{
			Process::Write32(u0, 0xE3A0800A);
			Process::Write8(0x665734, 0);
			Process::Write8(u1, difficulty);
		}
        
        if (!entry->IsActivated())
		{
            entry->Name() = "Survivor Battle Mode";
            Process::Write32(u0, 0xE0855000);
            Process::Write8(0x665734, 0xB4);
			Process::Write8(u1, 0);
        }
    }
	
	void	CountdownMode(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Countdown Mode";
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Countdown Mode";
		}
	
		u32 pointer = 0, value = 0, g_racePointer = GetRacePointer(), g_oldRacePointer5D0 = GetOldPointer5D0(), g_oldRacePointer5CC = GetOldPointer5CC();
		static u8 score = 0;
		static bool AddedToScore = false, AddedToTime = false, end_race = false;
		if (!IsInRace())
		{
			score = 0;
			AddedToScore = false;
			AddedToTime = false;
			end_race = false;
			return;
		}
		else
		{
			if (Process::Read32(0x65C528, pointer) && Process::Read32(pointer + 0xC8, pointer) && is_in_range(pointer, 0x16000000, 0x18000000))
			{
				Process::Write32(pointer + 0x34, 0);
				Process::Write32(pointer + 0x38, 0);
			}
			if (GetTime() == 300 && !AddedToTime)
			{
				SubToTime(-180);
				AddedToTime = true;
				Process::Write8(g_oldRacePointer5CC + 0x59, 0x25);
			}

			if (!AddedToScore && Process::Read32(0xFFFF6F0, pointer) && Process::Read32(pointer - 0xC, pointer) && Process::Read32(pointer - 0x24, pointer) && Process::Read32(pointer + 0x7C, pointer) && Process::Read32(pointer + 0x20, pointer) && Process::Read32(pointer + 0x70, pointer) && Process::Read32(pointer - 0x30, pointer) && Process::Read32(pointer + 0x38, value) && Process::Read32(pointer + 0x48, pointer) && is_in_range(value, 0x40000000, pointer))
			{
				SubToTime(3);
				AddedToScore = true;
				score++;
			}
			if (AddedToScore && Process::Read32(0xFFFF6F0, pointer) && Process::Read32(pointer - 0xC, pointer) && Process::Read32(pointer - 0x24, pointer) && Process::Read32(pointer + 0x7C, pointer) && Process::Read32(pointer + 0x20, pointer) && Process::Read32(pointer + 0x70, pointer) && Process::Read32(pointer - 0x30, pointer) && Process::Read32(pointer + 0x38, value) && Process::Read32(pointer + 0x48, pointer) && value == pointer)
				AddedToScore = false;

			if (GetTime() == 0 || end_race)
			{
				u8 total = 0;
				u32 totalPlayers = (int)GetTotalPlayers() + 1;
				Process::Write32(g_racePointer + 0xF2C, 0);
				end_race = true;
				Process::Read32(0x65DA44, value);
				for (int i = 1; i < totalPlayers; i++)
				{
					if (Process::Read32(value + 0x209C + (i * 0x44), pointer) && Process::Read32(pointer + 0xF2C, pointer) && pointer < 0x3DCCCCCD)
						total++;
					else if (pointer > 0x3DCCCCCD)
						break;
					if (total + 1 == totalPlayers)
					{
						Process::Write8(g_oldRacePointer5D0 + 0x4F8, 3);
						Process::Write8(g_oldRacePointer5D0 + 0x10EC, 3);
						Process::Write8(g_oldRacePointer5D0 + 0x18EC, 3);
						break;
					}
				}
			}
			OSD::Run([](const Screen &screen)
			{
				if (!screen.IsTop)
					return false;
				screen.Draw(Utils::Format("Score: %01d", score), 10, 211, Color::Yellow);
				screen.Draw(Utils::Format("Time: %01d:%02d", GetMinutes(), GetSeconds()), 10, 223, Color::Yellow);
				return true;
			});
		}
	}

	void	TwoHundredCCStable(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Drivable 200cc";
		}
		
		if (!entry->IsActivated())
		{
			entry->Name() = "Drivable 200cc";
		}
	
		u8 boost = 0;
		static float speed = 0;
		writeSpeed(0x41400000);
		Process::Write32(0x6655FC, 0x3E4CCCCD);
		Process::Write32(0x666094, 0x42000000);
		Process::Write32(0x66619C, 0x41900000);
		Process::Write32(0x6655A4, 0x41500000);

		if (!IsInRace())
			return;
		if (Controller::IsKeysDown(R + B + A) && GetTime() != 0 && Process::ReadFloat(GetRacePointer() + 0xF2C, speed) && speed > 2.5f && Process::Read8(GetRacePointer() + 0xF9C, boost) && boost != 0) // if pressing B and R and in race and in boost
		{
			speed = 5.f;
			Process::WriteFloat(GetRacePointer() + 0xF2C, speed);
		}
		else if (Controller::IsKeysDown(R + B + A) && GetTime() != 0 && Process::ReadFloat(GetRacePointer() + 0xF2C, speed) && speed > 1.5f && speed < 5.5f && Process::Read8(GetRacePointer() + 0xF9C, boost) && boost == 0) // if pressing R and B and in race and speed > 1.5f and y in boost
		{
			Process::ReadFloat(GetRacePointer() + 0xF2C, speed);
			speed += 1.5f;
			Process::WriteFloat(GetRacePointer() + 0xF2C, speed);
		}
		if (Controller::IsKeyDown(B) && Process::ReadFloat(GetRacePointer() + 0xF2C, speed) && speed > 3.f)
		{
			Process::ReadFloat(GetRacePointer() + 0xF2C, speed);
			Process::WriteFloat(GetRacePointer() + 0xF2C, (speed - 0.4f));
		}
	}

	void	FiveHundredCCStable(MenuEntry *entry)
	{
		if(entry->WasJustActivated())
		{
			entry->Name() = Color::LimeGreen << "Drivable 500cc";
		}
		
		if(!entry->IsActivated())
		{
			entry->Name() = "Drivable 500cc";
		}
	
		u8 boost = 0;
		static float speed = 0;
		writeSpeed(0x41A00000);
		Process::Write32(0x6655FC, 0x3E4CCCCD);
		Process::Write32(0x666094, 0x42555555);
		Process::Write32(0x66619C, 0x41F00000);
		Process::Write32(0x6655A4, 0x41A00000);
		if (!IsInRace())
			return;
		if (Controller::IsKeysDown(R + B + A) && GetTime() != 0 && Process::ReadFloat(GetRacePointer() + 0xF2C, speed) && speed > 2.5f && Process::Read8(GetRacePointer() + 0xF9C, boost) && boost != 0) // if pressing B and R and in race and in boost
		{
			speed = 7.f;
			Process::WriteFloat(GetRacePointer() + 0xF2C, speed);
		}
		else if (Controller::IsKeysDown(R + B + A) && GetTime() != 0 && Process::ReadFloat(GetRacePointer() + 0xF2C, speed) && speed > 1.5f && speed < 5.5f && Process::Read8(GetRacePointer() + 0xF9C, boost) && boost == 0) // if pressing R and B and in race and speed > 1.5f and not in boost
		{
			Process::ReadFloat(GetRacePointer() + 0xF2C, speed);
			speed += 1.5f;
			Process::WriteFloat(GetRacePointer() + 0xF2C, speed);
		}
		if (Controller::IsKeyDown(B) && Process::ReadFloat(GetRacePointer() + 0xF2C, speed) && speed > 5.f)
		{
			Process::ReadFloat(GetRacePointer() + 0xF2C, speed);
			Process::WriteFloat(GetRacePointer() + 0xF2C, (speed - 0.4f));
		}
	}









	// Information Codes
	
	void	contactMe(MenuEntry *entry)
	{
		std::vector<std::string>
		choice = {"Close"};
		Keyboard KB("- Contact -\n\n" << Color::Yellow << "- Youtube:" << Color::White << "SFC%hacker%\n\n" << Color::Yellow << "- Gbatemp:" << Color::White << "SFC-hacker\n\n" << Color::Yellow << "- Github:" << Color::White << "SFC-hacker\n\n" << Color::Yellow << "- Discord:" << Color::White << "_hckr", choice);
		KB.Open();
	}
	
	void	pluginInfo(MenuEntry *entry)
	{
		std::vector<std::string>
		choice = {"Close"};
		Keyboard KB("- Build Information -\n\n" << Color::Yellow << "- Creator: " << Color::White << "SFC@hacker@\n\n" << Color::Yellow << "- Version: " << Color::White << "3.0.3\n\n" << Color::Yellow << "- Last Compiled: " << Color::White << "01/07/2024 17:28\n\n" << Color::Yellow << "- Codes: " << Color::White << "170\n\n" << Color::Yellow << "- Next Update: " << Color::White << "version 3.0.4", choice);
		KB.Open();
	}

}
