#ifndef YGG_ENERGY_CPP
#define YGG_ENERGY_CPP

// Just to make this file compileable for clangd
#include "energy.hpp"
#include "debug.hpp"

#include <cmath>
#include <iostream>

// TODO currently, only a multi-set is implemented

namespace ygg {

template <class Node, class Options, class Tag, class Compare>
EnergyTree<Node, Options, Tag, Compare>::EnergyTree() : root(nullptr)
{}

template <class Node, class Options, class Tag, class Compare>
EnergyTree<Node, Options, Tag, Compare>::EnergyTree(MyClass && other)
{
	this->root = other.root;
	other.root = nullptr;
}

template <class Node, class Options, class Tag, class Compare>
EnergyTree<Node, Options, Tag, Compare> &
EnergyTree<Node, Options, Tag, Compare>::operator=(MyClass && other)
{
	this->root = other.root;
	other.root = nullptr;

	return *this;
}

template <class Node, class Options, class Tag, class Compare>
void
EnergyTree<Node, Options, Tag, Compare>::insert(Node & node)
{
	this->dbg_verify();

	node._et_size = 1;
	node._et_energy = 0;
	node._et_left = nullptr;
	node._et_right = nullptr;

	if (__builtin_expect(this->root == nullptr, false)) {
		this->root = &node;
		node._et_parent = nullptr;
		return;
	}

	Node * cur = this->root;
	Node * rebuild_at = nullptr;

	// TODO this implements a left-leaning multiset.
	// TODO sprinkle with expects?
	while (true) {
		cur->NB::_et_size += 1;
		cur->NB::_et_energy += 1;

		// TODO unroll two stages of the loop
		// TODO make 0.5 configurable
		if ((rebuild_at == nullptr) &&
		    (cur->NB::_et_energy > 0.5 * cur->NB::_et_size)) {
			rebuild_at = cur;
		}

		if (this->cmp(*cur, node)) {
			if (cur->NB::_et_right != nullptr) {
				cur = cur->NB::_et_right;
			} else {
				cur->NB::_et_right = &node;
				node.NB::_et_parent = cur;
				break;
			}
		} else {
			if (cur->NB::_et_left != nullptr) {
				cur = cur->NB::_et_left;
			} else {
				cur->NB::_et_left = &node;
				node.NB::_et_parent = cur;
				break;
			}
		}
	}

	this->dbg_verify();

	if (rebuild_at != nullptr) {
		this->rebuild_below(rebuild_at);
	}
}

template <class Node, class Options, class Tag, class Compare>
void
EnergyTree<Node, Options, Tag, Compare>::remove(Node & node)
{
	Node * cur = &node;
	Node * rebuild_at = nullptr;
	bool rebuild_set_upwards = false;

	while (cur->NB::_et_parent != nullptr) {
		cur = cur->NB::_et_parent;
		cur->NB::_et_size -= 1;
		cur->NB::_et_energy += 1;

		// TODO make 0.5 configurable
		// TODO ceil here?
		if (cur->NB::_et_energy > 0.5 * cur->NB::_et_size) {
			rebuild_at = cur;
			rebuild_set_upwards = true;
		}
	}

	cur = &node;
	Node * child = &node;

	if ((cur->NB::_et_left == nullptr) && (cur->NB::_et_right == nullptr)) {
		// This is a leaf. We can just delete.
		Node * parent = cur->NB::_et_parent;
		if (parent == nullptr) {
			this->root = nullptr;
		} else if (parent->NB::_et_left == cur) {
			parent->NB::_et_left = nullptr;
		} else {
			assert(parent->NB::_et_right == cur);
			parent->NB::_et_right = nullptr;
		}
	} else {
		if (cur->NB::_et_left != nullptr) {
			// TODO randomly select whether we do left-leaning or right-leaning
			// deletion? For now, left-leaning is implemented
			child = cur->NB::_et_left;

			// Find the largest among the less-or-equal children
			while (child->NB::_et_right != nullptr) {
				child->NB::_et_size -= 1;
				child->NB::_et_energy += 1;

				if ((rebuild_at == nullptr) &&
				    (cur->NB::_et_energy > 0.5 * cur->NB::_et_size)) {
					rebuild_at = child;
				}

				child = child->NB::_et_right;
			}

			// Splice this child out of the tree
			if (child->NB::_et_left != nullptr) {
				// TODO this is only non-true if we did not descend above.
				// unroll this case?
				if (__builtin_expect(child->NB::_et_parent->NB::_et_right == child,
				                     true)) {
					child->NB::_et_parent->NB::_et_right = child->NB::_et_left;
				} else {
					assert(child->NB::_et_parent->NB::_et_left == child);
					child->NB::_et_parent->NB::_et_left = child->NB::_et_left;
				}
				child->NB::_et_left->NB::_et_parent = child->NB::_et_parent;
			}

		} else {
			child = cur->NB::_et_right;

			// Find the smallest among the greater-or-equal children
			while (child->NB::_et_left != nullptr) {
				child->NB::_et_size -= 1;
				child->NB::_et_energy += 1;

				if ((rebuild_at == nullptr) &&
				    (cur->NB::_et_energy > 0.5 * cur->NB::_et_size)) {
					rebuild_at = child;
				}

				child = child->NB::_et_left;
			}

			// Splice this child out of the tree
			if (child->NB::_et_right != nullptr) {
				// TODO this is only non-true if we did not descend above.
				// unroll this case?
				if (__builtin_expect(child->NB::_et_parent->NB::_et_left == child,
				                     true)) {
					child->NB::_et_parent->NB::_et_left = child->NB::_et_right;
				} else {
					assert(child->NB::_et_parent->NB::_et_right == child);
					child->NB::_et_parent->NB::_et_right = child->NB::_et_right;
				}
				child->NB::_et_right->NB::_et_parent = child->NB::_et_parent;
			}
		}

		// Move the child up where the node to be deleted was
		child->NB::_et_left = node.NB::_et_left;
		child->NB::_et_right = node.NB::_et_right;

		if (node.NB::_et_parent == nullptr) {
			this->root = child;
		} else if (node.NB::_et_parent->NB::_et_left == &node) {
			node.NB::_et_parent->NB::_et_left = child;
		} else {
			assert(node.NB::_et_parent->NB::_et_right == &node);
			node.NB::_et_parent->NB::_et_right = child;
		}
		child->NB::_et_parent = node.NB::_et_parent;

		child->NB::_et_energy = node.NB::_et_energy + 1;
		child->NB::_et_size = node.NB::_et_size - 1;

		if (!rebuild_set_upwards &&
		    (child->NB::_et_energy > 0.5 * child->NB::_et_size)) {
			rebuild_at = child;
		}
	}

	if (rebuild_at != nullptr) {
		this->rebuild_below(rebuild_at);
	}
}

template <class Node, class Options, class Tag, class Compare>
void
EnergyTree<Node, Options, Tag, Compare>::dbg_verify() const
{
	this->dbg_verify_sizes();
}

template <class Node, class Options, class Tag, class Compare>
bool
EnergyTree<Node, Options, Tag, Compare>::verify_integrity() const
{
	try {
		this->dbg_verify();
	} catch (debug::VerifyException & e) {
		return false;
	}

	return true;
}

template <class Node, class Options, class Tag, class Compare>
bool
EnergyTree<Node, Options, Tag, Compare>::empty() const
{
	return this->root == nullptr;
}

template <class Node, class Options, class Tag, class Compare>
void
EnergyTree<Node, Options, Tag, Compare>::dbg_verify_sizes() const
{
	for (const Node & n : *this) {
		size_t left_size = 0;
		if (n.NB::_et_left != nullptr) {
			left_size = n.NB::_et_left->NB::_et_size;
		}
		size_t right_size = 0;
		if (n.NB::_et_right != nullptr) {
			right_size = n.NB::_et_right->NB::_et_size;
		}

		debug::yggassert(n.NB::_et_size == left_size + right_size + 1);
	}
}

template <class Node, class Options, class Tag, class Compare>
void
EnergyTree<Node, Options, Tag, Compare>::rebuild_below(Node * node)
{
	this->dbg_verify();

	// TODO this is a wild hack. Is this portable?
	//	size_t full_tree_size = 1 << (fls(node->NB::_et_size) + (1 -
	//__builtin_parity(node->NB::_et_size)));
	int tree_levels = (int)(std::ceil(std::log2(node->NB::_et_size + 1)));
	size_t full_tree_size = (1 << tree_levels) - 1;

	this->rebuild_buffer.resize(full_tree_size);

	Node * smallest;
	Node * largest;

	Node * original_parent = node->NB::_et_parent;
	size_t original_size = node->NB::_et_size;

	std::cout << "Rebuilding subtree of size " << original_size << "\n";

	// TODO it should be possible to prevent backtracking out of the node
	smallest = node;
	while (smallest->NB::_et_left != nullptr) {
		smallest = smallest->NB::_et_left;
	}
	largest = node;
	while (largest->NB::_et_right != nullptr) {
		largest = largest->NB::_et_right;
	}

	iterator<false> it(smallest);

	std::cout << "--------------------\n";

	// Handle first element manually
	this->rebuild_buffer[0] = smallest;
	size_t counter = 1;

	std::cout << "N: " << std::hex << (size_t)smallest << std::dec << "\n";
	std::cout << "Level offset 0 / index 0 \n";

	// TODO unroll this loop?
	while (&(*it) != largest) {
		++it;
		++counter;

		// Lowest level is 0, topmost is (tree_levels - 1)
		int level = __builtin_ctz(counter);
		size_t prev_levels_size = (1 << (tree_levels - 1 - level)) - 1;
		size_t this_level_size = prev_levels_size + 1;
		size_t level_offset =
		    full_tree_size - prev_levels_size -
		    this_level_size; // TODO can be simplified - just omit the "-1" above

		int index_in_level = counter >> (level + 1);

		std::cout << "N: " << std::hex << (size_t) & (*it) <<  std::dec << "\n";
		std::cout << "Level offset " << level_offset << " / index "
		          << index_in_level << "\n";
		this->rebuild_buffer[level_offset + index_in_level] = &(*it);
	}

	std::cout << "--------------------\n";

	size_t lower_offset = 0;
	size_t upper_offset = (full_tree_size + 1) / 2;
	size_t bottom_level_size =
	    upper_offset - (full_tree_size - node->NB::_et_size);

	std::cout << "-- Up-linking bottom level. Level Size: " << bottom_level_size << "\n";;
	
	if (tree_levels > 1) {
		// Linking the bottom level up is special, since it is not full
		size_t i = 0;
		for (; i < bottom_level_size - 1; i += 2) { // Go in steps of two
			std::cout << "## i: " << i << " Upper Node: " << std::hex << (size_t)this->rebuild_buffer[upper_offset + (i / 2)] <<  std::dec << "\n";
			this->rebuild_buffer[upper_offset + (i / 2)]->NB::_et_left =
			    this->rebuild_buffer[i];
			this->rebuild_buffer[upper_offset + (i / 2)]->NB::_et_right =
			    this->rebuild_buffer[i + 1];
			this->rebuild_buffer[upper_offset + (i / 2)]->NB::_et_size = 3;
			this->rebuild_buffer[upper_offset + (i / 2)]->NB::_et_energy = 0;
			std::cout << "  L: " << std::hex << this->rebuild_buffer[i] << " R: " << this->rebuild_buffer[i + 1] << std::dec << "\n";
			
			this->rebuild_buffer[i]->NB::_et_left = nullptr;
			this->rebuild_buffer[i]->NB::_et_size = 1;
			this->rebuild_buffer[i]->NB::_et_energy = 0;
			this->rebuild_buffer[i]->NB::_et_right = nullptr;
			this->rebuild_buffer[i]->NB::_et_parent =
			    this->rebuild_buffer[upper_offset + (i / 2)];
			this->rebuild_buffer[i + 1]->NB::_et_left = nullptr;
			this->rebuild_buffer[i + 1]->NB::_et_right = nullptr;
			this->rebuild_buffer[i + 1]->NB::_et_parent =
			    this->rebuild_buffer[upper_offset + (i / 2)];
			this->rebuild_buffer[i + 1]->NB::_et_size = 1;
			this->rebuild_buffer[i + 1]->NB::_et_energy = 0;
		}
		if (i < bottom_level_size) {
			this->rebuild_buffer[upper_offset + (i / 2)]->NB::_et_left =
			    this->rebuild_buffer[i];
			this->rebuild_buffer[upper_offset + (i / 2)]->NB::_et_right = nullptr;
			this->rebuild_buffer[upper_offset + (i / 2)]->NB::_et_size = 2;
			this->rebuild_buffer[upper_offset + (i / 2)]->NB::_et_energy = 0;

			this->rebuild_buffer[i]->NB::_et_left = nullptr;
			this->rebuild_buffer[i]->NB::_et_right = nullptr;
			this->rebuild_buffer[i]->NB::_et_parent =
			    this->rebuild_buffer[upper_offset + (i / 2)];
			this->rebuild_buffer[i]->NB::_et_size = 1;
			this->rebuild_buffer[i]->NB::_et_energy = 0;

			i += 2;
		}

		for (size_t j = i / 2; j < ((1 << (tree_levels - 2)) - 1); j++) {
			// Remaining nodes in the second-to-last level
			this->rebuild_buffer[j]->NB::_et_left = nullptr;
			this->rebuild_buffer[j]->NB::_et_right = nullptr;
			this->rebuild_buffer[j]->NB::_et_size = 1;
			this->rebuild_buffer[j]->NB::_et_energy = 0;
		}

		// Now, climb up the levels
		int level = 1;
		while (level < tree_levels - 1) {
			
			lower_offset = upper_offset;
			upper_offset = full_tree_size - (1 << (tree_levels - 1 - level)) + 1;
			int current_lower_level_size = upper_offset - lower_offset;

			std::cout << "-- Up-linking level " << level << ". Lower Offset: " << lower_offset << " | Upper Offset: " << upper_offset << "\n";

			
			// TODO it's probably faster to increment *_offset and save some
			// additions…
			for (i = 0; i < current_lower_level_size; i += 2) { // Go in steps of two
				std::cout << "## i: " << i << " Upper node: " << std::hex << this->rebuild_buffer[upper_offset + (i / 2)] <<  std::dec << "\n";
				this->rebuild_buffer[upper_offset + (i / 2)]->NB::_et_left =
				    this->rebuild_buffer[lower_offset + i];
				this->rebuild_buffer[upper_offset + (i / 2)]->NB::_et_right =
				    this->rebuild_buffer[lower_offset + i + 1];
				this->rebuild_buffer[upper_offset + (i / 2)]->NB::_et_size =
				    (this->rebuild_buffer[lower_offset + i]->NB::_et_size +
				     this->rebuild_buffer[lower_offset + i + 1]->NB::_et_size + 1);
				this->rebuild_buffer[upper_offset + (i / 2)]->NB::_et_energy = 0;

				this->rebuild_buffer[lower_offset + i]->NB::_et_parent =
				    this->rebuild_buffer[upper_offset + (i / 2)];
				this->rebuild_buffer[lower_offset + i + 1]->NB::_et_parent =
				    this->rebuild_buffer[upper_offset + (i / 2)];
			}

			level += 1;
		}
	}

	// Top level connects to the original parent
	this->rebuild_buffer.back()->NB::_et_parent = original_parent;
	if (original_parent == nullptr) {
		this->root = this->rebuild_buffer.back();
	} else if (original_parent->NB::_et_left == node) {
		original_parent->NB::_et_left = this->rebuild_buffer.back();
	} else {
		assert(original_parent->NB::_et_right == node);
		original_parent->NB::_et_right = this->rebuild_buffer.back();
	}
	this->rebuild_buffer.back()->NB::_et_size = original_size;
	this->rebuild_buffer.back()->NB::_et_energy = 0;

	this->dbg_verify();
}

template <class Node, class Options, class Tag, class Compare>
Node *
EnergyTree<Node, Options, Tag, Compare>::get_smallest() const
{
	Node * smallest = this->root;
	if (smallest == nullptr) {
		return nullptr;
	}

	while (smallest->NB::_et_left != nullptr) {
		smallest = smallest->NB::_et_left;
	}

	return smallest;
}

template <class Node, class Options, class Tag, class Compare>
Node *
EnergyTree<Node, Options, Tag, Compare>::get_largest() const
{
	Node * largest = this->root;
	if (largest == nullptr) {
		return nullptr;
	}

	while (largest->NB::_et_right != nullptr) {
		largest = largest->NB::_et_right;
	}

	return largest;
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template const_iterator<false>
EnergyTree<Node, Options, Tag, Compare>::iterator_to(const Node & node) const
{
	return const_iterator<false>(&node);
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template iterator<false>
EnergyTree<Node, Options, Tag, Compare>::iterator_to(Node & node)
{
	return iterator<false>(&node);
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template const_iterator<false>
EnergyTree<Node, Options, Tag, Compare>::cbegin() const
{
	Node * smallest = this->get_smallest();
	if (smallest == nullptr) { // TODO what the hell?
		return const_iterator<false>(nullptr);
	}

	return const_iterator<false>(smallest);
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template const_iterator<false>
EnergyTree<Node, Options, Tag, Compare>::cend() const
{
	return const_iterator<false>(nullptr);
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template const_iterator<false>
EnergyTree<Node, Options, Tag, Compare>::begin() const
{
	return this->cbegin();
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template iterator<false>
EnergyTree<Node, Options, Tag, Compare>::begin()
{
	Node * smallest = this->get_smallest();
	if (smallest == nullptr) {
		return iterator<false>(nullptr);
	}

	return iterator<false>(smallest);
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template const_iterator<false>
EnergyTree<Node, Options, Tag, Compare>::end() const
{
	return this->cend();
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template iterator<false>
EnergyTree<Node, Options, Tag, Compare>::end()
{
	return iterator<false>(nullptr);
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template const_iterator<true>
EnergyTree<Node, Options, Tag, Compare>::crbegin() const
{
	Node * largest = this->get_largest();
	if (largest == nullptr) {
		return const_iterator<true>(nullptr);
	}

	return const_iterator<true>(largest);
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template const_iterator<true>
EnergyTree<Node, Options, Tag, Compare>::crend() const
{
	return const_iterator<true>(nullptr);
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template const_iterator<true>
EnergyTree<Node, Options, Tag, Compare>::rbegin() const
{
	return this->crbegin();
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template const_iterator<true>
EnergyTree<Node, Options, Tag, Compare>::rend() const
{
	return this->crend();
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template iterator<true>
EnergyTree<Node, Options, Tag, Compare>::rbegin()
{
	Node * largest = this->get_largest();
	if (largest == nullptr) {
		return iterator<true>(nullptr);
	}

	return iterator<true>(largest);
}

template <class Node, class Options, class Tag, class Compare>
typename EnergyTree<Node, Options, Tag, Compare>::template iterator<true>
EnergyTree<Node, Options, Tag, Compare>::rend()
{
	return iterator<true>(nullptr);
}

} // namespace ygg

#endif // YGG_ENERGY_CPP
