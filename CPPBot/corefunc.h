#pragma once

//    Grotopia Bot
//    Copyright (C) 2018  Growtopia Noobs
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License as published
//    by the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "enet/enet.h"
#include <string>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

using namespace std;

string SERVER_HOST = "209.59.191.76";
int SERVER_PORT = 17093;

/*********  hidden vars  ********/
class GrowtopiaBot {
public:
	ENetPeer *peer;
	ENetHost * client;

	int login_user = 0;
	int login_token = 0;

	string currentWorld;
	int timeFromWorldEnter = 0; // in 10mss...

	string gameVersion = "3.011";
	/*********  hidden vars  ********/


	/********** user sutff ***************/
	int owner = -1;
	string ownerUsername;

	struct ObjectData
	{
		int netId = -1; // used to interact with stuff in world
		int userId = -1; // growtopia id
		string name = "";
		string country = "";
		string objectType = ""; // "avatar" is player creature
		float x = -1;
		float y = -1;
		bool isGone = false; // set true if character left
		int rectX; // collision stuff
		int rectY; // collision stuff
		int rectWidth; // collision stuff
		int rectHeight; // collision stuff
		bool isMod = false;
		bool isLocal = false;
	};

	vector<ObjectData> objects;

	string uname;
	string upass;
	string worldName; // excepted world name

	bool isFollowing = false;
	bool backwardWalk = false;

	int respawnX;
	int respawnY;
	/********** user sutff ***************/

	/*********** structs declaration *********/
	struct PlayerMoving {
		int netID;
		float x;
		float y;
		int characterState;
		int plantingTree;
		float XSpeed;
		float YSpeed;
		int punchX;
		int punchY;

	};
	/*********** structs declaration *********/


	/********* user funcs  *********/
	void userRender();
	void userRender2();
	void userLoop();
	void userInit();
	void onLoginRequested();
	void packet_type3(string text);
	void packet_type6(string text);
	void packet_unknown(ENetPacket* packet);
	void OnSendToServer(string address, int port, int userId, int token);
	void OnConsoleMessage(string message);
	void OnPlayPositioned(string sound);
	void OnSetFreezeState(int state);
	void OnRemove(string data);
	void OnSpawn(string data);
	void OnAction(string command);
	void SetHasGrowID(int state, string name, string password);
	void SetHasAccountSecured(int state);
	void OnTalkBubble(int netID, string bubbleText, int type);
	void SetRespawnPos(int respawnPos);
	void OnEmoticonDataChanged(int val1, string emoticons);
	void OnSetPos(float x, float y);
	void OnAddNotification(string image, string message, string audio, int val1);
	void AtApplyTileDamage(int x, int y, int damge);
	void AtApplyLock(int x, int y, int itemId);
	void AtPlayerMoving(PlayerMoving* data);
	void AtAvatarSetIconState(int netID, int state);
	void WhenConnected();
	void WhenDisconnected();

	void respawn();
	/********* user funcs  *********/

	/***************    debug stuff   ***************/
	void dbgPrint(string text)
	{
		cout << "[DBG] " + text << endl;
	}
	/***************    debug stuff   ***************/

	GrowtopiaBot(string username, string password)
	{
		this->uname = username;
		this->upass = password;
	}

	/******************* enet core *********************/
	void SendPacket(int a1, string a2, ENetPeer* enetPeer)
	{
		if (enetPeer)
		{
			ENetPacket* v3 = enet_packet_create(0, a2.length() + 5, 1);
			memcpy(v3->data, &a1, 4);
			//*(v3->data) = (DWORD)a1;
			memcpy((v3->data) + 4, a2.c_str(), a2.length());

			//cout << std::hex << (int)(char)v3->data[3] << endl;
			enet_peer_send(enetPeer, 0, v3);
		}
	}

	void SendPacketRaw(int a1, void *packetData, size_t packetDataSize, void *a4, ENetPeer* peer, int packetFlag)
	{
		ENetPacket *p;

		if (peer) // check if we have it setup
		{
			if (a1 == 4 && *((BYTE *)packetData + 12) & 8)
			{
				p = enet_packet_create(0, packetDataSize + *((DWORD *)packetData + 13) + 5, packetFlag);
				int four = 4;
				memcpy(p->data, &four, 4);
				memcpy((char *)p->data + 4, packetData, packetDataSize);
				memcpy((char *)p->data + packetDataSize + 4, a4, *((DWORD *)packetData + 13));
				enet_peer_send(peer, 0, p);
			}
			else
			{
				p = enet_packet_create(0, packetDataSize + 5, packetFlag);
				memcpy(p->data, &a1, 4);
				memcpy((char *)p->data + 4, packetData, packetDataSize);
				enet_peer_send(peer, 0, p);
			}
		}
	}

