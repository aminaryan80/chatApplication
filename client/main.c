#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <winsock2.h>
#include <stdarg.h>
#include "cJSON.h"

#define PORT 12345

void accountMenu();
void myRegister();
int myLogin();
void mainMenu();
int newChannel();
int joinChannel();
void myLogout();
void channelMenu();
void sendMessage();
void refresh();
void showChannelMembers();
void leaveChannel();
char** jsonParse(int, char*, ...);
void myConnect(char*);
void showMessage(char*, int);

char* response = NULL;
char* token = "";
char channelName[21] = "";

int main()
{
    system("color 0C");
	printf("Welcome to the IRC Project\n");
	system("color 07");
	printf("Developed by Mohammadamin Aryan\n");
	printf("For settings, type \"s\" in menu.\n\n");
	accountMenu();
	return 0;
}

void accountMenu() {
    char tmp[3] = "";
    while(1) {
        printf("Account Menu:\n");
        printf("1: Register\n2: Login\n3: Exit\n");
        char status;
        while(1) {
            scanf("%[^\n]s", tmp);
            if(tmp[0] != 0)
                break;
            else
                scanf("\n");
        }
        status = tmp[0];
        memset(tmp, 0, sizeof(tmp));
        if(status == '1') {
            myRegister();
        } else if(status == '2') {
            if(!myLogin())
                continue;
            else {
                mainMenu();
                break;
            }
        } else if(status == '3') {
            return;
        } else if(status == 's') {
            showMessage("", 0);
            setting();
        } else {
            printf("\nSorry. Something went wrong. Please try again.\n\n");
        }
    }
}

void myRegister() {
    showMessage("Register:", 1);
    char id[2][256] = {};
    char buffer[1024] = "register ";
    char prints[2][20] = {"Enter your username", "Enter your password"};
    int status = 0;
    while(status < 2) {
        if(!status)
            printf("\n");
        printf("%s: (only characters and numbers & maximum 20 characters)\n", prints[status]);
        scanf("\n");
        scanf("%[^\n]s21", id[status]);
        if(strlen(id[status]) > 20) {
            memset(id[status], 0, sizeof(id[status]));
            printf("Your Entered %s has more than 20 characters. Please try again\n", !status ? "username" : "password");
        } else {
            if(!isEnteredAllowed(id[status]))
                continue;
            status++;
        }
    }
    strncat(buffer, id[0], strlen(id[0]));
	strncat(buffer, ", ", strlen(", "));
	strncat(buffer, id[1], strlen(id[1]));
	strncat(buffer, "\n", strlen("\n"));
    myConnect(buffer);
    char** responsePtr = jsonParse(2, response, "type", "content");
    if(strcmp(responsePtr[0], "Successful") != 0) {
        printf("%s Please try again.\n\n", responsePtr[1]);
        return;
    }
    showMessage("your account has been created successfully. you can now log in.", 2);
    return;
}

int myLogin() {
    showMessage("Login:", 1);
    char id[2][256] = {};
    int sizeId[2] = {};
    char buffer[1024] = "login ";
    char prints[2][20] = {"Enter your username", "Enter your password"};
    int status = 0;
    while(1){
        printf("\n%s: (maximum 20 characters)\n", prints[status]);
        scanf("\n");
        scanf("%[^\n]s", id[status]);
        if(strlen(id[status]) > 20) {
            memset(id[status], 0, sizeof(id[status]));
            printf("Your Entered %s has more than 20 characters. Please try again\n", !status ? "username" : "password");
            continue;
        } else
            status++;
        if(!isEnteredAllowed(id[status]))
            continue;
        printf("%s: (maximum 20 characters)\n", prints[status]);
        while(1) {
            while((id[status][sizeId[status]++] = getch()) != '\r') {
                if(id[status][sizeId[status] - 1] == 8) {
                    printf("\b \b");
                    sizeId[status] -= 2;
                } else
                    printf("*");
            }
            if(sizeId[status] >= 21) {
                memset(id[status], 0, sizeof(id[status]));
                printf("\n\nYour Entered %s has more than 20 characters. Please try again\n", !status ? "username" : "password");
                memset(id[status], 0, sizeof(id[status]));
                sizeId[status] = 0;
                continue;
            }
            printf("\n");
            id[status][sizeId[status] - 1] = '\0';
            if(!isEnteredAllowed(id[status])) {
                printf("\n%s: (maximum 20 characters)\n", prints[status]);
                memset(id[status], 0, sizeof(id[status]));
                sizeId[status] = 0;
            } else {
                break;
            }
        }
        sizeId[status] -= 2;
        break;
	}
	printf("\n");
	strncat(buffer, id[0], strlen(id[0]));
	strncat(buffer, ", ", strlen(", "));
	strncat(buffer, id[1], strlen(id[1]));
	strncat(buffer, "\n", strlen("\n"));
    printf("\n");
    myConnect(buffer);
    char** responsePtr = jsonParse(2, response, "type", "content");
    if(strcmp(responsePtr[0], "AuthToken") != 0) {
        system("cls");
        printf("%s Please try again.\n\n", responsePtr[1]);
        return 0;
    }
    token = responsePtr[1];
    showMessage("You've successfully logged in.", 2);
    return 1;
}

