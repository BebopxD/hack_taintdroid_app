#ifndef TAINTDROIDHACK_NATIVE_LIB_H
#define TAINTDROIDHACK_NATIVE_LIB_H

#include <jni.h>
#include <string>
#include <string.h>
#include <unistd.h> /* read, write, close */
#include <sys/socket.h>
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>


#include "ScopedJniThreadState.h"
#include "Thread.h"
#include "TaintdroidHack.h"

#endif /* TAINTDROIDHACK_NATIVE_LIB_H */