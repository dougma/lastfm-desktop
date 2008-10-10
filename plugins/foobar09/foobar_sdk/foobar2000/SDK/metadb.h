#ifndef _METADB_H_
#define _METADB_H_

//! API for tag read/write operations. Legal to call from main thread only, except for hint_multi_async() / hint_async().\n
//! Implemented only by core, do not reimplement.\n
//! Use static_api_ptr_t template to access metadb_io methods.\n
//! WARNING: Methods that perform file access (tag reads/writes) run a modal message loop. They SHOULD NOT be called from global callbacks and such.
class NOVTABLE metadb_io : public service_base
{
public:
	enum t_load_info_type {
		load_info_default,
		load_info_force,
		load_info_check_if_changed
	};

	enum t_update_info_state {
		update_info_success,
		update_info_aborted,
		update_info_errors,		
	};
	
	enum t_load_info_state {
		load_info_success,
		load_info_aborted,
		load_info_errors,		
	};

	//! Returns whether some tag I/O operation is currently running. Another one can't be started.
	__declspec(deprecated) virtual bool is_busy() = 0;
	//! Returns whether - in result of user settings - all update operations will fail.
	__declspec(deprecated) virtual bool is_updating_disabled() = 0;
	//! Returns whether - in result of user settings - all update operations will silently succeed but without actually modifying files.
	__declspec(deprecated) virtual bool is_file_updating_blocked() = 0;
	//! If another tag I/O operation is running, this call will give focus to its progress window.
	__declspec(deprecated) virtual void highlight_running_process() = 0;
	//! Loads tags from multiple items.
	__declspec(deprecated) virtual t_load_info_state load_info_multi(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,t_load_info_type p_type,HWND p_parent_window,bool p_show_errors) = 0;
	//! Updates tags on multiple items.
	__declspec(deprecated) virtual t_update_info_state update_info_multi(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,const pfc::list_base_const_t<file_info*> & p_new_info,HWND p_parent_window,bool p_show_errors) = 0;
	//! Rewrites tags on multiple items.
	__declspec(deprecated) virtual t_update_info_state rewrite_info_multi(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,HWND p_parent_window,bool p_show_errors) = 0;
	//! Removes tags from multiple items.
	__declspec(deprecated) virtual t_update_info_state remove_info_multi(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,HWND p_parent_window,bool p_show_errors) = 0;

	virtual void hint_multi(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,const pfc::list_base_const_t<const file_info*> & p_infos,const pfc::list_base_const_t<t_filestats> & p_stats,const bit_array & p_fresh_mask) = 0;

	virtual void hint_multi_async(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,const pfc::list_base_const_t<const file_info*> & p_infos,const pfc::list_base_const_t<t_filestats> & p_stats,const bit_array & p_fresh_mask) = 0;

	virtual void hint_reader(service_ptr_t<class input_info_reader> p_reader,const char * p_path,abort_callback & p_abort) = 0;

	//! For internal use only.
	virtual void path_to_handles_simple(const char * p_path,pfc::list_base_t<metadb_handle_ptr> & p_out) = 0;

	//! Dispatches metadb_io_callback calls with specified items. To be used with metadb_display_hook when your component needs specified items refreshed.
	virtual void dispatch_refresh(const pfc::list_base_const_t<metadb_handle_ptr> & p_list) = 0;

	void hint_async(metadb_handle_ptr p_item,const file_info & p_info,const t_filestats & p_stats,bool p_fresh);

	__declspec(deprecated) t_load_info_state load_info(metadb_handle_ptr p_item,t_load_info_type p_type,HWND p_parent_window,bool p_show_errors);
	__declspec(deprecated) t_update_info_state update_info(metadb_handle_ptr p_item,file_info & p_info,HWND p_parent_window,bool p_show_errors);
	
	FB2K_MAKE_SERVICE_INTERFACE_ENTRYPOINT(metadb_io);
};

