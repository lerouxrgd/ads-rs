//! A cyclic list-based queue

use std::ptr::NonNull;

pub struct Queue<T> {
    entry: NonNull<Node<T>>,
    marker: std::marker::PhantomData<Box<Node<T>>>,
}

struct Node<T> {
    val: Option<T>,
    next: NonNull<Node<T>>,
}

impl<T> Queue<T> {
    pub fn new() -> Self {
        unsafe {
            let entry = Box::new(Node {
                val: None,
                next: NonNull::dangling(),
            });

            let placeholder = Box::new(Node {
                val: None,
                next: NonNull::dangling(),
            });

            let mut placeholder = NonNull::new_unchecked(Box::into_raw(placeholder));
            placeholder.as_mut().next = placeholder;

            let mut entry = NonNull::new_unchecked(Box::into_raw(entry));
            entry.as_mut().next = placeholder;

            Queue {
                entry,
                marker: std::marker::PhantomData,
            }
        }
    }

    pub fn is_empty(&self) -> bool {
        unsafe { self.entry.as_ref().next == self.entry.as_ref().next.as_ref().next }
    }

    pub fn push(&mut self, val: T) {
        unsafe {
            let new_node = Box::new(Node {
                val: Some(val),
                next: NonNull::dangling(),
            });
            let mut new_node = NonNull::new_unchecked(Box::into_raw(new_node));

            let mut tmp = self.entry.as_mut().next;
            self.entry.as_mut().next = new_node;
            new_node.as_mut().next = tmp.as_ref().next;
            tmp.as_mut().next = new_node;
        }
    }

    pub fn pop(&mut self) -> Option<T> {
        unsafe {
            if self.is_empty() {
                None
            } else {
                let mut tmp = self.entry.as_mut().next.as_mut().next.as_mut().next;
                self.entry.as_mut().next.as_mut().next.as_mut().next = tmp.as_ref().next;
                if tmp == self.entry.as_ref().next {
                    self.entry.as_mut().next = tmp.as_ref().next;
                }
                tmp.as_mut().val.take()
            }
        }
    }

    pub fn peek(&self) -> Option<&T> {
        unsafe {
            self.entry
                .as_ref()
                .next
                .as_ref()
                .next
                .as_ref()
                .next
                .as_ref()
                .val
                .as_ref()
        }
    }

    pub fn peek_mut(&mut self) -> Option<&mut T> {
        unsafe {
            self.entry
                .as_mut()
                .next
                .as_mut()
                .next
                .as_mut()
                .next
                .as_mut()
                .val
                .as_mut()
        }
    }

    pub fn into_iter(mut self) -> impl Iterator<Item = T> {
        std::iter::from_fn(move || self.pop())
    }

    pub fn iter(&self) -> impl Iterator<Item = &T> {
        unsafe {
            let mut cur = self.entry.as_ref().next.as_ref().next.as_ref().next;

            std::iter::from_fn(move || {
                let res = (&*cur.as_ptr()).val.as_ref();
                cur = cur.as_ref().next;
                res
            })
        }
    }

    pub fn iter_mut(&mut self) -> impl Iterator<Item = &mut T> {
        unsafe {
            let mut cur = self.entry.as_mut().next.as_mut().next.as_mut().next;

            std::iter::from_fn(move || {
                let res = (&mut *cur.as_ptr()).val.as_mut();
                cur = cur.as_mut().next;
                res
            })
        }
    }
}

impl<T> Drop for Queue<T> {
    fn drop(&mut self) {
        unsafe {
            let mut tmp = self.entry.as_mut().next.as_mut().next;
            while tmp != self.entry.as_ref().next {
                self.entry.as_mut().next.as_mut().next = tmp.as_mut().next;
                drop(tmp);
                tmp = self.entry.as_mut().next.as_mut().next;
            }
            drop(self.entry.as_mut());
            drop(self.entry);
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
