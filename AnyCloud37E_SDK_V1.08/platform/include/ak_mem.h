#ifndef _AK_MEM_H_
#define _AK_MEM_H_

#include "ak_common.h"

#define LEFT_ALIGN(a,b)  ((a/b)*b)
#define RIGHT_ALIGN(a,b) ((a+b-1)/b*b)

typedef struct _s_used_mem_info {
	int module_id;
	char func[20];
	long int time;
	unsigned int size;
	void *addr;
	struct _s_used_mem_info *next;
}s_used_mem_info;

/**
 * ak_mem_get_version: Get mem module version info
 * return: mem module version info
 */
const char* ak_mem_get_version(void);

/**
 * ak_mem_malloc_: re-wrap malloc function for detecting memory leak
 * @module_id[IN]: which module apply memory?
 * @size[IN]: what size of the ring buffer is?
 * @func_name[IN]: function entry where you apply ring buffer
 * @time[IN]: what time the ring buffer was applied?
 * return: mem blk address or NULL
 */
void * ak_mem_alloc_(int module_id, unsigned int size, const char *func_name, long time);
#define ak_mem_alloc(module_id, size) ak_mem_alloc_(module_id, size, __FUNCTION__, ak_get_os_timestamp())

/**
 * ak_mem_free: re-wrap free function for detecting memory leak
 * @ptr[IN]: malloced pointer
 * return: AK_SUCCESS or error_code
 */
int ak_mem_free(void *ptr);

/**
 * ak_mem_pool_init_: init a memory pool
 * @p_pool_id[OUT]: the memory pool id you applied
 * @module_id[IN]: which module apply memory pool?
 * @nmemb[IN]: how many memroy blk does the memory pool contains?
 * @size[IN]: what size of the memory blk is?
 * @func_name[IN]: function entry where you apply memory pool
 * @time[IN]: what time the memory pool was applied?
 * return: AK_SUCCESS or error_code
 */
int ak_mem_pool_init_(int *p_pool_id, int module_id, unsigned int nmemb, unsigned int size, const char *func_name, long time);
#define ak_mem_pool_init(p_pool_id, module_id, nmemb, size) ak_mem_pool_init_(p_pool_id, module_id, nmemb, size, __FUNCTION__, ak_get_os_timestamp())

/**
 * ak_mem_pool_destroy: destroy a memory pool
 * @mem_pool_id[IN]: which mem pool you will operate?
 * return: AK_SUCCESS or error_code
 */
int ak_mem_pool_destroy(int mem_pool_id);

/**
 * ak_mem_pool_alloc: alloc a memory pool blk
 * @p_mem_blk[OUT]: memory pool blk pointer 
 * @mem_pool_id[IN]: which mem pool you will use?
 * return: AK_SUCCESS or error_code
 */
int ak_mem_pool_alloc(void **p_mem_blk, int mem_pool_id);

/**
 * ak_mem_pool_free: free a memory pool blk
 * @ptr[IN]: memory pool blk pointer 
 * @mem_pool_id[IN]: which mem pool you will use?
 * return: AK_SUCCESS or error_code
 */
int ak_mem_pool_free(void *ptr, int mem_pool_id);

/**
 * ak_mem_rb_init_: init a ring buffer
 * @p_rb_id[OUT]: ring buffer id 
 * @module_id[IN]: which module apply memory?
 * @size[IN]: what size of the ring buffer is?
 * @func_name[IN]: function entry where you apply ring buffer
 * @time[IN]: what time the ring buffer was applied?
 * return: AK_SUCCESS or error_code
 */
int ak_mem_rb_init_(int *p_rb_id, int module_id, unsigned int size, const char *func_name, long time);
#define ak_mem_rb_init(p_rb_id, module_id, size) ak_mem_rb_init_(p_rb_id, module_id, size, __FUNCTION__, ak_get_os_timestamp())

/**
 * ak_mem_rb_destroy: destroy a ring buffer
 * @rb_id[IN]: which ring buffer will you operate? 
 * return: AK_SUCCESS or error_code
 */
int ak_mem_rb_destroy(int rb_id);

/**
 * ak_mem_rb_reset: reset a ring buffer
 * @rb_id[IN]: which ring buffer will you operate? 
 * return: AK_SUCCESS or error_code
 */
int ak_mem_rb_reset(int rb_id);

/**
 * ak_mem_rb_write: write data to ring buffer
 * @rb_id[IN]: which ring buffer will you operate? 
 * @from[IN]: where is the write data from? 
 # @write_len[IN]: write data len
 * return: AK_SUCCESS or error_code
 */
int ak_mem_rb_write(int rb_id, unsigned char *from, int write_len);

/**
 * ak_mem_rb_read_request: request a read ptr
 * @rb_id[IN]: which ring buffer will you operate? 
 * @readptr_id[OUT]: read ptr output to user
 * return: AK_SUCCESS or error_code
 */
int ak_mem_rb_read_request(int rb_id, int *readptr_id);

/**
 * ak_mem_rb_read_cancel: cancel a read ptr
 * @rb_id[IN]: which ring buffer will you operate? 
 * @readptr_id[IN]: which read_ptr will you cancel?
 * return: AK_SUCCESS or error_code
 */
int ak_mem_rb_read_cancel(int rb_id, int readptr_id);

/**
 * ak_mem_rb_read: read data from ring buffer
 * @rb_id[IN]: which ring buffer will you read? 
 * @readptr_id[IN]: which read_ptr will you use when read data from ring buffer?
 * @to[OUT]: where will be writed when read data?
 * @read_len[IN]: plan read data len
 * @result[OUT]: read data len when actual read
 * return: AK_SUCCESS or error_code
 */
