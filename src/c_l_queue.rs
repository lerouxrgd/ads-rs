//! A cyclic list-based queue

use std::alloc::{alloc, Layout};
use std::ptr;

pub struct Queue<T> {
    entry: *mut Node<T>,
}

struct Node<T> {
    val: Option<T>,
    next: *mut Node<T>,
}

impl<T> Queue<T> {
    pub fn new() -> Self {
        unsafe {
            let entry = alloc(Layout::new::<Node<T>>()) as *mut Node<T>;
            *entry = Node {
                val: None,
                next: ptr::null_mut(),
            };

            let placeholder = alloc(Layout::new::<Node<T>>()) as *mut Node<T>;
            *placeholder = Node {
                val: None,
                next: ptr::null_mut(),
            };

            (*entry).next = placeholder;
            (*placeholder).next = placeholder;

            Queue { entry }
        }
    }

    pub fn is_empty(&self) -> bool {
        unsafe { (*self.entry).next == (*(*self.entry).next).next }
    }

    pub fn push(&mut self, val: T) {
        unsafe {
            let new_node = alloc(Layout::new::<Node<T>>()) as *mut Node<T>;
            *new_node = Node {
                val: Some(val),
                next: ptr::null_mut(),
            };

            let mut tmp = (*self.entry).next;
            (*self.entry).next = new_node;
            (*new_node).next = (*tmp).next;
            (*tmp).next = new_node;
        }
    }

    pub fn pop(&mut self) -> Option<T> {
        unsafe {
            if self.is_empty() {
                None
            } else {
                let tmp = (*(*(*self.entry).next).next).next;
                (*(*(*self.entry).next).next).next = (*tmp).next;
                if tmp == (*self.entry).next {
                    (*self.entry).next = (*tmp).next;
                }
                (*tmp).val.take()
            }
        }
    }

    pub fn peek(&self) -> Option<&T> {
        unsafe { (*(*(*(*self.entry).next).next).next).val.as_ref() }
    }

    pub fn peek_mut(&mut self) -> Option<&mut T> {
        unsafe { (*(*(*(*self.entry).next).next).next).val.as_mut() }
    }

    pub fn into_iter(mut self) -> impl Iterator<Item = T> {
        std::iter::from_fn(move || self.pop())
    }

    pub fn iter(&self) -> impl Iterator<Item = &T> {
        unsafe {
            let mut cur = (*(*(*self.entry).next).next).next;

            std::iter::from_fn(move || {
                let res = (*cur).val.as_ref();
                cur = (*cur).next;
                res
            })
        }
    }

    pub fn iter_mut(&mut self) -> impl Iterator<Item = &mut T> {
        unsafe {
            let mut cur = (*(*(*self.entry).next).next).next;

            std::iter::from_fn(move || {
                let res = (*cur).val.as_mut();
                cur = (*cur).next;
                res
            })
        }
    }
}

impl<T> Drop for Queue<T> {
    fn drop(&mut self) {
        unsafe {
            let mut tmp = (*(*self.entry).next).next;
            while tmp != (*self.entry).next {
                (*(*self.entry).next).next = (*tmp).next;
                ptr::drop_in_place(tmp);
                tmp = (*(*self.entry).next).next;
            }
            ptr::drop_in_place((*self.entry).next);
            ptr::drop_in_place(self.entry);
        }
    }
}

#[cfg(test)]
mod test {
    use super::Queue;

    #[test]
    fn basics() {
        let mut queue = Queue::new();

        // Check empty queue behaves right
        assert_eq!(queue.pop(), None);

        // Populate queue
        queue.push(1);
        queue.push(2);
        queue.push(3);

        // Check normal removal
        assert_eq!(queue.pop(), Some(1));
        assert_eq!(queue.pop(), Some(2));

        // Push some more just to make sure nothing's corrupted
        queue.push(4);
        queue.push(5);

        // Check normal removal
        assert_eq!(queue.pop(), Some(3));
        assert_eq!(queue.pop(), Some(4));

        // Check exhaustion
        assert_eq!(queue.pop(), Some(5));
        assert_eq!(queue.pop(), None);

        // Check the exhaustion case fixed the pointer right
        queue.push(6);
        queue.push(7);

        // Check normal removal
        assert_eq!(queue.pop(), Some(6));
        assert_eq!(queue.pop(), Some(7));
        assert_eq!(queue.pop(), None);
    }

    #[test]
    fn into_iter() {
        let mut queue = Queue::new();
        queue.push(1);
        queue.push(2);
        queue.push(3);

        let mut iter = queue.into_iter();
        assert_eq!(iter.next(), Some(1));
        assert_eq!(iter.next(), Some(2));
        assert_eq!(iter.next(), Some(3));
        assert_eq!(iter.next(), None);
    }

    #[test]
    fn iter() {
        let mut queue = Queue::new();
        queue.push(1);
        queue.push(2);
        queue.push(3);

        let mut iter = queue.iter();
        assert_eq!(iter.next(), Some(&1));
        assert_eq!(iter.next(), Some(&2));
        assert_eq!(iter.next(), Some(&3));
        assert_eq!(iter.next(), None);
    }

    #[test]
    fn iter_mut() {
        let mut queue = Queue::new();
        queue.push(1);
        queue.push(2);
        queue.push(3);

        let mut iter = queue.iter_mut();
        assert_eq!(iter.next(), Some(&mut 1));
        assert_eq!(iter.next(), Some(&mut 2));
        assert_eq!(iter.next(), Some(&mut 3));
        assert_eq!(iter.next(), None);
    }
}
