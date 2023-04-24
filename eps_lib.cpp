#include "eps_lib.h"
#include <bsm/libbsm.h>
#include <EndpointSecurity/EndpointSecurity.h>
#include <iostream>
#include <queue>
#include <deque>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>

// creationData → st_ctime
// lastModifiedDate → st_mtime
// lastAccessDate → st_atime
// size → st_size
// permissions → st_mode
// uid → st_uid
// gid → st_gid


// struct file_metadata get_file_metadata(char filepath[ARR_SIZE]) {

// }

std::queue<struct proc_start_payload> g_q;
std::mutex g_q_mutex;
bool g_stop = false;

struct file_metadata get_file_metadata(char *filepath) {
    int fd = 0; 
    file_metadata ret = *(new file_metadata());
    struct stat st; 
    fd = open(filepath, O_RDONLY); 
    if(-1 == fd) 
    { 
        printf("\n NULL File descriptor\n"); 
        return ret; 
    } 
 
    // set the errno to default value 
    errno = 0; 
    if(fstat(fd, &st)) 
    { 
        printf("\nfstat error: [%s]\n",strerror(errno)); 
        close(fd); 
        return ret; 
    } 

    printf("Creating file metadata\n");
    
    ret.creation_date =         1000;   //st.buf.st_ctime;
    ret.last_modified_date =    2000;   //st.buf.st_mtime;
    ret.last_access_date =      3000;   //st.buf.st_atime;
    ret.size =                  50;     //st.buf.st_size;
    memset(ret.permissions, 0, sizeof(ret.permissions));
    sprintf(ret.permissions, "-rwxrwxrwx");
    ret.uid =                   123;    //st.buf.st_uid;
    ret.gid =                   543;    //st.buf.st_gid;
 
    close(fd); 
    return ret;
}

struct proc_start_payload build_proc_start_payload(const es_message_t *msg) {
    struct proc_start_payload payload;
    strcpy(payload.exe_path, msg->process->executable->path.data);
    payload.pid = audit_token_to_pid(msg->process->audit_token);
    payload.ppid = msg->process->ppid;
    //get_file_metadata("");
    return payload;
}

struct proc_start_payload get_fake_payload() {
    struct proc_start_payload payload;
    strcpy(payload.exe_path, "fakePath");
    payload.pid = 12345;
    payload.ppid = 54321;
    printf("Returning %s\n", payload.exe_path);
    return payload;
}

struct proc_start_payload read_from_global_struct_queue() {
    g_q_mutex.lock();
    struct proc_start_payload p = g_q.front();
    g_q.pop();
    g_q_mutex.unlock();
    return p;
}

static void handle_and_enqueue_event(const es_message_t *msg) {
    struct proc_start_payload payload;
    switch (msg->event_type) {
        case ES_EVENT_TYPE_NOTIFY_EXEC:
            payload = build_proc_start_payload(msg); 
            g_q_mutex.lock();
            g_q.push(payload);
            g_q_mutex.unlock();

        // case ES_EVENT_TYPE_NOTIFY_FORK:
        //     printf("%{public}s (pid: %d) | FORK: Child pid: %d",
        //         msg->process->executable->path.data,
        //         audit_token_to_pid(msg->process->audit_token),
        //         audit_token_to_pid(msg->event.fork.child->audit_token));

        //     break;

        // case ES_EVENT_TYPE_NOTIFY_EXIT:
        //     printf("%{public}s (pid: %d) | EXIT: status: %d",
        //         msg->process->executable->path.data,
        //         audit_token_to_pid(msg->process->audit_token),
        //         msg->event.exit.stat);
        //     break;

        // default:
        //     printf("Unexpected event type encountered: %d\n", msg->event_type);
        //     break;
    }
}

void start_handling_events() {
    es_client_t *client;

    es_new_client_result_t result = es_new_client(&client, ^(es_client_t *c, const es_message_t *msg) {
        handle_and_enqueue_event(msg);
    });
    if (result != ES_NEW_CLIENT_RESULT_SUCCESS) {
        printf("Failed to create new ES client: %d", result);
        return;
    }

    es_event_type_t events[] = { ES_EVENT_TYPE_NOTIFY_EXEC, ES_EVENT_TYPE_NOTIFY_FORK, ES_EVENT_TYPE_NOTIFY_EXIT };

    if (es_subscribe(client, events, sizeof(events) / sizeof(events[0])) != ES_RETURN_SUCCESS) {
        printf("Failed to subscribe to events");
        es_delete_client(client);
        return;
    }

    while (g_stop == false);
    printf("Deleting client and stopping program \n");
    es_delete_client(client);
    return;
}

void report_metrics() {
    while (g_stop == false) {
        std::cout << "Event Que Size: " << g_q.size() << std::endl;
        sleep(5);
    }
}

void shutdown() {
    g_stop = true;
}




