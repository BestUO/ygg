namespace utilities {

  template<class Node, class const_iterator>
  const_iterator &
  IteratorHelperUnspecialized<Node, const_iterator>::step_forward(const_iterator & it) {
    // No more equal elements
    if (it.n->_rbt_right != nullptr) {
      // go to smallest larger-or-equal child
      it.n = it.n->_rbt_right;
      while (it.n->_rbt_left != nullptr) {
        it.n = it.n->_rbt_left;
      }
    } else {
      // go up

      // skip over the nodes already visited
      while ((it.n->_rbt_parent != nullptr) && (it.n->_rbt_parent->_rbt_right == it.n)) { // these are the nodes which are smaller and were already visited
        it.n = it.n->_rbt_parent;
      }

      // go one further up
      if (it.n->_rbt_parent == nullptr) {
        // done
        it.n = nullptr;
      } else {
        // go up
        it.n = it.n->_rbt_parent;
      }
    }

    return it;
  }

  template<class Node, class const_iterator>
  const_iterator &
  IteratorHelperUnspecialized<Node, const_iterator>::step_back(const_iterator & it) {
    if (it.n->_rbt_left != nullptr) {
      // go to largest smaller child
      it.n = it.n->_rbt_left;
      while (it.n->_rbt_right != nullptr) {
        it.n = it.n->_rbt_right;
      }
    } else {
      // go up

      // skip over the nodes already visited
      while ((it.n->_rbt_parent != nullptr) && (it.n->_rbt_parent->_rbt_left == it.n)) { // these are the nodes which are larger and were already visited
        it.n = it.n->_rbt_parent;
      }

      // go one further up
      if (it.n->_rbt_parent == nullptr) {
        // done
        it.n = nullptr;
      } else {
        // go up
        it.n = it.n->_rbt_parent;
      }
    }

    return it;
  }

  template<class Node, class const_iterator>
  const_iterator &
  IteratorHelper<Node, const_iterator, false>::operator_plusplus(const_iterator & it)
  {
    return IteratorHelperUnspecialized<Node, const_iterator>::step_forward(it);
  }

  template<class Node, class const_iterator>
  const_iterator &
  IteratorHelper<Node, const_iterator, true>::operator_plusplus(const_iterator & it)
  {
    return IteratorHelperUnspecialized<Node, const_iterator>::step_back(it);
  }

  template<class Node, class const_iterator>
  const_iterator &
  IteratorHelper<Node, const_iterator, false>::operator_minusminus(const_iterator & it)
  {
    return IteratorHelperUnspecialized<Node, const_iterator>::step_back(it);
  }

  template<class Node, class const_iterator>
  const_iterator &
  IteratorHelper<Node, const_iterator, true>::operator_minusminus(const_iterator & it)
  {
    return IteratorHelperUnspecialized<Node, const_iterator>::step_forward(it);
  }

template<class Node, class Compare>
void
EqualityListHelper<Node, false, Compare>::equality_list_insert_before(Node & node, Node * predecessor)
{
  (void)node;
  (void)predecessor;
}

template<class Node, class Compare>
void
EqualityListHelper<Node, false, Compare>::equality_list_insert_after(Node & node, Node * successor)
{
  (void)node;
  (void)successor;
}

template<class Node, class Compare>
void
EqualityListHelper<Node, true, Compare>::equality_list_insert_before(Node & node, Node * predecessor)
{
  if (predecessor == nullptr) {
    node._rbt_next = nullptr;
    node._rbt_prev = nullptr;
  } else {
    node._rbt_next = predecessor->_rbt_next;
    node._rbt_prev = predecessor;

    if (predecessor->_rbt_next != nullptr) {
      predecessor->_rbt_next->_rbt_prev = &node;
    }

    predecessor->_rbt_next = &node;
  }
}

template<class Node, class Compare>
void
EqualityListHelper<Node, true, Compare>::equality_list_insert_after(Node & node, Node * successor)
{
  if (successor == nullptr) {
    node._rbt_next = nullptr;
    node._rbt_prev = nullptr;
  } else {
    node._rbt_prev = successor->_rbt_prev;
    node._rbt_next = successor;

    if (successor->_rbt_prev != nullptr) {
      successor->_rbt_prev->_rbt_next = &node;
    }
    successor->_rbt_prev = &node;
  }
}

template<class Node, class Compare>
void
EqualityListHelper<Node, false, Compare>::equality_list_delete_node(Node & node)
{
  (void)node;
}

template<class Node, class Compare>
void
EqualityListHelper<Node, true, Compare>::equality_list_delete_node(Node & node)
{
  if (node._rbt_next != nullptr) {
    node._rbt_next->_rbt_prev = node._rbt_prev;
  }

  if (node._rbt_prev != nullptr) {
    node._rbt_prev->_rbt_next = node._rbt_next;
  }

  // TODO Should pointers be nulled?
  //node._rbt_prev = nullptr;
  //node._rbt_next = nullptr;
}

template<class Node, class Compare>
Node *
EqualityListHelper<Node, false, Compare>::equality_list_find_first(Node * node)
{
  return node;
}

template<class Node, class Compare>
Node *
EqualityListHelper<Node, true, Compare>::equality_list_find_first(Node * node)
{
  Node * res = node;
  while (res->_rbt_prev != nullptr) {
    res = res->_rbt_prev;
  }

  return res;
}

template<class Node, class Compare>
Node *
EqualityListHelper<Node, true, Compare>::equality_list_next(Node * node)
{
  return node->_rbt_next;
}

template<class Node, class Compare>
Node *
EqualityListHelper<Node, false, Compare>::equality_list_next(Node * node)
{
  (void)node;
  return nullptr;
}

template<class Node, class Compare>
Node *
EqualityListHelper<Node, true, Compare>::equality_list_prev(Node * node)
{
  return node->_rbt_prev;
}

template<class Node, class Compare>
Node *
EqualityListHelper<Node, false, Compare>::equality_list_prev(Node * node)
{
  (void)node;
  return nullptr;
}

template<class Node, class Compare>
void
EqualityListHelper<Node, false, Compare>::equality_list_swap_if_necessary(Node & n1, Node & n2)
{
  (void)n1;
  (void)n2;
}

template<class Node, class Compare>
void
EqualityListHelper<Node, true, Compare>::equality_list_swap_if_necessary(Node & n1, Node & n2)
{
  auto c = Compare();

  if (c(n1, n2) || c(n2, n1)) {
    return; // elements are not equal
  }

  if (n1._rbt_next == &n2) {
    // n1 predecessor of n2
    n1._rbt_next = n2._rbt_next;
    n2._rbt_prev = n1._rbt_prev;

    n1._rbt_prev = &n2;
    n2._rbt_next = &n1;

    // fix neighbors
    if (n1._rbt_next != nullptr) {
      n1._rbt_next->_rbt_prev = &n1;
    }
    if (n2._rbt_prev != nullptr) {
      n2._rbt_prev->_rbt_next = &n2;
    }
  } else if (n2._rbt_next == &n1) {
    // n2 predecessor of n1
    n2._rbt_next = n1._rbt_next;
    n1._rbt_prev = n2._rbt_prev;

    n2._rbt_prev = &n1;
    n1._rbt_next = &n2;

    // fix neighbors
    if (n2._rbt_next != nullptr) {
      n2._rbt_next->_rbt_prev = &n2;
    }
    if (n1._rbt_prev != nullptr) {
      n1._rbt_prev->_rbt_next = &n1;
    }
  } else {
    // unrelated
    std::swap(n2._rbt_prev, n1._rbt_prev);
    std::swap(n2._rbt_next, n1._rbt_next);

    // fix neighbors
    if (n1._rbt_next != nullptr) {
      n1._rbt_next->_rbt_prev = &n1;
    }
    if (n1._rbt_prev != nullptr) {
      n1._rbt_prev->_rbt_next = &n1;
    }
    if (n2._rbt_next != nullptr) {
      n2._rbt_next->_rbt_prev = &n2;
    }
    if (n2._rbt_prev != nullptr) {
      n2._rbt_prev->_rbt_next = &n2;
    }
  }
}


template<class Node, class Compare>
bool
EqualityListHelper<Node, false, Compare>::verify(const Node & node)
{
  (void)node;
  return true;
}

template<class Node, class Compare>
bool
EqualityListHelper<Node, true, Compare>::verify(const Node & node)
{
  std::set<const Node *> seen;

  // go left
  const Node * cur = &node;
  seen.insert(cur);
  while (cur->_rbt_prev != nullptr) {
    if (cur->_rbt_prev->_rbt_next != cur) {
      assert(false);
      return false;
    }
    cur = cur->_rbt_prev;
    if (seen.find(cur) != seen.end()) {
      assert(false);
      return false;
    }
  }

  // go right
  cur = &node;
  while (cur->_rbt_next != nullptr) {
    if (cur->_rbt_next->_rbt_prev != cur) {
      assert(false);
      return false;
    }
    cur = cur->_rbt_next;
    if (seen.find(cur) != seen.end()) {
      assert(false);
      return false;
    }
  }

  return true;
}

} // namespace utilities



