#define DEFAULT_QUANTUM 10

enum { FF_SCHEDULING, RR_SCHEDULING, CUSTOM_SCHEDULING } scheduler;
enum { UNLIMITED_MEMORY, SWAPPING_X_MEMORY, VIRTUAL_MEMORY, CUSTOM_MEMORY } mem_allocator;
enum { NORMAL, VERBOSE, DEBUG } verbosity;

typedef struct {
    int id;
    int mem;
    int ta;
    int tr;
} Process;

typedef struct {
    int sys_mem;
    int quantum;
    int n_procs;
    Process *procs;
} ProcTable;

ProcTable *new_proc_table();
int add_process(ProcTable *proc_table, Process new_proc);
int get_procs_from_file(char *filename, ProcTable *proc_table);
void set_scheduler(int s);
void set_mem_allocator(int m);
void set_mem(int m, ProcTable *proc_table);
void set_quantum(int q, ProcTable *proc_table);
void set_verbosity(int v);
