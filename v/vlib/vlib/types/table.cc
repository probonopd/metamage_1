/*
	table.cc
	--------
*/

#include "vlib/types/table.hh"

// vlib
#include "vlib/symbol.hh"
#include "vlib/throw.hh"
#include "vlib/dispatch/dispatch.hh"
#include "vlib/dispatch/operators.hh"
#include "vlib/dispatch/verity.hh"
#include "vlib/iterators/array_iterator.hh"
#include "vlib/types/any.hh"
#include "vlib/types/boolean.hh"
#include "vlib/types/type.hh"


namespace vlib
{
	
	static const Type etc = etc_vtype;
	
	static
	bool table_truth( const Value& v )
	{
		// Check if the array is empty.
		return Boolean( v.expr()->right.expr() != 0 );  // NULL
	}
	
	static const veritization table_veritization =
	{
		&table_truth,
	};
	
	static
	Value unary_op_handler( op_type op, const Value& v )
	{
		switch ( op )
		{
			case Op_typeof:
				return Value( v.expr()->left, Op_empower, etc );
			
			default:
				break;
		}
		
		return Value();
	}
	
	static const operators ops =
	{
		&unary_op_handler,
	};
	
	const dispatch table_dispatch =
	{
		0,  // NULL
		&table_veritization,
		0,  // NULL
		&ops,
	};
	
	static
	void check_table( const Value& key_type, const Value& array )
	{
		array_iterator it( array );
		
		while ( it )
		{
			const Value& mapping = it.use();
			
			if ( Expr* expr = mapping.expr() )
			{
				if ( expr->op == Op_mapping )
				{
					if ( as_assigned( key_type, expr->left ).type() )
					{
						continue;
					}
					
					THROW( "non-conforming key type in table" );
				}
			}
			
			THROW( "non-mapping in table" );
		}
	}
	
	Table::Table( const Value& key_type, const Value& array )
	:
		Value( key_type, Op_empower, array, &table_dispatch )
	{
		check_table( key_type, array );
	}
	
}