	// Connect with default value
	void connectClient() {
		connectClient(SERVER_HOST, SERVER_PORT);
	}

	void connectClient(string hostName, int port)
	{
		cout << "Connecting bot to " << hostName << ":" << port << endl;
		client = enet_host_create(NULL /* create a client host */,
			1 /* only allow 1 outgoing connection */,
			2 /* allow up 2 channels to be used, 0 and 1 */,
			0 /* 56K modem with 56 Kbps downstream bandwidth */,
			0 /* 56K modem with 14 Kbps upstream bandwidth */);
		if (client == NULL)
		{
			cout << "An error occurred while trying to create an ENet client host.\n";
			_getch();
			exit(EXIT_FAILURE);
		}
		ENetAddress address;

		client->checksum = enet_crc32;
		enet_host_compress_with_range_coder(client);
		enet_address_set_host(&address, hostName.c_str());
		address.port = port;

		/* Initiate the connection, allocating the two channels 0 and 1. */
		peer = enet_host_connect(client, &address, 2, 0);
		if (peer == NULL)
		{
			cout << "No available peers for initiating an ENet connection.\n";
			_getch();
			exit(EXIT_FAILURE);
		}
		enet_host_flush(client);
	}
	/******************* enet core *********************/



	/*************** sender sutff **************/

	void RequestItemActivate(unsigned int item)
	{
		BYTE* data = new BYTE[56];
		for (int i = 0; i < 56; i++)
		{
			data[i] = 0;
		}
		BYTE ten = 10;
		memcpy(data + 0, &ten, 1);
		memcpy(data + 20, &item, 1);
		SendPacketRaw(4, data, 0x38u, 0, peer, 1);
		free(data);
	}

	void SetAndBroadcastIconState(int netID, int state)
	{
		BYTE* data = new BYTE[56];
		for (int i = 0; i < 56; i++)
		{
			data[i] = 0;
		}
		BYTE eighteen = 18;
		memcpy(data + 0, &eighteen, 1);
		memcpy(data + 4, &netID, 4); // (a1+40)
		memcpy(data + 44, &state, 4);
		SendPacketRaw(4, data, 0x38u, 0, peer, 1);
		free(data);
	}

	void SendPing()
	{
		BYTE* data = new BYTE[56];
		for (int i = 0; i < 56; i++)
		{
			data[i] = 0;
		}
		BYTE twentytwo = 22;
		memcpy(data + 0, &twentytwo, 1);
		SendPacketRaw(4, data, 56, 0, peer, 1);
		free(data);
	}

	/*************** sender sutff **************/


	int GetMessageTypeFromPacket(ENetPacket* packet)
	{
		int result;

		if (packet->dataLength > 3u)
		{
			result = *(packet->data);
		}
		else
		{
			cout << "Bad packet length, ignoring message" << endl;
			result = 0;
		}
		return result;
	}

	char* GetTextPointerFromPacket(ENetPacket* packet)
	{
		char zero = 0;
		memcpy(packet->data + packet->dataLength - 1, &zero, 1);
		return (char*)(packet->data + 4);
	}

	struct OnSendToServerStruct
	{
		string address;
		int port;
		int userId;
		int token;
	};

	struct OnConsoleMessageStruct
	{
		string message;
	};

	struct OnPlayPositionedStruct
	{
		string sound;
	};

	struct OnSetFreezeStateStruct
	{
		int state;
	};

	struct OnRemoveStruct
	{
		string data;
	};

	struct OnSpawnStruct
	{
		string data;
	};

	struct OnActionStruct
	{
		string data;
	};

	struct SetHasGrowIDStruct
	{
		int state;
		string name;
		string password;
	};

	struct SetHasAccountSecuredStruct
	{
		int state;
	};

	struct OnTalkBubbleStruct
	{
		int netID;
		string bubbleText;
		int type;
	};

	struct SetRespawnPosStruct
	{
		int respawnPos;
	};

	struct OnEmoticonDataChangedStruct
	{
		int val1;
		string emoticons;
	};

	struct OnSetPosStruct
	{
		float x;
		float y;
	};

	struct OnAddNotificationStruct
	{
		string image;
		string message;
		string audio;
		int val1;
	};

