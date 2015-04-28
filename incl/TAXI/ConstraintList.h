// Constraint List data structures for PlanarityTester
#include <list>

// OVERLOADED TEMPLATED OUTPUT OPERATORS, operator<<, FOR DEBUGGING

template<typename T>
ostream & operator<<(ostream & os, const list<T> & l) {
//for(list<T>::const_iterator it = l.begin(); it != l.end(); ++it) { 

  typename list<T>::const_iterator it; // modified Hubert
  for(it = l.begin(); it != l.end(); ++it) { 
		if (it != l.begin()) { os << ','; } 
		os << *it; 
	}	

	// The second arguement to ostream_iterator<T> constructor is a delimiter, 
	// e.g. a comma, that is inserted after every element including the last
	//copy(l.begin(), l.end(), std::ostream_iterator<T>(os, ","));	
	return os;
}

struct FundamentalCycleConstraint {
	list<int> c[2];	// the two lists of fundamental cycle heights

	FundamentalCycleConstraint(int h) { 
		c[0].push_front(h); 
	} 
	bool empty() { 
		return c[0].empty() && c[1].empty(); 
	} 

	friend ostream & operator<<(ostream & os, const FundamentalCycleConstraint & fcc) {
		return os << "[(" << fcc.c[0] << "), (" << fcc.c[1] << ")]";
	}
	
};

struct ConstraintList : public list<FundamentalCycleConstraint> {
	ConstraintList() { }
	ConstraintList(int h) { 
		push_front(FundamentalCycleConstraint(h)); 
	}

	friend ostream & operator<<(ostream & os, const ConstraintList & cl) {
		os << '{';
		for(list<FundamentalCycleConstraint>::const_iterator it = cl.begin(); it != cl.end(); ++it) { os << *it << ' '; }
		os << "} ";
		return os;
	}

	bool operator< (const ConstraintList & rhs) const {
		int diff = back().c[0].back() - rhs.back().c[0].back();
		if ( diff !=  0 ) { 
			return diff < 0; 
		}
		else { 
			return front().c[0].front() < rhs.front().c[0].front(); 
		}
	}

	// if all constraints are one-sided returns true and merges them 
	// into the first constraint.  Otherwise returns false.
	bool oneSided() {
		const_iterator it = begin();
		if (it->c[1].empty() == false) { 
			return false; 
		}
		for(++it; it != end(); ++it) {
			if (it->c[1].empty() == false) { 
				return false; 
			}
			front().c[0].splice(  front().c[0].end(), (list<int>&) it->c[0]);	
					
		}
		return true;
	}

	// Precondition: this < other
	bool merge(ConstraintList & other) {
		#ifdef _DEBUG
			cout <<  " this: " << *this << endl << " other: " << other << endl;
			assert (! empty() && ! other.empty());
		#endif
		// merge others non two-sided constraint set into first constraint
		if (other.oneSided() == false) { 
			return false; 
		}						 
		// move all the cycles in one-sided constraints in this that overlap
		// a cycle in the c[0] side of other to the c[1] side of other
		while (front().c[1].empty() 
			&& front().c[0].front() > other.front().c[0].back()) { 
			other.front().c[1].splice(other.front().c[1].end(), front().c[0]);
			pop_front();
		}
		if (front().c[1].empty()) {	// if front() is a one-sided constraint	
			if (back().c[0].front() == other.front().c[0].back()) { 
				other.front().c[0].pop_back(); // eliminate duplicate min back edges.
				// swap other's c[0] and c[1] cycles if c[0] is no longer the lowest
				if (other.front().c[0].empty() 
					|| (other.front().c[1].empty() == false 
						&& other.front().c[0].back() > other.front().c[1].back())) {
							other.front().c[0].swap(other.front().c[1]);
				}
			}
		}
		else {	// If front() is a two sided constraint 
			int highSide = front().c[0].front() < front().c[1].front(); 
			int lowSide = -highSide + 1;
			if (other.front().c[0].back() < front().c[lowSide].front()) {		
				// an other cycle ovelaps cycles on both sides of this's two-sided constraint
				return false;	
			}
			else if (other.front().c[0].back() < front().c[highSide].front()) { 
				front().c[lowSide].splice(front().c[lowSide].begin(), other.front().c[0]);
				front().c[highSide].splice(front().c[highSide].begin(), other.front().c[1]);
			}
		}
		if (other.front().c[0].empty() == false) { 
			push_front(other.front());		// add non-empty constraint
		}
		return true; 
	}

	// prune fundamental cycles that are above the current dfs tree edge 
	void prune(int dfs_height) { 
		while ( empty() == false && front().c[0].size() > 0 
			&& front().c[0].front() == dfs_height) { 
			if (front().c[0].size() == 1) { 
				pop_front(); 
			}
			else { 
				front().c[0].pop_front(); 
			}
		}
		while ( empty() == false && front().c[1].empty() == false 
			&& front().c[1].front() == dfs_height) { 
				front().c[1].pop_front(); 
		}
	}
};
