/*
 * main.cpp
 *
 *  Created on: Jan 16, 2015
 *      Author: nbingham
 */

#include <parse/parse.h>
#include <parse_hse/parallel.h>
#include <hse/graph.h>
#include <interpret_hse/import.h>
#include <interpret_dot/export.h>
#include <boolean/variable.h>

#ifndef NOGRAPHVIZ
namespace graphviz
{
	#include <graphviz/cgraph.h>
	#include <graphviz/gvc.h>
}
#endif

void print_help()
{
	cout << "Usage: plot [options] file..." << endl;
	cout << "Options:" << endl;
	cout << " -h,--help      Display this information" << endl;
	cout << "    --version   Display version information" << endl;
	cout << " -v,--verbose   Display verbose messages" << endl;
	cout << " -d,--debug     Display internal debugging messages" << endl;
	cout << endl;
	cout << " -o             Specify the output file name" << endl;
	cout << "    formats other than 'dot' are passed onto graphviz dot for rendering" << endl;
	cout << " -s,--sync\t\tRender half synchronization actions" << endl;
}

void print_version()
{
	cout << "plot 1.0.0" << endl;
	cout << "Copyright (C) 2013 Sol Union." << endl;
	cout << "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << endl;
	cout << endl;
}

int main(int argc, char **argv)
{
	bool synchronize = false;
	string filename = "";

	configuration config;
	config.set_working_directory(argv[0]);
	tokenizer tokens;
	parse_hse::parallel::register_syntax(tokens);

	for (int i = 1; i < argc; i++)
	{
		string arg = argv[i];
		if (arg == "--help" || arg == "-h")			// Help
			print_help();
		else if (arg == "--version")	// Version Information
			print_version();
		else if (arg == "--verbose" || arg == "-v")
			set_verbose(true);
		else if (arg == "--debug" || arg == "-d")
			set_debug(true);
		else if (arg == "--sync" || arg == "-s")
			synchronize = true;
		else if (arg == "-o")
		{
			i++;
			if (i < argc)
				filename = argv[i];
			else
				error("", "expected file name", __FILE__, __LINE__);
		}
		else
			config.load(tokens, argv[i], "");
	}

	string format;
	if (filename == "")
		error("", "file name not specified", __FILE__, __LINE__);
	else
	{
		size_t loc = filename.find_last_of(".");
		if (loc == filename.npos)
			error("", "file format could not be determined from the file name", __FILE__, __LINE__);
		else
			format = filename.substr(loc+1);
	}

	if (is_clean() && tokens.segments.size() > 0)
	{
		parse_hse::parallel syntax(tokens);

		boolean::variable_set v;
		hse::graph g = import_graph(tokens, syntax, v, true);
		g.compact();

		if (synchronize)
			g.synchronize();

		string dot = export_graph(g, v).to_string();

		if (format == "dot")
		{
			ofstream s(filename.c_str());
			s << dot;
			s.close();
		}

#ifndef NOGRAPHVIZ
		else
		{
			graphviz::Agraph_t* G = graphviz::agmemread(dot.c_str());
			graphviz::GVC_t* gvc = graphviz::gvContext();
			graphviz::gvLayout(gvc, G, "dot");
			graphviz::gvRenderFilename(gvc, G, format.c_str(), filename.c_str());
			graphviz::gvFreeLayout(gvc, G);
			graphviz::agclose(G);
			graphviz::gvFreeContext(gvc);
		}
#else
		else
			error("", "file format '" + format + "' not supported", __FILE__, __LINE__);
#endif

	}

	complete();
	return is_clean();
}
