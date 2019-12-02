#pragma once

#include <string>
#include <Windows.h>

using namespace std;

namespace patch
{
	template < typename T > std::string to_string(const T& n)
	{
		std::ostringstream stm;
		stm << n;
		return stm.str();
	}
}

int GetApp()
{
	return 0xA2Cu;
}

unsigned int HashString(const char *str, int len)
{
	if (!str) return 0;

	unsigned char *n = (unsigned char *)str;
	unsigned int acc = 0x55555555;

	if (len == 0)
	{
		while (*n)
			acc = (acc >> 27) + (acc << 5) + *n++;
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			acc = (acc >> 27) + (acc << 5) + *n++;
		}
	}
	return acc;
}

int GetDeviceHash(string deviceId)
{
	string v8 = deviceId;
	int v3;
	v8 += patch::to_string(GetApp() + 2280);
	v8 += "RT";
	v3 = HashString(v8.c_str(), 0);
	return v3;
}


int GetDeviceSecondaryHash(string macAddr)
{
	int v1;
	string v3 = "";
	for (std::string::size_type i = 0; i < macAddr.length(); ++i)
		v3 += tolower(macAddr[i]);
	v3 += "RT";
	v1 = HashString(v3.c_str(), 0);
	return v1;
}

string createLoginData(string swearFilter, string protocol, string gameVer, string fz, string lmode, string

	cbits, string hash2, string meta, string fhash, string rid, string platformID, string deviceVersion, string

	country, string hash, string mac, string wk)
{
	string loginPacket;
	loginPacket = "f|";
	loginPacket += swearFilter;
	loginPacket += "\n";
	loginPacket += "protocol|";
	loginPacket += protocol;
	loginPacket += "\n";
	loginPacket += "game_version|";
	loginPacket += gameVer;
	loginPacket += "\n";
	loginPacket += "fz|";
	loginPacket += fz;
	loginPacket += "\n";
	loginPacket += "lmode|";
	loginPacket += lmode;
	loginPacket += "\n";
	loginPacket += "cbits|";
	loginPacket += cbits;
	loginPacket += "\n";
	loginPacket += "hash2|";
	loginPacket += hash2;
	loginPacket += "\n";
	loginPacket += "meta|";
	loginPacket += meta;
	loginPacket += "\n";
	loginPacket += "fhash|";
	loginPacket += fhash;
	loginPacket += "\n";
	loginPacket += "rid|";
	loginPacket += rid;
	loginPacket += "\n";
	loginPacket += "platformID|";
	loginPacket += platformID;
	loginPacket += "\n";
	loginPacket += "deviceVersion|";
	loginPacket += deviceVersion;
	loginPacket += "\n";
	loginPacket += "country|";
	loginPacket += country;
	loginPacket += "\n";
	loginPacket += "hash|";
	loginPacket += hash;
	loginPacket += "\n";
	loginPacket += "mac|";
	loginPacket += mac;
	loginPacket += "\n";
	loginPacket += "wk|";
	loginPacket += wk;
	loginPacket += "\n";
	return loginPacket;
}

/*void SendPacketRaw(int a1, void *a2, size_t a3, void *a4, ENetPeer* a5, int a6)
{
ENetPacket *v6;
ENetPacket *v7;

if (a5)
{
if (a1 == 4 && *((BYTE *)a2 + 12) & 8)
{
v7 = enet_packet_create(0, a3 + *((DWORD *)a2 + 13) + 5, a6);
*(DWORD *)v7->data = 4;
memcpy((char *)v7->data + 4, a2, a3);
memcpy((char *)v7->data + a3 + 4, a4, *((DWORD *)a2 + 13));
enet_peer_send(a5, 0, v7);
}
else
{
v6 = enet_packet_create(0, a3 + 5, a6);
*(DWORD *)v6->data = a1;
memcpy((char *)v6->data + 4, a2, a3);
enet_peer_send(a5, 0, v6);
}
}
}*/


char* CreateGameUpdatePacketWithExtraDataAtEnd(int a1)
{
	char* result = new char[a1 + 56];

	*(DWORD *)result = 0;
	*(DWORD *)(result + 4) = 0;
	*(DWORD *)(result + 8) = 0;
	*(DWORD *)(result + 12) = 8;
	*(DWORD *)(result + 16) = 0;
	*(DWORD *)(result + 20) = 0;
	*(DWORD *)(result + 24) = 0;
	*(DWORD *)(result + 28) = 0;
	*(DWORD *)(result + 32) = 0;
	*(DWORD *)(result + 36) = 0;
	*(DWORD *)(result + 40) = 0;
	*(DWORD *)(result + 44) = 0;
	*(DWORD *)(result + 48) = 0;
	*(DWORD *)(result + 52) = a1;
	return result;
}



std::string colorstr(string str)
{
	string chrs = "0123456789bwpo^$#@!qertas";
	string s;
	for (int i = 0; i < str.length(); i++)
	{
		s += "`";
		char* x;
		x = (char*)malloc(2);
		x[0] = chrs[rand() % chrs.length()];
		x[1] = 0;
		string y = x;
		s += y;
		free(x);
		s += str[i];
	}
	return s;
}

std::string colorstr2(string str)
{
	string chrs = "0123456789bwpo^$#@!qertas";
	char* x;
	x = (char*)malloc(2);
	x[0] = chrs[rand() % chrs.length()];
	x[1] = 0;
	string y = x;
	free(x);
	return "`" + y + str;
}


/*string text_encode(char* text)
{
	string ret = "";
	while (text[0] != 0)
	{
		switch (text[0])
		{
		case '\n':
			ret += "\\n";
			break;
		case '\t':
			ret += "\\t";
			break;
		case '\b':
			ret += "\\b";
			break;
		case '\\':
			ret += "\\\\";
			break;
		case '\r':
			ret += "\\r";
			break;
		default:
			ret += text[0];
			break;
		}
		text++;
	}
	return ret;
}*/
