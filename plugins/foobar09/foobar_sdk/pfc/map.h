#ifndef _MAP_T_H_INCLUDED_
#define _MAP_T_H_INCLUDED_

namespace pfc {

	template<typename t_destination> class __map_overwrite_wrapper {
	public:
		__map_overwrite_wrapper(t_destination & p_destination) : m_destination(p_destination) {}
		template<typename t_from,typename t_to> void operator() (const t_from & p_from,const t_to & p_to) {m_destination.set(p_from,p_to);}
	private:
		t_destination & m_destination;
	};

	template<typename t_storage_from, typename t_storage_to, typename t_comparator = comparator_default>
	class map_t {
	private:
		typedef map_t<t_storage_from,t_storage_to,t_comparator> t_self;
	public:
		template<typename t_from,typename t_to>
		void set(const t_from & p_from, const t_to & p_to) {
			bool isnew;
			t_storage & storage = m_data.add_ex(t_search_set<t_from,t_to>(p_from,p_to), isnew);
			if (!isnew) storage.m_to = p_to;
		}

		template<typename t_from>
		t_storage_to & find_or_add(t_from const & p_from) {
			return m_data.add(t_search_query<t_from>(p_from)).m_to;
		}

		template<typename t_from>
		t_storage_to & find_or_add_ex(t_from const & p_from,bool & p_isnew) {
			return m_data.add_ex(t_search_query<t_from>(p_from),p_isnew).m_to;
		}

		template<typename t_from>
		bool have_item(const t_from & p_from) const {
			return m_data.have_item(t_search_query<t_from>(p_from));
		}

		template<typename t_from,typename t_to>
		bool query(const t_from & p_from,t_to & p_to) const {
			const t_storage * storage = m_data.find_ptr(t_search_query<t_from>(p_from));
			if (storage == NULL) return false;
			p_to = storage->m_to;
			return true;
		}

		template<typename t_from>
		const t_storage_to * query_ptr(const t_from & p_from) const {
			const t_storage * storage = m_data.find_ptr(t_search_query<t_from>(p_from));
			if (storage == NULL) return NULL;
			return &storage->m_to;
		}

		template<typename t_from>
		t_storage_to * query_ptr(const t_from & p_from) {
			t_storage * storage = m_data.find_ptr(t_search_query<t_from>(p_from));
			if (storage == NULL) return NULL;
			return &storage->m_to;
		}

		template<bool inclusive,bool above,typename t_from>
		const t_storage_to * query_nearest_ptr(t_from & p_from) const {
			const t_storage * storage = m_data.find_nearest_item<inclusive,above>(t_search_query<t_from>(p_from));
			if (storage == NULL) return NULL;
			p_from = storage->m_from;
			return &storage->m_to;
		}

		template<bool inclusive,bool above,typename t_from>
		t_storage_to * query_nearest_ptr(t_from & p_from) {
			t_storage * storage = m_data.find_nearest_item<inclusive,above>(t_search_query<t_from>(p_from));
			if (storage == NULL) return NULL;
			p_from = storage->m_from;
			return &storage->m_to;
		}

		template<bool inclusive,bool above,typename t_from,typename t_to>
		bool query_nearest(t_from & p_from,t_to & p_to) const {
			const t_storage * storage = m_data.find_nearest_item<inclusive,above>(t_search_query<t_from>(p_from));
			if (storage == NULL) return false;
			p_from = storage->m_from;
			p_to = storage->m_to;
			return true;
		}

		template<typename t_from>
		void remove(const t_from & p_from) {
			m_data.remove_item(t_search_query<t_from>(p_from));
		}

		template<typename t_callback>
		void enumerate(t_callback & p_callback) const {
			m_data.enumerate(enumeration_wrapper<t_callback>(p_callback));
		}

		template<typename t_callback>
		void enumerate(t_callback & p_callback) {
			m_data.__enumerate(enumeration_wrapper_var<t_callback>(p_callback));
		}


		t_size get_count() const {return m_data.get_count();}

		void remove_all() {m_data.remove_all();}

		template<typename t_source>
		void overwrite(const t_source & p_source) {
			__map_overwrite_wrapper<t_self> wrapper(*this);
			p_source.enumerate(wrapper);
		}

		//backwards compatibility method wrappers
		template<typename t_from> bool exists(const t_from & p_from) const {return have_item(p_from);}

	private:
		template<typename t_from>
		struct t_search_query {
			t_search_query(const t_from & p_from) : m_from(p_from) {}
			t_from const & m_from;
		};
		template<typename t_from,typename t_to>
		struct t_search_set {
			t_search_set(const t_from & p_from, const t_to & p_to) : m_from(p_from), m_to(p_to) {}

			t_from const & m_from;
			t_to const & m_to;
		};

		struct t_storage {
			t_storage_from m_from;
			t_storage_to m_to;


			
			template<typename t_from>
			t_storage(t_search_query<t_from> const & p_source) : m_from(p_source.m_from), m_to() {}

			template<typename t_from,typename t_to>
			t_storage(t_search_set<t_from,t_to> const & p_source) : m_from(p_source.m_from), m_to(p_source.m_to) {}
		};

		class comparator_wrapper {
		public:
			template<typename t_other>
			inline static int compare(const t_storage & p_item1,const t_other & p_item2) {
				return t_comparator::compare(p_item1.m_from,p_item2.m_from);
			}
		};

		template<typename t_callback>
		class enumeration_wrapper {
		public:
			enumeration_wrapper(t_callback & p_callback) : m_callback(p_callback) {}
			void operator()(const t_storage & p_item) {m_callback(p_item.m_from,p_item.m_to);}
		private:
			t_callback & m_callback;
		};

		template<typename t_callback>
		class enumeration_wrapper_var {
		public:
			enumeration_wrapper_var(t_callback & p_callback) : m_callback(p_callback) {}
			void operator()(t_storage & p_item) {m_callback(safe_cast<t_storage_from const&>(p_item.m_from),p_item.m_to);}
		private:
			t_callback & m_callback;
		};

		typedef avltree_t<t_storage,comparator_wrapper> t_content;

		t_content m_data;
	public:
		typedef traits_t<t_content> traits;
	};

	template<typename t_storage_from, typename t_storage_to, typename t_comparator>
	class traits_t<map_t<t_storage_from,t_storage_to,t_comparator> > : public map_t<t_storage_from,t_storage_to,t_comparator>::traits {};


}

#endif //_MAP_T_H_INCLUDED_
