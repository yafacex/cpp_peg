#pragma once
#include <stdio.h>
#include <cstdarg>
#include <string>
#include <sstream>
using namespace std;
namespace yfpeg {
	static bool sNodeEatDebug = false;

	#define MAX_LOG_LENGTH 16*1024
	static void _LOG(const char* fmt, va_list args) {
		char buf[MAX_LOG_LENGTH];
		vsnprintf(buf, MAX_LOG_LENGTH - 3, fmt, args);
		fprintf(stdout, "%s", buf);
		fflush(stdout);
	}
	static void LOG(const char* fmt, ...) {
		if (sNodeEatDebug) {
			va_list args;
			string fmt_line(fmt);
			fmt_line += "\n";
			va_start(args, fmt);
			_LOG(fmt_line.c_str(),args);
			va_end(args);
		}
	};
	static string getIndent(int level,int blank = 0) {
		string ret = "";
		int n = level * 2 - blank;
		for (int i = 0; i < n; ++i) {
			ret += " ";
		}
		return ret;
	};
	static string int2str(int i) {
		stringstream s;
		s << i;
		return s.str();
	};
	static string float2str(float f) {
		stringstream s;
		s << f;
		s.precision(4);
		return s.str();
	};
	static string int2addrStr(int i) {
		stringstream s;
		s << i;
		return "[" + s.str() + "]";
	};
	static string vec2_2str(float f1, float f2) {
		stringstream s1,s2;
		s1 << f1;
		s2 << f2;
		return "Vec2:(" + s1.str() + "," + s2.str() + ")";
	};
}

