#ifndef _AVLTREE_T_H_INCLUDED_
#define _AVLTREE_T_H_INCLUDED_

namespace pfc {

	template<typename t_storage,typename t_comparator = comparator_default>
	class avltree_t {
		typedef avltree_t<t_storage,t_comparator> t_self;

		template<typename t_item1,typename t_item2>
		inline static int compare(const t_item1 & p_item1, const t_item2 & p_item2) {
			return t_comparator::compare(p_item1,p_item2);
		}

		struct t_node {
			t_node *m_left,*m_right;
			t_size m_depth;
			t_storage m_data;

			template<bool p_which> t_node * & child() {return p_which ? m_right : m_left;}
			template<bool p_which> t_node * child() const {return p_which ? m_right : m_left;}

			~t_node() {
				if (m_left != NULL) delete m_left;
				if (m_right != NULL) delete m_right;
			}

			template<typename T>
			t_node(T const & p_param) : m_data(p_param), m_left(NULL), m_right(NULL), m_depth(0) {}
		};

		t_node * m_root;

		static t_size calc_depth(const t_node * ptr)
		{
			return ptr ? 1+ptr->m_depth : 0;
		}

		static void recalc_depth(t_node * ptr) {
			ptr->m_depth = pfc::max_t(calc_depth(ptr->m_left), calc_depth(ptr->m_right));
		}

		static void assert_children(t_node * ptr)
		{
	#ifdef _DEBUG
			PFC_ASSERT(ptr->m_depth == pfc::max_t(calc_depth(ptr->m_left),calc_depth(ptr->m_right)) );
	#endif
		}

		static int test_depth(t_node * ptr)
		{
			if (ptr==0) return 0;
			else return calc_depth(ptr->m_right) - calc_depth(ptr->m_left);
		}

		static t_node * extract_left_leaf(t_node * & p_base) {
			if (p_base->m_left != NULL) {
				t_node * ret = extract_left_leaf(p_base->m_left);
				recalc_depth(p_base);
				g_rebalance(p_base);
				return ret;
			} else {
				t_node * node = p_base;
				p_base = node->m_right;
				node->m_right = 0;
				node->m_depth = 0;
				return node;
			}
		}

		static t_node * extract_right_leaf(t_node ** p_base) {
			t_node * node = *p_base;
			if (node->m_right != NULL) {
				t_node * ret = extract_right_leaf(&node->m_right);
				recalc_depth(node);
				g_rebalance(p_base);
				return ret;
			} else {
				*p_base = node->m_left;
				node->m_left = 0;
				node->m_depth = 0;
				return node;
			}
		}

		static void remove_internal(t_node* & p_node) {
			t_node * deleteme = p_node;
			if (p_node->m_left==0)
				p_node = p_node->m_right;
			else if (p_node->m_right==0)
				p_node = p_node->m_left;
			else {
				t_node * swap = extract_left_leaf(p_node->m_right);
				swap->m_left = deleteme->m_left;
				swap->m_right = deleteme->m_right;
				recalc_depth(swap);
				p_node = swap;
			}
			deleteme->m_left = deleteme->m_right = NULL;
			deleteme->m_depth = 0;
			delete deleteme;
		}

		template<typename t_nodewalk,typename t_callback>
		static void __enum_items_recur(t_nodewalk * p_node,t_callback & p_callback) {
			if (p_node != NULL) {
				__enum_items_recur<t_nodewalk>(p_node->m_left,p_callback);
				p_callback (p_node->m_data);
				__enum_items_recur<t_nodewalk>(p_node->m_right,p_callback);			
			}
		}