int ak_mem_rb_read(int rb_id, int readptr_id, unsigned char *to, int read_len, int *result);

/**
 * ak_mem_rb_get_data_size: Get the size of ring buffer
 * @rb_id[IN]: ring buffer id
 * @size[OUT]: the data length in ring buffer
 * return: AK_SUCCESS or error_code
 */
int ak_mem_rb_get_data_size(int rb_id, int *size);

/**
 * ak_mem_dma_alloc_: alloc one dma memory blk
 * @module_id[IN]: which module apply memory?
 * @size[IN]: what size of the memory blk is?
 * @func_name[IN]: function entry where you apply memory blk
 * @time[IN]: what time the memory blk was applied?
 * return: dma memory blk pointer or NULL
 */
void * ak_mem_dma_alloc_(int module_id, unsigned int size, const char *func_name, long time);
#define ak_mem_dma_alloc(module_id, size) ak_mem_dma_alloc_(module_id, size, __FUNCTION__, ak_get_os_timestamp())

/**
 * ak_mem_dma_free: free one dma memory blk
 * @ptr[IN]: virtual address
 * return: AK_SUCCESS or error_code
 */
int ak_mem_dma_free(void *ptr);

/**
 * ak_mem_dma_vaddr2paddr: virtual address switch to physical address
 * @ptr[IN]: virtual address
 * @paddr[OUT] : physical address 
 * return: AK_SUCCESS or error_code
 */
int ak_mem_dma_vaddr2paddr(void *ptr, unsigned long *paddr);

/**
 * ak_mem_dma_get_used: get dma memory used total
 * @module_id[IN]: which module you want to query?
 * @p_total_used[OUTPUT] : where the result will be placed? 
 * return: AK_SUCCESS or error_code
 */
int ak_mem_dma_get_used(int module_id, unsigned long *p_total_used);

/**
 * ak_mem_get_used_total: get memory used total
 * @module_id[IN]: which module you want to query?
 * @p_total_used[OUT] : where the result will be placed? 
 * return: AK_SUCCESS or error_code
 */
int ak_mem_get_used_total(int module_id, unsigned long *p_total_used);

/**
 * ak_mem_get_used_detail: get memory used detail
 * @module_id[IN]: which module you want to query?
 * @p_mem_used[OUTPUT]: just give me a s_used_mem_info point, that will be ok 
 * return: AK_SUCCESS or error_code
 */
int ak_mem_get_used_detail(int module_id, s_used_mem_info **p_mem_used);

/**
 * ak_mem_release_used_detail: release memory used detail info
 * @p_mem_used[OUTPUT]: memory used detail info given by ak_mem_get_used_total interface 
 * return: AK_SUCCESS or error_code
 */
int ak_mem_release_used_detail(s_used_mem_info *p_mem_used);

/* ak_mem_dma_pool_create 		--		create dma memory pool 
 * @mid[IN]		:	module id that system used
 * @size[IN]	:	size info of the DMA pool
 * return AK_SUCCESS if success, AK_FAILED if failed
 * */
int ak_mem_dma_pool_create(enum module_id mid, unsigned int size);

/*
 * ak_mem_dma_pool_activate: init the dma memory pool
 * return: AK_SUCCESS if success, AK_FAILED if failed
 * */
int ak_mem_dma_pool_activate(void );

	/**
 * ak_mem_dma_pool_exit: exit memory module
 * return: AK_SUCCESS
 */
int ak_mem_dma_pool_exit(void);

/* ak_mem_dma_pool_get_used_mem	-- get the used dma pool info
 * @mid[IN]		:		module id 
 * @total[OUT]	:		pointer to record the mem info
 * return AK_SUCCESS if successful, and AK_FAILED if failure.
 * */
int ak_mem_dma_pool_get_used_mem(enum module_id mid, unsigned long *total);

/* ak_mem_dma_pool_get_free_mem	-- get the free dma pool info
 * @mid[IN]		:		module id 
 * @total[OUT]	:		pointer to record the mem info
 * return AK_SUCCESS if successful, and AK_FAILED if failure.
 * */
int ak_mem_dma_pool_get_free_mem(enum module_id mid, unsigned long *total);

/* ak_mem_dma_pool_set_threshold	---- set the dma pool alloc threadhold
 * @mid[IN]		:		module id 
 * @threadhold[OUT]	:		threshold of the dma pool alloc 
 * return AK_SUCCESS if successful, and AK_FAILED if failure.
 */
int ak_mem_dma_pool_set_threshold(enum module_id mid, unsigned long threshold);

/* ak_mem_dma_pool_create_reserved_obj	---	create reserved dma dma obj for specific usage
 * @mid[IN]					:			module id 
 * @size[IN]				:		 	size of the dma buf obj
 * @alloc_threadhold[IN]	:			threshold of the dma buf obj alloc 
 * return AK_SUCCESS if successful, and AK_FAILED if failure.
 */
int ak_mem_dma_pool_create_reserved_obj(enum module_id mid, unsigned long size, unsigned long alloc_threshold);

/* ak_mem_dma_get_reserved_size		--- get the system reserved total dma size		
 * @size[OUT]		:	record the system reserved total dma size
 * return AK_SUCCESS if succesful, and AK_FAILED if failure
 * */
int ak_mem_dma_get_reserved_size(unsigned long *size);

/* ak_mem_get_total_size		--- get the system total memory size		
 * @size[OUT]		:	record the system total memory size
 * return AK_SUCCESS if succesful, and AK_FAILED if failure
 * */
int ak_mem_get_total_size(unsigned long *size);
#endif

/* end of file */
