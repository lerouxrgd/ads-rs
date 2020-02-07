//! A trivial search tree, without rebalancing

use std::borrow::{Cow, ToOwned};
use std::marker::PhantomData;
use std::mem;
use std::ptr;

use std::collections::BTreeMap;

pub struct SearchTree<'a, K: ToOwned, V> {
    tree: Option<Box<TreeNode<'a, K, V>>>,
}

enum TreeNode<'a, K: ToOwned, V> {
    Internal {
        key: K,
        left: Box<TreeNode<'a, K, V>>,
        right: Box<TreeNode<'a, K, V>>,
    },
    Leaf {
        key: Cow<'a, K>,
        val: V,
    },
}

impl<'a, K, V> TreeNode<'a, K, V>
where
    K: ToOwned + Ord,
{
    pub fn new_internal(key: K) -> TreeNode<'a, K, V> {
        todo!()
        // TreeNode::Internal {
        //     key,
        //     left: None,
        //     right: None,
        // }
    }

    pub fn new_leaf(key: Cow<K>, val: V) -> TreeNode<K, V> {
        TreeNode::Leaf { key, val }
    }

    pub fn key(&self) -> &K {
        match self {
            Self::Internal { key, .. } => key,
            Self::Leaf { key, .. } => key,
        }
    }

    pub fn val_mut(&mut self) -> &mut V {
        match self {
            Self::Leaf { ref mut val, .. } => val,
            Self::Internal { .. } => unreachable!("no val on internal node"),
        }
    }

    // pub fn data_ref(&self) -> Option<&V> {
    //     self.left.map(|node| unsafe { &*(node.as_ptr() as *mut _) })
    // }
}

impl<'a, K, V> SearchTree<'a, K, V>
where
    K: ToOwned + Ord,
{
    pub fn new() -> SearchTree<'a, K, V> {
        SearchTree { tree: None }
    }

    pub fn insert(&mut self, key: K, val: V) -> Option<V> {
        if let None = &mut self.tree {
            self.tree = Some(Box::new(TreeNode::new_leaf(
                Cow::Owned(key.to_owned()),
                val,
            )));
            return None;
        }

        let mut tmp_node = self.tree.as_deref_mut().unwrap();
        while let TreeNode::Internal {
            key: tmp_key,
            left,
            right,
        } = tmp_node
        {
            if key < *tmp_key {
                tmp_node = left;
            } else {
                tmp_node = right;
            }
        }
        if let TreeNode::Leaf {
            key: tmp_key,
            val: tmp_val,
        } = tmp_node
        {
            match tmp_key {
                Cow::Owned(tmp_key) => {
                    // handle that (not necessarily root?) ...
                    todo!()
                }
                Cow::Borrowed(tmp_key) => {
                    if key == **tmp_key {
                        return Some(mem::replace(tmp_node.val_mut(), val));
                    }
                    let old_leaf = TreeNode::new_leaf(Cow::Borrowed(tmp_key), tmp_val);
                    let new_leaf = TreeNode::new_leaf(Cow::Borrowed(&key), val);
                    if *tmp_key < new_leaf.key() {
                        // unsafe {ptr::write(tmp_key, )}
                        // tmp_node
                    }
                }
            }
        }
        unreachable!()
    }
}
