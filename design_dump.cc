/*
 * Copyright (c) 1998 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#if !defined(WINNT)
#ident "$Id: design_dump.cc,v 1.39 1999/09/04 19:11:46 steve Exp $"
#endif

/*
 * This file contains all the dump methods of the netlist classes.
 */
# include  <typeinfo>
# include  <iostream>
# include  <iomanip>
# include  "netlist.h"


static ostream& operator<< (ostream&o, NetBlock::Type t)
{
      switch (t) {
	  case NetBlock::SEQU:
	    o << "begin";
	    break;
	  case NetBlock::PARA:
	    o << "fork";
	    break;
      }
      return o;
}

/* Dump a net. This can be a wire or register. */
void NetNet::dump_net(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << type() << ": " << name() << "[" <<
	    pin_count() << "]";
      if (local_flag_)
	    o << " (local)";
      o << " #(" << rise_time() << "," << fall_time() << "," <<
	    decay_time() << ") init=";
      for (unsigned idx = pin_count() ;  idx > 0 ;  idx -= 1)
	    o << ivalue_[idx-1];
      o << endl;
      dump_obj_attr(o, ind+4);
}

void NetMemory::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << ""  << name_ << "[" << width_ << "] " <<
	    "[" << idxh_ << ":" << idxl_ << "]" << endl;
}


/* Dump a NetNode and its pins. Dump what I know about the netnode on
   the first line, then list all the pins, with the name of the
   connected signal. */
void NetNode::dump_node(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "node: ";
      o << typeid(*this).name() << " #(" << rise_time()
	<< "," << fall_time() << "," << decay_time() << ") " << name()
	<< endl;

      dump_node_pins(o, ind+4);
      dump_obj_attr(o, ind+4);
}

/* This is the generic dumping of all the signals connected to each
   pin of the object. The "this" object is not printed, only the
   signals connected to this. */
void NetObj::dump_node_pins(ostream&o, unsigned ind) const
{
      for (unsigned idx = 0 ;  idx < pin_count() ;  idx += 1) {
	    o << setw(ind) << "" << idx;
	    switch (pin(idx).get_dir()) {
		case Link::PASSIVE:
		  o << " p";
		  break;
		case Link::INPUT:
		  o << " I";
		  break;
		case Link::OUTPUT:
		  o << " O";
		  break;
	    }
	    o << ":";

	    unsigned cpin;
	    const NetObj*cur;
	    for (pin(idx).next_link(cur, cpin)
		       ; (cur != this) || (cpin != idx)
		       ; cur->pin(cpin).next_link(cur, cpin)) {

		  const NetNet*sig = dynamic_cast<const NetNet*>(cur);
		  if (sig) o << " " << sig->name() << "[" << cpin << "]";
	    }
	    o << endl;
      }
}

void NetObj::dump_obj_attr(ostream&o, unsigned ind) const
{
      for (map<string,string>::const_iterator idx = attributes_.begin()
		 ; idx != attributes_.end()
		 ; idx ++) {
	    o << setw(ind) << "" << (*idx).first << " = \"" <<
		  (*idx).second << "\"" << endl;
      }
}

void NetAddSub::dump_node(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "Adder (NetAddSub): " << name() << endl;
      dump_node_pins(o, ind+4);
}

void NetAssign::dump_node(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "Procedural assign (NetAssign): " <<
	    *rval_ << endl;
      dump_node_pins(o, ind+4);
}

void NetAssignNB::dump_node(ostream&o, unsigned ind) const
{
      if (bmux_)
	    o << setw(ind) << "" << "Procedural NB assign (NetAssignNB): "
	      << name() << "[" << *bmux_ << "] <= " << *rval_ << endl;
      else
	    o << setw(ind) << "" << "Procedural NB assign (NetAssignNB): "
	      << name() << " <= " << *rval_ << endl;
      dump_node_pins(o, ind+4);
}

void NetBUFZ::dump_node(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "NetBUFZ: " << name() << endl;
      dump_node_pins(o, ind+4);
}