template<class Node, class NodeTraits, class Compare>
RBTree<Node, NodeTraits, Compare>::RBTree()
  : root(nullptr)
{}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::insert_leaf(Node & node, Node *start)
{
  this->insert_leaf_base<true>(node, start);
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::insert_leaf_right_biased(Node & node, Node *start)
{
  this->insert_leaf_base<false>(node, start);
}

template<class Node, class NodeTraits, class Compare>
template<bool on_equality_prefer_left>
void
RBTree<Node, NodeTraits, Compare>::insert_leaf_base(Node & node, Node *start)
{
  node._rbt_right = nullptr;
  node._rbt_left = nullptr;

  Node *parent = start;
  Node *cur = start;

  while (cur != nullptr) {
    parent = cur;
    if (Compare()(*cur, node)) {
      cur = cur->_rbt_right;
    } else {
      cur = cur->_rbt_left;
    }
  }

  if (parent == nullptr) {
    // new root!
    node._rbt_parent = nullptr;
    node._rbt_color = Base::Color::BLACK;
    this->root = &node;
    EqualityList::equality_list_insert_after(node, nullptr);
    NodeTraits::leaf_inserted(node);
  } else {
    node._rbt_parent = parent;
    node._rbt_color = Base::Color::RED;

    if (Compare()(node, *parent)) {
      parent->_rbt_left = &node;
      EqualityList::equality_list_insert_after(node, nullptr);
    } else if (Compare()(*parent, node)) {
      parent->_rbt_right = &node;
      EqualityList::equality_list_insert_after(node, nullptr);
    } else {
      //assert(multiple);
      if (!Node::_rbt_multiple) {
        return;
      }
      if (on_equality_prefer_left) {
        parent->_rbt_left = &node;
        EqualityList::equality_list_insert_before(node, parent);
      } else {
        parent->_rbt_right = &node;
        EqualityList::equality_list_insert_after(node, parent);
      }
    }

    NodeTraits::leaf_inserted(node);
    this->fixup_after_insert(&node);
  }


  return;
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::rotate_left(Node * parent)
{
  Node * right_child = parent->_rbt_right;
  parent->_rbt_right = right_child->_rbt_left;
  if (right_child->_rbt_left != nullptr) {
    right_child->_rbt_left->_rbt_parent = parent;
  }

  right_child->_rbt_left = parent;
  right_child->_rbt_parent = parent->_rbt_parent;

  if (parent->_rbt_parent != nullptr) {
    if (parent->_rbt_parent->_rbt_left == parent) {
      parent->_rbt_parent->_rbt_left = right_child;
    } else {
      parent->_rbt_parent->_rbt_right = right_child;
    }
  } else {
    this->root = right_child;
  }

  parent->_rbt_parent = right_child;

  NodeTraits::rotated_left(*parent);
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::rotate_right(Node * parent)
{
  Node * left_child = parent->_rbt_left;
  parent->_rbt_left = left_child->_rbt_right;
  if (left_child->_rbt_right != nullptr) {
    left_child->_rbt_right->_rbt_parent = parent;
  }

  left_child->_rbt_right = parent;
  left_child->_rbt_parent = parent->_rbt_parent;

  if (parent->_rbt_parent != nullptr) {
    if (parent->_rbt_parent->_rbt_left == parent) {
      parent->_rbt_parent->_rbt_left = left_child;
    } else {
      parent->_rbt_parent->_rbt_right = left_child;
    }
  } else {
    this->root = left_child;
  }

  parent->_rbt_parent = left_child;

  NodeTraits::rotated_right(*parent);
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::fixup_after_insert(Node * node)
{
  // Does not happen: We only call this if we are not the root.
  /*
  if (node->_rbt_parent == nullptr) {
    node->_rbt_color = Base::Color::BLACK;
    return;
  }
  */

  while ((node->_rbt_parent->_rbt_color == Base::Color::RED) && (this->get_uncle(node) != nullptr) && (this->get_uncle(node)->_rbt_color == Base::Color::RED)) {
    node->_rbt_parent->_rbt_color = Base::Color::BLACK;
    this->get_uncle(node)->_rbt_color = Base::Color::BLACK;

    if (node->_rbt_parent->_rbt_parent->_rbt_parent != nullptr) { // never iterate into the root
      node->_rbt_parent->_rbt_parent->_rbt_color = Base::Color::RED;
      node = node->_rbt_parent->_rbt_parent;
    } else {
      // Don't recurse into the root; don't color it red. We could immediately re-color it black.
      return;
    }
  }

  if (node->_rbt_parent->_rbt_color == Base::Color::BLACK) {
    return;
  }

  Node *parent = node->_rbt_parent;
  Node *grandparent = parent->_rbt_parent;

  if (grandparent->_rbt_left == parent) {
    if (parent->_rbt_right == node) {
      // 'folded in' situation
      this->rotate_left(parent);
      node->_rbt_color = Base::Color::BLACK;
    } else {
      // 'straight' situation
      parent->_rbt_color = Base::Color::BLACK;
    }

    this->rotate_right(grandparent);
  } else {
    if (parent->_rbt_left == node) {
      // 'folded in'
      this->rotate_right(parent);
      node->_rbt_color = Base::Color::BLACK;
    } else {
      // 'straight'
      parent->_rbt_color = Base::Color::BLACK;
    }
    this->rotate_left(grandparent);
  }

  grandparent->_rbt_color = Base::Color::RED;
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::insert(Node & node)
{
  // TODO flatten
  this->insert_leaf(node, this->root);
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::insert(Node & node, Node & hint)
{
  // find parent
  Node * parent = &hint;

  // walk up until we are not smaller anymore
  while ((parent->_rbt_parent != nullptr) && (Compare()(node, *parent->_rbt_parent))) {
    parent = parent->_rbt_parent;
  }

  this->insert_leaf(node, parent);
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::insert(Node & node, RBTree<Node, NodeTraits, Compare>::const_iterator<false> hint)
{
  if (hint == this->end()) {
    // special case: insert at the end
    Node *parent = this->root;

    if (parent == nullptr) {
      this->insert_leaf_right_biased(node, parent);
    } else {
      while (parent->_rbt_right != nullptr) {
        parent = parent->_rbt_right;
      }
      this->insert_leaf_right_biased(node, parent);
    }
  } else {
    // TODO non-const iterator?
    this->insert(node, const_cast<Node &>(*hint));
  }
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::clear()
{
  this->root = nullptr;
}

template<class Node, class NodeTraits, class Compare>
Node *
RBTree<Node, NodeTraits, Compare>::get_uncle(Node * node) const
{
  Node * parent = node->_rbt_parent;
  Node * grandparent = parent->_rbt_parent;

  if (grandparent->_rbt_left == parent) {
    return grandparent->_rbt_right;
  } else {
    return grandparent->_rbt_left;
  }
}

template<class Node, class NodeTraits, class Compare>
bool
RBTree<Node, NodeTraits, Compare>::verify_black_root() const
{
  return ((this->root == nullptr) || (this->root->_rbt_color == Base::Color::BLACK));
}

template<class Node, class NodeTraits, class Compare>
bool
RBTree<Node, NodeTraits, Compare>::verify_black_paths(const Node * node,
                                                       unsigned int  * path_length) const
{
  unsigned int left_length, right_length;

  if (node->_rbt_left == nullptr) {
    left_length = 0;
  } else {
    if (! this->verify_black_paths(node->_rbt_left, & left_length)) {
      return false;
    }
  }

  if (node->_rbt_right == nullptr) {
    right_length = 0;
  } else {
    if (! this->verify_black_paths(node->_rbt_right, & right_length)) {
      return false;
    }
  }

  if (left_length != right_length) {
    return false;
  }

  if (node->_rbt_color == Base::Color::BLACK) {
    *path_length = left_length + 1;
  } else {
    *path_length = left_length;
  }

  return true;
}

template<class Node, class NodeTraits, class Compare>
bool
RBTree<Node, NodeTraits, Compare>::verify_red_black(const Node * node) const
{
  if (node == nullptr) {
    return true;
  }

  if (node->_rbt_color == Base::Color::RED) {
    if ((node->_rbt_right != nullptr) && (node->_rbt_right->_rbt_color == Base::Color::RED)) {
      return false;
    }

    if ((node->_rbt_left != nullptr) && (node->_rbt_left->_rbt_color == Base::Color::RED)) {
      return false;
    }
  }

  return this->verify_red_black(node->_rbt_left) && this->verify_red_black(node->_rbt_right);
}

template<class Node, class NodeTraits, class Compare>
bool
RBTree<Node, NodeTraits, Compare>::verify_equality() const
{
  if (!Node::_rbt_multiple) {
    return true;
  }

  for (const auto & n : *this) {
    if (!EqualityList::verify(n)) {
      return false;
    }
  }

  return true;
}

template<class Node, class NodeTraits, class Compare>
bool
RBTree<Node, NodeTraits, Compare>::verify_order() const
{
  for (const Node & n : *this) {
    if (n._rbt_left != nullptr) {
      // left may not be larger
      if (Compare()(n, *(n._rbt_left))) {
        assert(false);
        return false;
      }
    }

    if (n._rbt_right != nullptr) {
      // right may not be smaller
      if (Compare()(*(n._rbt_right), n)) {
        assert(false);
        return false;
      }
    }
  }

  return true;
}

template<class Node, class NodeTraits, class Compare>
bool
RBTree<Node, NodeTraits, Compare>::verify_tree() const
{
  if (this->root == nullptr) {
    return true;
  }

  Node * cur = this->root;
  while (cur->_rbt_left != nullptr) {
    cur = cur->_rbt_left;
    if (cur->_rbt_left == cur) {
      assert(false);
      return(false);
    }
  }

  std::set<Node *> seen;

  while (cur != nullptr) {
    if (seen.find(cur) != seen.end()) {
      assert(false);
      return false;
    }
    seen.insert(cur);

    if (cur->_rbt_left != nullptr) {
      if (cur->_rbt_left->_rbt_parent != cur) {
        assert(false);
        return false;
     }
     if (cur->_rbt_right == cur) {
        assert(false);
        return false;
     }
   }

    if (cur->_rbt_right != nullptr) {
      if (cur->_rbt_right->_rbt_parent != cur) {
        assert(false);
        return false;
      }
      if (cur->_rbt_right == cur) {
         assert(false);
         return false;
      }
    }


    /*
     * Begin: find the next-largest vertex
     */
    if (cur->_rbt_right != nullptr) {
      // go to smallest larger-or-equal child
      cur = cur->_rbt_right;
      while (cur->_rbt_left != nullptr) {
        cur = cur->_rbt_left;
      }
    } else {
      // go up

      // skip over the nodes already visited
      while ((cur->_rbt_parent != nullptr) && (cur->_rbt_parent->_rbt_right == cur)) { // these are the nodes which are smaller and were already visited
        cur = cur->_rbt_parent;
      }

      // go one further up
      if (cur->_rbt_parent == nullptr) {
        // done
        cur = nullptr;
      } else {
        // go up
        cur = cur->_rbt_parent;
      }
    }
    /*
     * End: find the next-largest vertex
     */
  }

  return true;
}

template<class Node, class NodeTraits, class Compare>
bool
RBTree<Node, NodeTraits, Compare>::verify_integrity() const
{
  unsigned int dummy;

  bool tree_okay = this->verify_tree();
  //std::cout << "Tree1: " << tree_okay << "\n";
  bool root_okay = this->verify_black_root();
  bool paths_okay = (this->root == nullptr) || this->verify_black_paths(this->root, & dummy);
  bool children_okay = this->verify_red_black(this->root);

  bool order_okay = this->verify_order();

  bool equality_okay = this->verify_equality();

  //std::cout << "Root: " << root_okay << " Paths: " << paths_okay << " Children: " << children_okay << " Tree: " << tree_okay << "\n";

  assert(root_okay && paths_okay && children_okay && tree_okay && order_okay && equality_okay);

  (void)dummy;

  return root_okay && paths_okay && children_okay && tree_okay && order_okay && equality_okay;
}

template<class Node, class NodeTraits, class Compare>
template<class NodeNameGetter>
void
RBTree<Node, NodeTraits, Compare>::dump_to_dot_base(const std::string & filename, NodeNameGetter name_getter) const
{
  std::ofstream dotfile;
  dotfile.open(filename);
  dotfile << "digraph G {\n";
  this->output_node_base(this->root, dotfile, name_getter);
  dotfile << "}\n";
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::dump_to_dot(const std::string & filename) const
{
  this->dump_to_dot_base(filename, [&](const Node * node) {
    return NodeTraits::get_id(node);
  });
}

template<class Node, class NodeTraits, class Compare>
template<class NodeNameGetter>
void
RBTree<Node, NodeTraits, Compare>::output_node_base(const Node * node, std::ofstream & out, NodeNameGetter name_getter) const
{
  if (node == nullptr) {
    return;
  }

  std::string color;
  if (node->_rbt_color == Base::Color::BLACK) {
    color = "black";
  } else {
    color = "red";
  }

  out << "  " << std::to_string((long unsigned int)node) << "[ color=" << color <<" label=\"" << name_getter(node) << "\"]\n";

  if (node->_rbt_parent != nullptr) {
    std::string label;
    if (node->_rbt_parent->_rbt_left == node) {
      label = std::string("L");
    } else {
      label = std::string("R");
    }

    out << "  " << std::to_string((long unsigned int)node->_rbt_parent) << " -> " << std::to_string((long unsigned int)node) << "[ label=\"" << label << "\"]\n";
  }

  this->output_node_base(node->_rbt_left, out, name_getter);
  this->output_node_base(node->_rbt_right, out, name_getter);
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::swap_nodes(Node * n1, Node * n2, bool swap_colors)
{
  if (n1->_rbt_parent == n2) {
    this->swap_neighbors(n2, n1);
  } else if (n2->_rbt_parent == n1) {
    this->swap_neighbors(n1, n2);
  } else {
    this->swap_unrelated_nodes(n1, n2);
  }

  EqualityList::equality_list_swap_if_necessary(*n1, *n2);

  if (!swap_colors) {
    std::swap(n1->_rbt_color, n2->_rbt_color);
  }

  NodeTraits::swapped(*n1, *n2);
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::swap_neighbors(Node * parent, Node * child)
{
  child->_rbt_parent = parent->_rbt_parent;
  parent->_rbt_parent = child;
  if (child->_rbt_parent != nullptr) {
    if (child->_rbt_parent->_rbt_left == parent) {
      child->_rbt_parent->_rbt_left = child;
    } else {
      child->_rbt_parent->_rbt_right = child;
    }
  } else {
    this->root = child;
  }

  if (parent->_rbt_left == child) {
    parent->_rbt_left = child->_rbt_left;
    if (parent->_rbt_left != nullptr) {
      parent->_rbt_left->_rbt_parent = parent;
    }
    child->_rbt_left = parent;

    std::swap(parent->_rbt_right, child->_rbt_right);
    if (child->_rbt_right != nullptr) {
      child->_rbt_right->_rbt_parent = child;
    }
    if (parent->_rbt_right != nullptr) {
      parent->_rbt_right->_rbt_parent = parent;
    }
  } else {
    parent->_rbt_right = child->_rbt_right;
    if (parent->_rbt_right != nullptr) {
      parent->_rbt_right->_rbt_parent = parent;
    }
    child->_rbt_right = parent;

    std::swap(parent->_rbt_left, child->_rbt_left);
    if (child->_rbt_left != nullptr) {
      child->_rbt_left->_rbt_parent = child;
    }
    if (parent->_rbt_left != nullptr) {
      parent->_rbt_left->_rbt_parent = parent;
    }
  }
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::swap_unrelated_nodes(Node * n1, Node * n2)
{
  std::swap(n1->_rbt_left, n2->_rbt_left);
  if (n1->_rbt_left != nullptr) {
    n1->_rbt_left->_rbt_parent = n1;
  }
  if (n2->_rbt_left != nullptr) {
    n2->_rbt_left->_rbt_parent = n2;
  }

  std::swap(n1->_rbt_right, n2->_rbt_right);
  if (n1->_rbt_right != nullptr) {
    n1->_rbt_right->_rbt_parent = n1;
  }
  if (n2->_rbt_right != nullptr) {
    n2->_rbt_right->_rbt_parent = n2;
  }

  std::swap(n1->_rbt_parent, n2->_rbt_parent);
  if (n1->_rbt_parent != nullptr) {
    if (n1->_rbt_parent->_rbt_right == n2) {
      n1->_rbt_parent->_rbt_right = n1;
    } else {
      n1->_rbt_parent->_rbt_left = n1;
    }
  } else {
    this->root = n1;
  }
  if (n2->_rbt_parent != nullptr) {
    if (n2->_rbt_parent->_rbt_right == n1) {
      n2->_rbt_parent->_rbt_right = n2;
    } else {
      n2->_rbt_parent->_rbt_left = n2;
    }
  } else {
    this->root = n2;
  }
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::remove_to_leaf(Node & node)
{
  Node * cur = &node;
  Node * child = &node;

  if ((cur->_rbt_right != nullptr) && (cur->_rbt_left != nullptr)) {
    // Find the minimum of the larger-or-equal children
    child = cur->_rbt_right;
    while (child->_rbt_left != nullptr) {
      child = child->_rbt_left;
    }
  } else if (cur->_rbt_left != nullptr) {
    // Only a left child. This must be red and cannot have further children (otherwise, black-balance would be violated)
    child = child->_rbt_left;
  }

  //std::cout << "Selected Child has ID " << NodeTraits::get_id(child) << "…";

  if (child != &node) {
    this->swap_nodes(&node, child, false);
  }
  // Now, node is a pseudo-leaf with the color of child.

  // Node cannot have a left child, so if it has a right child, it must be red,
  // thus node must be black
  if (node._rbt_right != nullptr) {
    // replace node with its child and color the child black.
    auto right_child = node._rbt_right;
    this->swap_nodes(&node, right_child, true);
    right_child->_rbt_color = Base::Color::BLACK;
    right_child->_rbt_right = nullptr; // this stored the node to be deleted…
    // TODO null the pointers in node?

    // TODO do not swap *and* delete…
    EqualityList::equality_list_delete_node(node);

    NodeTraits::deleted_below(*right_child);

    return; // no fixup necessary
  }

  // Node has no children, so we have to just delete it, which is no problem if we are red. Otherwise, we must start a fixup at the parent.
  bool deleted_left = false;
  if (node._rbt_parent != nullptr) {
    if (node._rbt_parent->_rbt_left == &node) {
      node._rbt_parent->_rbt_left = nullptr;
      deleted_left = true;
    } else {
      node._rbt_parent->_rbt_right = nullptr;
    }

    EqualityList::equality_list_delete_node(node);
    NodeTraits::deleted_below(*node._rbt_parent);
  } else {
    this->root = nullptr; // Tree is now empty!
    return; // No fixup needed!
  }

  if (node._rbt_color == Base::Color::BLACK) {
    this->fixup_after_delete(node._rbt_parent, deleted_left);
  }
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::fixup_after_delete(Node * parent, bool deleted_left)
{
  bool propagating_up = true;
  Node * sibling;

  while (propagating_up) {
    //std::cout << "=> Parent has ID " << NodeTraits::get_id(parent) << " <= ";
    // We just deleted a black node from under parent.
    if (deleted_left) {
      sibling = parent->_rbt_right;
    } else {
      sibling = parent->_rbt_left;
    }

    // sibling must exist! If it didn't, then that branch would have had too few blacks…
    if ((parent->_rbt_color == Base::Color::BLACK) && (sibling->_rbt_color == Base::Color::BLACK) &&
        ((sibling->_rbt_left == nullptr) || (sibling->_rbt_left->_rbt_color == Base::Color::BLACK)) &&
        ((sibling->_rbt_right == nullptr) || (sibling->_rbt_right->_rbt_color == Base::Color::BLACK))) {

      // We can recolor and propagate up! (Case 3)
      //std::cout << "Case 3… ";
      sibling->_rbt_color = Base::Color::RED;
      // Now everything below parent is okay, but the branch started in parent lost a black!
      if (parent->_rbt_parent == nullptr) {
        // Doesn't matter! parent is the root, no harm done.
        return;
      } else {
        // propagate up!
        //std::cout << "propagating up…";
        deleted_left = parent->_rbt_parent->_rbt_left == parent;
        parent = parent->_rbt_parent;
      }
    } else { // could not recolor the sibling, do not propagate up
      propagating_up = false;
    }
  }

  if (sibling->_rbt_color == Base::Color::RED) {
    // Case 2
    //std::cout << "Case 2… ";
    sibling->_rbt_color = Base::Color::BLACK;
    parent->_rbt_color = Base::Color::RED;
    if (deleted_left) {
      this->rotate_left(parent);
      sibling = parent->_rbt_right;
    } else {
      this->rotate_right(parent);
      sibling = parent->_rbt_left;
    }
  }

  if ((sibling->_rbt_color == Base::Color::BLACK) &&
      ((sibling->_rbt_left == nullptr) || (sibling->_rbt_left->_rbt_color == Base::Color::BLACK)) &&
      ((sibling->_rbt_right == nullptr) || (sibling->_rbt_right->_rbt_color == Base::Color::BLACK))) {
    // case 4
    //std::cout << "Case 4… ";
    parent->_rbt_color = Base::Color::BLACK;
    sibling->_rbt_color = Base::Color::RED;

    return; // No further fixup necessary
  }

  if (deleted_left) {
    if ((sibling->_rbt_right == nullptr) || (sibling->_rbt_right->_rbt_color == Base::Color::BLACK)) {
      // left child of sibling must be red! This is the folded case. (Case 5) Unfold!
      //std::cout << "Case 5 (L)… ";
      this->rotate_right(sibling);
      sibling->_rbt_color = Base::Color::RED;
      // The new sibling is now the parent of the sibling
      sibling = sibling->_rbt_parent;
      sibling->_rbt_color = Base::Color::BLACK;
    }

    // straight situation, case 6 applies!
    //std::cout << "Case 6 (L)…";
    this->rotate_left(parent);
    std::swap(parent->_rbt_color, sibling->_rbt_color);
    sibling->_rbt_right->_rbt_color = Base::Color::BLACK;
  } else {
    if ((sibling->_rbt_left == nullptr) || (sibling->_rbt_left->_rbt_color == Base::Color::BLACK)) {
      // right child of sibling must be red! This is the folded case. (Case 5) Unfold!
      //std::cout << "Case 5 (R)… ";

      this->rotate_left(sibling);
      sibling->_rbt_color = Base::Color::RED;
      // The new sibling is now the parent of the sibling
      sibling = sibling->_rbt_parent;
      sibling->_rbt_color = Base::Color::BLACK;
    }

    // straight situation, case 6 applies!
    //std::cout << "Case 6 (R)…";
    this->rotate_right(parent);
    std::swap(parent->_rbt_color, sibling->_rbt_color);
    sibling->_rbt_left->_rbt_color = Base::Color::BLACK;
  }
}

template<class Node, class NodeTraits, class Compare>
void
RBTree<Node, NodeTraits, Compare>::remove(Node & node)
{
  // TODO collapse this method
  this->remove_to_leaf(node);
}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::const_iterator()
{}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::const_iterator(Node * n_in)
  : n(n_in)
{}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::const_iterator(const const_iterator & other)
  : n(other.n)
{}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::~const_iterator()
{}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<reverse> &
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::operator=(const typename RBTree<Node, NodeTraits, Compare>::template const_iterator<reverse> & other)
{
  this->n = other.n;
}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<reverse> &
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::operator=(typename RBTree<Node, NodeTraits, Compare>::template const_iterator<reverse> && other)
{
  this->n = other.n;
}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
bool
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::operator==(const typename RBTree<Node, NodeTraits, Compare>::template const_iterator<reverse> & other) const
{
  return (this->n == other.n);
}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
bool
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::operator!=(const typename RBTree<Node, NodeTraits, Compare>::template const_iterator<reverse> & other) const
{
  return (this->n != other.n);
}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<reverse> &
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::operator++()
{
  return utilities::IteratorHelper<Node, const_iterator<reverse>, reverse>::operator_plusplus(*this);
}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<reverse> &
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::operator--()
{
  return utilities::IteratorHelper<Node, const_iterator<reverse>, reverse>::operator_minusminus(*this);
}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<reverse>
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::operator++(int)
{
  const_iterator<reverse> cpy(*this);
  this->operator++();
  return cpy;
}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<reverse> &
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::operator+=(size_t steps)
{
  for (size_t i = 0 ; i < steps ; ++steps) {
    this->operator++();
  }

  return (*this);
}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<reverse>
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::operator+(size_t steps) const
{
  const_iterator<reverse> cpy(*this);
  cpy += steps;
  return cpy;
}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<reverse>::const_reference
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::operator*() const
{
  return *(this->n);
}

template<class Node, class NodeTraits, class Compare>
template<bool reverse>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<reverse>::const_pointer
RBTree<Node, NodeTraits, Compare>::const_iterator<reverse>::operator->() const
{
  return this->n;
}

template<class Node, class NodeTraits, class Compare>
Node *
RBTree<Node, NodeTraits, Compare>::get_smallest() const
{
  Node * smallest = this->root;
  if (smallest == nullptr) {
    return nullptr;
  }

  while (smallest->_rbt_left != nullptr) {
    smallest = smallest->_rbt_left;
  }

  return smallest;
}

template<class Node, class NodeTraits, class Compare>
Node *
RBTree<Node, NodeTraits, Compare>::get_largest() const
{
  Node * largest = this->root;
  if (largest == nullptr) {
    return nullptr;
  }

  while (largest->_rbt_right != nullptr) {
    largest = largest->_rbt_right;
  }

  return largest;
}

template<class Node, class NodeTraits, class Compare>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<false>
RBTree<Node, NodeTraits, Compare>::iterator_to(const Node & node) const
{
  return const_iterator<false>(&node);
}

template<class Node, class NodeTraits, class Compare>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<false>
RBTree<Node, NodeTraits, Compare>::cbegin() const
{
  Node * smallest = this->get_smallest();
  if (smallest == nullptr) {
    return const_iterator<false>(nullptr);
  }

  return const_iterator<false>(smallest);
}

template<class Node, class NodeTraits, class Compare>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<false>
RBTree<Node, NodeTraits, Compare>::cend() const
{
  return const_iterator<false>(nullptr);
}

template<class Node, class NodeTraits, class Compare>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<false>
RBTree<Node, NodeTraits, Compare>::begin() const
{
  return this->cbegin();
}

template<class Node, class NodeTraits, class Compare>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<false>
RBTree<Node, NodeTraits, Compare>::end() const
{
  return this->cend();
}

template<class Node, class NodeTraits, class Compare>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<true>
RBTree<Node, NodeTraits, Compare>::crbegin() const
{
  Node * largest = this->get_largest();
  if (largest == nullptr) {
    return const_iterator<true>(nullptr);
  }

  return const_iterator<true>(largest);
}

template<class Node, class NodeTraits, class Compare>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<true>
RBTree<Node, NodeTraits, Compare>::crend() const
{
  return const_iterator<true>(nullptr);
}

template<class Node, class NodeTraits, class Compare>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<true>
RBTree<Node, NodeTraits, Compare>::rbegin() const
{
  return this->crbegin();
}

template<class Node, class NodeTraits, class Compare>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<true>
RBTree<Node, NodeTraits, Compare>::rend() const
{
  return this->crend();
}

template<class Node, class NodeTraits, class Compare>
template<class Comparable>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<false>
RBTree<Node, NodeTraits, Compare>::find(const Comparable & query) const
{
  Node * cur = this->root;
  while (cur != nullptr) {
    if (Compare()(query, *cur)) {
      cur = cur->_rbt_left;
    } else if (Compare()(*cur, query)) {
      cur = cur->_rbt_right;
    } else {
      cur = EqualityList::equality_list_find_first(cur);
      return const_iterator<false>(cur);
    }
  }

  return this->cend();
}

template<class Node, class NodeTraits, class Compare>
template<class Comparable>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<false>
RBTree<Node, NodeTraits, Compare>::upper_bound(const Comparable & query) const
{
  Node * cur = this->root;
  Node * bound = nullptr;
  while (cur != nullptr) {
    if (Compare()(query, *cur)) {
      bound = cur;
      cur = cur->_rbt_left;
    } else if (Compare()(*cur, query)) {
      cur = cur->_rbt_right;
    } else {
      // hit the spot
      cur = cur->_rbt_right;
      if (cur != nullptr) {
        bound = cur;
        break;
      }
    }
  }

  return const_iterator<false>(bound);
}

/*
template<class Node, class NodeTraits, class Compare>
template<class Comparable>
typename RBTree<Node, NodeTraits, Compare>::template const_iterator<false>
RBTree<Node, NodeTraits, Compare>::lower_bound(const Comparable & query) const
{
  Node * cur = this->root;
  while (cur != nullptr) {
    if (Compare()(query, *cur)) {
      cur = cur->_rbt_left;
    } else if (Compare()(*cur, query)) {
      cur = cur->_rbt_right;
    } else {
      cur = EqualityList::equality_list_find_first(cur);
      return const_iterator<false>(cur);
    }
  }

  return this->cend();
}
*/
