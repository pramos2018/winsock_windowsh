// ==============================================================================================
// Title#1: Winsock 5 in 1 with Graphical Interface (PortScanner, TCP Server, TCP Client, UPD Server and UDP Client)
// Goals: 1) To Understand the Winsock Library;
//        2) To Understand the windows.h library and to create a GUI in C/C++
//        3) To Understand how threads work in C/C++

// Author  Paulo Ramos  Date : July 2018
// Compiled using CodeBlocks (GCC)

// ================================= CREDITS ==================================================
// Winsock.h Clients and Servers based on Codes availabe at: https://www.binarytides.com

// Windows.h Graphical Interface based on Ryan Shanks Windows.h C++ Tutorials available on youtube at:
//      https://www.youtube.com/watch?v=m0gdAwTp6qg&list=PLE8jJwiAlPPrdN2VGrF9cz_8Owt4uvOUH&index=1

// Socket Event Handler based on code available at:
//      https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancediomethod5b.html

// pthread.h Threads based on the Engineer Man tutorial "Threading Basics in C" available on youtube at:
//      https://www.youtube.com/watch?v=nVESQQg-Oiw

// Multi-line Edit, Event Handler, CreateWindow parameters, windows.h other options, etc,
//      based on Microsoft Online Help at: https://msdn.microsoft.com/

// ================================ IMPORTANT ==================================================
// Note: it is necessary to add the c:\windows\system32\ws2_32.dll library to the project.
// Go to menu: Project -> Build Options -> Debug -> Linker Options -> Add -> ..\ws2_32.dll (find it on windows)
// ==============================================================================================


#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include<io.h>
#include<winsock2.h>
#include <pthread.h>

#pragma comment(lib,"c:\windows\system32\ws2_32.dll") //Winsock Library - Add it to the project

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC oldEditProc;

void gui_components(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void event_handler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void combobox_items();
void print_out(char * text);
void clear_screen();
void send_messages();

HWND CreateLabel(HWND hwnd, char * text, int x, int y, int w, int h);
HWND CreateEntry(HWND hwnd, char * text, int x, int y, int w, int h);
HWND CreateButton(HWND hwnd, char * text, int id, int x, int y, int w, int h);
HWND CreateListBox(HWND hwnd, char * text, int id, int x, int y, int w, int h);
HWND CreateComboBox(HWND hwnd, char * text, int id, int x, int y, int w, int h);
HWND CreateTextArea(HWND hwnd, char * text, int id, int x, int y, int w, int h);
HWND CreateCheckBox(HWND hwnd, char * text, int id, int x, int y, int w, int h);

void start_winsock();
SOCKET create_socket(int domain, int type, int protocol);
void check_bind_socket(int bd);
void check_socket_error(int sk, char* err_msg);
struct sockaddr_in set_sock_addr(struct sockaddr_in sock, int family, int addr, int port, int opt);

int udp_client();
int port_scanner();
int tcp_client();

void send_msg_sock(SOCKET sk, char * message);
void receive_msg_sock(SOCKET sk);
void send_msg_sock_addr(SOCKET sk, sockaddr_in saddr_x, char * message);
void receive_msg_sock_addr(SOCKET sk, sockaddr_in saddr_x);
int socket_event_handler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void *scan_port(void *value);
int set_port_scanner();
void start_tcp_server_thread();
void start_udp_server_thread();
void *udp_server(void *value);
void *tcp_server(void *value);
void reset_gui();
void adjustable_gui(HWND hwnd);
void close_process();
void check_commands(char * text);
void help_summary();
int count_newline_intext(char *text);
int check_connection(SOCKET sk);
//=================== Global Variables and Constants ============================================
// typedef definition

#define WM_SOCKET (WM_USER + 1)
#define BUFLEN 512  //Max length of buffer

#define PORT_SCANNER 0;
#define TCP_SERVER 1;
#define TCP_CLIENT 2;
#define UDP_SERVER 3;
#define UDP_CLIENT 4;

//Global Variables Necessary for the Winsock Clients ans Servers
SOCKET s, new_socket;
WSADATA wsa;
struct sockaddr_in server, client, si_other, sa;
int slen = sizeof(struct sockaddr_in) , recv_len, ctx=0;
char buf[BUFLEN], message[BUFLEN], hostname[100], line[256];
fd_set readfds;
pthread_t thread_udp, thread_tcp, scanner_thread; // Threads to be canceled when closing

// Global Variables Necessary for the Graphical Interface
HWND hwnd;/* This is the handle for our window */
HWND textfield, button, button2, button3,btn_clear, TextBox, listbox, combobox, textarea;
HWND txt_msg, txt_port1, txt_port2, txt_host, label_1, label_2, label_3, check_box_1;

char textSaved[100], label1[20]="Label Test";
int gwtstat = 0, app_option = 0, startport, endport, thread_cancel_flag = 0, ck_box = 1;

char SERVER[100]= "127.0.0.1";  //ip address of udp server
int PORT = 8888;   //The port on which to listen for incoming data

//================================================================================================
//======== Windows.h Graphical Interface housing keep stuff - generated by CodeBlocks ============
//================================================================================================

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Winsock 5 in 1 wih GUI - PortScanner, Server (TCP/UDP), Client(TCP/UDP)"),       /* Title Text */
           WS_SYSMENU,          /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           500,                 /* The programs width */
           520,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);

    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    socket_event_handler(hwnd, message, wParam, lParam);
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            gui_components(hwnd, message, wParam, lParam);
            break;

        case WM_COMMAND:
            event_handler(hwnd, message, wParam, lParam);
            break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        case WM_CTLCOLORSTATIC:
        {
            HDC hdcStatic = (HDC)wParam; // or obtain the static handle in some other way
            //SetTextColor(hdcStatic, RGB(255, 0, 0)); // text color
            SetBkColor(hdcStatic, RGB(200,200,200));
            return (LRESULT)GetStockObject(NULL_BRUSH);
            break;
        }

        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