void NetConst::dump_node(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "constant " << value_ << ": " << name() << endl;
      dump_node_pins(o, ind+4);
}

void NetLogic::dump_node(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "logic: ";
      switch (type_) {
	  case AND:
	    o << "and";
	    break;
	  case BUF:
	    o << "buf";
	    break;
	  case BUFIF0:
	    o << "bufif0";
	    break;
	  case BUFIF1:
	    o << "bufif1";
	    break;
	  case NAND:
	    o << "nand";
	    break;
	  case NOR:
	    o << "nor";
	    break;
	  case NOT:
	    o << "not";
	    break;
	  case OR:
	    o << "or";
	    break;
	  case XNOR:
	    o << "xnor";
	    break;
	  case XOR:
	    o << "xor";
	    break;
      }
      o << " #(" << rise_time()
	<< "," << fall_time() << "," << decay_time() << ") " << name()
	<< endl;

      dump_node_pins(o, ind+4);
      dump_obj_attr(o, ind+4);
}

void NetTaskDef::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "task " << name_ << ";" << endl;

      for (unsigned idx = 0 ;  idx < ports_.count() ;  idx += 1) {
	    o << setw(ind+4) << "";
	    switch (ports_[idx]->port_type()) {
		case NetNet::PINPUT:
		  o << "input ";
		  break;
		case NetNet::POUTPUT:
		  o << "output ";
		  break;
		case NetNet::PINOUT:
		  o << "input ";
		  break;
		default:
		  o << "NOT_A_PORT ";
		  break;
	    }
	    o << ports_[idx]->name() << ";" << endl;
      }

      proc_->dump(o, ind+4);

      o << setw(ind) << "" << "endtask" << endl;
}

void NetUDP::dump_sequ_(ostream&o, unsigned ind) const
{
      string tmp = "";
      for (unsigned idx = 0 ;  idx < ind ;  idx += 1)
	    tmp += " ";

      o << tmp << "Sequential UDP" << " #(" << rise_time() <<
	    "," << fall_time() << "," << decay_time() << ") " << name() <<
	    endl;

      for (FSM_::const_iterator ent = fsm_.begin()
		 ; ent != fsm_.end() ;  ent++) {
	    o << setw(ind+6) << "" << (*ent).first << " -->";

	    state_t_*st = (*ent).second;
	    assert((*ent).first[0] == st->out);
	    for (unsigned idx = 1 ;  idx < pin_count() ;  idx += 1) {
		  string tmp = (*ent).first;
		  if (st->pins[idx].zer) {
			tmp[0] = st->pins[idx].zer->out;
			tmp[idx] = '0';
			o << " " << tmp;
		  }

		  if (st->pins[idx].one) {
			tmp[0] = st->pins[idx].one->out;
			tmp[idx] = '1';
			o << " " << tmp;
		  }

		  if (st->pins[idx].xxx) {
			tmp[0] = st->pins[idx].xxx->out;
			tmp[idx] = 'x';
			o << " " << tmp;
		  }
	    }

	    o << endl;
      }

      o << setw(ind+6) << ""  << "initial value == " << init_ << endl;

      dump_node_pins(o, ind+4);
      dump_obj_attr(o, ind+4);
}

void NetUDP::dump_comb_(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "Combinational UDP: ";
      o << " #(" << rise_time() << "," << fall_time() << "," << decay_time() <<
	    ") " << name() << endl;

      dump_node_pins(o, ind+4);
      dump_obj_attr(o, ind+4);
}

void NetUDP::dump_node(ostream&o, unsigned ind) const
{
      if (sequential_)
	    dump_sequ_(o, ind);
      else
	    dump_comb_(o, ind);
}

void NetNEvent::dump_node(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "event: ";
      switch (edge_) {
	  case ANYEDGE:
	    o << "anyedge ";
	      break;
	  case POSEDGE:
	    o << "posedge ";
	    break;
	  case NEGEDGE:
	    o << "negedge ";
	    break;
	  case POSITIVE:
	    o << "positive ";
	    break;
      }

      o << name() << " --> " << fore_ptr()->name() << endl;

      dump_node_pins(o, ind+4);
}