	//SerializeFromMem(GetExtendedDataPointerFromTankPacket(structPointer), *(int*)(structPointer + 52), 0, *(int*)(structPointer + 4));
	void SerializeFromMem(byte *pSrc, int bufferSize, int *pBytesReadOut, int netId)
	{
		string action = "";
		BYTE* dataStruct = NULL;
		byte* startPtr = pSrc;
		byte *pStartPos = pSrc;
		byte count = pSrc[0]; pSrc++;
		bool isNetIdHandled = false;
		cout << "Buffer size " << bufferSize << "NetID: " << netId << endl;
		cout << std::to_string(count) + "asdf" << endl;
		for (int i = 0; i < 10; i++) cout << (int)*(pSrc + i) << endl;
		for (int i = 0; i < count; i++)
		{
			byte index = pSrc[0]; pSrc++;
			byte type = pSrc[0]; pSrc++;
			cout << "Index " << i << " with type of " << std::to_string(type) << endl;
			switch (type)
			{
			case 2:
			{
				int strLen;
				memcpy(&strLen, pSrc, 4); pSrc += 4;

				string v;
				v.resize(strLen);
				memcpy(&v[0], pSrc, strLen); pSrc += strLen;
				if (index == 0)
				{
					action = v;
					if (action == "OnSendToServer")
					{
						dataStruct = (BYTE*)new OnSendToServerStruct;
					}
					else if (action == "OnConsoleMessage")
					{
						dataStruct = (BYTE*)new OnConsoleMessageStruct;
					}
					else if (action == "OnPlayPositioned")
					{
						dataStruct = (BYTE*)new OnPlayPositionedStruct;
					}
					else if (action == "OnSetFreezeState")
					{
						dataStruct = (BYTE*)new OnSetFreezeStateStruct;
					}
					else if (action == "OnRemove")
					{
						dataStruct = (BYTE*)new OnRemoveStruct;
					}
					else if (action == "OnSpawn")
					{
						dataStruct = (BYTE*)new OnSpawnStruct;
					}
					else if (action == "OnAction")
					{
						dataStruct = (BYTE*)new OnActionStruct;
					}
					else if (action == "SetHasGrowID")
					{
						dataStruct = (BYTE*)new SetHasGrowIDStruct;
					}
					else if (action == "SetHasAccountSecured")
					{
						dataStruct = (BYTE*)new SetHasAccountSecuredStruct;
					}
					else if (action == "OnTalkBubble")
					{
						dataStruct = (BYTE*)new OnTalkBubbleStruct;
					}
					else if (action == "SetRespawnPos")
					{
						dataStruct = (BYTE*)new SetRespawnPosStruct;
					}
					else if (action == "OnEmoticonDataChanged")
					{
						dataStruct = (BYTE*)new OnEmoticonDataChangedStruct;
					}
					else if (action == "OnSetPos")
					{
						dataStruct = (BYTE*)new OnSetPosStruct;
					}
					else if (action == "OnAddNotification")
					{
						dataStruct = (BYTE*)new OnAddNotificationStruct;
					} /*else*/ {
						if (v.find("`wAre you Human?``") != string::npos) 
						{
							cout << "Found captcha!" << endl;
						}
						else 
						{
							dbgPrint("String found at " + std::to_string((int)index) + ": " + v);
						}
					}
				}
				if (action == "OnSendToServer" && index == 4)
				{
					v.resize(v.length() - 1);
					((OnSendToServerStruct*)dataStruct)->address = v;
				}
				else if (action == "OnConsoleMessage" && index == 1)
				{
					((OnConsoleMessageStruct*)dataStruct)->message = v;
				}
				else if (action == "OnPlayPositioned" && index == 1)
				{
					((OnPlayPositionedStruct*)dataStruct)->sound = v;
				}
				else if (action == "OnRemove" && index == 1)
				{
					((OnRemoveStruct*)dataStruct)->data = v;
				}
				else if (action == "OnSpawn" && index == 1)
				{
					((OnRemoveStruct*)dataStruct)->data = v;
				}
				else if (action == "OnAction" && index == 1)
				{
					((OnRemoveStruct*)dataStruct)->data = v;
				}
				else if (action == "SetHasGrowID" && index == 2)
				{
					((SetHasGrowIDStruct*)dataStruct)->name = v;
				}
				else if (action == "SetHasAccountSecured" && index == 3)
				{
					((SetHasGrowIDStruct*)dataStruct)->password = v;
				}
				else if (action == "OnTalkBubble" && index == 2)
				{
					((OnTalkBubbleStruct*)dataStruct)->bubbleText = v;
				}
				else if (action == "OnEmoticonDataChanged" && index == 2)
				{
					((OnEmoticonDataChangedStruct*)dataStruct)->emoticons = v;
				}
				else if (action == "OnAddNotification" && index == 1)
				{
					((OnAddNotificationStruct*)dataStruct)->image = v;
				}
				else if (action == "OnAddNotification" && index == 2)
				{
					((OnAddNotificationStruct*)dataStruct)->message = v;
				}
				else if (action == "OnAddNotification" && index == 3)
				{
					((OnAddNotificationStruct*)dataStruct)->audio = v;
				}
				else if (index != 0)
				{
					dbgPrint("String found at " + std::to_string((int)index) + ": " + v);
				}
				break;
			}

			case 5:
			{
				int v;
				memcpy(&v, pSrc, sizeof(int));
				pSrc += sizeof(int);
				if (action == "OnSetFreezeState" && index == 1)
				{
					((OnSetFreezeStateStruct*)dataStruct)->state = v;
				}
				else if (action == "SetHasGrowID" && index == 1)
				{
					((SetHasGrowIDStruct*)dataStruct)->state = v;
				}
				else if (action == "SetHasAccountSecured" && index == 1)
				{
					((SetHasAccountSecuredStruct*)dataStruct)->state = v;
				}
				else if (action == "OnTalkBubble" && index == 1)
				{
					((OnTalkBubbleStruct*)dataStruct)->netID = v;
				}
				else if (action == "OnTalkBubble" && index == 3)
				{
					((OnTalkBubbleStruct*)dataStruct)->type = v;
				}
				else if (action == "SetRespawnPos" && index == 1)
				{
					((SetRespawnPosStruct*)dataStruct)->respawnPos = v;
				}
				else if (action == "OnAddNotification" && index == 4)
				{
					((OnAddNotificationStruct*)dataStruct)->val1 = v;
				}
				else {
					dbgPrint("Intx found at " + std::to_string((int)index) + ": " + std::to_string(v));
				}
				break;
			}
			case 9:
			{
				int v;
				memcpy(&v, pSrc, sizeof(int));
				pSrc += sizeof(int);
				if (action == "OnSendToServer" && index == 1)
				{
					((OnSendToServerStruct*)dataStruct)->port = v;
				}
				else if (action == "OnSendToServer" && index == 2)
				{
					((OnSendToServerStruct*)dataStruct)->token = v;
				}
				else if (action == "OnSendToServer" && index == 3)
				{
					((OnSendToServerStruct*)dataStruct)->userId = v;
				}
				else if (action == "OnEmoticonDataChanged" && index == 1)
				{
					((OnEmoticonDataChangedStruct*)dataStruct)->val1 = v;
				}
				else {
					dbgPrint("Int found at " + std::to_string((int)index) + ": " + std::to_string(v));
				}
				break;
			}

			case 1:
			{
				float v;
				memcpy(&v, pSrc, sizeof(float));
				pSrc += sizeof(float);
				dbgPrint("Float found at " + std::to_string((int)index) + ": " + std::to_string(v));

				break;
			}

			case 3:
			{
				/*CL_Vec2f v;
				memcpy(&v, pSrc, sizeof(CL_Vec2f));
				pSrc += sizeof(CL_Vec2f);
				m_variant[index].Set(v);*/
				//cout << bufferSize - (pSrc - startPtr);

				float v1;
				memcpy(&v1, pSrc, sizeof(float));
				pSrc += sizeof(float);
				float v2;
				memcpy(&v2, pSrc, sizeof(float));
				pSrc += sizeof(float);

				if (action == "OnSetPos" && index == 1)
				{
					((OnSetPosStruct*)dataStruct)->x = v1;
					((OnSetPosStruct*)dataStruct)->y = v2;
				}
				else {
					dbgPrint("Vector found at " + std::to_string((int)index) + ": " + std::to_string(v1) + "x" + std::to_string(v2));
				}
				break;

			}

			case 4:
			{
				/*CL_Vec3f v;
				memcpy(&v, pSrc, sizeof(CL_Vec3f));
				pSrc += sizeof(CL_Vec3f);
				m_variant[index].Set(v);*/
				float v1;
				memcpy(&v1, pSrc, sizeof(float));
				pSrc += sizeof(float);
				float v2;
				memcpy(&v2, pSrc, sizeof(float));
				pSrc += sizeof(float);
				float v3;
				memcpy(&v3, pSrc, sizeof(float));
				pSrc += sizeof(float);
				dbgPrint("Vector3 found at " + std::to_string((int)index) + ": " + std::to_string(v1) + "x" + std::to_string(v2) + "x" + std::to_string(v3));
				break;
			}

			case 8:
			{
				/*CL_Rectf v;
				memcpy(&v, pSrc, sizeof(CL_Rectf));
				pSrc += sizeof(CL_Rectf);
				m_variant[index].Set(v);*/
				dbgPrint("Uh3... Oh...");
				return;
				break;
			}

			default:
				dbgPrint("Unknown var type...");
				return;
				//pSrc += 4;
			}
		}
		if (action == "OnSendToServer")
		{
			OnSendToServer(((OnSendToServerStruct*)dataStruct)->address, ((OnSendToServerStruct*)dataStruct)->port, ((OnSendToServerStruct*)dataStruct)->userId, ((OnSendToServerStruct*)dataStruct)->token);
		}
		else if (action == "OnConsoleMessage")
		{
			OnConsoleMessage(((OnConsoleMessageStruct*)dataStruct)->message);
		}
		else if (action == "OnPlayPositioned")
		{
			OnPlayPositioned(((OnPlayPositionedStruct*)dataStruct)->sound);
		}
		else if (action == "OnSetFreezeState")
		{
			OnSetFreezeState(((OnSetFreezeStateStruct*)dataStruct)->state);
		}
		else if (action == "OnRemove")
		{
			OnRemove(((OnRemoveStruct*)dataStruct)->data);
		}
		else if (action == "OnSpawn")
		{
			OnSpawn(((OnSpawnStruct*)dataStruct)->data);
		}
		else if (action == "OnAction")
		{
			OnAction(((OnActionStruct*)dataStruct)->data);
		}
		else if (action == "SetHasGrowID")
		{
			SetHasGrowID(((SetHasGrowIDStruct*)dataStruct)->state, ((SetHasGrowIDStruct*)dataStruct)->name, ((SetHasGrowIDStruct*)dataStruct)->password);
		}
		else if (action == "SetHasAccountSecured")
		{
			SetHasAccountSecured(((SetHasAccountSecuredStruct*)dataStruct)->state);
		}
		else if (action == "OnTalkBubble")
		{
			OnTalkBubble(((OnTalkBubbleStruct*)dataStruct)->netID, ((OnTalkBubbleStruct*)dataStruct)->bubbleText, ((OnTalkBubbleStruct*)dataStruct)->type);
		}
		else if (action == "SetRespawnPos")
		{
			SetRespawnPos(((SetRespawnPosStruct*)dataStruct)->respawnPos);
		}
		else if (action == "OnEmoticonDataChanged")
		{
			OnEmoticonDataChanged(((OnEmoticonDataChangedStruct*)dataStruct)->val1, ((OnEmoticonDataChangedStruct*)dataStruct)->emoticons);
		}
		else if (action == "OnSetPos")
		{
			OnSetPos(((OnSetPosStruct*)dataStruct)->x, ((OnSetPosStruct*)dataStruct)->y);
		}
		else if (action == "OnAddNotification")
		{
			OnAddNotification(((OnAddNotificationStruct*)dataStruct)->image, ((OnAddNotificationStruct*)dataStruct)->message, ((OnAddNotificationStruct*)dataStruct)->audio, ((OnAddNotificationStruct*)dataStruct)->val1);
		}
		if (dataStruct != NULL)
		{
			free(dataStruct);
		}
		if (!isNetIdHandled && netId != -1)
		{
			dbgPrint("Unhandled NetID " + std::to_string(netId) + " for event " + action);
		}
	}

