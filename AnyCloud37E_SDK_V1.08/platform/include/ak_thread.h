#ifndef _AK_THREAD_H_
#define _AK_THREAD_H_

#include <semaphore.h>
#include <pthread.h>

#define ANYKA_THREAD_NORMAL_STACK_SIZE 	(200*1024)
#define ANYKA_THREAD_MIN_STACK_SIZE 	(100*1024)

#define ANYKA_THREAD_PRIO_HIGH      40     // 40 ~ 59
#define ANYKA_THREAD_PRIO_NORMAL    60     // 60 ~ 79
#define ANYKA_THREAD_PRIO_LOW       80     // 80 ~ 99

enum ak_mutex_attr {
    AK_MUTEX_ATTR_NORMAL,
    AK_MUTEX_ATTR_RECURSIVE,
};

typedef sem_t                ak_sem_t;

typedef pthread_t            ak_pthread_t;

typedef pthread_attr_t       ak_pthread_attr_t;

typedef pthread_mutex_t      ak_mutex_t;

typedef pthread_mutexattr_t	 ak_mutexattr_t;

typedef pthread_rwlock_t     ak_rwlock_t;

typedef pthread_rwlockattr_t ak_rwlockattr_t;

typedef pthread_cond_t       ak_cond_t;

typedef void* (* thread_func)(void *param);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ak_thread_get_version - get thread module version
 * return: version string
 */
const char* ak_thread_get_version(void);

/**
 * ak_thread_create - create a posix thread.
 * @id[OUT]: thread id
 * @func[IN]: thread function
 * @arg[IN]: argument for thread
 * @stack_size[IN]: thread stack size
 * @priority[IN]: thread priority, default -1
 * return: 0 success; otherwise error number
 */
int ak_thread_create(ak_pthread_t *id, thread_func func, void *arg,
		int stack_size, int priority);

/**
 * ak_thread_join - join the thread
 * @id[IN]: thread id
 * return: 0 success; otherwise error number
 */
int ak_thread_join(ak_pthread_t id);

/**
 * ak_thread_detach - detach the thread
 * @id[IN]: thread id
 * return: 0 success; otherwise error number
 */
int ak_thread_detach(ak_pthread_t id);

/**
 * ak_thread_cancel - cancel the thread
 * @id[IN]: thread id
 * return: 0 success; otherwise error number
 */
int ak_thread_cancel(ak_pthread_t id);

/**
 * ak_thread_exit - thread exit
 * return: do not return
 */
void ak_thread_exit(void);

/**
 * ak_thread_get_tid - get thread id
 * return: thread id belongs to call function
 */
long int ak_thread_get_tid(void);

/**
 * ak_thread_mutex_init - init mutex
 * @mutex[OUT]: mutex pointer
 * @attr[IN]: mutexattribute pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_mutex_init(ak_mutex_t *mutex, ak_mutexattr_t *attr);

/**
 * ak_thread_mutex_lock - lock mutex
 * @mutex[IN]: mutex pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_mutex_lock(ak_mutex_t *mutex);

/**
 * ak_thread_mutex_unlock - unlock mutex
 * @mutex[IN]: mutex pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_mutex_unlock(ak_mutex_t *mutex);

/**
 * ak_thread_mutex_destroy - destroy mutex
 * @mutex[IN]: mutex pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_mutex_destroy(ak_mutex_t *mutex);

/**
 * ak_thread_sem_init - init semaphore
 * @sem[IN]: semaphore pointer
 * @value[IN]: semaphore initial value
 * return: 0 success; otherwise error number
 */
int ak_thread_sem_init(ak_sem_t *sem, unsigned int value);

/**
 * ak_thread_sem_post - post semaphore
 * @sem[IN]: semaphore pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_sem_post(ak_sem_t *sem);

/**
 * ak_thread_sem_wait - wait semaphore
 * @sem[IN]: semaphore pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_sem_wait(ak_sem_t *sem);

/**
 * ak_thread_sem_destroy - destroy semaphore
 * @sem[IN]: semaphore pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_sem_destroy(ak_sem_t *sem);

/**
 * ak_thread_sem_trywait - try to wait semaphore
 * @sem[IN]: semaphore pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_sem_trywait(ak_sem_t *sem);

/**
 * ak_thread_sem_timedwait - wait semaphore timeout
 * @sem[IN]: semaphore pointer
 * @abs_timeout[IN]: specific timeout
 * return: 0 success; otherwise error number
 */
int ak_thread_sem_timedwait(ak_sem_t *sem, const struct timespec *abs_timeout);

/**
 * ak_thread_set_name - set thread name
 * @name[IN]: pointer to 'thread name' no longer than 15B
 * return: 0 success; otherwise error number
 */
int ak_thread_set_name(const char *name);