//==========================================================================================
//
//================ Change the Original GUI by Changing the Items below =====================
//==========================================================================================
//==========================================================================================
HWND CreateLabel(HWND hwnd, char * text, int x, int y, int w, int h) {
    return CreateWindow("STATIC",text, WS_VISIBLE | WS_CHILD | SS_SIMPLE,
                     x, y, w, h, hwnd, NULL, NULL, NULL);
}
HWND CreateEntry(HWND hwnd, char * text, int x, int y, int w, int h) {
    return CreateWindow("EDIT", text,WS_VISIBLE | WS_CHILD | WS_BORDER,
                             x, y, w, h, hwnd, NULL, NULL, NULL);
}
HWND CreateButton(HWND hwnd, char * text, int id, int x, int y, int w, int h) {
    return CreateWindow("BUTTON", text, WS_VISIBLE | WS_CHILD | WS_BORDER,
                         x, y, w, h, hwnd, (HMENU) id, NULL, NULL);
}
HWND CreateListBox(HWND hwnd, char * text, int id, int x, int y, int w, int h) {
    return CreateWindow("LISTBOX","", WS_VISIBLE | WS_CHILD | WS_BORDER,
                        x, y, w, h, hwnd, (HMENU) id, NULL, NULL);
}
HWND CreateComboBox(HWND hwnd, char * text, int id, int x, int y, int w, int h) {
    return CreateWindow("COMBOBOX","",CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
                        x, y, w, h, hwnd, (HMENU) id, NULL, NULL);
}
HWND CreateTextArea(HWND hwnd, char * text, int id, int x, int y, int w, int h) {
    return CreateWindow("EDIT", text, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL ,
                        x, y, w, h, hwnd, (HMENU) id, NULL, NULL);

}
HWND CreateCheckBox(HWND hwnd, char * text, int id, int x, int y, int w, int h) {
    return CreateWindow("BUTTON", text, WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
                         x, y, w, h, hwnd, (HMENU) id, 0, NULL);
}
void gui_components(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    //app_option: 0 - PortScanner, 1 - TCP Server, 2 - TCP Client, 3 - UDP Server, 4 - UDP Client
    // Add the GUI components here
    int x = 20, dx = 100, y = 20, dy = 30;
    CreateLabel(hwnd, "Select:", x, y, 100, 20);
    combobox = CreateComboBox(hwnd, "", 5, x+dx, y, 150, 125);

    x = x+170;
    CreateButton(hwnd, "Start", 101, x+dx, y, 70, 25);
    CreateButton(hwnd, "Stop", 102, x+dx+80, y, 70, 25);

    adjustable_gui(hwnd);

    y = y + dy+10;
    /* ================ Send Messages =============================*/
    x = 20;  y = y + dy+25 ;
    CreateLabel(hwnd, "Message:", x, y, 100, 20);
    txt_msg = CreateEntry(hwnd, "", x, y+dy, 375, 25);
    button3 = CreateButton(hwnd, "Send", 2, x+385, y+dy, 50, 25);
    oldEditProc = (WNDPROC)SetWindowLongPtr(txt_msg, GWLP_WNDPROC, (LONG_PTR)subEditProc);

    /* =================== Output TextArea =========================*/
    y = y+ dy*2+10; x = 20;
    CreateLabel(hwnd, "Output:", x, y, 100, 20);
    CreateButton(hwnd, "Help", 103, x+190, y, 50, 25);
    button  = CreateButton(hwnd, "Clear", 1, x+290, y, 50, 25);
    button2 = CreateButton(hwnd, "Exit", 3, x+390, y, 50, 25);
    textarea = CreateTextArea(hwnd, "", 6, x, y+dy, 450, 250);

    combobox_items();
    /* ===============================================================*/

}