void mainMenu() {
    while(1) {
        printf("Main Menu:\n");
        printf("1: Create Channel\n2: Join Channel\n3: Logout\ns: Settings\n");
        char status;
        while((status = getchar()) == '\n');
        if(status == '1') {
            if(newChannel()) {
                channelMenu();
                break;
            } else {
                continue;
            }
        } else if(status == '2') {
            if(joinChannel()) {
                channelMenu();
                break;
            } else {
                continue;
            }
        } else if(status == '3') {
            myLogout();
            accountMenu();
            break;
        } else if(status == 's') {
            showMessage("", 0);
            setting();
        } else {
            printf("\nSorry. Something went wrong. Please try again.\n\n");
        }
    }
}

int newChannel() {
    showMessage("Create new channel:", 1);
    char tmpName[21] = "";
    char buffer[1024] = "create channel ";
    while(1) {
        printf("\nEnter the name of channel:\n");
        scanf("\n");
        scanf("%[^\n]s", tmpName);
        if(strlen(tmpName) > 20) {
            printf("Sorry. Your entered channel name has more than 20 characters. Please try again\n");
            continue;
        } else {
            if(!isEnteredAllowed(tmpName))
                continue;
            break;
        }
    }
    printf("%s %s\n", tmpName, tmpName);
    strncat(buffer, tmpName, strlen(tmpName));
    strncat(buffer, ", ", strlen(", "));
    strncat(buffer, token, strlen(token));
    strncat(buffer, "\n", strlen("\n"));
    myConnect(buffer);
    char** responsePtr = jsonParse(2, response, "type", "content");
    if(strcmp(responsePtr[0], "Successful") != 0) {
        printf("%s Please try again.\n\n", responsePtr[1]);
        return 0;
    }
    strncpy(channelName, tmpName, strlen(tmpName));
    char tmp[200] = "Channel \"";
    strncat(tmp, tmpName, strlen(tmpName));
    strcat(tmp, "\" successfully Created.");
    showMessage(tmp, 2);
    return 1;
}

int joinChannel() {
    showMessage("Join an existing channel:", 1);
    char tmpName[21] = "";
    while(1) {
        printf("\nEnter the name of channel:(maximum 20 characters)\n");
        scanf("%s", tmpName);
        if(strlen(tmpName) > 20) {
            printf("Sorry. Your Entered channel name has more than 20 characters. Please try again\n");
            continue;
        } else {
            if(!isEnteredAllowed(tmpName))
                continue;
            break;
        }
    }
    char buffer[1024] = "join channel ";
    strncat(buffer, tmpName, strlen(tmpName));
    strncat(buffer, ", ", strlen(", "));
    strncat(buffer, token, strlen(token));
    strncat(buffer, "\n", strlen("\n"));
    myConnect(buffer);
    char** responsePtr = jsonParse(2, response, "type", "content");
    if(strcmp(responsePtr[0], "Successful") != 0) {
        printf("%s Please try again.\n\n", responsePtr[1]);
        return 0;
    }
    strncpy(channelName, tmpName, strlen(tmpName));
    char tmp[200] = "You joined channel \"";
    strcat(tmp, tmpName);
    strcat(tmp, "\" successfully.");
    showMessage(tmp, 2);
    return 1;
}

