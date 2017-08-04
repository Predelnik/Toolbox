#include "comp/btree.h"
#include "catch.hpp"
#include <random>

template <typename Tree>
struct tree_walker
{
private:
  using self = tree_walker;
public:
  tree_walker (const Tree &tree) : m_tree (&tree) { m_cur_node = m_tree->root (); }
  self &color (btree::color_t expected)
  {
    REQUIRE (m_cur_node->color () == expected);
    return *this;
  }

  template <typename ArgType>
  self &key (const ArgType &expected)
  {
    REQUIRE (m_cur_node->key () == expected);
    return *this;
  }

  self &walk (btree::direction_t direction)
  {
    m_cur_node = m_cur_node->child (direction);
    return *this;
  }

  self &up ()
  {
    m_cur_node = m_cur_node->parent ();
    return *this;
  }

  self &to_root ()
  {
    m_cur_node = m_tree->root ();
    return *this;
  }

private:
  const typename Tree::node_t *m_cur_node;
  const Tree *m_tree;
};

template <typename Tree>
void check_rb_tree (const Tree &tree)
{
  using namespace btree;
  REQUIRE (tree.root ()->color () == btree::color_t::black);
  auto cur_node = tree.root (), next_node = cur_node;
  int black_cnt = 0;
  auto calc = [&](auto node, int inc){ if (node->color () == color_t::black) black_cnt += inc; };
  do {
      calc (cur_node = next_node, 1);  
      next_node = cur_node->child (btree::left);
    } while (next_node);
   int etalon_black_cnt = black_cnt;
   while (cur_node)
     {
       if (cur_node->color () == color_t::red)
         {
          REQUIRE (cur_node->child_color (left) == color_t::black);
          REQUIRE (cur_node->child_color (right) == color_t::black);
         }

        if (cur_node->child_count () < 2)
          REQUIRE(black_cnt == etalon_black_cnt);    
        
        if (auto c = cur_node->child (right))
          {
            calc (cur_node = c, 1);
            while ((c = cur_node->child (left)))
              calc (cur_node = c, 1);
            continue;
          }

        while (cur_node->parent () && cur_node->direction_from_parent () == right)
          {
            calc (cur_node, -1);
            cur_node = cur_node->parent ();
          }
        calc (cur_node, -1);
        cur_node = cur_node->parent ();
     }
}

template <typename Tree>
tree_walker<Tree> walk (const Tree &tree) { return {tree}; }

TEST_CASE("rb_tree") {
  using namespace btree;
  rb_tree<int> t;
  for (auto i : {5, 11, 3, 16, 12})
    t.insert (i);
  walk (t).color(color_t::black).key (5)
    .walk(left).key (3).color (color_t::black)
  .up ().walk(right).key (12).color (color_t::black)
  .walk(left).key (11).color (color_t::red)
  .up ().walk(right).key (16).color (color_t::red);
  t.clear ();
  REQUIRE (t.size () == 0);
  for (auto i : {5, 11, 3, 16, 12, 7, 66, 15, 10})
    t.insert (i);

  REQUIRE (t.size () == 9);

  for (auto i : {12, 3, 66})
    t.erase (i);

  REQUIRE (t.size () == 6);

  walk (t).color(color_t::black).key (11).color (color_t::black)
    .walk(left).key (7).color (color_t::red)
  .walk(left).key (5).color (color_t::black)
  .up ().walk(right).key (10).color (color_t::black)
  .to_root()
  .walk(right).key (16).color (color_t::black)
  .walk(left).key (15).color (color_t::red);

  {
    rb_tree<int> tree;
    tree.insert (123);
    tree.erase (123);
  }
}

TEST_CASE("indexed_rb_tree")
{
  using namespace btree;
  indexed_rb_tree<int> tree;
  tree.insert(3);
  REQUIRE (tree.index (3) == 0);
  tree.insert(7);
  REQUIRE (tree.index (7) == 1);
  tree.insert(5);
  REQUIRE (tree.index (5) == 1);
  tree.insert(6);
  REQUIRE (tree.index (6) == 2);
  REQUIRE (tree.index (3) == 0);
  tree.erase (5);
  REQUIRE (tree.index (6) == 1);
  REQUIRE (tree.index (7) == 2);
  tree.insert (100);
  tree.insert (150);
  tree.insert (200);
  REQUIRE (tree.index (150) == 4);

  {
    indexed_rb_tree<int> t;
    for (auto i : {0, 1, 2, 3})
      t.insert (i);
    t.erase (2);
     walk (t).color(color_t::black).key (1).color (color_t::black)
    .walk(left).key (0).color (color_t::black).to_root ()
    .walk(right).key (3).color (color_t::black);
  }
}

TEST_CASE("random_rb_tree_test")
{
  using namespace btree;
  rb_tree<int> tree;
  std::random_device rd;
  std::uniform_int_distribution<int> insert_uid (0, 10000);
  int insert_count = 100;
  int check_frequency = 10;
  std::vector<int> tree_val;
  for (int i = 0; i < insert_count; ++i)
    {
      tree_val.push_back(insert_uid (rd));
      tree.insert (tree_val.back ());
    if (i % check_frequency == 0)
      check_rb_tree(tree);
    }
  int erase_count = 50;
  std::uniform_int_distribution<int> erase_uid (0, static_cast<int> (tree_val.size () - 1));
  for (int i = 0; i < erase_count; ++i)
    {
      tree.erase (tree_val[erase_uid (rd)]);
      if (i % check_frequency == 0)
        check_rb_tree(tree);
    }
}

TEST_CASE("iterator")
{
  using namespace btree;
  {
    rb_tree<int> tree;
    tree.insert (7);
    tree.insert (5);
    tree.insert (17);
    tree.insert (13);
    tree.insert (21);
    std::vector<int> v{5, 7, 13, 17, 21};
    REQUIRE (std::equal (tree.begin(), tree.end (), v.begin ()));
    std::reverse (v.begin (), v.end ());
    REQUIRE (std::equal (std::make_reverse_iterator(tree.end()), std::make_reverse_iterator(tree.begin ()), v.begin ()));
  }
  {
    rb_tree<int> tree;
    auto it = tree.end ();
    tree.insert (5);
    REQUIRE (*(--it) == 5);
  }
}