		template<typename t_search>
		static t_storage * g_find_or_add(t_node * & p_base,t_search const & p_search,bool & p_new)
		{
			if (p_base == NULL) {
				p_base = new t_node(p_search);
				p_new = true;
				return &p_base->m_data;
			}

			int result = compare(p_base->m_data,p_search);
			if (result > 0) {
				t_storage * ret = g_find_or_add<t_search>(p_base->m_left,p_search,p_new);
				if (p_new) {
					recalc_depth(p_base);
					g_rebalance(p_base);
				}
				return ret;
			} else if (result < 0) {
				t_storage * ret = g_find_or_add<t_search>(p_base->m_right,p_search,p_new);
				if (p_new) {
					recalc_depth(p_base);
					g_rebalance(p_base);
				}
				return ret;
			} else {
				p_new = false;
				return &p_base->m_data;
			}
		}


		static void g_rotate_right(t_node * & p_node) {
			t_node * oldroot = p_node;
			t_node * newroot = oldroot->m_right;
			PFC_ASSERT(newroot != NULL);
			oldroot->m_right = newroot->m_left;
			newroot->m_left = oldroot;
			recalc_depth(oldroot);
			recalc_depth(newroot);
			p_node = newroot;
		}

		static void g_rotate_left(t_node * & p_node) {
			t_node * oldroot = p_node;
			t_node * newroot = oldroot->m_left;
			PFC_ASSERT(newroot != NULL);
			oldroot->m_left = newroot->m_right;
			newroot->m_right = oldroot;
			recalc_depth(oldroot);
			recalc_depth(newroot);
			p_node = newroot;
		}

		static void g_rebalance(t_node * & p_node) {
			int balance = test_depth(p_node);
			if (balance > 1) {
				//right becomes root
				if (test_depth((p_node)->m_right) < 0) {
					g_rotate_left((p_node)->m_right);
				}
				g_rotate_right(p_node);
			} else if (balance < -1) {
				//left becomes root
				if (test_depth((p_node)->m_left) > 0) {
					g_rotate_right((p_node)->m_left);
				}
				g_rotate_left(p_node);
			}
		}

		template<typename t_search>
		static bool g_remove(t_node * & p_node,t_search const & p_search) {
			if (p_node == NULL) return false;

			int result = compare(p_node->m_data,p_search);
			if (result == 0) {
				remove_internal(p_node);
				if (p_node != NULL) {
					recalc_depth(p_node);
					g_rebalance(p_node);
					selftest(p_node);
				}
				return true;
			} else {
				if (g_remove<t_search>(result > 0 ? p_node->m_left : p_node->m_right,p_search)) {
					recalc_depth(p_node);
					g_rebalance(p_node);
					selftest(p_node);
					return true;
				} else {
					return false;
				}
			}
		}

		static void selftest(t_node * p_node) {
	#if 0 //def _DEBUG
			if (p_node != NULL) {
				selftest(p_node->m_left);
				selftest(p_node->m_right);
				assert_children(p_node);
				int delta = test_depth(p_node);
				assert(delta >= -1 && delta <= 1);
			}
	#endif
		}


		static t_size calc_count(const t_node * p_node) {
			if (p_node != NULL) {
				return 1 + calc_count(p_node->m_left) + calc_count(p_node->m_right);
			} else {
				return 0;
			}
		}

		template<typename t_item>
		t_storage * __find_item_ptr(t_item const & p_item) const {
			t_node* ptr = m_root;
			while(ptr != NULL) {
				int result = compare(ptr->m_data,p_item);
				if (result > 0) ptr=ptr->m_left;
				else if (result < 0) ptr=ptr->m_right;
				else return &ptr->m_data;
			}
			return NULL;
		}

		template<bool inclusive,bool above,typename t_search> t_storage * __find_nearest(const t_search & p_search) const {
			t_node * ptr = m_root;
			t_storage * found = NULL;
			while(ptr != NULL) {
				int result = compare(ptr->m_data,p_search);
				if (above) result = -result;
				if (inclusive && result == 0) {
					//direct hit
					found = &ptr->m_data;
					break;
				} else if (result < 0) {
					//match
					found = &ptr->m_data;
					ptr = ptr->child<!above>();
				} else {
					//mismatch
					ptr = ptr->child<above>();
				}
			}
			return found;
		}
	public:
		avltree_t() : m_root(NULL) {}
		~avltree_t() {reset();}
		const t_self & operator=(const t_self & p_other) {__copy(p_other);return *this;}
		avltree_t(const t_self & p_other) : m_root(NULL) {__copy(p_other);}

