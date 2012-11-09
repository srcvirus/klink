/*
 * CS 454 Assignment 3
 * July 13, 2012
 * Group Member 1: Evgeny Shulyatyev (ID 20242378)
 * Group Member 2: Shihabur Rahman Chowdhury (ID 20453766)
 * */

#ifndef ERRORS_H_
#define ERRORS_H_


// 			******* SUCCESS CODES ********

#define GENERAL_SUCCESS_CODE 0



//			******* CLIENT ERROR MESSAGES *********

//1a. Cannot connect to the binder / binder socket create fail (ERROR_BINDER_CONNECT_FAIL)
#define ERROR_BINDER_CONNECT_FAIL -100
//1b. Connected to binder but request send failed (ERROR_BINDER_REQUEST_SEND_FAIL)
#define ERROR_BINDER_REQUEST_SEND_FAIL -101
//1c. Connection from the binder was closed before request was served (ERROR_BINDER_CONNECTION_CLOSED)
#define ERROR_BINDER_CONNECTION_CLOSED -103
//1d. No server was found for the request (ERROR_NO_SERVER_FOUND) [LOC_Fail message received]
#define ERROR_NO_SERVER_FOUND -104
//1e. Server found but connect to server failed (ERROR_SERVER_CONNECT_FAIL)
#define ERROR_SERVER_CONNECT_FAIL -105
//1f. Connected to the server but request send failed (ERROR_SERVER_REQUEST_SEND_FAIL)
#define ERROR_SERVER_REQUEST_SEND_FAIL -106
//1g. Request sent to server but server closed the connection before the request was serverd (ERROR_SERVER_CONNECTION_CLOSED)
#define ERROR_SERVER_CONNECTION_CLOSED -107

// Warnings:
//array length greater than max length (WARNING_MAX_ARRAY_SIZE_EXCEEDED)
#define WARNING_MAX_ARRAY_SIZE_EXCEEDED 901


//			******* SERVER ERROR MESSAGES *********


//Improper Sequencing Error Messages:
//1. rpcRegister without calling rpcInit (ERROR_NO_CONNECTION_INITIALIZED)
#define ERROR_NO_CONNECTIONS_INITIALIZED -201
//2. rpcExecute without rpcRegister (ERROR_NO_PROCEDURES_REGISTERED)
#define ERROR_NO_PROCEDURES_REGISTERED -202


// rpcInit Error Messages:
//3. Unable to initialize the server socket to listen for incoming connection;
#define ERROR_SERVER_SOCKET_INIT -203
//4. Binder Address environmental variable not set
#define ERROR_SERVER_RPCINIT_BINDER_ENVIRONMENT_VARIABLE_NOT_SET -204
//5. Unable to establish connection with binder
#define ERROR_SERVER_BINDER_CONNECTION_FAILED -205
//6. Sending of registration message to the binder failed
#define ERROR_SERVER_SENDING_REGISTER_MESSAGE -206
//7. Failed to receive confirmation of registration message from the binder
#define ERROR_SERVER_RECEIVING_REGISTRATION_CONFIRMATION_MESSAGE -207
//8. Received unknown message type from the binder
#define ERROR_SERVER_RECEIVED_UNKNOWN_MESSAGE_TYPE_FROM_BINDER -208


// rpcRegister Error Messages:
//9. Select syscall failed, fatal error
#define ERROR_SERVER_SELECT_SYSCALL_FAILED -209
//10. Unable to accept incoming client connection
#define ERROR_SERVER_UNABLE_ACCEPT_NEW_CLIENT_CONNECTION -210
//11. Failed to receive an incoming message from the binder
#define ERROR_SERVER_FAILED_BINDER_MESSAGE_RECEIVE -211
//12. Failed to receive an incoming client request
#define ERROR_SERVER_FAILED_CLIENT_REQUEST_RECEIVE -212

#define ERROR_SERVER_FUNCTION_DOES_NOT_EXIST -213











#endif /* ERRORS_H_ */