/**
 * ak_thread_cond_init - init condition
 * @cond[OUT]: condition pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_cond_init(ak_cond_t *cond);

/**
 * ak_thread_cond_destroy - destroy condition
 * @cond[IN]: condition pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_cond_destroy(ak_cond_t *cond);

/**
 * ak_thread_cond_wait - wait condition
 * @cond[IN]: condition pointer
 * @mutex[IN]: mutext pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_cond_wait(ak_cond_t *cond, ak_mutex_t *mutex);

/**
 * ak_thread_cond_timedwait - wait condition timeout
 * @cond[IN]: condition pointer
 * @mutex[IN]: mutext pointer
 * @abstime[IN]: timeout value
 * return: 0 success; otherwise error number
 */
int ak_thread_cond_timedwait(ak_cond_t *cond,
		ak_mutex_t *mutex, const struct timespec *abstime);

/**
 * ak_thread_cond_signal - post signal on condition
 * @cond[IN]: condition pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_cond_signal(ak_cond_t *cond);

/**
 * ak_thread_cond_broadcast - post signal by broadcast on condition
 * @cond[IN]: condition pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_cond_broadcast(ak_cond_t *cond);

/**
 * ak_thread_mutexattr_init - init mutexattribute
 * @attr[IN]: mutexattribute pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_mutexattr_init(ak_mutexattr_t *attr);

/**
 * ak_thread_mutexattr_destroy - destroy mutexattribute
 * @attr[IN]: mutexattribute pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_mutexattr_destroy(ak_mutexattr_t *attr);

/**
 * ak_thread_mutexattr_settype - set mutexattribute type
 * @attr[IN]: mutexattribute pointer
 * @kind[IN]: mutexattribute type
 * return: 0 success; otherwise error number
 */
int ak_thread_mutexattr_settype(ak_mutexattr_t *attr, int kind);

/**
 * ak_thread_mutexattr_gettype - get mutexattribute type
 * @attr[IN]: mutexattribute pointer
 * @kind[OUT]: mutexattribute type
 * return: 0 success; otherwise error number
 */
int ak_thread_mutexattr_gettype(const ak_mutexattr_t *attr, int *kind);

/**
 * ak_thread_rwlock_init - init rwlock
 * @rwlock[IN] : rwlock pointer
 * @rwlockattr[IN] : rwlockattr pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_rwlock_init(ak_rwlock_t *rwlock, ak_rwlockattr_t *rwlockattr);

/**
 * ak_thread_rwlock_destroy - destroy rwlock
 * @rwlock[IN] : rwlock pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_rwlock_destroy(ak_rwlock_t *rwlock);

/**
 * ak_thread_rwlock_rdlock - get read lock
 * @rwlock[IN] : rwlock pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_rwlock_rdlock(ak_rwlock_t *rwlock);

/**
 * ak_thread_rwlock_wrlock - get write lock
 * @rwlock[IN] : rwlock pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_rwlock_wrlock(ak_rwlock_t *rwlock);

/**
 * ak_thread_rwlock_tryrdlock - try to get read lock
 * @rwlock[IN] : rwlock pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_rwlock_tryrdlock(ak_rwlock_t *rwlock);

/**
 * ak_thread_rwlock_trywrlock - try to get write lock
 * @rwlock[IN] : rwlock pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_rwlock_trywrlock(ak_rwlock_t *rwlock);

/**
 * ak_thread_rwlock_unlock - unlock rwlock
 * @rwlock[IN] : rwlock pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_rwlock_unlock(ak_rwlock_t *rwlock);

/**
 * ak_thread_rwlockattr_init - init rwlock attribute
 * @rwlocakattr[IN] : rwlockattr pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_rwlockattr_init(ak_rwlockattr_t * rwlockattr);

/**
 * ak_thread_rwlockattr_destroy - destroy rwlock attribute
 * @rwlocakattr[IN] : rwlockattr pointer
 * return: 0 success; otherwise error number
 */
int ak_thread_rwlockattr_destroy(ak_rwlockattr_t * rwlockattr);

/**
 * ak_thread_rwlockattr_getpshared - get process shared attribute
 * @rwlocakattr[IN] : rwlockattr pointer
 * @value[OUT] : value of process shared attribute 
 * return: 0 success; otherwise error number
 */
int ak_thread_rwlockattr_getpshared(ak_rwlockattr_t * rwlockattr, int *value);

/**
 * ak_thread_rwlockattr_setpshared - set process shared attribute
 * @rwlocakattr[IN] : rwlockattr pointer
 * @value[IN] : value of process shared attribute
 * return: 0 success; otherwise error number
 */
int ak_thread_rwlockattr_setpshared(ak_rwlockattr_t * rwlockattr, int value);

#ifdef __cplusplus
}
#endif
#endif	//end of _AK_THREAD_H_