void reset_gui() {
    //Deletes the Components to be Changed on a Screen
    DestroyWindow(txt_port1);
    DestroyWindow(txt_port2);
    DestroyWindow(txt_host);
    DestroyWindow(label_1);
    DestroyWindow(label_2);
    DestroyWindow(label_3);
    DestroyWindow(check_box_1);

}
void adjustable_gui(HWND hwnd) {
    // Components of the GUI Adjusted according to the Application selected
    char label1[20] = "Host/IP"; char label2[20] = "From Port "; char label3[10] = " To ";
    char val1[40] = "127.0.0.1"; char val2[10] = "75"; char val3[10] = "85";
    int x = 20, dx = 100, y = 20, dy = 30, ctg = 0;

    if (app_option==1 | app_option==3) ctg = 1;//Servers
    if (app_option==2 | app_option==4) ctg = 2;//Clients
    if (ctg!=0) {strcpy(label2, "Port:"); strcpy (val2, "8888");}

    if (ctg==0 | ctg == 2) {
        y = y + dy;
        label_1 = CreateLabel(hwnd, label1, x, y, 75, 25);
        txt_host = CreateEntry(hwnd, val1, x+dx, y, 150, 25);
    }
    if (ctg==0 | ctg == 2 | ctg == 1) {
        y = y + dy;
        label_2 = CreateLabel(hwnd, label2, x, y, 100, 25);
        txt_port1 = CreateEntry(hwnd, val2, x+dx, y, 50, 25);
    }
    if (ctg==0) {
        x = x + 150;
        label_3 = CreateLabel(hwnd, label3, x+10, y, 30, 25);
        txt_port2 = CreateEntry(hwnd, val3, x+50, y, 50, 25);
    }
    if (ctg==1) {
        x = x + 150;
        check_box_1 = CreateCheckBox(hwnd, "Echo Client",201, x+10, y, 100, 25);
        CheckDlgButton(hwnd, 201, ck_box);
    }
}

void event_handler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (LOWORD(wParam)) {
        case 1://Clear Screen
            clear_screen();
            break;
        case 2://Send Message to Socket
            send_messages();
            break;
        case 3://Exit
            PostQuitMessage (0);
            break;
        case 101:
            if (app_option==0) port_scanner();
            if (app_option==1) start_tcp_server_thread();
            if (app_option==2) tcp_client();
            if (app_option==3) start_udp_server_thread();
            if (app_option==4) udp_client();
            break;
        case 102: // Close App
            close_process();
            break;
        case 103: // Close App
            help_summary();
            break;
        case 201: // check box
            if (ck_box==1) ck_box = 0; else ck_box = 1;//flip-flop
            CheckDlgButton(hwnd, 201, ck_box);
            break;


    }
    if(HIWORD(wParam) == CBN_SELCHANGE){
        TCHAR  ListItem[256];
        int ItemIndex = SendMessage((HWND) lParam, (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);
        (TCHAR) SendMessage((HWND) lParam, (UINT) CB_GETLBTEXT,(WPARAM) ItemIndex, (LPARAM) ListItem);
        clear_screen();
        print_out(ListItem);
        app_option = ItemIndex;

        reset_gui();
        adjustable_gui(hwnd);
    }

}

