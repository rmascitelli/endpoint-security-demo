#ifndef eps_lib_h
#define eps_lib_h

#define ARR_SIZE            100

// Makes a function-name in C++ have C linkage (compiler does not mangle the name)
extern "C" {
   void shutdown();
   void start_handling_events();
   struct proc_start_payload read_from_global_struct_queue();
   struct proc_start_payload get_fake_payload();
   void report_metrics();
}

struct proc_start_payload {
    char exe_path[ARR_SIZE];
    int pid;
    int ppid;
    //struct file_metadata meta;
};

struct file_metadata {
    long creation_date;
    long last_modified_date;
    long last_access_date;
    long size;
    char permissions[10];
    int uid;
    int gid;
    char uid_name[ARR_SIZE];
    char gid_name[ARR_SIZE];
};

struct bsm_payload {
    int ruid;
    int rgid;    
    int euid;    
    int egid;    
    int auid;  
};

#endif /* eps_lib_h */