		template<typename t_other> const t_self & operator=(const t_other & p_other) {copy_list_enumerated(*this,p_other);return *this;}
		template<typename t_other> avltree_t(const t_other & p_other) : m_root(NULL) {copy_list_enumerated(*this,p_other);}


		template<bool inclusive,bool above,typename t_search> const t_storage * find_nearest_item(const t_search & p_search) const {
			return __find_nearest<inclusive,above>(p_search);
		}

		template<bool inclusive,bool above,typename t_search> t_storage * find_nearest_item(const t_search & p_search) {
			return __find_nearest<inclusive,above>(p_search);
		}

		template<typename t_item>
		t_storage & add_item(t_item const & p_item) {
			bool dummy;
			return add_item_ex(p_item,dummy);
		}

		template<typename t_item>
		t_storage & add_item_ex(t_item const & p_item,bool & p_isnew) {
			t_storage * ret = g_find_or_add(m_root,p_item,p_isnew);
			selftest(m_root);
			return *ret;
		}

		template<typename t_item>
		void set_item(const t_item & p_item) {
			bool isnew;
			t_storage & found = add_item_ex(p_item,isnew);
			if (isnew) found = p_item;
		}

		template<typename t_item>
		const t_storage * find_item_ptr(t_item const & p_item) const {
			return __find_item_ptr(p_item);
		}

		//! WARNING: caller must not alter the item in a way that changes the sort order.
		template<typename t_item>
		t_storage * find_item_ptr(t_item const & p_item) {
			return __find_item_ptr(p_item);
		}

		template<typename t_item>
		bool have_item(const t_item & p_item) const {
			return find_item_ptr(p_item) != NULL;
		}

		void remove_all() {
			delete pfc::replace_null_t(m_root);
		}

		template<typename t_item>
		void remove_item(t_item const & p_item) {
			g_remove<t_item>(m_root,p_item);
			selftest(m_root);
		}

		t_size get_count() const {
			return calc_count(m_root);
		}

		template<typename t_callback>
		void enumerate(t_callback & p_callback) const {
			__enum_items_recur<const t_node>(m_root,p_callback);
		}

		//! Allows callback to modify the tree content.
		//! WARNING: items must not be altered in a way that changes their sort order.
		template<typename t_callback>
		void __enumerate(t_callback & p_callback) {
			__enum_items_recur<t_node>(m_root,p_callback);
		}

		//deprecated backwards compatibility method wrappers
		template<typename t_item> t_storage & add(const t_item & p_item) {return add_item(p_item);}
		template<typename t_item> t_storage & add_ex(const t_item & p_item,bool & p_isnew) {return add_item_ex(p_item,p_isnew);}
		template<typename t_item> const t_storage * find_ptr(t_item const & p_item) const {return find_item_ptr(p_item);}
		template<typename t_item> t_storage * find_ptr(t_item const & p_item) {return find_item_ptr(p_item);}
		template<typename t_item> bool exists(t_item const & p_item) const {return have_item(p_item);}
		void reset() {remove_all();}
		template<typename t_item> void remove(t_item const & p_item) {remove_item(p_item);}

	private:
		static t_node * __copy_recur(const t_node * p_source) {
			if (p_source == NULL) {
				return NULL;
			} else {
				pfc::ptrholder_t<t_node> newnode = new t_node(p_source->m_data);
				newnode->m_depth = p_source->m_depth;
				newnode->m_left = __copy_recur(p_source->m_left);
				newnode->m_right = __copy_recur(p_source->m_right);
				return newnode.detach();
			}
		}

		void __copy(const t_self & p_other) {
			reset();
			m_root = __copy_recur(p_other.m_root);
		}
	};


	template<typename t_storage,typename t_comparator>
	class traits_t<avltree_t<t_storage,t_comparator> > : public traits_default_movable {};
}
#endif //_AVLTREE_T_H_INCLUDED_