//! Implementing this class gives you direct control over which part of file_info gets altered during a tag update uperation. To be used with metadb_io_v2::update_info_async().
class NOVTABLE file_info_filter : public service_base {
public:
	//! Alters specified file_info entry; called as a part of tag update process. Specified file_info has been read from a file, and will be written back.\n
	//! WARNING: This will be typically called from another thread than main app thread (precisely, from thread created by tag updater). You should copy all relevant data to members of your file_info_filter instance in constructor and reference only member data in apply_filter() implementation.
	//! @returns True when you have altered file_info and changes need to be written back to the file; false if no changes have been made.
	virtual bool apply_filter(metadb_handle_ptr p_location,t_filestats p_stats,file_info & p_info) = 0;
	
	FB2K_MAKE_SERVICE_INTERFACE(file_info_filter,service_base);
};

//! Advanced interface for passing infos read from files to metadb backend. Use metadb_io_v2::create_hint_list() to instantiate.
class NOVTABLE metadb_hint_list : public service_base {
public:
	//! Adds a hint to the list.
	//! @param p_location Location of the item the hint applies to.
	//! @param p_info file_info object describing the item.
	//! @param p_stats Information about the file containing item the hint applies to.
	//! @param p_freshflag Set to true if the info has been directly read from the file, false if it comes from another source such as a playlist file.
	virtual void add_hint(metadb_handle_ptr const & p_location,const file_info & p_info,const t_filestats & p_stats,bool p_freshflag) = 0;
	//! Reads info from specified info reader instance and adds hints. May throw an exception in case info read has failed.
	virtual void add_hint_reader(const char * p_path,service_ptr_t<input_info_reader> const & p_reader,abort_callback & p_abort) = 0;
	//! Call this when you're done working with this metadb_hint_list instance, to apply hints and dispatch callbacks. If you don't call this, all added hints will be ignored.
	virtual void on_done() = 0;

	FB2K_MAKE_SERVICE_INTERFACE(metadb_hint_list,service_base);
};

//! New in 0.9.3. Extends metadb_io functionality with nonblocking versions of tag read/write functions, and some other utility features.
class NOVTABLE metadb_io_v2 : public metadb_io {
public:
	enum {
		//! By default, when some part of requested operation could not be performed for reasons other than user abort, a popup dialog with description of the problem is spawned.\n
		//! Set this flag to disable error notification.
		op_flag_no_errors		= 1 << 0,
		//! Set this flag to make the progress dialog not steal focus on creation.
		op_flag_background		= 1 << 1,
		//! Set this flag to delay the progress dialog becoming visible, so it does not appear at all during short operations. Also implies op_flag_background effect.
		op_flag_delay_ui		= 1 << 2,
	};

	//! @param p_list List of items to process.
	//! @param p_op_flags Can be null, or one or more of op_flag_* enum values combined, altering behaviors of the operation.
	//! @param p_notify Called when the task is completed. Status code is one of t_load_info_state values. Can be null if caller doesn't care.
	virtual void load_info_async(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,t_load_info_type p_type,HWND p_parent_window,t_uint32 p_op_flags,completion_notify_ptr p_notify) = 0;
	//! @param p_list List of items to process.
	//! @param p_op_flags Can be null, or one or more of op_flag_* enum values combined, altering behaviors of the operation.
	//! @param p_notify Called when the task is completed. Status code is one of t_update_info values. Can be null if caller doesn't care.
	//! @param p_filter Callback handling actual file_info alterations. Typically used to replace entire meta part of file_info, or to alter something else such as ReplayGain while leaving meta intact.
	virtual void update_info_async(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,service_ptr_t<file_info_filter> p_filter,HWND p_parent_window,t_uint32 p_op_flags,completion_notify_ptr p_notify) = 0;
	//! @param p_list List of items to process.
	//! @param p_op_flags Can be null, or one or more of op_flag_* enum values combined, altering behaviors of the operation.
	//! @param p_notify Called when the task is completed. Status code is one of t_update_info values. Can be null if caller doesn't care.
	virtual void rewrite_info_async(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,HWND p_parent_window,t_uint32 p_op_flags,completion_notify_ptr p_notify) = 0;
	//! @param p_list List of items to process.
	//! @param p_op_flags Can be null, or one or more of op_flag_* enum values combined, altering behaviors of the operation.
	//! @param p_notify Called when the task is completed. Status code is one of t_update_info values. Can be null if caller doesn't care.
	virtual void remove_info_async(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,HWND p_parent_window,t_uint32 p_op_flags,completion_notify_ptr p_notify) = 0;

