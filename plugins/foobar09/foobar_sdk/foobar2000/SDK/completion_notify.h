//! Generic service for receiving notifications about async operation completion. Used by various other services.
class completion_notify : public service_base {
public:
	//! Called when an async operation has been completed. Note that on_completion is always called from main thread. You can use on_completion_async() helper if you need to signal completion while your context is in another thread.\n
	//! IMPLEMENTATION WARNING: If process being completed creates a window taking caller's window as parent, you must not destroy the parent window inside on_completion(). If you need to do so, use PostMessage() or main_thread_callback to delay the deletion.
	//! @param p_code Context-specific status code. Possible values depend on the operation being performed.
	virtual void on_completion(unsigned p_code) = 0;

	//! Helper. Queues a notification, using main_thread_callback.
	void on_completion_async(unsigned p_code);

	//! Helper. Checks for null ptr and calls on_completion_async when the ptr is not null.
	static void g_signal_completion_async(service_ptr_t<completion_notify> p_notify,unsigned p_code);

	FB2K_MAKE_SERVICE_INTERFACE(completion_notify,service_base);
};

//! Helper implementation.
class completion_notify_orphanable : public completion_notify {
public:
	virtual void orphan() = 0;
};

//! Helper implementation.
//! IMPLEMENTATION WARNING: If process being completed creates a window taking caller's window as parent, you must not destroy the parent window inside on_task_completion(). If you need to do so, use PostMessage() or main_thread_callback to delay the deletion.
template<typename t_receiver>
class completion_notify_impl : public completion_notify_orphanable {
public:
	void on_completion(unsigned p_code) {
		if (m_receiver != NULL) {
			m_receiver->on_task_completion(m_taskid,p_code);
		}
	}
	void setup(t_receiver * p_receiver, unsigned p_task_id) {m_receiver = p_receiver; m_taskid = p_task_id;}
	void orphan() {m_receiver = NULL; m_taskid = 0;}
private:
	t_receiver * m_receiver;
	unsigned m_taskid;
};

template<typename t_receiver>
service_ptr_t<completion_notify_orphanable> completion_notify_create(t_receiver * p_receiver,unsigned p_taskid) {
	service_ptr_t<completion_notify_impl<t_receiver> > instance = new service_impl_t<completion_notify_impl<t_receiver> >();
	instance->setup(p_receiver,p_taskid);
	return instance;
}

typedef service_ptr_t<completion_notify> completion_notify_ptr;