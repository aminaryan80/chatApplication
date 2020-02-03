
# Welcome to the IRC Chat_Application
## Table of contents
* [Getting Started](#getting-started)
* [Client Functions](#client-functions)
  * [Menus](#menus)
  * [Account Managing Functions](#account-managing-functions)
  * [Entering/Exiting Channels Functions](#entering-or-exiting-channels-functions)
  * [In-Channel Functions](#in-channel-functions)
  * [Settings Functions](#settings-functions)
  * [Others](#others)
 * [Server Functions](#server-functions)
   * [Menu](#menu)
    * [Account Managing Functions](#server-account-managing-functions)
   * [Entering/Exiting Channels Functions](#server-entering-or-exiting-channels-functions)
   * [In-Channel Functions](#server-in-channel-functions)
   * [Others](#server-others)
  * [Enjoy This Program!](#enjoy-this-program)

## Getting started
First you have to add winsock2 library before running client/server. you can find out more about winsock2 library in [here](https://docs.microsoft.com/en-us/windows/win32/api/winsock2/).
## Client Functions

### Menus

#### `void accountMenu() & void mainMenu() & void channelMenu()`
These functions shows menus to user wait for user’s response. If user choose one of options in menus, these functions call requested function. otherwise, an error message shows up and user have to try again.

### Account Managing Functions

#### `void myRegister() & int myLogin() & void myLogout()`

Managing account actions are up-to these functions. `myRegister()` and `myLogin()` get username and password from user and check if it’s allowed, then send a request for server in JSON format and process response and show a successful of error message to user according to servers response. If logging-in goes well, `myLogin()` returns 1 and if not, returns 0. `myLogout()` sends a logout request to server.

Username and password must be less than 20 characters and only include letters and numbers.

<h3 id="entering-or-exiting-channels-functions">Entering/Exiting Channels Functions</h3>

#### `int newChannel() & int joinChannel() & void leaveChannel()`

When user requests to join and create a new channel or leave current channel, these functions do that. `newChannel()` and `joinChannel()` get channel name from user and check if it’s allowed, then send a request for server in JSON format and process response and show a successful of error message to user according to servers response. If creating/joining goes well, these functions returns 1 and if not, returns 0. `leaveChannel()` sends a logout request to server.

**Notice:** Channel name also must be less than 20 characters and only include letters and numbers.
### In-Channel Functions

#### `void sendMessage() & void refresh() & void showChannelMembers()`

When user wants to do something in channel, client will call these functions. `sendMessage()` gets a message from user and check if it’s less than 70 characters and then sends a request to server and receives response. `refresh()` sends a request to server and receives a JSON array which includes new messages, then `refresh()` process the JSON array and prints new messages in console. `showChannelMembers()` is also receives a JSON array which includes channel members and after processing, prints list of channel members.

### Settings Functions

#### `void setting() & void colorSetting()`

This functions show settings menu and color settings menu and user can change the color of texts in this section.
### Others

#### `int isEnteredAllowed(char *)`

This function gets a string which contains username, password or channel name and checks if this string is allowed to use or not. if true returns 1 and if false returns 0.

#### `char** jsonParse(int, char*, …)`

Most of the JSON processing in client is done by this function. first parameter is count of items which we want to extract, second parameter is JSON which is going to be processed and the rest of parameters is the key of items that we want to extract. This function returns an array of strings which contains value of JSON items.

#### `void myConnect(char *)`

This function connects client to sever and sends request which is first parameter of the function and receives servers response and puts response to an global variable.
## Server Functions

### Menu

#### `int main()`

This function communicates with client and waits for receiveing a request. When it gets a request, it will process the request and call wanted function according to the request.

<h3 id="server-account-managing-functions">Account Managing Functions</h3>

#### `void myRegister(char *) & void myLogin(char *) & void myLogout(char *)`

These functions get clients request. `myRegister(char *)` checks the entered username is available, then creates a new account in database; Otherwise sends a failure message to client. `myLogin(char *)` checks the entered username and password is valid, then creates a token and save it for user and change users status to online. `myLogout(char *)` gets token and checks if the token is valid, then removes users token and changes user status to offline.

<h3 id="server-entering-or-exiting-channels-functions">Entering/Exiting Channels Functions</h3>

#### `void myNewChannel(char *) & void myJoinChannel(char *) & void myLeaveChannel(char *)`

These functions first check if given token and channel name is valid. `myNewChannel(char *)` creates a new channel in database and send first message which is “user created this channel”. `myJoinChannel(char *)` joins user to channel and send a new message which is “user joined the channel”. `myLeaveChannel(char *)` exits user from the channel and send a new message which is “user left the channel”.

<h3 id="server-in-channel-functions">In-Channel Functions</h3>

#### `void mySendMessage(char *) & void myRefresh(char *) & void myChannelMembers(char *)`

These functions first check if given token and channel name is valid. `mySendMessage(char *)` reads channel file and parses it to JSON array which contains all of channel messages, then adds new message to this array and write it in file again. `myRefresh(char *)` reads channel file and parses it to JSON array, then creates a new JSON array from new messages and sends it to client. `myChannelMembers(char *)` creates a new JSON array from users which are joined to channel and sends it to client.

<h3 id="server-others">Others</h3>

#### `int fileExists(const char *)`

This function gets an address and check if file exists in this address, then returns 1. Otherwise returns 0.

#### `void tokenCreator(char *)`

This function gets a char pointer and creates a token and puts it in char pointer.

#### `int authentication(char *)`

This function gets a token and checks if it’s valid, then returns index of user with this token. Otherwise, returns -1.

#### `void success(char *type, char *content)`

This function creates a JSON object according to the pattern below and sends it to client.

<p align=center><code>{“type”:<type>,”content”:<content>}</code></p>

#### `void success(char *type, char *content)`

This function creates a JSON object according to the pattern below and sends it to client.

<p align=center><code>{“type”:”Error”,”content”:<content>}</code></p>

# Enjoy This Program!

- developed by Mohammadamin Aryan