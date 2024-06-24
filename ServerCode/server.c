#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>


#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_PORT 587
#define EMAIL_FROM "WEATHER STATION <tranminhdat.contact@gmail.com>"
#define EMAIL_PASSWORD "INSERT YOUR APP PASSWORD HERE"
#define EMAIL_TO "dattran2003.ttn@gmail.com"
#define EMAIL_SUBJECT "Gas Detected!"
#define EMAIL_BODY "Gas has been detected. Please take necessary actions."

// Struct to pass parameters to thread
void send_email();
struct thread_data {
    int client_socket;
};


static const char *payload_text[] = {
    "To: " EMAIL_TO "\r\n",
    "From: " EMAIL_FROM "\r\n",
    "Subject: " EMAIL_SUBJECT "\r\n",
    "\r\n", /* blank line to separate headers from body */
    EMAIL_BODY "\r\n",
    NULL
};

struct upload_status {
    int lines_read;
};

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    struct upload_status *upload_ctx = (struct upload_status *)userp;
    const char *data;

    if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
        return 0;
    }

    data = payload_text[upload_ctx->lines_read];;

    if (data) {
        size_t len = strlen(data);
        memcpy(ptr, data, len);
        upload_ctx->lines_read++;
        return len;
    }

    return 0;
}


//Xử lý mỗi kết nối từ client
void *client_handler(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int client_socket = data->client_socket;

    char buffer[1024] = {0};
    int valread;

    // Read incoming data from ESP32
    valread = read(client_socket, buffer, 1024);
    printf("Received message from ESP32: %s\n", buffer);

    // Send email when fire detected
    send_email();

    close(client_socket);
    free(data);
    pthread_exit(NULL);
}


void send_email() {
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx;

    upload_ctx.lines_read = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl) {
        // Thiết lập địa chỉ email người gửi
        curl_easy_setopt(curl, CURLOPT_USERNAME, "tranminhdat.contact@gmail.com");

        // Thiết lập mật khẩu email
        curl_easy_setopt(curl, CURLOPT_PASSWORD, EMAIL_PASSWORD);

        // Thiết lập SMTP server và cổng
        curl_easy_setopt(curl, CURLOPT_URL, SMTP_SERVER);
        curl_easy_setopt(curl, CURLOPT_PORT, SMTP_PORT);

        // Sử dụng STARTTLS để kích hoạt TLS
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        // Bật xác thực chứng chỉ (có thể cần thiết tùy vào máy chủ email)
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/usr/local/etc/ca-certificates/cert.pem"); // Đường dẫn đến file chứng chỉ CA

        // Đặt địa chỉ email người nhận
        recipients = curl_slist_append(recipients, EMAIL_TO);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Thiết lập tiêu đề và nội dung email
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        // Gửi email
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Giải phóng danh sách người nhận
        curl_slist_free_all(recipients);

        // Dọn dẹp curl
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    
}

int main (){
 // Create socket for connection
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) {
        perror("socket() failed");
        return 1;
    }

    // Declare server address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000); // Server listens on port 8000

    // Bind socket to address and port
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind() failed");
        return 1;
    }

    // Set socket to listen for incoming connections
    if (listen(listener, 5)) {
        perror("listen() failed");
        return 1;
    }

    printf("Server is listening on port 8000...\n");

    while (1) {
        // Accept incoming connection
        struct sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        int client_socket = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);

        if (client_socket == -1) {
            perror("accept() failed");
            continue;
        }

        // Create thread to handle client connection
        pthread_t thread;
        struct thread_data *data = (struct thread_data *)malloc(sizeof(struct thread_data));
        data->client_socket = client_socket;

        if (pthread_create(&thread, NULL, client_handler, (void *)data) != 0) {
            perror("pthread_create failed");
            close(client_socket);
            free(data);
            continue;
        }

        pthread_detach(thread);
    }

    close(listener);
    return 0;
}