void send_messages() {
    GetWindowText(txt_msg, textSaved, 100);
    sprintf(line, "[Client]: %s\r\n", textSaved);
    print_out(line);
    if (strlen(textSaved)>0) {
        if (app_option==1) send_msg_sock_addr(new_socket, client, textSaved);// TCP Server
        if (app_option==2) send_msg_sock(s, textSaved);                      // TCP Client
        if (app_option==3) send_msg_sock_addr(s, si_other, textSaved);       // UDP Server
        if (app_option==4) send_msg_sock_addr(s, si_other, textSaved);       // UDP Client

        check_commands(textSaved);
        if (strcmp(textSaved, "/help")==0) help_summary();
        if (strcmp(textSaved, "/cls")==0)  clear_screen();
    }
    SetWindowText(txt_msg, "");
}

/* Handling the Enter Key to send messages */
LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam){
   if(msg==WM_KEYDOWN && wParam==VK_RETURN) send_messages();
   return CallWindowProc(oldEditProc, wnd, msg, wParam, lParam);
}

void combobox_items() {
    SendMessage(combobox,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM) "Port Scanner\r\n");
    SendMessage(combobox,(UINT) CB_ADDSTRING,(WPARAM) 1,(LPARAM) "TCP Server\r\n");
    SendMessage(combobox,(UINT) CB_ADDSTRING,(WPARAM) 2,(LPARAM) "TCP Client\r\n");
    SendMessage(combobox,(UINT) CB_ADDSTRING,(WPARAM) 3,(LPARAM) "UDP Server\r\n");
    SendMessage(combobox,(UINT) CB_ADDSTRING,(WPARAM) 4,(LPARAM) "UDP Client\r\n");
    SendMessage(combobox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);//Item Selected
}
int count_newline_intext(char *text){
    int i = 0, nl=0;
    for (i=0; i<strlen(text); i++) {
        if (text[i]=='\n') nl++;
    }
    return nl;
}

void print_out(char * str2) {
    /*Routine made consulting msdn help guide at: https://msdn.microsoft.com/ */
    char * str1 ; int n, gt;
    n = GetWindowTextLength(textarea)+1;
    str1 = (char *)malloc(sizeof(char)*(n+2));
    str1[0]='\0';
    gt = GetWindowText(textarea, str1, n);

    if (strlen(str2)>0) {
        char * new_str ;
            new_str = (char *)malloc(sizeof(char)*(strlen(str1)+strlen(str2)+1));
            new_str[0] = '\0';   // ensures the memory is an empty string
            strcat(new_str,str1);
            strcat(new_str,str2);

            SendMessage(textarea, WM_SETTEXT, 0, (LPARAM) new_str);
            int nl = count_newline_intext(new_str);
            SendMessage(textarea, EM_LINESCROLL, 0, nl);
            free(new_str);
    }
    free(str1);
}

void clear_screen() {
    SendMessage(textarea, WM_SETTEXT, 0, (LPARAM) "");
}
void close_process() {
    thread_cancel_flag = 1;
    closesocket(s);
    closesocket(new_socket);
    WSACleanup();
    print_out("\r\n *** Closing Process *** \r\n");
}

//=============================================================================================
//======================= Common Routines for the Winsock Project==============================
//=============================================================================================
void start_winsock() {
    //Initialise winsock
    print_out("\r\nInitializing Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        sprintf(line, "Failed. Error Code : %d\n\r",WSAGetLastError());
        print_out(line);
        exit(EXIT_FAILURE);
    }
    print_out("Initialized.\r\n");
}
SOCKET create_socket(int domain, int type, int protocol) {
    //Create a socket
    SOCKET ls;
    if((ls = socket(domain, type, protocol)) == INVALID_SOCKET) {
        sprintf(line, "Could not create socket : %d\n\r" , WSAGetLastError());
        print_out(line);
        exit(EXIT_FAILURE);
    }
    return ls;
}
void check_bind_socket(int bd) {
    check_socket_error(bd, "Bind failed with error code :");
    print_out("Bind done\r\n");
}
void check_socket_error(int sk, char* err_msg) {
    if (sk == SOCKET_ERROR) {
        sprintf(line, "%s %s\r\n" , err_msg, WSAGetLastError());
        print_out(line);
        exit(EXIT_FAILURE);
    }
}
struct sockaddr_in set_sock_addr(struct sockaddr_in sock, int family, int addr, int port, int opt) {
    //Prepare the sockaddr_in structure
    sock.sin_family = family;
    if (opt==0) {sock.sin_addr.s_addr = addr;} else {sock.sin_addr.S_un.S_addr = addr;}
    sock.sin_port = htons(port);