void NetProcTop::dump(ostream&o, unsigned ind) const
{
      switch (type_) {
	  case NetProcTop::KINITIAL:
	    o << "initial  /* " << get_line() << " */" << endl;
	    break;
	  case NetProcTop::KALWAYS:
	    o << "always  /* " << get_line() << " */" << endl;
	    break;
      }

      statement_->dump(o, ind+2);
}

/* Dump an assignment statement */
void NetAssign::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "";

      const NetNet*sig;
      unsigned msb, lsb;
      find_lval_range(sig, msb, lsb);
      o << sig->name() << "[" << msb;
      if (pin_count() > 1)
	    o << ":" << lsb;
      o << "] = ";
      
      rval_->dump(o);
      o << ";" << endl;
}

void NetAssignNB::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "";

      if (bmux_) {
	    o << name() << "[" << *bmux_ << "] <= ";
	    if (rise_time())
		  o << "#" << rise_time() << " ";
	    o << *rval_ << ";" << endl;

      } else {
	    o << name() << " <= ";
	    if (rise_time())
		  o << "#" << rise_time() << " ";
	    o << *rval_ << ";" << endl;
      }
}

void NetAssignMem::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "";
      o << "/* " << get_line() << " */" << endl;
      o << setw(ind) << "";
      o << mem_->name() << "[";
      index_->dump(o);
      o << "] = ";
      rval_->dump(o);
      o << ";" << endl;
}

/* Dump a block statement */
void NetBlock::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << type_ << endl;

      if (last_) {
	    const NetProc*cur = last_;
	    do {
		  cur = cur->next_;
		  cur->dump(o, ind+4);
	    } while (cur != last_);
      }

      o << setw(ind) << "" << "end" << endl;
}

void NetCase::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "case (" << *expr_ << ")" << endl;

      for (unsigned idx = 0 ;  idx < nitems_ ;  idx += 1) {
	    o << setw(ind+2) << "";
	    if (items_[idx].guard)
		  o << *items_[idx].guard << ":";
	    else
		  o << "default:";

	    if (items_[idx].statement) {
		  o << endl;
		  items_[idx].statement->dump(o, ind+6);
	    } else {
		  o << " ;" << endl;
	    }
      }

      o << setw(ind) << "" << "endcase" << endl;
}

void NetCondit::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "if (";
      expr_->dump(o);
      o << ")" << endl;

      if (if_) if_->dump(o, ind+4);
      else o << setw(ind+4) << "" << "/* empty */ ;" << endl;

      if (else_) {
	    o << setw(ind) << "" << "else" << endl;
	    else_->dump(o, ind+4);
      }
}

void NetForever::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "forever" << endl;
      statement_->dump(o, ind+2);
}

void NetFuncDef::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "function " << name_ << endl;
      if (statement_)
	    statement_->dump(o, ind+2);
      else
	    o << setw(ind+2) << "" << "// NO STATEMENT" << endl;
}

void NetPDelay::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "#" << delay_;
      if (statement_) {
	    o << endl;
	    statement_->dump(o, ind+2);
      } else {
	    o << " /* noop */;" << endl;
      }
}

void NetPEvent::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "@(";
      svector<const NetNEvent*>*list = back_list();
      (*list)[0]->dump_proc(o);
      for (unsigned idx = 1 ;  idx < list->count() ;  idx += 1) {
	    o << " or ";
	    (*list)[idx]->dump_proc(o);
      }
      delete list;
      o << ") /* " << name_ << " */";

      if (statement_) {
	    o << endl;
	    statement_->dump(o, ind+2);
      } else {
	    o << " /* noop */;" << endl;
      }
}

void NetNEvent::dump_proc(ostream&o) const
{
      switch (edge_) {
	  case ANYEDGE:
	    o << "anyedge ";
	    break;
	  case POSEDGE:
	    o << "posedge ";
	    break;
	  case NEGEDGE:
	    o << "negedge ";
	    break;
	  case POSITIVE:
	    o << "positive ";
	    break;
      }
      o << name();
}