void myLogout() {
    char buffer[1024] = "logout ";
    strncat(buffer, token, strlen(token));
    strncat(buffer, "\n", strlen("\n"));
    printf("\n");
    myConnect(buffer);
    char** responsePtr = jsonParse(2, response, "type", "content");
    if(strcmp(responsePtr[0], "Successful") != 0) {
        printf("%s Please try again.\n\n", responsePtr[1]);
        return;
    }
    showMessage("You've successfully logged out.", 2);
    return;
}

void channelMenu() {
    while(1) {
        printf("Channel Menu:\n");
        printf("1: Send Message\n2: Refresh\n3: Channel Members\n4: Leave Channel\ns: Settings\n");
        char status;
        while((status = getchar()) == '\n');
        if(status == '1') {
            // Send Message
            sendMessage();
        } else if(status == '2') {
            // Refresh
            refresh();
        } else if(status == '3') {
            // Channel Members
            showChannelMembers();
        } else if(status == '4') {
            // Leave Channel
            leaveChannel();
            mainMenu();
            break;
        } else if(status == 's') {
            showMessage("", 0);
            setting();
        } else {
            printf("\nSorry. Something went wrong. Please try again.\n\n");
        }
    }
}

void sendMessage() {
    char tmp[39] = "New message to \"";
    strncat(tmp, channelName, strlen(channelName));
    strcat(tmp, "\":");
    showMessage(tmp, 1);
    char message[71] = "";
    char buffer[1024] = "send ";
    while(1) {
        printf("\nWrite your message(maximum 70 characters):\n");
        scanf("\n");
        int i = 0;
        do {
            message[i++] = getchar();
        } while(message[i - 1] != '\n');
        message[i - 1] = '\0';
        if(strlen(message) > 70) {
            printf("Sorry. Your entered message has more than 70 characters. Please try again\n");
            memset(message, 0, 71);
            continue;
        } else
            break;
    }
    strncat(buffer, message, strlen(message));
    strncat(buffer, ", ", strlen(", "));
    strncat(buffer, token, strlen(token));
    strncat(buffer, "\n", strlen("\n"));
    myConnect(buffer);
    char** responsePtr = jsonParse(2, response, "type", "content");
    if(strcmp(responsePtr[0], "Successful") != 0) {
        printf("%s Please try again.\n\n", responsePtr[1]);
        return;
    }
    showMessage("Your message successfully sent.", 2);
    return;
}

void refresh() {
    char tmp[39] = "Channel \"";
    strncat(tmp, channelName, strlen(channelName));
    strcat(tmp, "\"");
    showMessage(tmp, 1);
    printf("Messages:\n");
    char buffer[1024] = "refresh ";
    strncat(buffer, token, strlen(token));
    strncat(buffer, "\n", strlen("\n"));
    myConnect(buffer);
    cJSON *json = cJSON_Parse(response);
    cJSON *contentArray = cJSON_GetObjectItemCaseSensitive(json, "content");
    cJSON *message = NULL;
    int isNoMessage = 1;
    cJSON_ArrayForEach(message, contentArray) {
        cJSON *sender = cJSON_GetObjectItemCaseSensitive(message, "sender");
        cJSON *content = cJSON_GetObjectItemCaseSensitive(message, "content");
        if (!cJSON_IsString(sender) || !cJSON_IsString(content))
        {
            printf("Message receiving error\n");
            return;
        }
        isNoMessage = 0;
        printf("\t%s : \"%s\"\n", sender->valuestring, content->valuestring);
    }
    if(isNoMessage)
        printf("\tno new messages\n");
    printf("\n");
    return;
}

void showChannelMembers() {
    char tmp[39] = "\"";
    strncat(tmp, channelName, strlen(channelName));
    strcat(tmp, "\" members:");
    showMessage(tmp, 1);
    char buffer[1024] = "channel members ";
    strncat(buffer, token, strlen(token));
    strncat(buffer, "\n", strlen("\n"));
    myConnect(buffer);
    cJSON *json = cJSON_Parse(response);
    cJSON *contentArray = cJSON_GetObjectItemCaseSensitive(json, "content");
    cJSON *name = contentArray->child;
    while(cJSON_IsString(name)) {
        printf("\t%s\n", name->valuestring);
        name = name->next;
    }
    printf("\n");
    return;
}

