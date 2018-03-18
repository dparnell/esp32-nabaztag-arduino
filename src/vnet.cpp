// VLISP Virtual Machine - 2006 - by Sylvain Huet
// Lowcost IS Powerfull
#include "vmem.h"
#include "properties.h"

#include "vloader.h"
#include "vnet.h"
#include "vlog.h"
#include "vinterp.h"

#include <WiFi.h>

int netState()
{  
  WiFi.getMode();
  int status = WiFi.status();
  // printf("netState: %d\n", status);

  switch(status) {
    case WL_NO_SHIELD:
      return 1;  // RT2501_S_IDLE
    case WL_IDLE_STATUS:
      return 1; // RT2501_S_IDLE
    case WL_NO_SSID_AVAIL:
      return 0; // RT2501_S_BROKEN
    case WL_SCAN_COMPLETED:
      return 2; // RT2501_S_SCAN
    case WL_CONNECTED:
      return 4; // RT2501_S_CONNECTED
    case WL_CONNECT_FAILED:
      return 0; // RT2501_S_BROKEN
    case WL_CONNECTION_LOST:
      return 3; // RT2501_S_CONNECTING
    case WL_DISCONNECTED:
      return 0; // RT2501_S_BROKEN
    default:
      return 0; // RT2501_S_BROKEN
  }
}

int netSend(char* src,int indexsrc,int lentosend,int lensrc,char* macdst,int inddst,int lendst,int speed)
{
  // NOT IMPLEMENTED
}

int netCb(char* src,int lensrc,char* macsrc)
{
  VPUSH(PNTTOVAL(VMALLOCSTR(src,lensrc)));
  VPUSH(PNTTOVAL(VMALLOCSTR(macsrc,6)));
  VPUSH(VCALLSTACKGET(sys_start,SYS_CBTCP));
  if (VSTACKGET(0)!=NIL) interpGo();
  else { VPULL();VPULL();}
  VPULL();
  return 0;
}

uint8_t mac[6];

char* netMac()
{
  // make sure that the WiFi stuff is initialized first
  WiFi.getMode();

  WiFi.macAddress(mac);
  return (char*)mac;
}

int netChk(char* src, int indexsrc, int lentosend, int lensrc, unsigned int val)
{
  unsigned short* p;

  if (indexsrc < 0) return val;
  if (indexsrc + lentosend > lensrc) lentosend = lensrc - indexsrc;
  if (lentosend <= 0) return val;

  src += indexsrc;
  p=(unsigned short*)src;

  val=((val << 8) & 0xff00) + ((val >> 8) & 0xff);
  while(lentosend > 1)
  {
	  val += *(p++);
	  lentosend -= 2;
  }

  if (lentosend) val += *(unsigned char*)p;

  val = (val >> 16) + (val & 0xffff);
  val = (val >> 16) + (val & 0xffff);
  val = ((val << 8) & 0xff00) + ((val >> 8) & 0xff);
  return val;
}

void netSetmode(int mode, char* ssid, int _chn)
{
  if(mode == 1) {
    // set access point mode
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid);
  } else {
    // set station mode
    WiFi.mode(WIFI_STA);
  }
}

void netScan(char* ssid)
{
  int nscan = WiFi.scanNetworks();
  for(int i = 0; i < nscan; i++) {
    const char* ssid = WiFi.SSID(i).c_str();
    uint8_t* bssid = WiFi.BSSID(i);

    VPUSH(PNTTOVAL(VMALLOCSTR((char*)ssid, strlen(ssid))));
    VPUSH(PNTTOVAL(VMALLOCSTR((char*)bssid,6)));
    VPUSH(PNTTOVAL(VMALLOCSTR((char*)bssid,6)));
    VPUSH(INTTOVAL(WiFi.RSSI(i)));
    VPUSH(INTTOVAL(WiFi.channel(i)));
    VPUSH(INTTOVAL(1)); // rateset
    VPUSH(INTTOVAL(WiFi.encryptionType(i)));
    VMKTAB(7);
  }
  VPUSH(NIL);
  while(nscan--) VMKTAB(2);
}

void netAuth(char* ssid, char* mac, char* bssid, int chn, int rate, int authmode, int encrypt, char* key)
{
  WiFi.begin(ssid, key);
}

void netSeqAdd(unsigned char* seq,int n)
{
  unsigned char res[4];
  unsigned int val;
  val=(seq[0]<<24)+(seq[1]<<16)+(seq[2]<<8)+seq[3];
  val+=n;
  res[3]=val; val>>=8;
  res[2]=val; val>>=8;
  res[1]=val; val>>=8;
  res[0]=val;
  VPUSH(PNTTOVAL(VMALLOCSTR((char*)res,4)));
}

void netPmk(char* ssid, char* key, char* buf)
{
  // NOT IMPLEMENTED
  printf("xxxx netPmk %s %s\n",ssid,key);
	strcpy(buf,"01234567012345670123456701234567");
}

int netRssi()
{
  return WiFi.RSSI();
}
