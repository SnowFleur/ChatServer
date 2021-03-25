
#include"ChatClient.h"
int main() {

	std::locale::global(std::locale("kor"));
	std::wcout.imbue(std::locale("kor")); // ?�것??추�??�면 ?�다.
	//std::wcin.imbue(std::locale("kor")); 

	CChatClient *pChatClient = NULL;
	pChatClient = new CChatClient();

	if (pChatClient == NULL)
		std::cout << "Chat Clinet pointer Is Null\n";

#ifdef _DEBUG
	std::cout << "Not Release Mode\n";
#endif
	pChatClient->Run(TCP_TYPE);

	if (pChatClient != NULL)
		delete pChatClient;
}