void NetRepeat::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "repeat (" << *expr_ << ")" << endl;
      statement_->dump(o, ind+2);
}

void NetSTask::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << name_;

      if (parms_.count() > 0) {
	    o << "(";
	    if (parms_[0])
		  parms_[0]->dump(o);

	    for (unsigned idx = 1 ;  idx < parms_.count() ;  idx += 1) {
		  o << ", ";
		  if (parms_[idx])
			parms_[idx]->dump(o);
	    }

	    o << ")";
      }
      o << ";" << endl;
}

void NetUTask::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << task_->name() << ";" << endl;
}

void NetWhile::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "while (" << *cond_ << ")" << endl;
      proc_->dump(o, ind+3);
}

/* Dump a statement type that someone didn't write a dump for. */
void NetProc::dump(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "// " << typeid(*this).name() << endl;
}

/* Dump an expression that noone wrote a dump method for. */
void NetExpr::dump(ostream&o) const
{
      o << "(?)";
}

void NetEBinary::dump(ostream&o) const
{
      o << "(";
      left_->dump(o);
      o << ")";
      switch (op_) {
	  default:
	    o << op_;
	    break;
	  case 'a':
	    o << "&&";
	    break;
	  case 'E':
	    o << "===";
	    break;
	  case 'e':
	    o << "==";
	    break;
	  case 'G':
	    o << ">=";
	    break;
	  case 'l':
	    o << "<<";
	    break;
	  case 'L':
	    o << "<=";
	    break;
	  case 'n':
	    o << "!=";
	    break;
	  case 'N':
	    o << "!==";
	    break;
	  case 'o':
	    o << "||";
	    break;
	  case 'r':
	    o << ">>";
	    break;
      }
      o << "(";
      right_->dump(o);
      o << ")";
}

void NetEConcat::dump(ostream&o) const
{
      o << "{" << *parms_[0];
      for (unsigned idx = 1 ;  idx < parms_.count() ;  idx += 1)
	    o << ", " << *parms_[idx];
      o << "}";
}

void NetEConst::dump(ostream&o) const
{
      if (value_.is_string())
	    o << "\"" << value_.as_string() << "\"";
      else
	    o << value_;
}

void NetEIdent::dump(ostream&o) const
{
      o << name_;
}

void NetESignal::dump(ostream&o) const
{
      o << name();
}

void NetESubSignal::dump(ostream&o) const
{
      sig_->dump(o);
      o << "[";
      idx_->dump(o);
      o << "]";
}

void NetEMemory::dump(ostream&o) const
{
      o << mem_->name() << "[";
      idx_->dump(o);
      o << "]";
}

void NetESignal::dump_node(ostream&o, unsigned ind) const
{
      o << setw(ind) << "" << "Expression Node (NetESignal): " <<
	    name() << endl;

      dump_node_pins(o, ind+4);
}

void NetETernary::dump(ostream&o) const
{
      o << "(" << *cond_ << ")? (" << *true_val_ << ") : (" <<
	    false_val_ << ")";
}

void NetEUFunc::dump(ostream&o) const
{
      o << name() << "(";
      assert(parms_.count() > 0);
      parms_[0]->dump(o);
      for (unsigned idx = 1 ;  idx < parms_.count() ;  idx += 1) {
	    o << ", ";
	    parms_[idx]->dump(o);
      }
      o << ")";
}

void NetEUnary::dump(ostream&o) const
{
      o << op_ << "(";
      expr_->dump(o);
      o << ")";
}