	//! Creates a metadb_hint_list object.
	virtual service_ptr_t<metadb_hint_list> create_hint_list() = 0;

	//! @param p_list List of items to process.
	//! @param p_op_flags Can be null, or one or more of op_flag_* enum values combined, altering behaviors of the operation.
	//! @param p_notify Called when the task is completed. Status code is one of t_update_info values. Can be null if caller doesn't care.
	//! @param p_new_info New infos to write to specified items.
	void update_info_async_simple(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,const pfc::list_base_const_t<const file_info*> & p_new_info, HWND p_parent_window,t_uint32 p_op_flags,completion_notify_ptr p_notify);

	FB2K_MAKE_SERVICE_INTERFACE(metadb_io_v2,metadb_io);
};

//! Callback service receiving notifications about metadb contents changes.
class NOVTABLE metadb_io_callback : public service_base {
public:
	//! Called when metadb contents change. (Or, one of display hook component requests display update).
	//! @param p_items_sorted List of items that have been updated. The list is always sorted by pointer value, to allow fast bsearch to test whether specific item has changed.
	//! @param p_fromhook Set to true when actual contents haven't changed but one of display hooks requested an update.
	virtual void on_changed_sorted(const pfc::list_base_const_t<metadb_handle_ptr> & p_items_sorted, bool p_fromhook) = 0;

	FB2K_MAKE_SERVICE_INTERFACE_ENTRYPOINT(metadb_io_callback);
};

//! Entrypoint service for metadb_handle related operations.\n
//! Implemented only by core, do not reimplement.\n
//! Use static_api_ptr_t template to access it, e.g. static_api_ptr_t<metadb>()->handle_create(myhandle,mylocation);
class NOVTABLE metadb : public service_base
{
public:
	//! Locks metadb to prevent other threads from modifying it while you're working with some of its contents. Some functions (metadb_handle::get_info_locked(), metadb_handle::get_info_async_locked()) can be called only from inside metadb lock section.
	virtual void database_lock()=0;
	//! Unlocks metadb after database_lock(). Some functions (metadb_handle::get_info_locked(), metadb_handle::get_info_async_locked()) can be called only from inside metadb lock section.
	virtual void database_unlock()=0;
	
	//! Returns metadb_handle object referencing specified location (attempts to find existing one, creates new one if doesn't exist).
	//! @param p_out Receives metadb_handle pointer.
	//! @param p_location Location to create metadb_handle for.
	virtual void handle_create(metadb_handle_ptr & p_out,const playable_location & p_location)=0;

	void handle_create_replace_path_canonical(metadb_handle_ptr & p_out,const metadb_handle_ptr & p_source,const char * p_new_path);
	void handle_replace_path_canonical(metadb_handle_ptr & p_out,const char * p_new_path);
	void handle_create_replace_path(metadb_handle_ptr & p_out,const metadb_handle_ptr & p_source,const char * p_new_path);

	//! Helper function; attempts to retrieve a handle to any known playable location to be used for e.g. titleformatting script preview.\n
	//! @returns True on success; false on failure (no known playable locations).
	static bool g_get_random_handle(metadb_handle_ptr & p_out);

	enum {case_sensitive = true};
	typedef pfc::comparator_strcmp path_comparator;

	inline static int path_compare_ex(const char * p1,unsigned len1,const char * p2,unsigned len2) {return case_sensitive ? pfc::strcmp_ex(p1,len1,p2,len2) : stricmp_utf8_ex(p1,len1,p2,len2);}
	inline static int path_compare(const char * p1,const char * p2) {return case_sensitive ? strcmp(p1,p2) : stricmp_utf8(p1,p2);}
	inline static int path_compare_metadb_handle(const metadb_handle_ptr & p1,const metadb_handle_ptr & p2) {return path_compare(p1->get_path(),p2->get_path());}

	FB2K_MAKE_SERVICE_INTERFACE_ENTRYPOINT(metadb);
};


class in_metadb_sync {
public:
	in_metadb_sync() {
		m_api->database_lock();
	}
	~in_metadb_sync() {
		m_api->database_unlock();
	}
private:
	static_api_ptr_t<metadb> m_api;
};

