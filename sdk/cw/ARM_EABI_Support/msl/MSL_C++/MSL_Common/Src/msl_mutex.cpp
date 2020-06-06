/* Metrowerks Standard Library
 * Copyright © 1995-2004 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2004/04/16 01:17:01 $
 * $Revision: 1.2.4.4 $
 */

// msl_mutex.cpp

#include <msl_mutex>
#include <msl_condition>

#ifdef _MSL_FORCE_ENUMS_ALWAYS_INT
	#if _MSL_FORCE_ENUMS_ALWAYS_INT
		#pragma enumsalwaysint on
	#else
		#pragma enumsalwaysint off
	#endif
#endif

#ifdef _MSL_FORCE_ENABLE_BOOL_SUPPORT
	#if _MSL_FORCE_ENABLE_BOOL_SUPPORT
		#pragma bool on
	#else
		#pragma bool off
	#endif
#endif

#ifndef _MSL_NO_CPP_NAMESPACE
	namespace Metrowerks {
#else
	#ifndef Metrowerks
		#define Metrowerks
	#endif
#endif

#ifdef _MSL_USE_PTHREADS

#ifndef _MSL_NO_TIME_SUPPORT

timed_mutex::timed_mutex()
	: locked_(false)
{
	if (pthread_mutex_init(&m_, 0))
		detail::throw_thread_resource_error();
	if (pthread_cond_init(&c_, 0))
	{
		pthread_mutex_destroy(&m_);
		detail::throw_thread_resource_error();
	}
}

timed_mutex::~timed_mutex()
{
	pthread_cond_destroy(&c_);
	pthread_mutex_destroy(&m_);
}

void
timed_mutex::lock()
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	while (locked_)
		pthread_cond_wait(&c_, &m_);
	locked_ = true;
	pthread_mutex_unlock(&m_);
}

bool
timed_mutex::try_lock()
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (!locked_)
		locked_ = true;
	bool result = locked_;
	pthread_mutex_unlock(&m_);
	return result;
}

bool
timed_mutex::timed_lock(const universal_time& unv_time)
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	int cond = 0;
	while (locked_)
	{
		cond = pthread_cond_timedwait(&c_, &m_, (const timespec*)&unv_time);
		if (cond)
			break;
	}
	if (!locked_)
	{
		locked_ = true;
		cond = 0;
	}
	pthread_mutex_unlock(&m_);
	return cond == 0;
}

void
timed_mutex::unlock()
{
	if (pthread_mutex_lock(&m_) || !locked_)
		detail::throw_lock_error();
	locked_ = false;
	pthread_mutex_unlock(&m_);
	pthread_cond_broadcast(&c_);
}

void
timed_mutex::wait_prefix(state&)
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	locked_ = false;
	pthread_cond_broadcast(&c_);
}

void
timed_mutex::wait_suffix(const state&)
{
	while (locked_)
		pthread_cond_wait(&c_, &m_);
	locked_ = true;
	pthread_mutex_unlock(&m_);
}

#endif  // _MSL_NO_TIME_SUPPORT

#ifdef PTHREAD_MUTEX_RECURSIVE

recursive_mutex::recursive_mutex()
{
	pthread_mutexattr_t attr;
	if (pthread_mutexattr_init(&attr))
		detail::throw_thread_resource_error();
	if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) ||
	    pthread_mutex_init(&m_, &attr))
	{
		pthread_mutexattr_destroy(&attr);
		detail::throw_thread_resource_error();
	}
	pthread_mutexattr_destroy(&attr);
}

#else   // PTHREAD_MUTEX_RECURSIVE

recursive_mutex::recursive_mutex()
	:	count_(0)
{
	if (pthread_mutex_init(&m_, 0))
		detail::throw_thread_resource_error();
	if (pthread_cond_init(&c_, 0))
	{
		pthread_mutex_destroy(&m_);
		detail::throw_thread_resource_error();
	}
}

recursive_mutex::~recursive_mutex()
{
	pthread_cond_destroy(&c_);
	pthread_mutex_destroy(&m_);
}

void
recursive_mutex::lock()
{
	pthread_t self = pthread_self();
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (count_ > 0 && pthread_equal(self, owner_))
		++count_;
	else
	{
		while (count_ > 0)
			pthread_cond_wait(&c_, &m_);
		count_ = 1;
		owner_ = self;
	}
	pthread_mutex_unlock(&m_);
}

void
recursive_mutex::unlock()
{
	pthread_t self = pthread_self();
	if (pthread_mutex_lock(&m_) || count_ == 0 || !pthread_equal(self, owner_))
		detail::throw_lock_error();
	bool notify = --count_ == 0;
	pthread_mutex_unlock(&m_);
	if (notify)
		pthread_cond_broadcast(&c_);
}

void
recursive_mutex::wait_prefix(state& st)
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	st.count_ = count_;
	st.owner_ = owner_;
	count_ = 0;
	pthread_cond_broadcast(&c_);
}

