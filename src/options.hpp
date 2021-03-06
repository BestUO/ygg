//
// Created by Lukas Barth on 09.08.17.
//

#ifndef YGG_OPTIONS_HPP
#define YGG_OPTIONS_HPP

#include <cstddef>
#include <limits>
#include <type_traits>

#include "util.hpp"

namespace ygg {

/**
 * @brief Contains possible options for the RBTree
 *
 * This class contains all possible options (that can each either be set or not
 * set) for an RBTree. See the TreeOptions documentation on how to set them. See
 * the individual inner classes for information about what each option does.
 */
class TreeFlags {
public:
	/**
	 * @brief RBTree option: Allow multiple elements that compare equally
	 *
	 * If this option is set, an RBTree allows you to insert elements a and b for
	 * which "Compare(a,b) == Compare(b,a) == false" holds, i.e., which compare
	 * equally to each other. This option is set by default. You can unset it if
	 * you know that this case will never happen, saving a little space and
	 * gaining a little performance.
	 */
	class MULTIPLE {
	};
	/**
	 * @brief RBTree option: Support order queries
	 *
	 * If this flag is set, the RBTree efficiently supports order queries, i.e.,
	 * queries of the form "is a before b in the tree". Please note that this only
	 * makes sense if MULTIPLE is also set --- otherwise, Compare(a,b) answers
	 * that question for every pair (a,b). For elements that compare equally (i.e.
	 * Compare(a,b) == Compare(b,a) == false), the hinted version of
	 * RBTree::insert allows you to enforce a certain order on equal elements.
	 * TODO link to method
	 */
	class ORDER_QUERIES {
	};
	/**
	 * @brief RBTree / List option: support size() in O(1)
	 *
	 * If this flag is set, the data structures keep track of the number of
	 * elements in them, thereby supporting size() queries in O(1). This requires
	 * a little space and will slow down insert and remove operations minimally.
	 */
	class CONSTANT_TIME_SIZE {
	};

	/**
	 * @brief RBTree option: Indicates that color information should be compressed
	 * into the parent pointer
	 *
	 * This saves a little memory and can in fact increase speed if your nodes are
	 * very small. It uses some pointer magic which is technically not standard
	 * compliant but should work on almost all systems.
	 */
	class COMPRESS_COLOR {
	};

	/**
	 * @brief Zip Tree Option: Indicates that nodes' ranks should be derived from
	 * a std::hash hash of the node.
	 */
	class ZTREE_USE_HASH {
	};

	/**
	 * @brief Zip Tree Option: Causes ranks to be stored in nodes and sets their
	 * type.
	 *
	 * The ranks of the nodes need not necessarily be stored at the nodes. If you
	 * e.g. generate ranks from hashes, they can be recomputed whenever needed.
	 * This saves some space in the nodes.
	 *
	 * @warning If you do not enable ZTREE_USE_HASH, you *must* set
	 * ZTREE_RANK_TYPE.
	 *
	 * @tparam T The type that you want to use to store the rank. Usually, uint8_t
	 * is sufficient.
	 */
	template <class T>
	class ZTREE_RANK_TYPE {
	public:
		using type = T;
	};

	/**
	 * @brief Zip Tree Option: Apply universal hashing to compute node ranks. This
	 * sets the coefficient.
	 *
	 * The default way of computing ranks (from hashes) can apply universal
	 * hashing to (to some extents) guarantee a geometric distribution of the
	 * nodes' ranks. The universal hashing of a std::hash-value x is done via:
	 *
	 *   h = (x * <coefficient>) % <modul>
	 *
	 * This parameter sets the coefficeint.
	 *
	 * @warning Universal hashing is only activated if you also set
	 * ZTREE_RANK_HASH_UNIVERSALIZE_MODUL.
	 *
	 * @tparam coefficient_in The desired coefficient. Randomize this!
	 */
	template <size_t coefficient_in>
	class ZTREE_RANK_HASH_UNIVERSALIZE_COEFFICIENT {
	public:
		constexpr static size_t value = coefficient_in;
	};