class in_metadb_sync_fromptr {
public:
	in_metadb_sync_fromptr(const service_ptr_t<metadb> & p_api) : m_api(p_api) {m_api->database_lock();}
	~in_metadb_sync_fromptr() {m_api->database_unlock();}
private:
	service_ptr_t<metadb> m_api;
};

class in_metadb_sync_fromhandle {
public:
	in_metadb_sync_fromhandle(const service_ptr_t<metadb_handle> & p_api) : m_api(p_api) {m_api->metadb_lock();}
	~in_metadb_sync_fromhandle() {m_api->metadb_unlock();}
private:
	service_ptr_t<metadb_handle> m_api;
};


//! Deprecated - use metadb_io_v2::update_info_async w/ file_info_filter whenever possible.
class __declspec(deprecated("Use metadb_io_v2::update_info_async instead whenever possible.")) file_info_update_helper
{
public:
	file_info_update_helper(const pfc::list_base_const_t<metadb_handle_ptr> & p_data);
	file_info_update_helper(metadb_handle_ptr p_item);

	bool read_infos(HWND p_parent,bool p_show_errors);

	enum t_write_result
	{
		write_ok,
		write_aborted,
		write_error
	};
	t_write_result write_infos(HWND p_parent,bool p_show_errors);

	t_size get_item_count() const;
	bool is_item_valid(t_size p_index) const;//returns false where info reading failed
	
	file_info & get_item(t_size p_index);
	metadb_handle_ptr get_item_handle(t_size p_index) const;

	void invalidate_item(t_size p_index);

private:
	metadb_handle_list m_data;
	pfc::array_t<file_info_impl> m_infos;
	pfc::array_t<bool> m_mask;
};

class titleformat_text_out;
class titleformat_hook_function_params;


/*
	Implementing this service installs a global hook for metadb_handle::format_title field processing. \n
	This should be implemented only where absolutely necessary, for safety and performance reasons. \n
	metadb_display_hook methods should NEVER make any other API calls (other than possibly querying information from passed metadb_handle pointer), only read implementation-specific private data and return as soon as possible. Since those are called from metadb_handle::format_title, no assumptions should be made about calling context (threading etc). \n
	Both methods are called from inside metadb lock, so no additional locking is required to use *_locked metadb_handle methods.
	See titleformat_hook for more info about methods/parameters. \n
	If there are multiple metadb_display_hook implementations registered, call order is undefined.
*/

class metadb_display_hook : public service_base {
public:
	virtual bool process_field(metadb_handle * p_handle,titleformat_text_out * p_out,const char * p_name,t_size p_name_length,bool & p_found_flag) = 0;
	virtual bool process_function(metadb_handle * p_handle,titleformat_text_out * p_out,const char * p_name,t_size p_name_length,titleformat_hook_function_params * p_params,bool & p_found_flag) = 0;

	FB2K_MAKE_SERVICE_INTERFACE_ENTRYPOINT(metadb_display_hook);
};







//! Helper implementation of file_info_filter_impl.
class file_info_filter_impl : public file_info_filter {
public:
	file_info_filter_impl(const pfc::list_base_const_t<metadb_handle_ptr> & p_list,const pfc::list_base_const_t<const file_info*> & p_new_info) {
		pfc::dynamic_assert(p_list.get_count() == p_new_info.get_count());
		pfc::array_t<t_size> order;
		order.set_size(p_list.get_count());
		order_helper::g_fill(order.get_ptr(),order.get_size());
		p_list.sort_get_permutation_t(pfc::compare_t<metadb_handle_ptr,metadb_handle_ptr>,order.get_ptr());
		m_handles.set_count(order.get_size());
		m_infos.set_size(order.get_size());
		for(t_size n = 0; n < order.get_size(); n++) {
			m_handles[n] = p_list[order[n]];
			m_infos[n] = *p_new_info[order[n]];
		}
	}

	bool apply_filter(metadb_handle_ptr p_location,t_filestats p_stats,file_info & p_info) {
		t_size index;
		if (m_handles.bsearch_t(pfc::compare_t<metadb_handle_ptr,metadb_handle_ptr>,p_location,index)) {
			p_info = m_infos[index];
			return true;
		} else {
			return false;
		}
	}
private:
	metadb_handle_list m_handles;
	pfc::array_t<file_info_impl> m_infos;
};

#endif