void
recursive_mutex::wait_suffix(const state& st)
{
	while (count_ > 0)
		pthread_cond_wait(&c_, &m_);
	count_ = st.count_;
	owner_ = st.owner_;
	pthread_mutex_unlock(&m_);
}

#endif  // PTHREAD_MUTEX_RECURSIVE

#ifdef PTHREAD_MUTEX_RECURSIVE

recursive_try_mutex::recursive_try_mutex()
{
	pthread_mutexattr_t attr;
	if (pthread_mutexattr_init(&attr))
		detail::throw_thread_resource_error();
	if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) ||
	    pthread_mutex_init(&m_, &attr))
	{
		pthread_mutexattr_destroy(&attr);
		detail::throw_thread_resource_error();
	}
	pthread_mutexattr_destroy(&attr);
}

#else  // PTHREAD_MUTEX_RECURSIVE

recursive_try_mutex::recursive_try_mutex()
	:	count_(0)
{
	if (pthread_mutex_init(&m_, 0))
		detail::throw_thread_resource_error();
	if (pthread_cond_init(&c_, 0))
	{
		pthread_mutex_destroy(&m_);
		detail::throw_thread_resource_error();
	}
}

recursive_try_mutex::~recursive_try_mutex()
{
	pthread_cond_destroy(&c_);
	pthread_mutex_destroy(&m_);
}

void
recursive_try_mutex::lock()
{
	pthread_t self = pthread_self();
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (count_ > 0 && pthread_equal(self, owner_))
		++count_;
	else
	{
		while (count_ > 0)
			pthread_cond_wait(&c_, &m_);
		count_ = 1;
		owner_ = self;
	}
	pthread_mutex_unlock(&m_);
}

bool
recursive_try_mutex::try_lock()
{
	pthread_t self = pthread_self();
	bool owns = false;
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (count_ == 0)
	{
		owner_ = self;
		++count_;
		owns = true;
	}
	else if (pthread_equal(self, owner_))
	{
		++count_;
		owns = true;
	}
	pthread_mutex_unlock(&m_);
	return owns;
}

void
recursive_try_mutex::unlock()
{
	pthread_t self = pthread_self();
	if (pthread_mutex_lock(&m_) || count_ == 0 || !pthread_equal(self, owner_))
		detail::throw_lock_error();
	bool notify = --count_ == 0;
	pthread_mutex_unlock(&m_);
	if (notify)
		pthread_cond_broadcast(&c_);
}

void
recursive_try_mutex::wait_prefix(state& st)
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	st.count_ = count_;
	st.owner_ = owner_;
	count_ = 0;
	pthread_cond_broadcast(&c_);
}

void
recursive_try_mutex::wait_suffix(const state& st)
{
	while (count_ > 0)
		pthread_cond_wait(&c_, &m_);
	count_ = st.count_;
	owner_ = st.owner_;
	pthread_mutex_unlock(&m_);
}

#endif  // PTHREAD_MUTEX_RECURSIVE

#ifndef _MSL_NO_TIME_SUPPORT

recursive_timed_mutex::recursive_timed_mutex()
	:	count_(0)
{
	if (pthread_mutex_init(&m_, 0))
		detail::throw_thread_resource_error();
	if (pthread_cond_init(&c_, 0))
	{
		pthread_mutex_destroy(&m_);
		detail::throw_thread_resource_error();
	}
}

recursive_timed_mutex::~recursive_timed_mutex()
{
	pthread_cond_destroy(&c_);
	pthread_mutex_destroy(&m_);
}

void
recursive_timed_mutex::lock()
{
	pthread_t self = pthread_self();
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (count_ > 0 && pthread_equal(self, owner_))
		++count_;
	else
	{
		while (count_ > 0)
			pthread_cond_wait(&c_, &m_);
		count_ = 1;
		owner_ = self;
	}
	pthread_mutex_unlock(&m_);
}

bool
recursive_timed_mutex::try_lock()
{
	pthread_t self = pthread_self();
	bool owns = false;
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (count_ == 0)
	{
		owner_ = self;
		++count_;
		owns = true;
	}
	else if (pthread_equal(self, owner_))
	{
		++count_;
		owns = true;
	}
	pthread_mutex_unlock(&m_);
	return owns;
}

bool
recursive_timed_mutex::timed_lock(const universal_time& unv_time)
{
	pthread_t self = pthread_self();
	bool owns = false;
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	if (count_ == 0)
	{
		owner_ = self;
		++count_;
		owns = true;
	}
	else if (pthread_equal(self, owner_))
	{
		++count_;
		owns = true;
	}
	else
	{
		while (count_ > 0)
		{
			if (pthread_cond_timedwait(&c_, &m_, (const timespec*)&unv_time))
				break;
		}
		if (count_ == 0)
		{
			owner_ = self;
			++count_;
			owns = true;
		}
	}
	pthread_mutex_unlock(&m_);
	return owns;
}

