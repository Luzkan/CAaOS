//telnet localhost 8888 to connect

#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h> 
#include <arpa/inet.h>   
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h>

#define PORT 8888  
#define MAX_CON 30
#define MAXMSG 1025
#define MAXNICK 30

typedef struct {
    int fd;
    char *name;
 } User;

static User *users[MAX_CON]; 
static unsigned int usr_count = 0;
fd_set readfds, read_fd_set;
static int srv_sock;

static User *newUser(int fd); 
int usersID(int fd);
int findUser(char *name);
char *listUsers();
int userQuit(User *usr); 

char *clientRead(int filedes);
int clientWrite(int filedes, char *msg);
void inputManager(User *sender, char *message); 

int main(int argc , char *argv[]) {   
    int opt = 1;   
    int master_socket , addrlen , new_socket , client_socket[30] ,  
          max_clients = MAX_CON , activity, i , valread , sd;   
    int max_sd;   
    struct sockaddr_in address;   

    char username[MAXNICK];
    char buffer[MAXMSG]; 
         
    // Welcome Message
    char *welcomeMsg = "Type in your nickname using /nick:\r\n";   
     
    // Initialise all client_socket[] to 0 so not checked  
    for (i = 0; i < max_clients; i++) {   
        client_socket[i] = 0;   
    }   
         
    // Create a master socket  
    if((master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
     
    // Set master socket to allow multiple connections ,  
    // This is just a good habit, it will work without this  
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   
     
    // Type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( PORT );   
         
    // Bind the socket to localhost port 8888  
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
    printf("Listener on port %d \n", PORT);   
         
    // Try to specify maximum of 3 pending connections for the master socket  
    if (listen(master_socket, 3) < 0) {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
         
    // Accept the incoming connection  
    addrlen = sizeof(address);   
    puts("Waiting for connections ...");   
         
    while(1) {   
        // Clear the socket set  
        FD_ZERO(&readfds);   
     
        // Add master socket to set  
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;   
             
        // Add child sockets to set  
        for (i = 0; i<max_clients; i++) {   
            // Socket descriptor  
            sd = client_socket[i];   
                 
            // If valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            // Highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }   
     
        // Wait for an activity on one of the sockets , timeout is NULL ,  
        // So wait indefinitely  
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);   
       
        if ((activity < 0) && (errno!=EINTR)) {   
            printf("select error");   
        }   
             
        // If something happened on the master socket,  
        // then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds)) {   
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   

            if (usr_count < MAX_CON) {
                fprintf(stderr, "Server: new connection\n");
                users[usr_count] = newUser(new_socket);
                usr_count++;
                // Add new fd to be listned to
                FD_SET(new_socket, &readfds);
            }
             
            // Inform user of socket number - used in send and receive commands  
            printf("[New connection] Socket fd is %d, ip is: %s, port: %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs (address.sin_port));   
           
            // Send new connection greeting message  
            if(send(new_socket, welcomeMsg, strlen(welcomeMsg), 0) != strlen(welcomeMsg)) {   
                perror("send");   
            }   
                 
            puts(" - Welcome message sent successfully");   
                 
            // Add new socket to array of sockets  
            for (i = 0; i < max_clients; i++) {   
                // if position is empty  
                if(client_socket[i] == 0) {   
                    client_socket[i] = new_socket;   
                    printf(" - Adding to list of sockets as #%d\n" , i);   
                    break;   
                }   
            }   
        }   
             
        // Else its some IO operation on some other socket 
        for (i = 0; i < max_clients; i++){    
            sd = client_socket[i];   

            if (FD_ISSET(sd, &readfds)) {  
                // Check if it was for closing 
                // Also read incoming message  
                if ((valread = read(sd, buffer, 1024)) == 0) {   
                    // Somebody disconnected  
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);   
                    printf("[Client disconnected], ip %s, port %d\n" ,  
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                         
                    // Close the socket and mark as 0 in list for reuse  
                    close(sd);   
                    client_socket[i] = 0;   
                }   
                     
                // Manage User input:  
                else  {   
                    // Set the string terminating NULL byte on the end of the data read  
                    buffer[valread] = '\0';

                    int usr_id = usersID(sd);
                    inputManager(users[usr_id], buffer);
                }   
            }   
        }   
    }   
    return 0;   
}   

void inputManager(User *sender, char *message) {
    // User send message that starts with "/nick"
    if(strncmp(message, "/nick", 5) == 0) {
        // Signing in
        char nickname[MAXNICK];
        strncpy(nickname, message+6, MAXNICK);
        nickname[strlen(nickname)-2] = '\0';
        // Name is initialized with 0/
        // If it's not equal to 0 then it means user is already logged in
        if(sender->name[0] != '\0') {
            clientWrite(sender->fd, "You can't signup another account.\n");
            return;
        }

        // If he doesn't then check if anybody else have that nickname.
        else if(findUser(nickname) >= 0) {
            clientWrite(sender->fd, "This nickname is already taken.\n");
            return;
        }

        // Create new user
        else {
            strncpy(sender->name, nickname, MAXNICK);
        }

        // Inform server console about new reg
        printf("[New login]: %s\n", sender->name);

        // Create welcome messege upon sucessfull registration.
        char response[MAXNICK+9];
        snprintf(response, MAXNICK+9,"Successfully logged in as %s\n", sender->name);
        clientWrite(sender->fd, response);
        clientWrite(sender->fd, "Available commands:\n");
        clientWrite(sender->fd, "/list - lists all currently logged in users\n");
        clientWrite(sender->fd, "/exit - closes minitalk\n");
        clientWrite(sender->fd, "\nUsers Online:\n");
        clientWrite(sender->fd, listUsers(sender));
        clientWrite(sender->fd, "\nType in: '<username> <message>' to send text.\n");
    }
    // User doesn't have a nickname assigned
    else if(sender->name[0] == '\0') {
        clientWrite(sender->fd, "Type in: '/nick <nickname>' to sign-up.\n");
        return;
    }
    // User quits the program
    else if(strncmp(message, "/exit", 5) == 0) {
        userQuit(sender); 
    }
    // User checks if anybody is alive
    else if(strncmp(message, "/list", 5) == 0) {
        clientWrite(sender->fd, "\nUsers Online:\n");
        clientWrite(sender->fd, listUsers(sender));
    }
    // User is logged in
    else {
        // Format: <toWho> <theMessage>
        // Get nickname and message from input
        char sendTo[MAXNICK+1] = {0};
        for (int i = 0; i < MAXNICK+1; i++) {
            if(message[i] == ' ')
                break;
            sendTo[i] = message[i];
        }

        int userID = findUser(sendTo);

        // If userID == -1 then user was not found.
        if(userID < 0) {
            char resp[MAXNICK+23];
            snprintf(resp, MAXNICK+23,"User '%s' is offline.\n", sendTo);
            clientWrite(sender->fd, resp);
            return;
        }

        // Sending the message
        User *iWantThatMessage = users[userID];
        char sendingThisMessage[MAXMSG+MAXNICK+4];
        snprintf(sendingThisMessage, MAXMSG+MAXNICK+3, "[%s]: %s", sender->name, message+strlen(sendTo)+1);
        clientWrite(iWantThatMessage->fd, sendingThisMessage);
    }
}

// New User
static User *newUser(int fd){
    User *usr = malloc(sizeof(User));
    usr->fd = fd;
    usr->name = calloc(MAXNICK+1, sizeof(char));
    return usr;
} 

// Scans for all users online
char *listUsers(User *caller) {
    //list = MAX_CON*(MAXNICK<username> + "\n") + "\0"
    char *list = calloc(MAXNICK*MAX_CON+MAX_CON+1, sizeof(char));
    for(int i = 0; i < MAX_CON; i++) {
        if(users[i] != NULL && users[i] != caller) {
            strncat(list, users[i]->name,MAXNICK);
            strcat(list, "\n");
        }
    }
    if (list[0] == '\0')
        strcpy(list, "Nobody is logged in.\n");
    return list;
}

// User exits
int userQuit(User *usr){ 
    // Get userID
    int i = 0;
    for(i = 0; i < MAX_CON; i++) {
        if (users[i] == usr)
            break;
    }
    printf("[Logout] User logging out: %d\n", i);
    if(usr == NULL)
        return -1;
    FD_CLR(usr->fd, &readfds);
    close(usr->fd);
    free(usr);
    // All users after the disconnected one
    // Are moved back one place in the list
    for(;i < MAX_CON - 1; i++)
        users[i] = users[i+1];
    usr_count--;
    return 0;
} 

char *clientRead(int filedes){ 
    char *buffer = calloc(MAXMSG, sizeof(char));
    int nbytes;
    nbytes = read(filedes, buffer, MAXMSG);
    // EOF: Connection Closed
    if (nbytes == 0)
        return NULL;
    // Read
    else {
        buffer[strlen(buffer)-1]='\0';
        return buffer;
    }
    return NULL; 
}

// Sends message to user
int clientWrite(int filedes, char *msg) {
    return write(filedes, msg, strlen(msg));
} 

// Looks up for the given user
// Used to lookup if online or if nickname is in use
// JESUS HERE I SPEND LIKE 3,5 HOURS TO FIND THAT ONE OF STRINGS
// WAS WITH \N AT THE END SO STRCMP COULDN'T COMPARE
// MY GOD JESUS CHRIST AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
int findUser(char *name) {
    printf("[Lookup]: Searching for name: %s\n", name);
    for(int i = 0; i < MAX_CON; i++)
        if(users[i] != NULL){
            if(users[i]->name != NULL){
                if(strcmp(users[i]->name, name) == 0){
                    printf("[Message] Sent to %s\n", name);
                    return i;
                }
            }
        }
    return -1;
} 

// Find users ID
int usersID(int fd) {
    for (int i = 0; i < MAX_CON; i++)
        if(users[i] != NULL)
            if(users[i]->fd == fd)
                return i;
    return -1;
} 