    return sock;
}
int check_connection(SOCKET sk) {
    if (sk<=0) {
        print_out("\r\n\r\n *** Connection Closed *** \r\n\r\n");
        return -1;
    }
    return 1;
}

//===============================================================================================
// Socket Event Handler based on code available at:
// https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancediomethod5b.html
// Date: 20 July 2018
//===============================================================================================
int socket_event_handler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message)  {
    // Add the Event Handler here
        case WM_SOCKET:
            if (WSAGETSELECTERROR(lParam)){
                closesocket( (SOCKET) wParam);
                WSACleanup();
                break;
            }
            switch(WSAGETSELECTEVENT(lParam)) {
                case FD_ACCEPT:
                    break;
                case FD_READ:
                    if (app_option==2 | app_option==4) {
                        if (check_connection(s)==1) receive_msg_sock(s);
                    }
                    break;
                case FD_WRITE:
                    break;
                case FD_CLOSE:
                    closesocket((SOCKET)wParam);
                    WSACleanup();
                    break;
            }
            break;
    }
    return 0;
}

//===============================================================================================
// Simple Port Scanner Based on codes from BinaryTides
// https://www.binarytides.com/udp-socket-programming-in-winsock/
// Date: 06 July 2018
//===============================================================================================
int port_scanner() {
    char port_1 [20], port_2[20];
    GetWindowText(txt_host, hostname, 100);
    GetWindowText(txt_port1, port_1, 100);
    GetWindowText(txt_port2, port_2, 100);

    startport = atoi(port_1);
    endport = atoi(port_2);
    sprintf(line, "Starting Port Scanner: \r\nHost: %s \r\nInitial Port: %d \r\nFinal Port %d\r\n\r\nScanning...\r\n", hostname, startport, endport);

    print_out(line);
    set_port_scanner();
}

int set_port_scanner(){
    struct hostent *host;
    int err, i;

    strncpy((char *)&sa,"",sizeof sa);
    sa.sin_family = AF_INET; //this line must be like this coz internet
    start_winsock();//Initialise winsock

    if(isdigit(hostname[0])) { // translating the hostname to ip when necessary
        sa.sin_addr.s_addr = inet_addr(hostname); //get ip into s_addr
    } else if((host=gethostbyname(hostname)) != 0){
        strncpy((char *)&sa.sin_addr , (char *)host->h_addr_list[0] , sizeof sa.sin_addr);
    } else {
        print_out("Error resolving hostname");
        exit(EXIT_FAILURE);
    }
    //Start the portscan loop
    print_out("Starting the scan loop...\r\n");
    thread_cancel_flag = 0;
    pthread_create(&scanner_thread, NULL, scan_port, NULL);

    return 0;
}

void *scan_port(void *value) {
    int err, i;
    for(i = startport ; i<= endport ; i++) {
        s = create_socket(AF_INET , SOCK_STREAM , 0);
        sprintf(line, "%d\r\n", i);
        print_out(line);
        sa.sin_port = htons(i);
        //connect to the server with that socket
        err = connect(s , (struct sockaddr *)&sa , sizeof sa);
        if(err != SOCKET_ERROR) {//connection not accepted
            sprintf(line, "%s %-5d accepted            \r\n" , hostname , i);
            print_out(line);
            if( shutdown( s ,SD_BOTH ) == SOCKET_ERROR )  {
                perror("\r\nshutdown");// perror function prints an error message to stderr
                exit(EXIT_FAILURE);
            }
        }
        closesocket(s);   //closes the net socket
        if (thread_cancel_flag==1) break;
    }
    fflush(stdout); //clears the contents of a buffer or flushes a stream
    print_out("\r\n*** Port Scanner Ended ***\r\n");
}