void leaveChannel() {
    char buffer[1024] = "leave ";
    strncat(buffer, token, strlen(token));
    strncat(buffer, "\n", strlen("\n"));
    printf("\n");
    myConnect(buffer);
    char** responsePtr = jsonParse(2, response, "type", "content");
    if(strcmp(responsePtr[0], "Successful") != 0) {
        printf("%s Please try again.\n\n", responsePtr[1]);
        return;
    }
    showMessage("You've successfully left the channel.", 2);
    return;
}

void setting() {
    char tmp[3] = "";
    while(1) {
        printf("Settings:\n");
        printf("1: Color setting\n2: Exit settings\n");
        char status;
        while(1) {
            scanf("%[^\n]s", tmp);
            if(tmp[0] != 0)
                break;
            else
                scanf("\n");
        }
        status = tmp[0];
        memset(tmp, 0, sizeof(tmp));
        if(status == '1') {
            system("cls");
            colorSetting();
            break;
        } else if(status == '2') {
            showMessage("", 0);
            break;
        } else {
            showMessage("Sorry. Something went wrong. Please try again.", 2);
        }
    }
}

void colorSetting() {
    char tmp[3] = "";
    while(1) {
        printf("Choose color:\n");
        printf("1: Blue\n2: Green\n3: Red\n4: Purple\n5: Yellow\n6: White\n");
        char status;
        while(1) {
            scanf("%[^\n]s", tmp);
            if(tmp[0] != 0)
                break;
            else
                scanf("\n");
        }
        status = tmp[0];
        memset(tmp, 0, sizeof(tmp));
        if(status == '1') {
            system("color 09");
            break;
        } else if(status == '2') {
            system("color 0A");
            break;
        } else if(status == '3') {
            system("color 0C");
            break;
        } else if(status == '4') {
            system("color 0D");
            break;
        } else if(status == '5') {
            system("color 0E");
            break;
        } else if(status == '6') {
            system("color 07");
            break;
        } else {
            showMessage("Sorry. Something went wrong. Please try again.", 2);
        }
    }
    showMessage("", 0);
}

int isEnteredAllowed(char* input) {
    int isNotAllowed = 0;
    for(int i = 0; input[i]; i++) {
        if(isalnum(input[i]) == 0)
            isNotAllowed = 1;
    }
    if(isNotAllowed == 1) {
        printf("Only characters and numbers allowed. Please try again\n");
        return 0;
    }
    return 1;
}

char** jsonParse(int num, char* buffer, ...) {
    char** result = malloc(num * 512);
    va_list vaList;
    va_start(vaList, num);
    cJSON *json = cJSON_Parse(buffer);
    cJSON **inputs = malloc(num * sizeof(*inputs));
    for(int i = 0; i < num; i++) {
        char* tmp = va_arg(vaList, char*);
        *(inputs + i) = cJSON_GetObjectItemCaseSensitive(json, tmp);
        //printf("%s : %s\n", tmp, (*(inputs + i))->valuestring);
        *(result + i) = (*(inputs + i))->valuestring;
    }
    va_end(vaList);
    return result;
}

void myConnect(char* buffer) {
    int client_socket;
	struct sockaddr_in servaddr;
	WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
        return;
    }
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	if (connect(client_socket, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection to the server failed...\n");
		exit(0);
	}
    send(client_socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, 1024);
    free(response);
    response = malloc(1025);
    memset(response, 0, 1025);
    char buff[1025] = "";
    int i = 1;
    while(recv(client_socket, buff, 1024, 0) != 0) {
        response = realloc(response, i * 1025);
        strcat(response, buff);
        memset(buff, 0, 1025);
        i++;
    }
    closesocket(client_socket);
}

void showMessage(char* message, int num) {
    system("cls");
    printf("%s", message);
    for(int i = 0; i < num; i++) {
        printf("\n");
    }
}
