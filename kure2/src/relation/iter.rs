use std::ops::Range;

use cudd2_sys::DdHalfWord;
use rug::Integer;

use crate::{Relation, dd::DdNodeRef};

pub struct Iter<'a> {
    stack: Vec<State<'a>>,
}

#[derive(Debug)]
struct State<'a> {
    node: DdNodeRef<'a>,
    index: DdHalfWord,
    min_i: Integer,
    max_i: Integer,
    min_j: Integer,
    max_j: Integer,
}

impl Relation {
    /// Returns an iterator over the bits that are set to true in this relation.
    pub fn iter<'a>(&'a self) -> Iter<'a> {
        Iter {
            stack: vec![State {
                node: self.dd_node(),
                index: 0,
                min_i: Integer::ZERO,
                max_i: self.rows().next_power_of_two(),
                min_j: Integer::ZERO,
                max_j: self.cols().next_power_of_two(),
            }],
        }
    }
}

impl Iterator for Iter<'_> {
    type Item = (Integer, Integer);

    fn next(&mut self) -> Option<Self::Item> {
        loop {
            let Some(state) = self.stack.pop() else {
                return None;
            };

            if state.node.is_false() {
                continue;
            }

            match state.split() {
                Ok((left, right)) => {
                    self.stack.push(right);
                    self.stack.push(left);
                }
                Err((i, j)) => return Some((i, j)),
            }
        }
    }
}

impl<'a> State<'a> {
    fn split(self) -> Result<(Self, Self), (Integer, Integer)> {
        if self.max_i != self.min_i.clone() + 1 {
            Ok(self.split_i())
        } else if self.max_j != self.min_j.clone() + 1 {
            Ok(self.split_j())
        } else {
            Err((self.min_i.clone(), self.min_j.clone()))
        }
    }

    fn split_i(self) -> (Self, Self) {
        let (lhs_node, rhs_node) = self.split_node();
        let (lhs_i_range, rhs_i_range) = Self::split_range(self.min_i.clone()..self.max_i.clone());
        (
            State {
                node: lhs_node,
                index: self.index + 1,
                min_i: lhs_i_range.start,
                max_i: lhs_i_range.end,
                min_j: self.min_j.clone(),
                max_j: self.max_j.clone(),
            },
            State {
                node: rhs_node,
                index: self.index + 1,
                min_i: rhs_i_range.start,
                max_i: rhs_i_range.end,
                min_j: self.min_j,
                max_j: self.max_j,
            },
        )
    }

    fn split_j(self) -> (Self, Self) {
        let (lhs_node, rhs_node) = self.split_node();
        let (lhs_j_range, rhs_j_range) = Self::split_range(self.min_j.clone()..self.max_j.clone());
        (
            State {
                node: lhs_node,
                index: self.index + 1,
                min_i: self.min_i.clone(),
                max_i: self.max_i.clone(),
                min_j: lhs_j_range.start,
                max_j: lhs_j_range.end,
            },
            State {
                node: rhs_node,
                index: self.index + 1,
                min_i: self.min_i,
                max_i: self.max_i,
                min_j: rhs_j_range.start,
                max_j: rhs_j_range.end,
            },
        )
    }

    fn split_node(&self) -> (DdNodeRef<'a>, DdNodeRef<'a>) {
        if self.node.is_const() || self.node.index() > self.index {
            (self.node, self.node)
        } else {
            (self.node.else_child(), self.node.then_child())
        }
    }

    fn split_range(range: Range<Integer>) -> (Range<Integer>, Range<Integer>) {
        let mid: Integer = (range.start.clone() + &range.end) >> 1;
        (
            Range {
                start: range.start,
                end: mid.clone(),
            },
            Range {
                start: mid,
                end: range.end,
            },
        )
    }
}
