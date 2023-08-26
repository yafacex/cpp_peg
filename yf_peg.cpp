#include <iostream>
#include <string>
#include <vector>
#include "Node.h"
#include "Pattern.h"
#include "Ast.h"
#include "Ctx.h"
using namespace std;
using namespace yfpeg;
void test_node()
{
	string code = "abc 123xyz";
	Node* range_char = new RangeNode('a','z');
	Node* set_digit = new SetNode("0123456789");
	Node* space = new SetNode(" \t\n\r");
	Node* repeat_char = new RepeatNode(range_char);
	Node* repeat_digit = new RepeatNode(set_digit);
	Node* repeat_space = new RepeatNode(space);
	AndNode* _and = new AndNode();
	_and->addChild(repeat_char);
	_and->addChild(repeat_space);
	_and->addChild(repeat_digit);
	string matched;
	int idx = 0;
	_and->walk_level(0);
	Ast ast;
	Ctx ctx;
	ctx.code = code;
	bool ret = _and->eat(&ctx,&matched, &ctx.ast.root);
	cout << ret << endl;
}
void test_pat() {
	string code = "abcd 124zzzz";
	Pattern grammer = capture(
		(range('a','z')^1) * capture(
			(ch(' ')^1)* (range('1','9')^1),"tag2"
		)
		,"tag1"
	);
	Ctx ctx = grammer.parse(code);
	cout << ctx.ast.matched << endl;
}
void test_vector_calc() {
	//place holders
	Pattern stats = placeholder();
	Pattern stat = placeholder();
	Pattern exp = placeholder();
	Pattern block = placeholder();
	Pattern data_type_name = placeholder();
	Pattern varname = pat_name() - str("void") -data_type_name;
	//misc
	Pattern space_sep = pat_space()^1;
	Pattern space_maybe = pat_space()^0;
	Pattern semi = pat_named(space_maybe * ch(';') * space_maybe,"semi");
	//data types
	Pattern data_float = pat_float()<<"float";
	Pattern data_int = pat_int()<<"int";
	Pattern data_vec2 = str("vec2") * space_maybe * ch('(') 
					* space_maybe * (pat_float()<<"x")* space_maybe * ch(',') 
					* space_maybe * (pat_float()<<"y") * space_maybe
					* ch(')') << "vec2";
	Pattern data_vec3 = str("vec3") * space_maybe * ch('(') 
					* (space_maybe * pat_float()<<"x")* space_maybe * ch(',') 
					* (space_maybe * pat_float()<<"y")* space_maybe * ch(',') 
					* (space_maybe * pat_float()<<"z")* space_maybe 
					* ch(')') << "vec3";
	Pattern data_vec4 = str("vec4") * space_maybe * ch('(') 
					* (space_maybe * pat_float()<<"x") * space_maybe * ch(',') 
					* (space_maybe * pat_float()<<"y") * space_maybe * ch(',') 
					* (space_maybe * pat_float()<<"z") * space_maybe * ch(',') 
					* (space_maybe * pat_float()<<"w") * space_maybe 
					* ch(')') << "vec4";
	Pattern data_ivec2 = str("ivec2") * space_maybe * ch('(') 
					* (space_maybe * pat_int()<<"x")* space_maybe * ch(',') 
					* (space_maybe * pat_int()<<"y") * space_maybe
					* ch(')') << "ivec2";
	Pattern data_ivec3 = str("ivec3") * space_maybe * ch('(') 
					* (space_maybe * pat_int()<<"x")* space_maybe * ch(',') 
					* (space_maybe * pat_int()<<"y")* space_maybe * ch(',') 
					* (space_maybe * pat_int()<<"z")* space_maybe 
					* ch(')') << "ivec3";
	Pattern data_ivec4 = str("ivec4") * space_maybe * ch('(') 
					* (space_maybe * pat_int()<<"x") * space_maybe * ch(',') 
					* (space_maybe * pat_int()<<"y") * space_maybe * ch(',') 
					* (space_maybe * pat_int()<<"z") * space_maybe * ch(',') 
					* (space_maybe * pat_int()<<"w") * space_maybe 
					* ch(')') << "ivec4";
	Pattern datasExp = (data_float + data_vec2 + data_vec3 + data_vec4 +
		data_int + data_ivec2 + data_ivec3 + data_ivec4);// << "datas";
	//data type names
	data_type_name = str("int") + str("float") + str("vec2") + str("vec3") + str("vec4")
							+ str("ivec2") + str("ivec3") + str("ivec4");
	//data declare
	Pattern stat_var_name = varname << "stat_var_name";
	Pattern stat_float_var = scope_vartype(str("float")) * space_sep * scope_varname(stat_var_name) * space_maybe * ch('=') * space_maybe * (data_float + exp);
	Pattern stat_vec2_var = scope_vartype(str("vec2")) * space_sep * scope_varname(stat_var_name) * space_maybe * ch('=') * space_maybe * (data_vec2 + exp) ;
	Pattern stat_vec3_var = scope_vartype(str("vec3")) * space_sep * scope_varname(stat_var_name) * space_maybe * ch('=') * space_maybe * (data_vec3 + exp) ;
	Pattern stat_vec4_var = scope_vartype(str("vec4")) * space_sep * scope_varname(stat_var_name) * space_maybe * ch('=') * space_maybe * (data_vec4 + exp) ;
	Pattern stat_int_var = scope_vartype(str("int")) * space_sep * scope_varname(stat_var_name) * space_maybe * ch('=') * space_maybe * (data_int + exp);
	Pattern stat_ivec2_var = scope_vartype(str("ivec2")) * space_sep * scope_varname(stat_var_name) * space_maybe * ch('=') * space_maybe * (data_ivec2 + exp) ;
	Pattern stat_ivec3_var = scope_vartype(str("ivec3")) * space_sep * scope_varname(stat_var_name) * space_maybe * ch('=') * space_maybe * (data_ivec3 + exp) ;
	Pattern stat_ivec4_var = scope_vartype(str("ivec4")) * space_sep * scope_varname(stat_var_name) * space_maybe * ch('=') * space_maybe * (data_ivec4 + exp) ;

	Pattern stat_fvec_var = (stat_float_var + stat_vec2_var + stat_vec3_var + stat_vec4_var);// << "stat_fvec_stat";
	Pattern stat_ivec_var = (stat_int_var + stat_ivec2_var + stat_ivec3_var + stat_ivec4_var);// << "stat_ivec_stat";
	//stat var
	Pattern stat_var = (stat_fvec_var + stat_ivec_var) << "stat_var";
	//stat exp
	Pattern indexExp = (varname<<"index_varname") * ch('.') * (repeat_range(set("xyzw"),1,4)<<"index_marks");
	Pattern callArglist = ((exp * ((space_maybe * ch(',') * space_maybe * exp)^0) + space_maybe) << "call_arg_list");
	Pattern callExp = ((varname<<"func_name") * space_maybe * ch('(') * space_maybe * callArglist * space_maybe * ch(')'))<<"callExp";
	Pattern simpleExp = (datasExp + (varname<<"exp_var_name") + indexExp)<<"simpleExp">> FILTER_IF_ONE_NODE;
	Pattern negExp = (ch('-')^-1) * simpleExp;
	Pattern powExp = (negExp * ((space_maybe * ch('^') * space_maybe * negExp)^0)) << "powExp" >> FILTER_IF_ONE_NODE;
	Pattern multExp = (powExp * ((space_maybe * set("*/%") * space_maybe * powExp)^0)) << "multExp" >> FILTER_IF_ONE_NODE;
	Pattern addExp = (multExp * ((space_maybe * set("+-") * space_maybe * multExp)^0)) << "addExp" >> FILTER_IF_ONE_NODE;
	Pattern compareOp = (ch('>') + ch('<') + str(">=") + str("<=") + str("==")) << "compareOp" >> FILTER_IF_ONE_NODE;
	Pattern compareExp = (addExp * ((space_maybe * compareOp * space_maybe * addExp)^0)) << "compareExp" >> FILTER_IF_ONE_NODE;
	Pattern andExp = (compareExp * ( space_maybe * (str("&&") * space_maybe* compareExp)^0)) << "andExp" >> FILTER_IF_ONE_NODE;
	Pattern orExp = (compareExp * (( space_maybe * str("||") * space_maybe * compareExp)^0)) << "orExp" >> FILTER_IF_ONE_NODE;
	exp = (andExp + orExp) << "exp" >> FILTER_IF_ONE_NODE;
	//stat assign
	Pattern assignOp = (ch('=') + str("+=") + str("*=") + str("-=") + str("/=")) << "assignOp";
	Pattern index_assign = (indexExp << "index") * space_maybe * assignOp * space_maybe * ((exp) << "rvalue");
	Pattern var_assign = (varname<<"varname") * space_maybe * assignOp * space_maybe * ((exp) << "rvalue");
	Pattern stat_assign = (index_assign + var_assign) << "stat_assign";
	//stat else
	Pattern stat_else = str("else") * space_maybe * block;
	//stat else if
	Pattern stat_elseif = str("else") * space_maybe * str("if") * block;
	//stat if
	Pattern stat_if = (
						scope((
							str("if") * space_maybe * ch('(') 
							* space_maybe * (compareExp << "if_cond") * space_maybe * ch(')')
							* space_maybe * ( scope(block << "if_block") ) * space_maybe  
							* space_maybe * ( scope(stat_elseif << "elseif_block")^0 )
							* space_maybe * ( scope(stat_else << "else_block")^-1) 
						)<< "stat_if")
					) ;
	//stat for
	Pattern stat_for = (
						scope((
							str("for") * space_maybe * ch('(') 
							* space_maybe * ((stat_var + stat_assign)  << "stat_for_exp_start") * space_maybe * ch(';')
							* space_maybe * (compareExp << "stat_for_exp_cond") * space_maybe * ch(';')
							* space_maybe * (stat_assign << "stat_for_exp_itr") * space_maybe * ch(')')
							* space_maybe * block
						)<< "stat_for")
					) ;
	//stat call
	Pattern arg =  scope_vartype(data_type_name<<"arg_type") * space_sep * scope_varname(varname<<"arg_name") ;
	Pattern arg_list = ((arg * ((space_maybe * ch(',') * space_maybe * arg)^0) + space_maybe) << "arg_list");
	Pattern ret_type_name = data_type_name + str("void");
	Pattern stat_func = 
		(
			scope_vartype("function") * (ret_type_name<<"func_ret_type") * space_sep * scope_varname(varname << "stat_func_name") *
			scope(
				mark("stat_func_start") * 
				space_maybe * ch('(') * space_maybe * arg_list * space_maybe * ch(')')
				* space_maybe * block *
				mark("stat_func_end") 
			)
		) << "stat_func";

	stat = stat_var + stat_assign + stat_if + stat_for + stat_func + callExp;
	stats = (((stat * space_maybe * semi)^0)) <<"stats";
	block = ch('{') * space_maybe * stats * space_maybe * ch('}');
	Pattern main_pat = scope(stats<<"out_scope");
	string code =
		//"void print(int a,float b){a = 123;b = 1.0;a = 456;};"
		//"int log(int a,float b){int kk = 1;a = 123;b = 1.0;print(a);};"
		"void main(int k){"
		"float a1 = 1.0;"
		" a1 = 3.0   ;"
		"vec2 b = vec2(1.0,1.0);"
		"b = vec2(3.0,3.0) ;"
		" float c = a1 ;"
		//"   print(1,2.0);    "
		//"   log(1,2.0);    "
		"};"
		//"b = vec2(2.0,3.0) + vec2(5.0,6.0);"
		//"float a2 = 1.0;"
		//"a2 = a1;"
		//"a1 = a2;"
		//"vec2 a2 = vec2(1.0,33.3);"
		//"a2 = vec2(1.0,111.1);"
		//"a2.xy = vec2(1.0,2.0);"
		//"func(3.0);"
		//"if(a1 == 3.0){float k1 = 0.0;a1 = 99.0;};"
		//"int kkk = 3;"
		//"for(int k = 3; k < 10; k = k + 1){"
		//"	a1 *= a1 * 77.0;vec4 vvv4 = vec4(1.0,1.0,1.0,1.0);"
		//"};"
		//"int qqq = 3;"
		//"int zzz = 3;"
		//"vvv4 = 3;"
		;

	//code = "print(1,2.0);";
	Ctx ctx = main_pat.parse(code);
	ctx.ast.root.walk(&ctx);
	//ctx.scope.root->walk();
	//below code not tested
	//ctx.vm.dumpInstructions();
	//ctx.vm.pc_entrance = ctx.scope.findEntrance("main");
	//cout << ctx.ast.matched << endl;
	//cout << "entrance is:" << ctx.vm.pc_entrance << endl;
	//ctx.vm.run();
}

int main() {
	test_node();
	test_pat();
	test_vector_calc();
}

