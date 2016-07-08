/*
	list-utils.cc
	-------------
*/

#include "vlib/list-utils.hh"

// vlib
#include "vlib/iterators/list_iterator.hh"


namespace vlib
{
	
	const Value& get_nth( const Value& list, unsigned i )
	{
		const Value* next = &list;
		
		while ( i-- > 0  &&  ! is_empty( *next ) )
		{
			next = &rest( *next );
		}
		
		return first( *next );
	}
	
	unsigned long count( const Value& list )
	{
		list_iterator it( list );
		
		unsigned long total = 0;
		
		while ( it.next() )
		{
			++total;
		}
		
		return total;
	}
	
	static
	Value& last_mutable( Value& list )
	{
		Value* next = &list;
		
		while ( next->listexpr() != 0 )  // NULL
		{
			next = &rest_mutable( *next );
		}
		
		return *next;
	}
	
	void splice_lists( Value& left, const Value& right )
	{
		if ( is_empty( right ) )
		{
			return;
		}
		
		if ( is_empty( left ) )
		{
			left = right;
			return;
		}
		
		Value& last = last_mutable( left );
		
		last = Value( last, right );
	}
	
	Value make_list( const Value& left, const Value& right )
	{
		Value list = left;
		
		splice_lists( list, right );
		
		return list;
	}
	
	Value reverse_list( const Value& list )
	{
		Expr* expr = list.listexpr();
		
		if ( expr == 0 )  // NULL
		{
			return list;  // empty list or single item
		}
		
		const Value* next = &expr->right;
		
		Value result = expr->left;
		
		while (( expr = next->listexpr() ))
		{
			result = Value( expr->left, result );
			
			next = &expr->right;
		}
		
		return Value( *next, result );
	}
	
}
