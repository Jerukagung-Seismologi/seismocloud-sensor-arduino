
#define DEFAULTHOST "www.sapienzaapps.it"
//#define DEFAULTHOST "192.0.2.20"

int readLine(EthernetClient *client, char* buf, int bufmax);
void httpRequest(char* host, unsigned short port, char* path, String postVars, char* reply, int replymax);
void httpAliveRequest();
void httpQuakeRequest();

void ShowSockStatus();