//===============================================================================================
// Simple TCP Server Adapted from BinaryTides
// https://www.binarytides.com/code-tcp-socket-server-winsock/
// Date: 06 July 2018
//===============================================================================================
int tcp_client()
{
    char port_1 [20];
    GetWindowText(txt_host, SERVER, 100);
    GetWindowText(txt_port1, port_1, 100);
    PORT = atoi(port_1);

    start_winsock();// Initialize socket
    s = create_socket(AF_INET , SOCK_STREAM , 0);//create socket
    server = set_sock_addr(server, AF_INET, inet_addr(SERVER), PORT, 0);// set sokaddr_in

    //Connect to remote server
    if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0) {
        print_out("connect error\r\n");
        return 1;
    }

    sprintf(line, "TCP Client Connected to %s : %d \r\n", SERVER, PORT);
    print_out(line);

    WSAAsyncSelect(s, hwnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);
    return 0;
}
void send_msg_sock(SOCKET sk, char * message) {
    if (check_connection(sk)==1) check_socket_error(send(sk , message , strlen(message) , 0), "Send failed");
}
void receive_msg_sock(SOCKET sk) {
    memset(buf,'\0', BUFLEN);
    if (check_connection(sk)==1) check_socket_error((recv_len = recv(sk , buf , BUFLEN , 0)), "Receive failed");
    if (recv_len >0) {
        sprintf(line, "[Server]: %s\r\n", buf);
        print_out(line);
        check_commands(buf);
    }
}
//===============================================================================================
// Simple UDP Client Adapted from BinaryTides
// https://www.binarytides.com/udp-socket-programming-in-winsock/
// Date: 06 July 2018
//===============================================================================================
int udp_client(){
    char port_1 [20];
    GetWindowText(txt_host, SERVER, 100);
    GetWindowText(txt_port1, port_1, 20);
    PORT = atoi(port_1);

    start_winsock();//Initialise winsock
    s = create_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);//Create a socket

    //setup address structure
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other = set_sock_addr(si_other, AF_INET, inet_addr(SERVER), PORT, 1);

    sprintf(line, "TCP Client Connected to %s : %d \r\n", SERVER, PORT);
    print_out(line);

    WSAAsyncSelect(s, hwnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);

    return 0;
}
void send_msg_sock_addr(SOCKET sk, sockaddr_in saddr_x, char * message) {
    if (sk!0) check_socket_error(sendto(sk, message, strlen(message) , 0 , (struct sockaddr *) &saddr_x, slen),"sendto() failed with error code :");
}
void receive_msg_sock_addr(SOCKET sk, sockaddr_in saddr_x) {
    memset(buf,'\0', BUFLEN);
    if (check_connection(sk)==1) check_socket_error(recvfrom(sk, buf, BUFLEN, 0, (struct sockaddr *) &saddr_x, &slen), "recvfrom() failed with error code :");
    if (recv_len >0) {
        sprintf(line, "[UDP Server]: %s\r\n", buf);
        print_out(line);
        check_commands(buf);
    }
}
//===============================================================================================
// Simple TCP Server Adapted from BinaryTides
// https://www.binarytides.com/code-tcp-socket-server-winsock/
// Date: 06 July 2018
//===============================================================================================
void start_tcp_server_thread() {
    char port_1 [20];
    GetWindowText(txt_port1, port_1, 100);
    PORT = atoi(port_1);

    thread_cancel_flag = 0;
    pthread_create(&thread_tcp, NULL, tcp_server, NULL);
}
void* tcp_server(void *value) {
    start_winsock();// Initialize socket
    s = create_socket(AF_INET , SOCK_STREAM , 0);//create socket
    server = set_sock_addr(server, AF_INET, INADDR_ANY, PORT, 0);//set sock_addr_in
    check_bind_socket(bind(s ,(struct sockaddr *)&server , sizeof(server)));//Bind
    listen(s , 3);//Listen to incoming connections

    //Accept and incoming connection
    sprintf(line, "TCP Server Listening on Port %d \r\nWaiting for incoming connections...\r\n", PORT);
    print_out(line);

    check_socket_error((new_socket = accept(s , (struct sockaddr *)&client, &slen)), "accept failed with error code :");
    print_out("Connection accepted\r\n");

    //Reply to client
    strcpy(message, "*** You are connected to the TCP Server, Welcome! *** \r\n");
    send(new_socket , message , strlen(message) , 0);

    while(1) {
        if (s==0 || new_socket==0 || thread_cancel_flag==1) break;
            memset(buf,'\0', BUFLEN);
            check_socket_error(recv_len = recvfrom(new_socket, buf, BUFLEN, 0, (struct sockaddr *) &client, &slen), "recvfrom() failed :");
            if (strlen(buf)>0) {
                sprintf(line, "[client %s:%d]:%s\r\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buf);
                print_out(line);
                check_commands(buf);
                if (ck_box == 1) check_socket_error(sendto(new_socket, buf, recv_len, 0, (struct sockaddr*) &client, slen), "sendto() failed :");
            }
    }
    closesocket(s);
    WSACleanup();
    print_out("\r\n*** TCP Server Closed ***\r\n");
    return 0;
}


//===============================================================================================
// Simple UPD Server Adapted from BinaryTides
// https://www.binarytides.com/udp-socket-programming-in-winsock/
// Date: 06 July 2018
//===============================================================================================
void start_udp_server_thread() {
    char port_1 [20];
    GetWindowText(txt_port1, port_1, 20);
    PORT = atoi(port_1);

    thread_cancel_flag = 0;
    pthread_create(&thread_udp, NULL, udp_server, NULL);

}
void * udp_server(void *value)
{
    start_winsock();//Initialise winsock
    s = create_socket(AF_INET , SOCK_DGRAM , 0);//Create a socket
    server = set_sock_addr(server, AF_INET, INADDR_ANY, PORT, 0);//set sock_addr_in
    check_bind_socket(bind(s ,(struct sockaddr *)&server , sizeof(server)));//Bind
    //keep listening for data
    sprintf(line, "UDP Server Listening on Port %d \r\nWaiting for data...\r\n", PORT);
    print_out(line);

    while(1)
    {
        if (s==0 || thread_cancel_flag==1) break;
        fflush(stdout); //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);  //try to receive some data, this is a blocking call
        check_socket_error((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)), "recvfrom() failed with error code :");
        if (strlen(buf)>0) {
            sprintf(line, "[client: %s:%d]:%s\r\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
            print_out(line);
            check_commands(buf);
            //Echo Client
            if (ck_box==1) check_socket_error(sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen), "sendto() failed with error code :");
        }
    }
    closesocket(s);
    WSACleanup();
    print_out("\r\n*** UDP Server Closed ***\r\n");
    return 0;
}
//===============================================================================================
// Help and Commands
// Date: 06 July 2018
//===============================================================================================
void check_commands(char * text) {
    if (strcmp(text, "/echo_off")==0) {
        ck_box = 0; CheckDlgButton(hwnd, 201, ck_box);
        print_out("\r\n*** Echo OFF ***\r\n\r\n");
    }
    if (strcmp(text, "/echo_on")==0) {
        ck_box = 1; CheckDlgButton(hwnd, 201, ck_box);
        print_out("\r\n*** Echo ON ***\r\n\r\n");
    }
    if (strcmp(text, "/quit")==0) {
        closesocket(s);
        closesocket(new_socket);
        WSACleanup();
        exit(0);
        print_out("\r\n*** Quiting Applications ***\r\n");
    }
}

void help_summary() {
    print_out("=====================================================\r\n");
    print_out(" *** Winsock 5 in 1  with Graphical Interface *** \r\n");
    print_out(" PortScanner, TCP/UDP Servers, TCP/UDP Clients  \r\n");
    print_out("=====================================================\r\n");
    print_out(" Code Based on www.BinaryTides.com code examples\r\n");
    print_out("\r\n");
    print_out(" Select the Application, set the parameters and click on 'Start'. \r\n");
    print_out("\r\n*** List of Commands ***\r\n");
    print_out(" /quit - Quit the server and client\r\n");
    print_out(" /echo_on, /echo_off - Enable/Disable Echo on the servers\r\n");
    print_out(" /cls  - Clear the screen\r\n");
    print_out(" /help - List this help\r\n");
    print_out("\r\n");
    print_out(" Made by Paulo Ramos @ July 2018 - C/C++ Winsock Projects\r\n");
    print_out("=====================================================\r\n");
}
//===============================================================================================