void Design::dump(ostream&o) const
{
      o << "ELABORATED PARAMETERS:" << endl;
      {
	    map<string,NetExpr*>::const_iterator pp;
	    for (pp = parameters_.begin()
		       ; pp != parameters_.end() ;  pp ++) {
		  o << "    " << (*pp).first << " = "  <<
			*(*pp).second << ";" << endl;
	    }
      }

      o << "ELABORATED SIGNALS:" << endl;

	// Dump the signals,
      if (signals_) {
	    NetNet*cur = signals_->sig_next_;
	    do {
		  cur->dump_net(o, 0);
		  cur = cur->sig_next_;
	    } while (cur != signals_->sig_next_);
      }

      o << "ELABORATED MEMORIES:" << endl;
      {
	    map<string,NetMemory*>::const_iterator pp;
	    for (pp = memories_.begin()
		       ; pp != memories_.end() ; pp ++) {
		  (*pp).second->dump(o, 0);
	    }
      }

      o << "ELABORATED FUNCTION DEFINITIONS:" << endl;
      {
	    map<string,NetFuncDef*>::const_iterator pp;
	    for (pp = funcs_.begin()
		       ; pp != funcs_.end() ; pp ++) {
		  (*pp).second->dump(o, 0);
	    }
      }

      o << "ELABORATED TASK DEFINITIONS:" << endl;
      {
	    map<string,NetTaskDef*>::const_iterator pp;
	    for (pp = tasks_.begin()
		       ; pp != tasks_.end() ; pp ++) {
		  (*pp).second->dump(o, 0);
	    }
      }

      o << "ELABORATED NODES:" << endl;

	// dump the nodes,
      if (nodes_) {
	    NetNode*cur = nodes_->node_next_;
	    do {
		  cur->dump_node(o, 0);
		  cur = cur->node_next_;
	    } while (cur != nodes_->node_next_);
      }

      o << "ELABORATED PROCESSES:" << endl;

	// Dump the processes.
      for (const NetProcTop*idx = procs_ ;  idx ;  idx = idx->next_)
	    idx->dump(o, 0);

}

