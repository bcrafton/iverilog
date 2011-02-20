#ifndef __expression_H
#define __expression_H
/*
 * Copyright (c) 2011 Stephen Williams (steve@icarus.com)
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

# include  "StringHeap.h"
# include  "LineInfo.h"
# include  <inttypes.h>

class Entity;
class Architecture;

/*
 * The Expression class represents parsed expressions from the parsed
 * VHDL input. The Expression class is a virtual class that holds more
 * specific derived expression types.
 */
class Expression : public LineInfo {

    public:
      Expression();
      virtual ~Expression() =0;

	// The emit virtual method is called bu architecture emit to
	// output the generated code for the expression. The derived
	// class fills in the details of what exactly happend.
      virtual int emit(ostream&out, Entity*ent, Architecture*arc) =0;

	// The evaluate virtual method tries to evaluate expressions
	// to constant literal values. Return true and set the val
	// argument if the evaluation works, or return false if it
	// cannot be done.
      virtual bool evaluate(int64_t&val) const;

	// Debug dump of the expression.
      virtual void dump(ostream&out, int indent) const =0;

    private:

    private: // Not implemented
      Expression(const Expression&);
      Expression& operator = (const Expression&);
};

/*
 * This is an abstract class that collects some of the common features
 * of binary operators.
 */
class ExpBinary : public Expression {

    public:
      ExpBinary(Expression*op1, Expression*op2);
      ~ExpBinary();

    protected:
      inline Expression* operand1() { return operand1_; }
      inline Expression* operand2() { return operand2_; }
      inline const Expression* operand1() const { return operand1_; }
      inline const Expression* operand2() const { return operand2_; }

    private:
      Expression*operand1_;
      Expression*operand2_;
};

class ExpArithmetic : public ExpBinary {

    public:
      enum fun_t { PLUS, MINUS, MULT, DIV, MOD, REM, POW };

    public:
      ExpArithmetic(ExpArithmetic::fun_t op, Expression*op1, Expression*op2);
      ~ExpArithmetic();

      int emit(ostream&out, Entity*ent, Architecture*arc);
      void dump(ostream&out, int indent) const;

    private:
      fun_t fun_;
};

class ExpInteger : public Expression {

    public:
      ExpInteger(int64_t val);
      ~ExpInteger();

      int emit(ostream&out, Entity*ent, Architecture*arc);
      bool evaluate(int64_t&val) const;
      void dump(ostream&out, int indent) const;

    private:
      int64_t value_;
};

class ExpLogical : public ExpBinary {

    public:
      enum fun_t { AND, OR, NAND, NOR, XOR, XNOR };

    public:
      ExpLogical(ExpLogical::fun_t ty, Expression*op1, Expression*op2);
      ~ExpLogical();

      int emit(ostream&out, Entity*ent, Architecture*arc);
      void dump(ostream&out, int indent) const;

    private:
      fun_t fun_;
};

/*
 * The ExpName class represents an expression that is an identifier or
 * other sort of name.
 */
class ExpName : public Expression {

    public:
      ExpName(perm_string nn);
      ~ExpName();

      int emit(ostream&out, Entity*ent, Architecture*arc);
      void dump(ostream&out, int indent) const;

    private:
      perm_string name_;
};


#endif