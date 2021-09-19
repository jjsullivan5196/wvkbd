#ifndef shm_open_h_INCLUDED
#define shm_open_h_INCLUDED

void randname(char *buf);
int create_shm_file(void);
int allocate_shm_file(size_t size);

#endif // shm_open_h_INCLUDED