/*
 * $Log: design_dump.cc,v $
 * Revision 1.39  1999/09/04 19:11:46  steve
 *  Add support for delayed non-blocking assignments.
 *
 * Revision 1.38  1999/09/03 04:28:38  steve
 *  elaborate the binary plus operator.
 *
 * Revision 1.37  1999/09/01 20:46:19  steve
 *  Handle recursive functions and arbitrary function
 *  references to other functions, properly pass
 *  function parameters and save function results.
 *
 * Revision 1.36  1999/08/31 22:38:29  steve
 *  Elaborate and emit to vvm procedural functions.
 *
 * Revision 1.35  1999/08/25 22:22:41  steve
 *  elaborate some aspects of functions.
 *
 * Revision 1.34  1999/08/01 16:34:50  steve
 *  Parse and elaborate rise/fall/decay times
 *  for gates, and handle the rules for partial
 *  lists of times.
 *
 * Revision 1.33  1999/07/24 02:11:20  steve
 *  Elaborate task input ports.
 *
 * Revision 1.32  1999/07/17 19:50:59  steve
 *  netlist support for ternary operator.
 *
 * Revision 1.31  1999/07/03 02:12:51  steve
 *  Elaborate user defined tasks.
 *
 * Revision 1.30  1999/06/19 21:06:16  steve
 *  Elaborate and supprort to vvm the forever
 *  and repeat statements.
 *
 * Revision 1.29  1999/06/15 05:38:15  steve
 *  Handle total lack of signals or nodes.
 *
 * Revision 1.28  1999/06/09 03:00:05  steve
 *  Add support for procedural concatenation expression.
 *
 * Revision 1.27  1999/06/06 20:45:38  steve
 *  Add parse and elaboration of non-blocking assignments,
 *  Replace list<PCase::Item*> with an svector version,
 *  Add integer support.
 *
 * Revision 1.26  1999/05/31 15:46:20  steve
 *  Compilation warning.
 *
 * Revision 1.25  1999/05/30 01:11:46  steve
 *  Exressions are trees that can duplicate, and not DAGS.
 *
 * Revision 1.24  1999/05/17 04:53:47  steve
 *  translate the letter synonyms for operators.
 *
 * Revision 1.23  1999/05/10 00:16:58  steve
 *  Parse and elaborate the concatenate operator
 *  in structural contexts, Replace vector<PExpr*>
 *  and list<PExpr*> with svector<PExpr*>, evaluate
 *  constant expressions with parameters, handle
 *  memories as lvalues.
 *
 *  Parse task declarations, integer types.
 *
 * Revision 1.22  1999/05/06 02:29:32  steve
 *  Excesss endl.
 *
 * Revision 1.21  1999/05/05 03:04:46  steve
 *  Fix handling of null delay statements.
 *
 * Revision 1.20  1999/05/01 20:43:55  steve
 *  Handle wide events, such as @(a) where a has
 *  many bits in it.
 *
 *  Add to vvm the binary ^ and unary & operators.
 *
 *  Dump events a bit more completely.
 *
 * Revision 1.19  1999/05/01 02:57:52  steve
 *  Handle much more complex event expressions.
 *
 * Revision 1.18  1999/04/25 00:44:10  steve
 *  Core handles subsignal expressions.
 *
 * Revision 1.17  1999/04/19 01:59:36  steve
 *  Add memories to the parse and elaboration phases.
 *
 * Revision 1.16  1999/03/15 02:43:32  steve
 *  Support more operators, especially logical.
 *
 * Revision 1.15  1999/03/01 03:27:53  steve
 *  Prevent the duplicate allocation of ESignal objects.
 *
 * Revision 1.14  1999/02/21 17:01:57  steve
 *  Add support for module parameters.
 *
 * Revision 1.13  1999/02/15 02:06:15  steve
 *  Elaborate gate ranges.
 *
 * Revision 1.12  1999/02/08 02:49:56  steve
 *  Turn the NetESignal into a NetNode so
 *  that it can connect to the netlist.
 *  Implement the case statement.
 *  Convince t-vvm to output code for
 *  the case statement.
 *
 * Revision 1.11  1999/02/03 04:20:11  steve
 *  Parse and elaborate the Verilog CASE statement.
 *
 * Revision 1.10  1999/02/01 00:26:48  steve
 *  Carry some line info to the netlist,
 *  Dump line numbers for processes.
 *  Elaborate prints errors about port vector
 *  width mismatch
 *  Emit better handles null statements.
 *
 * Revision 1.9  1998/12/20 02:05:41  steve
 *  Function to calculate wire initial value.
 *
 * Revision 1.8  1998/12/14 02:01:34  steve
 *  Fully elaborate Sequential UDP behavior.
 *
 * Revision 1.7  1998/12/07 04:53:17  steve
 *  Generate OBUF or IBUF attributes (and the gates
 *  to garry them) where a wire is a pad. This involved
 *  figuring out enough of the netlist to know when such
 *  was needed, and to generate new gates and signales
 *  to handle what's missing.
 *
 * Revision 1.6  1998/12/02 04:37:13  steve
 *  Add the nobufz function to eliminate bufz objects,
 *  Object links are marked with direction,
 *  constant propagation is more careful will wide links,
 *  Signal folding is aware of attributes, and
 *  the XNF target can dump UDP objects based on LCA
 *  attributes.
 *
 * Revision 1.5  1998/12/01 00:42:13  steve
 *  Elaborate UDP devices,
 *  Support UDP type attributes, and
 *  pass those attributes to nodes that
 *  are instantiated by elaboration,
 *  Put modules into a map instead of
 *  a simple list.
 *
 * Revision 1.4  1998/11/23 00:20:22  steve
 *  NetAssign handles lvalues as pin links
 *  instead of a signal pointer,
 *  Wire attributes added,
 *  Ability to parse UDP descriptions added,
 *  XNF generates EXT records for signals with
 *  the PAD attribute.
 *
 * Revision 1.3  1998/11/09 18:55:34  steve
 *  Add procedural while loops,
 *  Parse procedural for loops,
 *  Add procedural wait statements,
 *  Add constant nodes,
 *  Add XNOR logic gate,
 *  Make vvm output look a bit prettier.
 *
 * Revision 1.2  1998/11/07 17:05:05  steve
 *  Handle procedural conditional, and some
 *  of the conditional expressions.
 *
 *  Elaborate signals and identifiers differently,
 *  allowing the netlist to hold signal information.
 *
 * Revision 1.1  1998/11/03 23:28:56  steve
 *  Introduce verilog to CVS.
 *
 */

