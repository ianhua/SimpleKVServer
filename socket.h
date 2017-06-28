#ifndef MM_TRAINING_SOCKET_H
#define MM_TRAINING_SOCKET_H

#include <string>

/**
 * Socket header
 * @author ianhua
 */

namespace mmtraining {

/**
 * Client socket
 */
class ClientSocket {
public:
    /**
     * Constructor
     */
    ClientSocket();
    
    /**
     * Constructor with fd
     */
    ClientSocket(int fd);

    /**
     * Destructor
     */
    ~ClientSocket();
    
    /**
     * Connect server
     * @return -1 if fail, 0 if success
     */
    int Connect(const char* ip, unsigned short port);
    
    /**
     * Send data
     * @return -1 if fail, number of bytes otherwise
     */
    int Write(const void* buffer, int bufferSize);
    
    /**
     * Send one line
     * @return -1 if fail, number of bytes otherwise
     */
    int WriteLine(const std::string& line);
    
    /**
     * Receive data
     * @return -1 if fail, 0 if EOF, number of bytes otherwise
     */
    int Read(void* buffer, int bufferSize);
    
    /**
     * Receive one line
     * @return -1 if fail, number of bytes otherwise
     */
    int ReadLine(std::string& line);
    
    /**
     * Close connection
     * @return -1 if fail, 0 if success
     */
    int Close();
    
private:
    
    // connect fd
    int fd;
};

/**
 * Server socket
 */
class ServerSocket {
public:
    /**
     * Constructor
     */
    ServerSocket();

    /**
     * Destructor
     */
    ~ServerSocket();
    
    /**
     * Listen port
     * @return -1 if fail, 0 if success
     */
    int Listen(const char* ip, unsigned short port);
    
    /**
     * Accept new request
     * @return NULL if fail, pointer to ClientSocket if success
     */
    ClientSocket* Accept();

    /**
     * Close connection
     * @return -1 if fail, 0 if success
     */
    int Close();
    
private:

    // listen fd
    int fd;
};

} // namespace mmtraining

#endif // MM_TRAINING_SOCKET_H