	class EntityComponent {

	};

	BYTE* GetExtendedDataPointerFromTankPacket(BYTE* a1)
	{
		return (BYTE*)(/*(*(int *)(a1 + 12) << 28 >> 31) & */(int)(a1 + 56));
	}

	/*void AtSetTile(int x, int y, int itemId)
	{
		cout << "Item " + std::to_string(itemId) + " applied at X:" + std::to_string(x) + " Y: " + std::to_string(y) << endl;
	}*/

	void HandlePacketTileChangeRequest(BYTE* data)
	{
		switch ((*(int*)data) - 8)
		{
		case 0:
			AtApplyTileDamage(*(int*)(data + 44), *(int*)(data + 48), *(int*)(data + 20));
			break;
		case 4:
			break;
		case 7:
			AtApplyLock(*(int*)(data + 44), *(int*)(data + 48), *(int*)(data + 20));
			break;
		default:
			/*if (*(BYTE *)data != 3)
				return;
			AtSetTile(*(int*)(data + 44), *(int*)(data + 48), *(int*)(data + 20));*/
			break;
		}
	}

	struct WorldThingStruct
	{

	};

	struct WorldStruct
	{
		int XSize;
		int YSize;
		int tileCount;
		string name;
		__int16* foreground;
		__int16* background;
		WorldThingStruct* specials;
	};