void
recursive_timed_mutex::unlock()
{
	pthread_t self = pthread_self();
	if (pthread_mutex_lock(&m_) || count_ == 0 || !pthread_equal(self, owner_))
		detail::throw_lock_error();
	bool notify = --count_ == 0;
	pthread_mutex_unlock(&m_);
	if (notify)
		pthread_cond_broadcast(&c_);
}

void
recursive_timed_mutex::wait_prefix(state& st)
{
	if (pthread_mutex_lock(&m_))
		detail::throw_lock_error();
	st.count_ = count_;
	st.owner_ = owner_;
	count_ = 0;
	pthread_cond_broadcast(&c_);
}

void
recursive_timed_mutex::wait_suffix(const state& st)
{
	while (count_ > 0)
		pthread_cond_wait(&c_, &m_);
	count_ = st.count_;
	owner_ = st.owner_;
	pthread_mutex_unlock(&m_);
}

#endif  // _MSL_NO_TIME_SUPPORT

#endif  // _MSL_USE_PTHREADS

#ifdef _MSL_USE_MPTASKS

#ifndef _MSL_NO_TIME_SUPPORT

bool
timed_mutex::timed_lock(const elapsed_time& elps_time)
{
	int dt = 0;
	if (elps_time.sec_ > 0)
		dt = elps_time.sec_ * 1000;
	else if (elps_time.nsec_ > 0)
		dt = -elps_time.nsec_ / 1000;
	int result = MPEnterCriticalRegion(m_, dt);
	if (result != 0 && result != kMPTimeoutErr)
		detail::throw_lock_error();
	return result == 0;
}

bool
recursive_timed_mutex::timed_lock(const elapsed_time& elps_time)
{
	int dt = 0;
	if (elps_time.sec_ > 0)
		dt = elps_time.sec_ * 1000;
	else if (elps_time.nsec_ > 0)
		dt = -elps_time.nsec_ / 1000;
	int result = MPEnterCriticalRegion(m_, dt);
	if (result != 0 && result != kMPTimeoutErr)
		detail::throw_lock_error();
	return result == 0;
}

#endif  // _MSL_NO_TIME_SUPPORT

#endif // _MSL_USE_MPTASKS

#ifdef _MSL_USE_WINTHREADS

#ifndef _MSL_NO_TIME_SUPPORT

bool
timed_mutex::timed_lock(const elapsed_time& elps_time)
{
	DWORD dt = (DWORD)(elps_time.sec_ * 1000 + elps_time.nsec_ / 1000);
	DWORD result = WaitForSingleObject(m_, dt);
	if (result != WAIT_OBJECT_0 && result != WAIT_TIMEOUT)
		detail::throw_lock_error();
	return result == WAIT_OBJECT_0;
}

bool
recursive_timed_mutex::timed_lock(const elapsed_time& elps_time)
{
	DWORD dt = (DWORD)(elps_time.sec_ * 1000 + elps_time.nsec_ / 1000);
	DWORD result = WaitForSingleObject(m_, dt);
	if (result != WAIT_OBJECT_0 && result != WAIT_TIMEOUT)
		detail::throw_lock_error();
	return result == WAIT_OBJECT_0;
}

#endif  // _MSL_NO_TIME_SUPPORT

#endif  // _MSL_USE_WINTHREADS

#ifndef _MSL_NO_CONDITION

rw_mutex::rw_mutex()
	:	n_readers_(0),
		write_mode_(false)
{
}

void
rw_mutex::lock_read()
{
	scoped_lock lk(mut_);
	while (write_mode_)
		read_.wait(lk);
	++n_readers_;
}

bool
rw_mutex::try_lock_read()
{
	scoped_try_lock lk(mut_, false);
	if (!lk.try_lock() || write_mode_)
		return false;
	++n_readers_;
	return true;
}

void
rw_mutex::unlock_read()
{
	scoped_lock lk(mut_);
	if (--n_readers_ == 0)
		write_.notify_all();
}

void
rw_mutex::lock_write()
{
	scoped_lock lk(mut_);
	while (write_mode_ + n_readers_)
		write_.wait(lk);
	write_mode_ = true;
}

bool
rw_mutex::try_lock_write()
{
	scoped_try_lock lk(mut_, false);
	if (!lk.try_lock() || write_mode_ + n_readers_)
		return false;
	write_mode_ = true;
	return true;
}

void
rw_mutex::unlock_write()
{
	scoped_lock lk(mut_);
	write_mode_ = false;
	write_.notify_all();
	read_.notify_all();
}

#endif  // _MSL_NO_CONDITION

#ifndef _MSL_NO_CPP_NAMESPACE
	} // namespace Metrowerks
#endif

// hh 030616 Created
// hh 031202 Added rw_mutex
