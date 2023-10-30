#include <pthread.h>

#include <ogc/lwp.h>
#include <ogc/mutex.h>
#include <ogc/cond.h>

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	return LWP_MutexLock(*(mutex_t *)mutex);
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
	return LWP_MutexTryLock(*(mutex_t *)mutex);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	return LWP_MutexUnlock(*(mutex_t *)mutex);
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
	return LWP_MutexInit((mutex_t *)mutex, false);
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	return LWP_MutexDestroy(*(mutex_t *)mutex);
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
	return LWP_CondBroadcast(*(cond_t *)cond);
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
	return LWP_CondWait(*(cond_t *)cond, *(mutex_t *)mutex);
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
	return LWP_CondInit((cond_t *)cond);
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
	return LWP_CondDestroy(*(cond_t *)cond);
}

int pthread_cond_signal(pthread_cond_t *cond)
{
	return LWP_CondSignal(*(cond_t *)cond);
}

int pthread_create(pthread_t *thd, const pthread_attr_t *attr,
		   void * (*fn)(void *), void *data)
{
	return LWP_CreateThread((lwp_t *)thd, fn, data, NULL, 0, 1);
}

int pthread_join(pthread_t thd, void **value)
{
	return LWP_JoinThread((lwp_t)thd, value);
}