	BYTE* packPlayerMoving(PlayerMoving* dataStruct)
	{
		BYTE* data = new BYTE[56];
		for (int i = 0; i < 56; i++)
		{
			data[i] = 0;
		}
		memcpy(data + 4, &dataStruct->netID, 4);
		memcpy(data + 12, &dataStruct->characterState, 4);
		memcpy(data + 20, &dataStruct->plantingTree, 4);
		memcpy(data + 24, &dataStruct->x, 4);
		memcpy(data + 28, &dataStruct->y, 4);
		memcpy(data + 32, &dataStruct->XSpeed, 4);
		memcpy(data + 36, &dataStruct->YSpeed, 4);
		memcpy(data + 44, &dataStruct->punchX, 4);
		memcpy(data + 48, &dataStruct->punchY, 4);
		return data;
	}

	PlayerMoving* unpackPlayerMoving(BYTE* data)
	{
		PlayerMoving* dataStruct = new PlayerMoving;
		memcpy(&dataStruct->netID, data + 4, 4);
		memcpy(&dataStruct->characterState, data + 12, 4);
		memcpy(&dataStruct->plantingTree, data + 20, 4);
		memcpy(&dataStruct->x, data + 24, 4);
		memcpy(&dataStruct->y, data + 28, 4);
		memcpy(&dataStruct->XSpeed, data + 32, 4);
		memcpy(&dataStruct->YSpeed, data + 36, 4);
		memcpy(&dataStruct->punchX, data + 44, 4);
		memcpy(&dataStruct->punchY, data + 48, 4);
		return dataStruct;
	}

