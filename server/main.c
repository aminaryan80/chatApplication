#include<io.h>
#include<stdio.h>
#include<winsock2.h>
#include<string.h>
#include"mycJSON.h"

char *response = NULL;

void myRegister(char *);
void myLogin(char *);
int fileExists(const char *);
void myLogout(char *);
void myNewChannel(char *);
void myJoinChannel(char *);
void mySendMessage(char *);
void myRefresh(char *);
void myChannelMembers(char *);
void myLeaveChannel(char *);
int fileExists(const char *);
void tokenCreator(char *);
int authentication(char *);
void success(char *, char *);
void error(char *);

int loggedInUsersCount = 0;

struct Users
{
    char username[24];
    char token[38];
    int loggedIn;
    char inChannel[24];
    int messageCount;
} users[1000];

int main()
{
    system("mkdir Resources\\Users\\");
    system("mkdir Resources\\Channels\\");
    WSADATA wsa;
    SOCKET s, new_socket;
    struct sockaddr_in server, client;
    int c;
    char message[100] = "";
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
    printf("Initialised.\n");
    while (1)
    {
        memset(message, 0, sizeof(message));
        if((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        {
            printf("Could not create socket : %d", WSAGetLastError());
        }

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(12345);

        if( bind(s,(struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
        {
            printf("Bind failed with error code : %d", WSAGetLastError());
        }
        /// Listen to incoming connections
        listen(s, 3);
        printf("----------------------------------------------------------------------------------------------\n");
        c = sizeof(struct sockaddr_in);
        /// Accept and incoming connection
        new_socket = accept(s, (struct sockaddr *)&client, &c);
        if (new_socket == INVALID_SOCKET)
        {
            printf("accept failed with error code : %d", WSAGetLastError());
        }
        recv(new_socket, message, 100, 0);
        printf("From client : %s\n", message);
        if (!strncmp("register", message, strlen("register")))
        {
            myRegister(message);
        }
        else if (!strncmp("login", message, strlen("login")))
        {
            myLogin(message);
        }
        else if (!strncmp("create channel", message, strlen("create channel")))
        {
            myNewChannel(message);
        }
        else if (!strncmp("join channel", message, strlen("join channel")))
        {
            myJoinChannel(message);
        }
        else if (!strncmp("send", message, strlen("send")))
        {
            mySendMessage(message);
        }
        else if (!strncmp("refresh", message, strlen("refresh")))
        {
            myRefresh(message);
        }
        else if (!strncmp("channel members", message, strlen("channel members")))
        {
            myChannelMembers(message);
        }
        else if (!strncmp("leave", message, strlen("leave")))
        {
            myLeaveChannel(message);
        }
        else if (!strncmp("logout", message, strlen("logout")))
        {
            myLogout(message);
        }
        send(new_socket, response, strlen(response), 0);
        printf("To client : %s\n", response);
        closesocket(s);
    }
    WSACleanup();
    return 0;
}

void myRegister(char *request)
{
    request += 9;
    char *breaker = strchr(request, ',');
    char *address = calloc(1000, 1);
    strncpy(address, "Resources/Users/", strlen("Resources/Users/"));
    strncat(address, request, breaker - request);
    strncat(address, ".User.json", strlen(".User.json"));


    /// is user available
    if(fileExists(address))
    {
        error("this username is not available.");
        return;
    }

    /// json part
    char *username = calloc(25, 1);
    char *password = calloc(25, 1);
    strncpy(username, request, breaker - request);
    breaker += 2;
    strncpy(password, breaker, strlen(breaker) - 1);
    cJSON *monitor = cJSON_CreateObject();
    cJSON_AddStringToObject(monitor, "username", username);
    cJSON_AddStringToObject(monitor, "password", password);
    request = cJSON_Print(monitor);

    /// database part
    FILE *file = fopen(address, "w+");
    fprintf(file, "%s", request);
    fclose(file);
    cJSON_Delete(monitor);

    success("Successful", "-1");
}

void myLogin(char *request)
{
    request += 6;
    char *breaker = strchr(request, ',');
    char *address = calloc(1000, 1);
    strncpy(address, "Resources/Users/", strlen("Resources/Users/"));
    strncat(address, request, breaker - request);
    strncat(address, ".User.json", strlen(".User.json"));
    breaker += 2;

    /// is user available
    if(!fileExists(address))
    {
        error("Username is not valid.");
        return;
    }

    /// database part
    FILE *file = fopen(address, "r");
    char *tmp = calloc(1025, 1);
    int x = 0;
    while (1)
    {
        tmp[x] = fgetc(file);
        if (feof(file))
            break;
        x++;
    }
    fclose(file);

    /// processing
    cJSON *monitor_json = cJSON_Parse(tmp);
    cJSON *username = NULL;
    cJSON *password = NULL;
    username = cJSON_GetObjectItem(monitor_json, "username");
    password = cJSON_GetObjectItem(monitor_json, "password");
    breaker[strlen(breaker) - 1] = 0;
    if(strcmp(breaker, password->valuestring))
    {
        error("Wrong password.");
        return;
    }
    for(int i = 0; i < loggedInUsersCount; i++)
    {
        if(!strncmp(users[i].username, username->valuestring, strlen(username->valuestring)))
        {
            if(users[i].loggedIn == 0)
            {
                /// login
                users[i].loggedIn = 1;
                tokenCreator(users[i].token);
                success("AuthToken", users[i].token);
                return;
            }
            else
            {
                /// already logged in
                error("This user is already logged in.");
                return;
            }
        }
    }
    /// login and create new User struct
    strncpy(users[loggedInUsersCount].username, username->valuestring, strlen(username->valuestring));
    users[loggedInUsersCount].loggedIn = 1;
    tokenCreator(users[loggedInUsersCount].token);
    success("AuthToken", users[loggedInUsersCount].token);
    loggedInUsersCount++;
    cJSON_Delete(monitor_json);
}

void myLogout(char *request)
{
    char *token = strchr(request, ' ');
    token++;

    /// Authentication
    int isAuth = authentication(token);
    if(isAuth != -1)
    {
        users[isAuth].loggedIn = 0;
        memset(users[isAuth].token, 0, strlen(users[isAuth].token));
        success("Successful", "-1");
    }
}

void myNewChannel(char *request)
{
    request += 15;
    char *token = strchr(request, ',');
    char *channelName = calloc(25, 1);
    strncpy(channelName, request, token - request);
    token += 2;
    int isAuth = authentication(token);
    if(isAuth == -1)
        return;
    char *address = calloc(1000, 1);
    strncpy(address, "Resources/Channels/", strlen("Resources/Channels/"));
    strncat(address, channelName, strlen(channelName));
    strncat(address, ".Channel.json", strlen(".Channel.json"));


    /// is user available
    if(fileExists(address))
    {
        error("Channel name is not available.");
        return;
    }


    cJSON *monitor = cJSON_CreateObject();
    cJSON_AddStringToObject(monitor, "name", users[isAuth].username);

    char content[100] = "";
    sprintf(content, "%s created %s.", users[isAuth].username, channelName);
    cJSON *messages;
    messages = cJSON_AddArrayToObject(monitor, "messages");
    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "sender", "server");
    cJSON_AddStringToObject(message, "content", content);
    cJSON_AddItemToArray(messages, message);

    request = cJSON_Print(monitor);
    printf("%s\n", request);
    /// database part
    FILE *file = fopen(address, "w+");
    fprintf(file, "%s", request);
    fclose(file);
    cJSON_Delete(monitor);

    strncpy(users[isAuth].inChannel, channelName, strlen(channelName));
    success("Successful", "-1");
}

void myJoinChannel(char *request)
{
    request += 13;
    char *token = strchr(request, ',');
    char *channelName = calloc(24, 1);
    strncpy(channelName, request, token - request);
    token += 2;
    int isAuth = authentication(token);
    if(isAuth == -1)
        return;
    char *address = calloc(1000, 1);
    strncpy(address, "Resources/Channels/", strlen("Resources/Channels/"));
    strncat(address, channelName, strlen(channelName));
    strncat(address, ".Channel.json", strlen(".Channel.json"));


    /// is user available
    if(!fileExists(address))
    {
        error("Channel name is not available.");
        return;
    }

    FILE *file = fopen(address, "r+");
    char *tmp = calloc(5120, 1);
    int i = 0;
    while (1)
    {
        tmp[i] = fgetc(file);
        if (feof(file))
            break;
        i++;
    }
    fclose(file);

    /// processing
    char content[100] = "";
    sprintf(content, "%s joined the channel.", users[isAuth].username);
    cJSON *monitor_json = cJSON_Parse(tmp);
    cJSON *messages = NULL;
    messages = cJSON_GetObjectItem(monitor_json, "messages");
    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "sender", "server");
    cJSON_AddStringToObject(message, "content", content);
    cJSON_AddItemToArray(messages, message);

    request = cJSON_Print(monitor_json);

    file = fopen(address, "w");
    fprintf(file, request);
    fclose(file);
    strncpy(users[isAuth].inChannel, channelName, strlen(channelName));
    success("Successful", "-1");
    cJSON_Delete(monitor_json);
}

void mySendMessage(char *request)
{
    request += 5;
    char *token = strchr(request, ',');
    char *messageContent = calloc(74, 1);
    strncpy(messageContent, request, token - request);
    token += 2;
    int isAuth = authentication(token);
    if(isAuth == -1)
        return;
    char *address = calloc(1000, 1);
    strncpy(address, "Resources/Channels/", strlen("Resources/Channels/"));
    strncat(address, users[isAuth].inChannel, strlen(users[isAuth].inChannel));
    strncat(address, ".Channel.json", strlen(".Channel.json"));


    /// is user available
    if(!fileExists(address))
    {
        error("Channel name is not available.");
        return;
    }

    FILE *file = fopen(address, "r+");
    char *tmp = calloc(5120, 1);
    int i = 0;
    while (1)
    {
        tmp[i] = fgetc(file);
        if (feof(file))
            break;
        i++;
    }
    fclose(file);

    /// processing
    cJSON *monitor_json = cJSON_Parse(tmp);
    cJSON *messages = NULL;
    messages = cJSON_GetObjectItem(monitor_json, "messages");
    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "sender", users[isAuth].username);
    cJSON_AddStringToObject(message, "content", messageContent);
    cJSON_AddItemToArray(messages, message);

    request = cJSON_Print(monitor_json);

    file = fopen(address, "w");
    fprintf(file, request);
    fclose(file);
    success("Successful", "-1");
    cJSON_Delete(monitor_json);
}

void myRefresh(char *request)
{
    request += 8;
    char *token = request;
    int isAuth = authentication(token);
    if(isAuth == -1)
        return;
    char *address = calloc(1000, 1);
    strncpy(address, "Resources/Channels/", strlen("Resources/Channels/"));
    strncat(address, users[isAuth].inChannel, strlen(users[isAuth].inChannel));
    strncat(address, ".Channel.json", strlen(".Channel.json"));


    /// is user available
    if(!fileExists(address))
    {
        error("Channel name is not available.");
        return;
    }

    FILE *file = fopen(address, "r+");
    char *tmp = calloc(5120, 1);
    int i = 0;
    while (1)
    {
        tmp[i] = fgetc(file);
        if (feof(file))
            break;
        i++;
    }
    fclose(file);

    /// processing
    cJSON *monitor_json = cJSON_Parse(tmp);
    cJSON *monitor_pack = cJSON_CreateObject();
    cJSON *messages = NULL;
    cJSON *packs = NULL;
    cJSON_AddStringToObject(monitor_pack, "type", "List");
    packs = cJSON_AddArrayToObject(monitor_pack, "content");
    messages = cJSON_GetObjectItem(monitor_json, "messages");
    int size = cJSON_GetArraySize(messages);
    if(size == users[isAuth].messageCount) {
        cJSON_AddStringToObject(monitor_pack, "content", "-1");
    }
    for(int i = users[isAuth].messageCount; i < size; i++)
    {
        cJSON *message = cJSON_GetArrayItem(messages, i);
        cJSON *sender = NULL;
        cJSON *content = NULL;
        sender = cJSON_GetObjectItem(message, "sender");
        content = cJSON_GetObjectItem(message, "content");

        cJSON *pack = cJSON_CreateObject();
        cJSON_AddStringToObject(pack, "sender", sender->valuestring);
        cJSON_AddStringToObject(pack, "content", content->valuestring);
        cJSON_AddItemToArray(packs, pack);
    }
    response = cJSON_Print(monitor_pack);
    printf("%s\n", response);
    users[isAuth].messageCount = size;
    cJSON_Delete(monitor_json);
    cJSON_Delete(monitor_pack);
}

void myChannelMembers(char *request)
{
    request += 16;
    char *token = request;
    int isAuth = authentication(token);
    if(isAuth == -1)
        return;
    char *address = calloc(1000, 1);
    strncpy(address, "Resources/Channels/", strlen("Resources/Channels/"));
    strncat(address, users[isAuth].inChannel, strlen(users[isAuth].inChannel));
    strncat(address, ".Channel.json", strlen(".Channel.json"));

    /// is user available
    if(!fileExists(address))
    {
        error("Channel name is not available.");
        return;
    }

    /// processing
    cJSON *monitor_pack = cJSON_CreateObject();
    cJSON *packs = NULL;
    cJSON_AddStringToObject(monitor_pack, "type", "List");
    packs = cJSON_AddArrayToObject(monitor_pack, "content");

    for(int i = 0; i < loggedInUsersCount; i++)
    {
        if(!strncmp(users[isAuth].inChannel, users[i].inChannel, strlen(users[isAuth].inChannel)))
        {
            cJSON_AddItemToArray(packs, cJSON_CreateString(users[i].username));
        }
    }
    response = cJSON_Print(monitor_pack);
    cJSON_Delete(monitor_pack);
}

void myLeaveChannel(char *request)
{
    request += 6;
    char *token = request;
    int isAuth = authentication(token);
    if(isAuth == -1)
        return;
    char *address = calloc(1000, 1);
    strncpy(address, "Resources/Channels/", strlen("Resources/Channels/"));
    strncat(address, users[isAuth].inChannel, strlen(users[isAuth].inChannel));
    strncat(address, ".Channel.json", strlen(".Channel.json"));


    /// is user available
    if(!fileExists(address))
    {
        error("Channel name is not available.");
        return;
    }

    FILE *file = fopen(address, "r+");
    char *tmp = calloc(5120, 1);
    int i = 0;
    while (1)
    {
        tmp[i] = fgetc(file);
        if (feof(file))
            break;
        i++;
    }
    fclose(file);

    /// processing
    char content[100] = "";
    sprintf(content, "%s left the channel.", users[isAuth].username);
    cJSON *monitor_json = cJSON_Parse(tmp);
    cJSON *messages = NULL;
    messages = cJSON_GetObjectItem(monitor_json, "messages");
    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "sender", "server");
    cJSON_AddStringToObject(message, "content", content);
    cJSON_AddItemToArray(messages, message);

    request = cJSON_Print(monitor_json);

    file = fopen(address, "w");
    fprintf(file, request);
    fclose(file);

    memset(users[isAuth].inChannel, 0, strlen(users[isAuth].inChannel));
    users[isAuth].messageCount = 0;
    success("Successful", "-1");
    cJSON_Delete(monitor_json);
}

int fileExists(const char *filename)
{
    FILE *file;
    if (file = fopen(filename, "r"))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

void tokenCreator(char *token)
{
    srand(time(0));
    for(int i = 0; i < 37;)
    {
        int r = rand() % 59;
        if(r <= 58 && r != 0 && r != 28 && r != 27 && r != 29 && r != 30 && r != 32)
        {
            token[i] = r + 64;
            i++;
        }
    }
    token[37] = 0;
}

int authentication(char *token)
{
    for(int i = 0; i < loggedInUsersCount; i++)
    {
        if(!strncmp(token, users[i].token, strlen(token) - 1))
        {
            return i;
        }
    }
    error("Authentication failed!");
    return -1;
}

void success(char *type, char* content)
{
    cJSON *monitor = cJSON_CreateObject();
    cJSON_AddStringToObject(monitor, "type", type);
    cJSON_AddStringToObject(monitor, "content", content);
    response = cJSON_Print(monitor);
    cJSON_Delete(monitor);
}

void error(char *error)
{
    cJSON *monitor = cJSON_CreateObject();
    cJSON_AddStringToObject(monitor, "type", "Error");
    cJSON_AddStringToObject(monitor, "content", error);
    response = cJSON_Print(monitor);
    cJSON_Delete(monitor);
}