	/**
	 * @brief Zip Tree Option: Apply universal hashing to compute node ranks. This
	 * sets the modul.
	 *
	 * The default way of computing ranks (from hashes) can apply universal
	 * hashing to (to some extents) guarantee a geometric distribution of the
	 * nodes' ranks. The universal hashing of a std::hash-value x is done via:
	 *
	 *   h = (x * <coefficient>) % <modul>
	 *
	 * This parameter sets the modul.
	 *
	 * @warning Universal hashing is only activated if you also set
	 * ZTREE_RANK_HASH_UNIVERSALIZE_COEFFICIENT.
	 *
	 * @tparam coefficient_in The desired modul. Randomize this! You might want to
	 * chose a prime.
	 */
	template <size_t modul_in>
	class ZTREE_RANK_HASH_UNIVERSALIZE_MODUL {
	public:
		constexpr static size_t value = modul_in;
	};
};

/**
 * @brief Class holding the options for the data structures in this library.
 *
 * This class acts as a container for the options of all the data structures in
 * this library. Note that this class should never (and in fact, cannot be)
 * instantiated. All options are passed as template parameters, and the class
 * itself is in turn passed as template parameter to the data structures.
 *
 * Example, setting both the MULTIPLE and ORDER_QUERIES options for a red-black
 * tree:
 *
 * @code{.c++}
 * using MyTreeOptions = TreeOptions<TreeFlags::MULTIPLE,
 * TreeFlags::ORDER_QUERIES>; class Node : public RBTreeNodeBase<Node,
 * MyTreeOptions> { … }; using MyTree = RBTree<Node, RBDefaultNodeTraits,
 * MyTreeOptions>;
 * @endcode
 *
 * @tparam Opts List of the options that should be set in the tree.
 */
template <class... Opts>
class TreeOptions {
private:
	static constexpr size_t ztree_universalize_modul_default =
	    std::numeric_limits<size_t>::max();
	static constexpr size_t ztree_universalize_coefficient_default = 1103515245;

public:
	/// @cond INTERNAL
	static constexpr bool multiple =
	    rbtree_internal::pack_contains<TreeFlags::MULTIPLE, Opts...>();
	static constexpr bool order_queries =
	    rbtree_internal::pack_contains<TreeFlags::ORDER_QUERIES, Opts...>();
	static constexpr bool constant_time_size =
	    rbtree_internal::pack_contains<TreeFlags::CONSTANT_TIME_SIZE, Opts...>();
	static constexpr bool compress_color =
	    rbtree_internal::pack_contains<TreeFlags::COMPRESS_COLOR, Opts...>();

	static constexpr bool ztree_use_hash =
	    rbtree_internal::pack_contains<TreeFlags::ZTREE_USE_HASH, Opts...>();

	using ztree_rank_type =
	    typename utilities::get_type_if_present<TreeFlags::ZTREE_RANK_TYPE, bool,
	                                            Opts...>::type;

	static constexpr bool ztree_universalize =
	    (utilities::get_value_if_present<
	         TreeFlags::ZTREE_RANK_HASH_UNIVERSALIZE_MODUL, Opts...>::found &&
	     utilities::get_value_if_present<
	         TreeFlags::ZTREE_RANK_HASH_UNIVERSALIZE_COEFFICIENT,
	         Opts...>::found);

	static constexpr bool ztree_store_rank =
	    !std::is_same<ztree_rank_type, bool>::value;

	static constexpr size_t ztree_universalize_modul =
	    utilities::get_value_if_present_else_default<
	        TreeFlags::ZTREE_RANK_HASH_UNIVERSALIZE_MODUL,
	        ztree_universalize_modul_default, Opts...>::value;
	static constexpr size_t ztree_universalize_coefficient =
	    utilities::get_value_if_present_else_default<
	        TreeFlags::ZTREE_RANK_HASH_UNIVERSALIZE_COEFFICIENT,
	        ztree_universalize_coefficient_default, Opts...>::value;

	/// @endcond
private:
	TreeOptions(); // Instantiation not allowed
};

using DefaultOptions =
    TreeOptions<TreeFlags::MULTIPLE, TreeFlags::CONSTANT_TIME_SIZE>;

} // namespace ygg

#endif // YGG_OPTIONS_HPP