	WorldStruct* world = NULL;

	//ProcessTankUpdatePacket(0, NULL, tankUpdatePacket);
	void ProcessTankUpdatePacket(float someVal, EntityComponent* entityComponent, BYTE* structPointer)
	{
		cout << "Processing tank packet with id of: " << +(*(char*)structPointer) << " Where first byte is " << std::to_string(structPointer[0]) << endl;
		switch (*(char*)structPointer)
		{
		case 1:
		{
			try {
				SerializeFromMem(GetExtendedDataPointerFromTankPacket(structPointer), *(int*)(structPointer + 52), 0, *(int*)(structPointer + 4));

			}
			catch (int e)
			{

			}
			break;
		}
		case 0x15:
		{
			dbgPrint("Ping reply found!");
			break;
		}
		case 0x10:
		{
			BYTE* itemsData = GetExtendedDataPointerFromTankPacket(structPointer);
			__int16 val1 = *(__int16*)itemsData;
			int itemsCount = *(int*)(itemsData + 2);
			break;
		}
		case 0xF:
		{
			HandlePacketTileChangeRequest(structPointer);
			//cout << *(int*)(structPointer + 4) << endl;
			break;
		}
		case 8:
		{
			HandlePacketTileChangeRequest(structPointer);
			//cout << *(int*)(structPointer + 4) << endl;
			break;
		}
		case 9:
		{
			BYTE* playerItems = GetExtendedDataPointerFromTankPacket(structPointer);
			dbgPrint("Update player inventory..");
			break;
		}

		case 5:
		{
			BYTE* tileData = GetExtendedDataPointerFromTankPacket(structPointer);
			dbgPrint("Update of blocks visuals..");
		}
		case 0x16:
		{
			dbgPrint("We need to send packet raw response!");
			// SendPacketRaw(4, &v205, 0x38u, 0, peer, 1);
		}
		break;
		case 0x12:
		{
			AtAvatarSetIconState(*(int*)(structPointer + 4), *(int*)(structPointer + 44));
			break;
		}
		case 0x14:
		{
			// *(int*)(structPointer + 4) <-- NetID
			// NetAvatar::SetCharacterState
			dbgPrint("Set character state");
			/*cout << std::to_string(*(BYTE*)(structPointer + 1)) << endl;
			cout << std::to_string((*(BYTE*)(structPointer + 3)) - 128) << endl;
			cout << std::to_string((*(BYTE*)(structPointer + 2)) - 128) << endl;
			cout << std::to_string(*(int*)(structPointer + 16)) << endl;
			cout << std::to_string(*(int*)(structPointer + 20)) << endl;
			cout << std::to_string(*(float*)(structPointer + 24)) << endl;
			cout << std::to_string(*(float*)(structPointer + 28)) << endl;
			cout << std::to_string(*(float*)(structPointer + 32)) << endl;
			cout << std::to_string(*(float*)(structPointer + 36)) << endl;
			cout << std::to_string(*(int*)(structPointer + 44)) << endl;
			cout << "----------------------------------" << endl;*/
			break;
		}
		case 0xC:
		{
			string x = std::to_string(*(int*)(structPointer + 44));
			string y = std::to_string(*(int*)(structPointer + 48));
			cout << "Some tile change at X:" + x + " Y: " + y << endl;
			break;
		}
		case 0xE:
		{
			dbgPrint("Object change request! TODO!!!!!!");
			/*cout << *(int*)(structPointer + 4) << endl;
			cout << *(int*)(structPointer + 20) << endl;*/
		}
		case 0x23u:
			// MemorySerialize((TileExtra *)((char *)this + 12), v61, v7, a4);
			dbgPrint("Got some tile extra information!!! TODO!!!");
			break;
		case 3:
			dbgPrint("Destroyed/placed tile might be tree also!!! TODO!!!");
			// *(int*)(structPointer + 4) <-- NetID
			/*cout << std::to_string(*(int*)(structPointer + 44)) << endl;
			cout << std::to_string(*(int*)(structPointer + 48)) << endl;
			cout << std::to_string(*(int*)(structPointer + 20)) << endl;*/ // <---- 18 at destroy aka fist 
			/*HandlePacketTileChangeRequest(structPointer);*/
			break;
		case 4:
		{
			BYTE* worldPtr = GetExtendedDataPointerFromTankPacket(structPointer); // World::LoadFromMem
			world = new WorldStruct;
			/*for (int i = 0; i < 40000; i++)
			{
				cout << std::hex << " " << (int)worldPtr[i];
			}*/
			/*ofstream fout;
			fout.open("file.bin", ios::binary | ios::out);

			fout.write((const char*)worldPtr-120, 60000);

			fout.close();*/
			worldPtr += 6;
			__int16 strLen = *(__int16*)worldPtr;
			worldPtr += 2;
			world->name = "";
			for (int i = 0; i < strLen; i++)
			{
				world->name += worldPtr[0];
				worldPtr++;
			}
			world->XSize = *(int*)worldPtr;
			worldPtr += 4;
			world->YSize = *(int*)worldPtr;
			worldPtr += 4;
			world->tileCount = *(int*)worldPtr;
			worldPtr += 4;
			world->foreground = (__int16*)malloc(world->tileCount * sizeof(__int16));
			world->background = (__int16*)malloc(world->tileCount * sizeof(__int16));

			for (int i = 0; i < world->tileCount; i++)
			{
				world->foreground[i] = *(__int16*)worldPtr;
				//if (i<100)
				cout << std::hex << *(__int16*)worldPtr << " ";
				worldPtr += 2;
				world->background[i] = *(__int16*)worldPtr;
				worldPtr += 2;
				if (*(BYTE*)worldPtr == 0xae) // locked area
				{
					cout << "?";
					for (int j = 0; j < 6; j++)
					{
						cout << std::hex << " " << (int)worldPtr[j];
					}
					cout << "?";
					worldPtr += 2; //10  // 0 4 6 8 10
				}/* else
				if(*(BYTE*)worldPtr == 2)
				{
					worldPtr+=4;
				}*/ else if (world->foreground[i] == 0xca) // small lock
				{
					worldPtr += 10;
				}
				else if (world->foreground[i] == 242) // world lock
				{
					worldPtr += 30; // 12
				}
				else if (world->foreground[i] == 4802) // crown lock?
				{
					worldPtr += 14;
				}
				else if (world->foreground[i] == 6) // main door
				{
					worldPtr += (*(__int16*)(worldPtr + 5)) + 4;
				}
				else if (world->foreground[i] == 20) // sign
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					worldPtr += (*(__int16*)(worldPtr + 5)) + 4 + 3;
				}
				else if (world->foreground[i] == 28) // danger sign // TODO
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					worldPtr += (*(__int16*)(worldPtr + 5)) + 4 + 3;
				}
				else if (world->foreground[i] == 1682) // gateway to adventure
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					worldPtr += (*(__int16*)(worldPtr + 5)) + 4;
				}
				else if (world->foreground[i] == 858) // screen door
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					worldPtr += (*(__int16*)(worldPtr + 5)) + 4;
				}
				else if (world->foreground[i] == 382) // time space rupture
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					worldPtr += (*(__int16*)(worldPtr + 5)) + 4;
				}
				else if (world->foreground[i] == 546) // blue portal
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					for (int j = 0; j < *(__int16*)(worldPtr + 5); j++)
					{
						cout << worldPtr[j + 7];
					}
					cout << endl;
					worldPtr += (*(__int16*)(worldPtr + 5)) + 4;
				}
				else if (world->foreground[i] == 12) // door
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					for (int j = 0; j < *(__int16*)(worldPtr + 5); j++)
					{
						cout << worldPtr[j + 7];
					}
					cout << endl;
					worldPtr += (*(__int16*)(worldPtr + 5)) + 4;
				}
				else if (world->foreground[i] == 3808) // race end flag
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					worldPtr += 1;
				}
				else if (world->foreground[i] == 3804) // challenge timer
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					worldPtr += 1;
				}
				else if (world->foreground[i] == 3806) // challenge start flag
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					worldPtr += 1;
				}
				else if (world->foreground[i] == 658) // bulletin board
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					worldPtr += 7;
				}
				else if (world->foreground[i] == 1684) // path marker
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					worldPtr += 7;
				}
				else if (world->foreground[i] == 3760) // data bedrock
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					worldPtr += 2;
				}
				else if (world->foreground[i] == 1420) // mannequin
				{
					cout << *(__int16*)(worldPtr - 4) << "x" << endl;
					for (int j = -10; j < 60; j++)
					{
						cout << "? " << (int)worldPtr[j];
					}
					worldPtr += 26;
				}
				else if (world->foreground[i] % 2) { // seeds
					worldPtr += 6;

				}
				else if (0 != *(BYTE*)worldPtr)
				{
					cout << std::hex << "!" << (int)(*(BYTE*)worldPtr) << "!";
					//_getch();
				}
				worldPtr += 4;
			}
			cout << "World " + std::to_string(world->XSize) + "x" + std::to_string(world->YSize) + " with name " + world->name << endl;
			currentWorld = world->name;
			break;
		}
		case 0: // AvatarPacketReceiver::LerpState
		{
			PlayerMoving* structCahce = unpackPlayerMoving(structPointer);
			AtPlayerMoving(structCahce);
			free(structCahce);
			break;
		}
		default:
			dbgPrint("Unknown tank update packet type " + std::to_string(*(char*)structPointer));
			break;
		}

	}

	BYTE* GetStructPointerFromTankPacket(ENetPacket* packet)
	{
		unsigned int packetLenght = packet->dataLength;
		BYTE* result = NULL;
		if (packetLenght >= 0x3C)
		{
			BYTE* packetData = packet->data;
			result = packetData + 4;

			if (*(BYTE*)(packetData + 16) & 8)
			{
				cout << "Is zero!" << *(int*)(packetData + 56) << endl;
				if (packetLenght < *(int*)(packetData + 56) + 60)
				{
					cout << "Packet too small for extended packet to be valid" << endl;
					cout << "Sizeof float is 4.  TankUpdatePacket size: 56" << endl;
					result = 0;
				}
			}
			else
			{
				int zero = 0;
				memcpy(packetData + 56, &zero, 4);
			}
		}
		return result;
	}

	void ProcessPacket(ENetEvent* event, ENetPeer* peer)
	{
		int messageType = GetMessageTypeFromPacket(event->packet);
		//cout << "Packet type is " << messageType << endl;
		//cout << (event->packet->data+4) << endl;
		switch (messageType) {
		case 1:
			onLoginRequested();
			break;
		default:
			packet_unknown(event->packet);
			break;
		case 3:
			packet_type3(GetTextPointerFromPacket(event->packet));
			break;
		case 4:
		{
			BYTE* tankUpdatePacket = GetStructPointerFromTankPacket(event->packet);
			if (tankUpdatePacket)
			{
				//cout << std::hex << (char*)(tankUpdatePacket+4) << endl;
				/*v12 = (EntityComponent *)GetGameLogic();
				GameLogicComponent::ProcessTankUpdatePacket(a1, v12, tankUpdatePacket);*/
				//cout << "Size: " << event->packet->dataLength << endl;
				ProcessTankUpdatePacket(0, NULL, tankUpdatePacket);
			}

			else {
				cout << "Got bad tank packet";
			}
			/*char buffer[2048];
			for (int i = 0; i < event->packet->dataLength; i++)
			{
			sprintf(&buffer[2 * i], "%02X", event->packet->data[i]);
			}
			cout << buffer;*/
		}
		break;
		case 5:

			break;
		case 6:
			packet_type6(GetTextPointerFromPacket(event->packet));
			break;
		}
	}

	void eventLoop()
	{
		ENetEvent event;
		while (enet_host_service(client, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_NONE:
				cout << "No event???" << endl;
				break;
			case ENET_EVENT_TYPE_CONNECT:
				WhenConnected();
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				WhenDisconnected();
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				ProcessPacket(&event, peer);
				enet_packet_destroy(event.packet);
				break;
			default:
				cout << "WTF???" << endl;
				break;
			}
		}
		userLoop();
	}
};

void init() {
	if (enet_initialize() != 0) {
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		exit(0);
	}
	atexit(enet_deinitialize);
	srand(time(NULL));
}
