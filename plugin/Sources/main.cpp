#include "3ds.h"
#include "cheats.hpp"
#include "csvc.h"
#include "CTRPluginFramework.hpp"
#include <vector>

namespace CTRPluginFramework
{
    void InitMenu(PluginMenu &menu);
    int main(void)
    {
		PluginMenu *menu = new PluginMenu(Color::LimeGreen << "Mario Kart 7 Plugin Deluxe", 3, 0, 5, "- Public plugin for Mario Kart 7 EUR/JPN/USA.\n- Made for games with 1.2 online update.");
		OSD::Notify("ver: 3.0.5", Color::Yellow);
		
		menu->SynchronizeWithFrame(true);
		menu->SetHexEditorState(false);
		
		MenuFolder *subfolder = nullptr, *kartcodes = nullptr, *itemcodes = nullptr, *racecodes = nullptr, *battlecodes = nullptr, *profilecodes = nullptr, *utilitycodes = nullptr, *gamemodes = nullptr, *helpcodes = nullptr;
		
		menu->Append(new MenuEntry(""<< Color::Yellow << "Private Room Network", privateMatchmake, KeySetter, "Share the room you created/joined with your friends. Once you chose a room, go to worlwide race/battle." << Color::LimeGreen << "\nPress A or the keyboard icon to set your choice.\n\n" << Color::Orange << "Hold START button to check your room ID."));
		
		kartcodes = new MenuFolder(""<< Color::Yellow << "[ Kart Codes ]", "" << Color::LimeGreen << "Codes for kart.");
		subfolder = new MenuFolder("" << Color::Yellow << "[ Status ]", "" << Color::LimeGreen << "Codes for status.");
		subfolder->Append(new MenuEntry("Star Power Cycler", starPowerCycler, CycleSetter, "This code allows you to get Star power constantly turning ON and OFF." << Color::Red << "\nEnable the code and press on the keyboard icon on the bottom screen to set your choice." << Color::LimeGreen << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Star Power", alwaysStarPower, "This code allows you to get the Star power forever." << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Invincibility", invincibility, "This code allows you to be invincible." << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Always Small", alwaysSmall, "This code allows you to crush your kart.\n(Lightning effect)."));
		subfolder->Append(new MenuEntry("Instant Small Recovery", instantSmallRecovery, "This code allows you to recover from crush effect instantly."));
		subfolder->Append(new MenuEntry("Always Black Kart", alwaysBlackKart, "This code allows you to get a black kart.\n(Blooper effect)."));
		subfolder->Append(new MenuEntry("Instant Blooper Recovery", instantBlooperRecovery, "This code allows you to recover from ink effect instantly."));
		subfolder->Append(new MenuEntry("Always Flat", alwaysFlat, "This code allows you to squash your kart.\n(Thwomp effect)."));
		subfolder->Append(new MenuEntry("Instant Flat Recovery", instantFlatRecovery, "This code allows you to recover from squash effect instantly."));
		subfolder->Append(new MenuEntry("Never Flat Recovery", neverFlatRecovery, "This code allows you to stay flat."));
		subfolder->Append(new MenuEntry("Always Glider", alwaysGlider, "This code allows you to get the glider.\nDeployed only when flying or jumping high."));
		subfolder->Append(new MenuEntry("Always Confused", alwaysConfused, "This code allows you to get the confusion effect."));
		subfolder->Append(new MenuEntry("Micro Kart", microKart, "This code allows you to get the micro kart.\nUse instant speed codes with it." << Color::LimeGreen << "\nActivators:\n-D-Pad Down + R = Micro\n-Start = Respawn"));
		kartcodes->Append(subfolder);//13
		
		subfolder = new MenuFolder("" << Color::Yellow << "[ Abilities ]", "" << Color::LimeGreen << "Codes for abilities.");
		subfolder->Append(new MenuEntry("Instant Respawn", instantRespawn, "This code allows you to respawn quickly." << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Drive Out Of Bounds", driveBounds, "This code allows you to drive out of the boundaries. This code is not compatible with the Bullet Bill." << Color::LimeGreen << "\nActivator:\n-Y = Enabled\n-Start = Disabled"));
		subfolder->Append(new MenuEntry("Disable Wall Collisions", disableWalls, "This code allows you to drive through walls. Use it with Drive Out Of Bounds code."));
		subfolder->Append(new MenuEntry("Better Turning Sensitivity", turningSpeed, "This code allows you to turn faster when pressing A + B."));
		subfolder->Append(new MenuEntry("Control Bullet Bill", controlKiller, "This code allows you to control the movements of your Bullet Bill, it allows the item use too." << Color::LimeGreen << "\nActivators:\n-D-Pad Right + Y = Enable Bullet Bill\n-D-Pad Left + Y = Disable Bullet Bill\n-B = Stop Bullet Bill\n-A = Accelerate" << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Item Use In Bullet", itemsKiller, "This code allows you to use items while you are in a Bullet Bill."));
		subfolder->Append(new MenuEntry("Rapidfire", rapidfire, "This code allows you to throw items very fast." << Color::LimeGreen << "\nActivators:\n-X = Fast Fire\n-Left = Normal Fire"));
		subfolder->Append(new MenuEntry("Look Backward", lookBack, "This code allows you to look backward." << Color::LimeGreen << "\nActivators:\n-B + Y = Backward View\n-A = Classic View"));
		subfolder->Append(new MenuEntry("Item Despawner", itemDespawner, "You can remove your current spinning or trailed item. Works with Red Shell, Green Shell, Banana, Bob-Omb and all triple items."<< Color::LimeGreen <<"\nActivators:\n-Left = Remove current item\n-X = Throw item."));
		subfolder->Append(new MenuEntry("Trick Anywhere", trickAnywhere, "This code allows you to perform tricks anywhere." << Color::LimeGreen << "\nActivator:\n-R = Trick"));
		subfolder->Append(new MenuEntry("Disable Lucky-7 Mushroom Acceleration", disableLuckyShroom, "This code allows you to disable the acceleration from the Mushroom in the Lucky-7."));
		subfolder->Append(new MenuEntry("Disable Star Power Acceleration", disableStarAcceleration, "This code allows you to disable the instant acceleration of the Star item."));
		subfolder->Append(new MenuEntry("Disable First Person View", disableFirstPersonView, "This code allows you to disable the first person view."));
		subfolder->Append(new MenuEntry("Drive On Walls", driveWalls, "This code allows you to drive on any vertical surfaces."));
		subfolder->Append(new MenuEntry("Autopilot", autopilot, "This code allows your kart to drive automatically.\n\nOnce disabled, finish the race to stop the code properly."));
		subfolder->Append(new MenuEntry("Autojump", autoJump, "This code allows your kart to jump automatically." << Color::LimeGreen << "\nActivator:\n-Hold R = Jump"));
		subfolder->Append(new MenuEntry("Instant Stop Super Leaf", stopTanooki, "This code allows you to stop the Super Leaf instantly." << Color::LimeGreen << "\nActivator:\n-D-Pad Down = Despawn Super Leaf"));
		subfolder->Append(new MenuEntry("Instant Mini Turbo", instantMT, "This code allows you to get turbo instantly when drifting."));
		subfolder->Append(new MenuEntry("Inside Drifing Kart", insideDrift, "This code allows you to drift inwards, like certain bikes did in Mario Kart Wii."));
		subfolder->Append(new MenuEntry("Huge Bunnyhop", hugeBunnyhop, "This code allows you to jump very high."));
		subfolder->Append(new MenuEntry("Size Changer", sizeChanger, "This code constantly changes the size of the kart." << Color::LimeGreen << "\nActivators:\n-D-Pad Left = Decrease\n-D-Pad Right = Increase"));
		kartcodes->Append(subfolder);//21
		
		subfolder = new MenuFolder("" << Color::Yellow << "[ Speeds ]", "" << Color::LimeGreen << "Codes for speeds.");
		subfolder->Append(new MenuEntry("Instant Acceleration", instantAcceleration, "This code allows you to reach the max speed instantly." << Color::LimeGreen << "\nActivator:\n-A" << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Instant Backward Acceleration", instantBackAcceleration, "This code allows you to reach the max backward speed instantly." << Color::LimeGreen << "\nActivator:\n-B"));
		subfolder->Append(new MenuEntry("Instant Stop", instantStop, "This code allows you to stop the kart instantly." << Color::LimeGreen << "\nActivators:\n-A + B"));
		subfolder->Append(new MenuEntry("Configurable Speed", forceSpeed, SpeedSetter, "This code allows you to choose your speed." << Color::LimeGreen << "\nEnable the code and press on the keyboard icon on the bottom screen to set your choice." << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Configurable Backward Speed", forceBack, BackSetter, "This code allows you to choose your backwards speed." << Color::LimeGreen << "\nEnable the code and press on the keyboard icon on the bottom screen to set your choice."));
		subfolder->Append(new MenuEntry("Automatic Kart Acceleration", autoForward, AutomSetter, "This code allows you to accelerate automatically as in Mario Kart Tour." << Color::LimeGreen << "\nEnable the code and press on the keyboard icon on the bottom screen to set your choice." << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Fast Reverse Speeds", fastReverse, "This code allows you to get high backward speeds." << Color::LimeGreen << "\nActivator:\n-B"));
		kartcodes->Append(subfolder);//7
		
		subfolder = new MenuFolder("" << Color::Yellow << "[ Movements ]", "" << Color::LimeGreen << "Codes for movements.");
		subfolder->Append(new MenuEntry("Moonjump", moonjump, "This code allows you to jump high." << Color::LimeGreen << "\nActivators:\n-Start + R = Go up\n-Start + L = Go down"));
		subfolder->Append(new MenuEntry("Freefly", freefly, "This code allows you to move freely. The X,Y,Z axis rely on your position at the beginning of the race." << Color::LimeGreen << "\nActivators:\n-R = Go up\n-L = Go Down\n-Y = Stop\n-D-Pad Up = Move Forward\n-D-Pad Down = Move Backward\n-D-Pad Left = Move Left\n-D-Pad Right = Move Right" << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Touch Screen Kart Teleporter", touchCode, "This code allows you to teleport your kart anywhere on the map using your stylus. The bottom screen map is locked on the extended map view. Use this code with Out Of Bounds for a better experience" << Color::LimeGreen << "\nActivator:\n-Stylus = Move" << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Stalking Hack", stalking, "This code allows you to stalk anyone during races or battles." << Color::LimeGreen << "\nActivators:\n-Y + Right = Increase Player\n-Y + Left = Decrease Player\n-Y + Up = Stalk\n-Hold Y = Stalk Temporarily\n-Y + Down = Disable Code"));
		subfolder->Append(new MenuEntry("Drive Anywhere", driveAnywhere, "This code allows you to drive in the air as if you were on the ground.This code is a remake of Supastarrio's code for MKDS." << Color::LimeGreen << "\nActivators:\n-Start + D-Pad Left = Disable\n-Start + D-Pad Right = Enable\n-R + Y = Go Up\n-L + Y = Go Down"));
		subfolder->Append(new MenuEntry("Aimbot", aimbot, "This code allows you to aim onto a specific player." << Color::LimeGreen << "\nActivators:\n-D-Pad Up = Increase Player\n-D-Pad Down = Reset"));
		subfolder->Append(new MenuEntry("Blue Shell Ride", blueShellRide, "This code allows you to ride a Blue Shell." << Color::LimeGreen << "\nActivator:\n-D-Pad Left" << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Configurable Gravity", gravityForce, GravitySetter, "This code allows you to choose among three types of gravity."));
		subfolder->Append(new MenuEntry("Save Slot Teleporter", saveSlotTeleporter, "This code allows you to teleport your kart between 2 places." << Color::LimeGreen << "\nActivators:\n-Start + X = Save Current Position\n-Start + Y = Load Saved Position"));
		kartcodes->Append(subfolder);//9
		menu->Append(kartcodes);
		
		itemcodes = new MenuFolder(""<< Color::Yellow << "[ Item Codes ]", "" << Color::LimeGreen << "Codes for items.");
		subfolder = new MenuFolder("" << Color::Yellow << "[ Item Possession ]", "" << Color::LimeGreen << "Codes for item possession.");
		subfolder->Append(new MenuEntry("Item Cycler", itemWheelDeluxe, "This code allows you to cycle between every items." << Color::LimeGreen << "\nActivators:\n-D-Pad Right = Next Item\n-D-Pad Left = Previous Item"));
		subfolder->Append(new MenuEntry("Classic Item Hack", masterItemHack, "This code allows you to get items on command. The first person view is off." << Color::LimeGreen << "\nActivators N3DS:\n-D-Pad Up = Green Shell\n-D-Pad Down = Bob-Omb\n-D-Pad Left = Lucky-7\n-D-Pad Right = Red Shell\n-Start = Lightning\n-A + B = Bullet Bill\n-Y = Blue Shell" << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Single Shell Randomizer", randomShell, "This code allows you to get a random a random Green or Red Shell constantly." << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Configurable Item", forceItem, ItemSetter, "This code allows you to choose the item you want in the item slot." << Color::LimeGreen << "\nEnable the code and press on the keyboard icon on the bottom screen to set your choice." << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Configurable Item Box", forceBox, ItemBoxSetter, "This code allows you to force the item you will get from item boxes." << Color::LimeGreen << "\nEnable the code and press on the keyboard icon on the bottom screen to set your choice." << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Cycle Completely Random Item", randomItems, "This code allows you to get a random item constantly." << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Truly Random Items", trulyRandomItems, "This code allows you to get random items, regardless of your position." << Color::Red << "This code is restricted online!"));
		itemcodes->Append(subfolder);//7
		
		subfolder = new MenuFolder("" << Color::Yellow << "[ Item Droppers ]", "" << Color::LimeGreen << "Codes to drop items.");
		subfolder->Append(new MenuEntry("Pop Items", popItems, "This code allows you to pop items from your kart." << Color::LimeGreen << "\nActivators:\n-Hold X + Press L = Pop Items"));
		subfolder->Append(new MenuEntry("Drop Items", dropItems, "This code allows you to drop Green Shell, Red Shell, Blue Shell and fireballs on the ground."));
		subfolder->Append(new MenuEntry("Drop Lucky-7 Mushroom", dropLuckyShroom, "Use the Lucky-7 to drop a Mushroom on the ground."));
		subfolder->Append(new MenuEntry("Drop Lucky-7 Star", dropLuckyStar, "Use the Lucky-7 to drop a Star on the ground."));
		subfolder->Append(new MenuEntry("Drop Lucky-7 Blooper", dropLuckyBlooper, "Use the Lucky-7 to drop a Blooper on the ground."));
		subfolder->Append(new MenuEntry("Drop Mushroom", dropMushroom, "This code allows you to drop Mushrooms when you use a Green Shell."));
		itemcodes->Append(subfolder);//6
		
		subfolder = new MenuFolder("" << Color::Yellow << "[ Item Limits ]", "" << Color::LimeGreen << "Codes for item limits.");
		subfolder->Append(new MenuEntry("Extended Blue Shell Limit", blueLimit, "This code allows you to use more Blue Shells.\n\nEnable this code in a menu or restart the race to apply the change properly."));
		subfolder->Append(new MenuEntry("Extended Green Shell Limit", greenLimit, "This code allows you use more Green Shells.\n\nEnable this code in a menu or restart the race to apply the change properly."));
		subfolder->Append(new MenuEntry("Extended Red Shell Limit", redLimit, "This code allows you to use more Red Shells.\n\nEnable this code in a menu or restart the race to apply the change properly."));
		subfolder->Append(new MenuEntry("Extended Star Limit", starLimit, "This code allows you to drop more Stars on the ground.\n\nEnable this code in a menu or restart the race to apply the change properly."));
		subfolder->Append(new MenuEntry("Extended Mushroom Limit", shroomLimit, "This code allows you to drop more Mushrooms on the ground.\n\nEnable this code in a menu or restart the race to apply the change properly."));
		subfolder->Append(new MenuEntry("Extended Bob-Omb Limit", bombLimit, "This code allows you to use more Bob-Ombs.\n\nEnable this code in a menu or restart the race to apply the change properly."));
		subfolder->Append(new MenuEntry("Extended Banana Limit", bananaLimit, "This code allows you to drop more Bananas on the ground.\n\nEnable this code in a menu or restart the race to apply the change properly."));
		subfolder->Append(new MenuEntry("Extended Fireball Limit", fireballLimit, "This code allows you to drop more Fireballs on the ground.\n\nEnable this code in a menu or restart the race to apply the change properly."));
		subfolder->Append(new MenuEntry("Extended Blooper Limit", blooperLimit, "This code allows you to drop more Bloopers on the ground.\n\nEnable this code in a menu or restart the race to apply the change properly."));
		itemcodes->Append(subfolder);//9
		
		subfolder = new MenuFolder("" << Color::Yellow << "[ Item Speeds ]", "" << Color::LimeGreen << "Codes item speeds.");
		subfolder->Append(new MenuEntry("Bullet Speed Modifier", killerSpeed, "This code allows you to control the speed of the Bullet Bill." << Color::LimeGreen << "\nActivators:\n-X = Very Fast\n-B = Stopped\n-A = Normal Speed" << Color::Red << "This code is restricted online!"));
		subfolder->Append(new MenuEntry("Fast Blue Shell", fastBlueShell, "This code allows you to increase the speed of the Blue Shell."));
		subfolder->Append(new MenuEntry("Stopped Blue Shell", stoppedBlueShell, "This code allows you to stop the Blue Shell." << Color::Red << "\nThis code is disabled online!"));
		subfolder->Append(new MenuEntry("Instant Blue Shell Hit", blueShellInstantHit, "The Blue Shell hits the first player quickly."));
		subfolder->Append(new MenuEntry("Fast Green Shell", fastGreenShell, "This code allows you to increase the speed of the Green Shell."));
		subfolder->Append(new MenuEntry("Stopped Green Shell", stoppedGreenShell, "This code allows you to stop the Green Shell."));
		itemcodes->Append(subfolder);//6
		
		subfolder = new MenuFolder("" << Color::Yellow << "[ Item Sizes ]", "" << Color::LimeGreen << "Codes for item sizes.");
		subfolder->Append(new MenuEntry("Configurable Banana Size", bananaSize, SizeSetter, "This code allows you to set the size of the Bananas."));
		subfolder->Append(new MenuEntry("Configurable Fireball Size", fireballSize, SizeSetter, "This code allows you to set the size of the fireballs from the Fire Flower."));
		subfolder->Append(new MenuEntry("Configurable Bob-Omb Size", bombSize, SizeSetter, "This code allows you to set the size of the Bob-Ombs."));
		itemcodes->Append(subfolder);//3
		
		subfolder = new MenuFolder("" << Color::Yellow << "[ Item Misc ]", "" << Color::LimeGreen << "Codes for various item settings.");
		subfolder->Append(new MenuEntry("Intangible Items", itemReact, "This code allows you to throw multiples items which won't collide and break between each others."));
		subfolder->Append(new MenuEntry("Triple Rotating Bananas", rotatingBananas, "This code allows you to get rotating Triple Bananas around your kart like in MK8."));
		subfolder->Append(new MenuEntry("Invisible Lightning", invisibleLightning, "This code allows you to spoof the darkness and the visual of the Lightning."));
		subfolder->Append(new MenuEntry("Lightning Color Selector", lightningColor, LightningColorSetter, "This code allows you to configure the color of the Lightning." << Color::LimeGreen << "\nEnable the code and press on the keyboard icon on the bottom screen to set your choice."));
		subfolder->Append(new MenuEntry("Lightning Selfstriking", lightningSelfstriking, "This code allows you to throw a Lightning on yourself."));
		subfolder->Append(new MenuEntry("Blooper Selfstriking", blooperSelfstriking, "This code allows you to throw a Blooper on yourself."));
		subfolder->Append(new MenuEntry("Never Lose Items", noItemClear, "This code allows you to keep your current item forever."));
		subfolder->Append(new MenuEntry("No Lucky-7 Rotation", stuckedLucky, "This code allows you to stop the rotation of the Lucky-7."));
		subfolder->Append(new MenuEntry("No Lucky-7 Items Rotation", stuckedLuckyItem, "This code allows you to stop the rotation of the items from the Lucky-7."));
		subfolder->Append(new MenuEntry("No Bob-Omb Jump", noBombJump, "This code allows you to remove the jump of the Bob-Omb."));
		subfolder->Append(new MenuEntry("Jumping Bob-Omb", jumpingBomb, "This code allows you to increase the jump of the Bob-Omb."));
		subfolder->Append(new MenuEntry("No Shell Rotation", noRotation, "This code allows you to remove the rotation of the Green Shell."));
		subfolder->Append(new MenuEntry("Flying Dropped Star", flyingStar, "This code allows you to increase the height of the dropped Star."));
		subfolder->Append(new MenuEntry("Inflatable Dropped Star", inflatableStar, "This code allows you to get inflatable dropped Star."));
		itemcodes->Append(subfolder);//14
		menu->Append(itemcodes);
		
		racecodes = new MenuFolder(""<< Color::Yellow << "[ Race Codes ]", "" << Color::LimeGreen << "Codes for races.");
		racecodes->Append(new MenuEntry("Drive During Countdown", driveDuringCountdown, "This code allows you to drive during the countdown." << Color::Red << "This code is restricted online!"));
		racecodes->Append(new MenuEntry("No Disconnect", NoDC, "This code allows you to drive backward and stay stopped during online races without being disconnected." << Color::Red << "This code is restricted online!"));
		racecodes->Append(new MenuEntry("Race Time Displayer", TimeIndicator, "This code allows you to check the time left during races."));
		racecodes->Append(new MenuEntry("Remove Lakitu", removeLakitu, "This code allows you to remove Lakitu during races and battles."));
		racecodes->Append(new MenuEntry("No Countdown", noCountdown, "This code allows you to remove the countdown."));
		racecodes->Append(new MenuEntry("Instant Countdown", instantCountdown, "This code allows you to start the countdown instantly."));
		racecodes->Append(new MenuEntry("CPU Brawl", cpuBrawl, "This code allows you to make CPUs crazy."));
		racecodes->Append(new MenuEntry("CPU Propeller Spawn Controller", cpuPropeller, "This code allows you to trigger or remove the propeller from the karts of the CPUs." << Color::LimeGreen << "\nActivators:\n-D-Pad Right = Enabled\n-D-Pad Left = Disabled"));
		racecodes->Append(new MenuEntry("Water Everywhere", waterEverywhere, "This code allows you to get the propeller by enabling the water everywhere."));
		racecodes->Append(new MenuEntry("Rank Points Modifier", nullptr, SetPoints, "This code allows you to set your points. Set the points during the second race or battle round." << Color::LimeGreen << "\nPress A or the keyboard icon to set your choice."));
		racecodes->Append(new MenuEntry("Random Rank Points", randomPoints, "This code allows you to get a random point amount. It doesn't display the points until the 2nd race is finished."));
		racecodes->Append(new MenuEntry("Instant Win", instantWin, "This code allows you to win the race instantly." << Color::LimeGreen << "\nActivators:\n-Start + D-Pad Down"));
		menu->Append(racecodes);//12
		
		battlecodes = new MenuFolder(""<< Color::Yellow << "[ Battle Codes ]", "" << Color::LimeGreen << "Codes for battles.");
		battlecodes->Append(new MenuEntry("Configurable Blinking Effect", blinkingEffect, BlinkSetter, "This code allows you to prevent the crash occuring when a Blue Shell goes out of the boundaries."));
		battlecodes->Append(new MenuEntry("Anti Blue Shell Crash", antiKouraB, "This code allows you to prevent the crash occuring when a Blue Shell goes out of the boundaries in battle mode."));
		battlecodes->Append(new MenuEntry("Coin Score Modifier", battleCoin, "You can control your coin score." << Color::LimeGreen << "\nActivators:\n-D-Pad Right + R = Increase Score\n-D-Pad Left + R = Decrease Score"));
		battlecodes->Append(new MenuEntry("Balloon Score Modifier", battleBalloon, "You can control your balloon score." << Color::LimeGreen << "\nActivators:\n-D-Pad Right + R = Increase Score\n-D-Pad Left + R = Decrease Score"));
		battlecodes->Append(new MenuEntry("Coin Score Randomizer", randCoins, "This code allows you to randomize your coin score."));
		battlecodes->Append(new MenuEntry("Balloon Score Randomizer", randBalloons, "This code allows you to randomize your balloon score."));
		battlecodes->Append(new MenuEntry("Coins Limit Extender", coinLimit, "This code allows you to pick more than 10 coins during coin battles."));
		battlecodes->Append(new MenuEntry("Automatic Balloons Cycler", balloonsCycler, "This code allows you to constantly get a different amount of balloons above your kart."));
		battlecodes->Append(new MenuEntry("Configurable Balloons Amount", forceBal, BalSetter, "This code allows you to choose your amount of balloons on your kart." << Color::LimeGreen << "\nEnable the code and press on the keyboard icon on the bottom screen to set your choice."));
		battlecodes->Append(new MenuEntry("Max Timer", maxTimer, "This code allows you to stop the timer at 99:59.999."));
		battlecodes->Append(new MenuEntry("Min Timer", minTimer, "This code allows you to stop the party instantly."));
		menu->Append(battlecodes);//11
		
		profilecodes = new MenuFolder(""<< Color::Yellow << "[ Profile Codes ]", "" << Color::LimeGreen << "Codes for online profile.");
		subfolder = new MenuFolder("" << Color::Yellow << "[ Region & VR ]", "" << Color::LimeGreen << "Contains codes about your region of residence and your VR amount online.");
		subfolder->Append(new MenuEntry("Configurable Country", nullptr, SetCountry, "This code allows you to choose your country and your region of residence." << Color::LimeGreen << "\nPress A or the keyboard icon to set your choice."));
		subfolder->Append(new MenuEntry("Configurable Coordinates", nullptr, SetCoord, "This code allows you to choose your location on the globe." << Color::LimeGreen << "\nPress A or the keyboard icon to set your choice."));
		subfolder->Append(new MenuEntry("Random Country", randomCountry, "This code allows you to get a random country constantly."));
		subfolder->Append(new MenuEntry("Random Coordinates", randomCoordinates, "This code allows you to get random coordinates constantly."));
		subfolder->Append(new MenuEntry("Set Custom Flag", nullptr, SetFlag, "This code allows you to choose your flag." << Color::LimeGreen << "\nPress A or the keyboard icon to set your choice."));
		subfolder->Append(new MenuEntry("Set Custom State", nullptr, SetState, "This code allows you to choose your state." << Color::LimeGreen << "\nPress A or the keyboard icon to set your choice."));
		subfolder->Append(new MenuEntry("Set Custom Globe Coordinates", nullptr, SetCoordinates, "This code allows you to choose your coordinates." << Color::LimeGreen << "\nPress A or the keyboard icon to set your choice."));
		subfolder->Append(new MenuEntry("VR Extender", vrExtender, "This code allows you to get a specific amount of VR/PC on command." << Color::LimeGreen << "\nActivators:\n-Start = 999999\n-Select = 111111\n-D-Pad Down = 0\n-D-Pad Up = 696969\n-D-Pad Right = 666666\n-D-Pad Left = 777777"));
		subfolder->Append(new MenuEntry("Set Custom VR", nullptr, SetVR, "This code allows you to choose your VR amount. Set your desired amount once you joined a room or you will have to set the amount again." << Color::LimeGreen << "\nPress A or the keyboard icon to set your choice."));
		subfolder->Append(new MenuEntry("VR Randomizer", vrRandomizer, "This code allows you to get a random VR/PC/BR amount constantly."));
		subfolder->Append(new MenuEntry("VR Indicator", vrIndicator, "This code allows you to check your VR amount online.\n\nUse this code is useful when one of the 2 codes above is enabled."));
		profilecodes->Append(subfolder);//11
		
		subfolder = new MenuFolder("" << Color::Yellow << "[ Unlockations ]", "" << Color::LimeGreen << "Codes for statistics.");
		subfolder->Append(new MenuEntry("Configurable Handlemark", markSelector, MarkSetter, "This code allows you to get the handlemark on result screen." << Color::LimeGreen << "\nEnable the code and press on the keyboard icon on the bottom screen to set your choice."));
		subfolder->Append(new MenuEntry("Unlock Everything", unlockEverything, "This code allows you to unlock every karts, characters and tracks.\n\nEnable this code in a menu or restart the race to apply the change properly."));
		subfolder->Append(new MenuEntry("Stars & Trophies Patcher", starAmount, "This code allows you to get 3 stars and golden trophies for every cups.\n\nEnable this code in a menu or restart the race to apply the change properly."));
		subfolder->Append(new MenuEntry("Stars & Trophies Resetter", starReset, "This code allows you to reset the stars and trophies of every cups.\n\nEnable this code in a menu or restart the race to apply the change properly."));
		profilecodes->Append(subfolder);//4
		
		subfolder = new MenuFolder("" << Color::Yellow << "[ Mario Kart Channel ]", "" << Color::LimeGreen << "Codes for Mario Kart Channel statistics.");
		subfolder->Append(new MenuEntry("Set VR Counter", nullptr, SetVrCounter, "This code allows you to choose your amount of VR (Mario Kart Channel)." << Color::LimeGreen << "\nPress the keyboard icon to set your choice."));
		subfolder->Append(new MenuEntry("Random VR Counter", randomVrCounter, "This code allows you to get a random amount of VR/PC/BR constantly (Mario Kart Channel)."));
		subfolder->Append(new MenuEntry("Set Victories", nullptr, SetVictories, "This code allows you to choose your amount of victories." << Color::LimeGreen << "\nPress the keyboard icon to set your choice."));
		subfolder->Append(new MenuEntry("Random Victories", randomVictories, "This code allows you to get a random amount of victories."));
		subfolder->Append(new MenuEntry("Set Defeats", nullptr, SetDefeats, "This code allows you to choose your amount of defeats." << Color::LimeGreen << "\nPress the keyboard icon to set your choice."));
		subfolder->Append(new MenuEntry("Random Defeats", randomDefeats, "This code allows you to get a random amount of defeats."));
		subfolder->Append(new MenuEntry("Set Coins", nullptr, SetCoins, "This code allows you to choose your amount of coins." << Color::LimeGreen << "\nPress the keyboard icon to set your choice ."));
		subfolder->Append(new MenuEntry("Random Coins", randomCoins, "This code allows you to get a random amount of coins."));
		subfolder->Append(new MenuEntry("Set Streetpass Tags", nullptr, SetStreetpass, "This code allows you to choose your amount of streetpass tags." << Color::LimeGreen << "\nPress the keyboard icon to set your choice."));
		subfolder->Append(new MenuEntry("Random Streetpass Tags", randomStreetpass, "This code allows you to get a random amount of streetpass tags."));
		subfolder->Append(new MenuEntry("Configurable Driver Class", nullptr, SetDriver, "This code allows you to choose your driver class." << Color::LimeGreen << "\nPress A or the keyboard icon to set your choice."));
		subfolder->Append(new MenuEntry("Random Driver Class", randomDriverClass, "This code allows you to get a random driver class."));
		profilecodes->Append(subfolder);//12
		menu->Append(profilecodes);

		utilitycodes = new MenuFolder(""<< Color::Yellow << "[ Utility Codes ]", "" << Color::LimeGreen << "Codes for various stuff.");
		subfolder = new MenuFolder("" << Color::Yellow << "[ Online ]", "" << Color::LimeGreen << "Codes for online features.");
		subfolder->Append(new MenuEntry("Disable Item Spam Delay", itemSpam, "This code allows you to spam items online. It bypasses the online delay."));
		subfolder->Append(new MenuEntry("High Data Rate", highDataRate, "This code allows you to send network data more frequently to other people."));
		subfolder->Append(new MenuEntry("Lag Elimination", lagElim, "This code allows you to remove online lag."));
		subfolder->Append(new MenuEntry("No Kart Position Lag", lagKart, "This code allows you to remove the position lag online."));
		subfolder->Append(new MenuEntry("Pause Menu Online", pauseOnline, "This code allows you to leave a race or a battle at anytime online."));
		subfolder->Append(new MenuEntry("HOME Menu Anywhere", homeAnywhere, "This code allows you to have access to the HOME Menu at anytime."));
		subfolder->Append(new MenuEntry("Skip Please Wait...", skipWait, "This code allows you to skip the please wait... message."));
		subfolder->Append(new MenuEntry("Anti VR Crasher", antiVR, "This code allows you to prevent the crash occuring between the player bubble presentation and the VR screen."));
		subfolder->Append(new MenuEntry("Anti Matchmaking Packet Crasher", antiMatchCrash, "This code prevents the matchmaking crash which affects people during character/kart selection and the room search menu."));
		subfolder->Append(new MenuEntry("Mii Dumper", nullptr, miiDumper, "This code allows you to dump any Mii online. Follow the instructions on screen after enabling the code. If the room is not fully populated, certain buttons may be blank or contain previous player's names. You can dump previous players even if they left the room." << Color::LimeGreen << "\nPress the keyboard icon to set your choice."));
		utilitycodes->Append(subfolder);//10
		
		subfolder = new MenuFolder("" << Color::Yellow << "[ Misc. ]", "" << Color::LimeGreen << "Codes for online features.");
		subfolder->Append(new MenuEntry("Bypass User Agreement Prompt", bypassEULA, "This code allows you to use online services without reading and accepting Nintendo 3DS Service User Agreement and the Privacy Policy. "));
		subfolder->Append(new MenuEntry("Skip Streetpass/Spotpass Prompt", skipPass, "This code allows you to use Mario Kart Channel without any Streetpass/Spotpass prompt."));
		subfolder->Append(new MenuEntry("Skip Live View", skipLive, "This code allows you to skip the live view. You will wait until players have finished their race."));
		subfolder->Append(new MenuEntry("Speedometer", speedometer, "This code allows you to check your current speed in km/h."));
		subfolder->Append(new MenuEntry("Configurable Field Of View", forceView, ViewSetter, "This code allows you to increase or decrease your field of view."));
		subfolder->Append(new MenuEntry("Configurable Custom Message", messageConfig, MessageSetter, "This code allows you to change your messages." << Color::LimeGreen << "\nActivator:\n-Start" << Color::LimeGreen << "\nEnable the code and press on the keyboard icon on the bottom screen to set your choice."));
		subfolder->Append(new MenuEntry("Custom Message Randomizer", randomMessage, "This code allows you to send randomized messages constantly when pressing R button." << Color::LimeGreen << "\nActivator:\n-Hold R"));
		subfolder->Append(new MenuEntry("Disable Star Music", disableStarMusic, "This code allows you to remove the music of the Star."));
		subfolder->Append(new MenuEntry("Fast Game", fastGame, "This code allows you to accelerate the speed of the game."));
		subfolder->Append(new MenuEntry("Track Music Changer", trackMusicChanger, MusicSetter, "This code allows you to replace the music of the track you are playing on, by the music of another track.\n\nEnable this code in a menu or restart the race to apply the change properly." << Color::LimeGreen << "\nEnable the code and press on the keyboard icon on the bottom screen to set your choice."));
		subfolder->Append(new MenuEntry("Live Time Trial Ghost Replay", timeTrialGhost, "This code allows you to watch ghosts." << Color::LimeGreen << "\nFollow these steps:\n1. Activate this code and then enter the time trial with the ghost you want to view.\n2. Your kart will be out of bounds. Restart the race.\n3. Done! The code should be working."));
		utilitycodes->Append(subfolder);//11
		menu->Append(utilitycodes);
		
		gamemodes = new MenuFolder(""<< Color::Yellow << "[ Game Modes ]", "" << Color::LimeGreen << "Codes for custom game modes.");
		gamemodes->Append(new MenuEntry("Explosive Grand-Prix", explosiveMode, "This is a custom game mode for Grand-Prix inspired by MKWII and MK8 settings.\nThe only available items are Bob-Omb, Blue Shell, Bullet Bill & Lightning.\n\nEnable this code before starting the Grand-Prix for a better experience." << Color::Red << "\nThis code is disabled online!"));
		gamemodes->Append(new MenuEntry("Mario Kart DS Balloon Battle Mode", mariokartdsMode, "Everyone starts with 3 balloons, burst your opponents' balloons with items.\nIf all your inflated balloons burst, you lose.\nYou can collect up to 5 balloons if you steal them with a Mushroom or a Star." << Color::Red << "\nThis code is disabled online!"));
		gamemodes->Append(new MenuEntry("Survivor Battle Mode", survivorMode, DifficultySetter, "The only available item is the Lightning, you have to survive and win points without blinking delay. You can set the difficulty." << Color::LimeGreen << "\nEnable the code and press on the keyboard icon on the bottom screen to set your choice." << Color::Red << "\nThis code is disabled online!"));
		gamemodes->Append(new MenuEntry("Countdown Mode", CountdownMode, "You have 2 minutes to race. Every time you hit a player with an item, your time will increase by 3 seconds. You will need to strategically gain more time while still being towards the front of the pack. The player in first place when everyone's time runs out wins!"));
		gamemodes->Append(new MenuEntry("Drivable 200cc", TwoHundredCCStable, "This version of 200cc has brake drifting by pressing B during drifts (also works in boosts and while in the air), and drifting at lower speeds and in off road. Items will also travel at speeds proportional to the CC." << Color::Red << "\nThis code is restricted online!"));
		gamemodes->Append(new MenuEntry("Drivable 500cc", FiveHundredCCStable, "This version of 500cc has brake drifting by pressing B during drifts (also works in boosts and while in the air), and drifting at lower speeds and in off road. Items will also travel at speeds proportional to the CC." << Color::Red << "\nThis code is restricted online!"));
		menu->Append(gamemodes);//6
		
		helpcodes = new MenuFolder(""<< Color::Orange << "[ Contact & Help! ]", "" << Color::LimeGreen << "...");
		helpcodes->Append(new MenuEntry("Questions", nullptr, questions, "..."));
		helpcodes->Append(new MenuEntry("Contact", nullptr, contactMe, "..."));
		helpcodes->Append(new MenuEntry("Build Information", nullptr, pluginInfo, "..."));
		helpcodes->Append(new MenuEntry("Credits", nullptr, creditsList, "..."));
		menu->Append(helpcodes);//4
        menu->Run();//176
        delete menu;
        return (0);
    }

    static void    ToggleTouchscreenForceOn(void)
    {
        static u32 original = 0;
        static u32 *patchAddress = nullptr;

        if (patchAddress && original)
        {
            *patchAddress = original;
            return;
        }

        static const std::vector<u32> pattern =
        {
            0xE59F10C0, 0xE5840004, 0xE5841000, 0xE5DD0000,
            0xE5C40008, 0xE28DD03C, 0xE8BD80F0, 0xE5D51001,
            0xE1D400D4, 0xE3510003, 0x159F0034, 0x1A000003
        };

        Result  res;
        Handle  processHandle;
        s64     textTotalSize = 0;
        s64     startAddress = 0;
        u32 *   found;

        if (R_FAILED(svcOpenProcess(&processHandle, 16)))
            return;

        svcGetProcessInfo(&textTotalSize, processHandle, 0x10002);
        svcGetProcessInfo(&startAddress, processHandle, 0x10005);
        if(R_FAILED(svcMapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, processHandle, (u32)startAddress, textTotalSize)))
            goto exit;

        found = (u32 *)Utils::Search<u32>(0x14000000, (u32)textTotalSize, pattern);

        if (found != nullptr)
        {
            original = found[13];
            patchAddress = (u32 *)PA_FROM_VA((found + 13));
            found[13] = 0xE1A00000;
        }

        svcUnmapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, textTotalSize);
exit:
        svcCloseHandle(processHandle);
    }

    void    PatchProcess(FwkSettings &settings)
    {
        ToggleTouchscreenForceOn();
		settings.AllowSearchEngine = false;
		settings.AllowActionReplay = false;
        if (R_SUCCEEDED(plgLdrInit()))
		{
            PLGLDR__SetRosalinaMenuBlock(true);

            plgLdrExit();
		}
    }
	
    void    OnProcessExit(void)
    {
        ToggleTouchscreenForceOn();
    }
}
